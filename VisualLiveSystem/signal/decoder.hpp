

#ifndef DECODER___
#define DECODER___

#include "signal/signal.hpp"
#include <QString>
#include <QObject>

class Decoder : public QObject
{
    Q_OBJECT

  public:
    Decoder(QObject* parent) :
    QObject(parent)
    {
    }

    virtual ~Decoder() {
    }

    virtual unsigned int frames() = 0;
    inline virtual QString name() { return ""; }
    inline virtual QString artist() { return ""; }
    inline virtual QString album() { return ""; }
    inline virtual QString gene() { return ""; }
    virtual unsigned int nextFrame(Signal& left, Signal& right)=0;
    virtual bool ended() = 0;

public slots:
    virtual bool open(QString filename)=0;
    virtual void rewind() = 0; 
signals:
  void trackFinished();

};

#include "signal/bassproc.h"

class BassDecoder : public Decoder
{
  Q_OBJECT

  public:  
    BassDecoder(QObject* parent=0);
    virtual ~BassDecoder();

    inline virtual bool ended() {return _ended;}
    virtual unsigned int frames();
    virtual unsigned int nextFrame(Signal& left, Signal& right);

public slots:
    virtual bool open(QString filename);
    virtual void rewind();
    void reset();


protected:
    HANDLE _music;
    bool _mod;
    bool _ended;
    BASS_CHANNELINFO _infos;
    unsigned _bytesStep;
    float* _samplesForSignals;

};

class PlaylistMixer : public QObject, public AbstractStereoSignalMixer
{
    Q_OBJECT
public:
    PlaylistMixer(Decoder& decoder);
    virtual void step(const Signal& inleft, const Signal& inright, Signal& outleft,  Signal& outright);

private:
    Decoder* _decoder;
};


#endif
