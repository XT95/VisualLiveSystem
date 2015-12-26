#include "signal/decoder.hpp"
#include "signal/basserrorhandler.hpp"

#include <iostream>
#include <assert.h>

#include <QMessageBox>

BassDecoder::BassDecoder(QObject* parent):
Decoder(parent),
_music(0),
_mod(false),
_ended(true),
_bytesStep(0),
_samplesForSignals(0)
{
  int proc_loaded = load_bass_procs("bass.dll");
  std::cerr << proc_loaded << " loaded from bass.dll" << std::endl;

  if (!BASS_EnsureBassInit(0,Signal::frequency,0))
  {
	  std::cerr << "error : " << handleBassInitError() << std::endl;
      QMessageBox::critical(0,QString("BASS Error"),QString::fromStdString(handleBassInitError()),QMessageBox::Ok,QMessageBox::Ok);
      exit(0xdead);
  }
}

BassDecoder::~BassDecoder()
{
  reset();

  if (_music)
  {
	  if (_mod) BASS_MusicFree((HMUSIC) _music);
	  else BASS_StreamFree((HSTREAM) _music);
  }
  if (_samplesForSignals)
    free(_samplesForSignals);
  BASS_Free();
}

void BassDecoder::reset()
{
  if (_music) {
    if (_mod) BASS_MusicFree((HMUSIC) _music);
	  else BASS_StreamFree((HSTREAM) _music);
		_music = 0;
  }
  _ended=true;
}

bool BassDecoder::open(QString filename)
{
  reset();
  if (_music=(HANDLE)BASS_MusicLoad(FALSE, (void*) filename.toStdString().c_str(), 0,0,BASS_MUSIC_PRESCAN | BASS_MUSIC_DECODE | BASS_SAMPLE_FLOAT,Signal::frequency))
	{
		_mod=true;
	}
    else if (_music=(HANDLE)BASS_StreamCreateFile(FALSE, (void*) filename.toStdString().c_str(), 0,0,BASS_STREAM_PRESCAN |BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT))
	{
		_mod=false;
	}
    else if (_music=(HANDLE)BASS_StreamCreateURL(filename.toStdString().c_str(),0,BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT, (DOWNLOADPROC*) 0,(void*)0))
	{
		_mod=false;
	}
  if (_music)
  {
    BASS_ChannelGetInfo((DWORD)_music,&_infos);
    _bytesStep= sizeof(float)*Signal::size*_infos.chans;
    _samplesForSignals = (float*) realloc(_samplesForSignals,_bytesStep);
    if (_infos.freq != Signal::frequency)
      std::cout << "warning : file rate (" << _infos.freq << "Hz) is different of " << Signal::frequency << "Hz" << std::endl;
    _ended=false;
  }

  return (_music);
}

unsigned int BassDecoder::nextFrame(Signal& left, Signal& right){
  if (_music)
  {
    DWORD readed;
    BASS_ChannelGetInfo((DWORD)_music,&_infos);
    _bytesStep= sizeof(float)*Signal::size*_infos.chans;
    readed=BASS_ChannelGetData((DWORD)_music, (void*)_samplesForSignals, _bytesStep);

    if(readed==-1 && BASS_ErrorGetCode() == BASS_ERROR_ENDED) {
      left.reset();
      right.reset();
      _ended=true;
      emit trackFinished();
      return 0;
    }

    if (readed < _bytesStep)
    {
      for (unsigned int i=readed; i< _bytesStep ;i++)
        ((char*)_samplesForSignals)[i]=0;
    }
    if (_infos.chans>=2)
    {
      unsigned int k=0;
		  for (unsigned int i=0; i < Signal::size;)
		  {
			  left.samples[i]=_samplesForSignals[k++];
			  right.samples[i++]=_samplesForSignals[k++];
        k+=_infos.chans-2;
		  }
    }
    else if (_infos.chans==1) //mono
    {
      unsigned int k=0;
		  for (unsigned int i=0; i < Signal::size;)
		  {
			  left.samples[i]=_samplesForSignals[k];
			  right.samples[i++]=_samplesForSignals[k++];
		  }
    }
    return readed;
  }
  return 0;
}


void BassDecoder::rewind()
{
  if (_music)
  {
    if (_mod)
    {
      BASS_ChannelSetPosition((DWORD) _music,0,BASS_MUSIC_POSRESETEX|BASS_MUSIC_POSRESET);
    }
    else 
      BASS_ChannelSetPosition((DWORD) _music,0,0);
    _ended=false;
  }
}

unsigned int BassDecoder::frames()
{
  if (_music)
  {
    rewind();
    unsigned int frames=1;
    Signal left,right;
    DWORD ret;
    while ((ret=BASS_ChannelGetData((DWORD)_music, (void*)_samplesForSignals, _bytesStep)) == _bytesStep) {
      if (ret==-1 && BASS_ErrorGetCode() == BASS_ERROR_ENDED)
        break;
      frames++;
      if (frames>108000) break;
    }
    rewind();
    return frames;
  }
  return 0;
}


PlaylistMixer::PlaylistMixer(Decoder& decoder):
_decoder(&decoder)
{

}

void PlaylistMixer::step(const Signal& inleft, const Signal& inright, Signal& outleft,  Signal& outright)
{
    if (!_decoder->nextFrame(outleft,outright))
    {
       outleft = inleft;
       outright = inright;
    }
    else {
        outleft.add(&inleft);
        outright.add(&inright);
    }
    outleft.saturate(-1,1);
    outright.saturate(-1,1);
}
