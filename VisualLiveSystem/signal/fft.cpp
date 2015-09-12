/****************************************************************************
Nom ......... : fft.cpp
Role ........ : Implémente une classe permettant de faire une transformé de 
                fourrier continue...
Auteur ...... : Kwon-Young CHOI & Julien DE LOOR
Version ..... : V1.7 olol
Licence ..... : © Copydown™
****************************************************************************/

#include "signal/fft.hpp"
#include <iostream>


FFT::FFT(unsigned int size) :
  _input(NULL),
  _real(NULL),
  _imaginary(NULL),
  _module(NULL),
  _indexTable(NULL),
  _twidleFactor(NULL),
  _twidleFactorI(NULL),
  _size(0),
  _inputOffset(0),
  _pow2(0)
{
  this->realloc(size);
}

FFT::~FFT() {
  if (_input) free(_input);
  if (_real) free(_real);
  if (_imaginary) free(_imaginary);
  if (_module) free(_module);
  if (_indexTable) free(_indexTable);
  if (_twidleFactor) free(_twidleFactor);
  if (_twidleFactorI) free(_twidleFactorI);
}

unsigned short FFT::getInfPow2(unsigned int v)
{
  unsigned short bit=0;
  for (unsigned int i=0; i < sizeof(unsigned int) << 3; i++)
  {
    if (v & 1) bit = i; v >>=1; 
  }
  return bit;
}

unsigned short FFT::getSupPow2(unsigned int v)
{
  unsigned int v_mem=v;
  unsigned short bit=0;
  for (unsigned int i=0; i < sizeof(unsigned int) << 3; i++)
  {
    if (v & 1) bit = i; v >>=1; 
  }
  if (((unsigned)1<<bit)==v_mem)
    return bit;
  else
    return bit+1;
}

void FFT::pushSignal(const Signal &s){
  const unsigned int copy_size = _inputOffset + Signal::size > _size ? _size : _inputOffset + Signal::size;
  const unsigned int signal_size = Signal::size;
  unsigned int i=0;
  while (_inputOffset< copy_size)
    _input[_inputOffset++] = s.samples[i++];
  if (_inputOffset==_size) _inputOffset = 0;
  while (i<signal_size)
    _input[_inputOffset++] = s.samples[i++]; 
    
  compute(_input,_size);
}

//calcul et allocation de l'espace requis pour la fft
//size represente le nombre de point complexe que contient le signal
//dans notre exemple : 9
void FFT::realloc(unsigned int size) {
  //on recupere la puissance de 2 superieur a size, un tableau de la taille 2^_pow2
  //nous permettra donc de contenir le signal
  //comme on travail avec des complexe, on utilise deux tableaux : un tableau avec les
  //parties reels et un tableau avec les parties imaginaires
  //dans notre cas, on obtient 4
  //la puissance de 2 superieur a 9 est 16(2^4)
  
  if (getSupPow2(size)) {
    _pow2=getSupPow2(size);
    //calcul de _size
    //_size est la taille des deux tableaux contenant les parties reels et imaginaires 
    //des points du signal
    //_size doit toujours etre une puissance de 2
    //c'est pour cela que l'on calcul _size par rapport a _pow2
    //_size=puissance de 2 superieur au nbre de points du signal
    //dans notre exemple 16
    _size = 1 << _pow2;
    
    std::cout << "FFT configuration : " << _size << " samples (2^" << _pow2 << ")" << std::endl;
    _inputOffset=0;
    _input=(sample*)std::realloc((void*)_input,_size*sizeof(float));
    _real=(sample*)std::realloc((void*)_real,_size*sizeof(float));
    _imaginary=(sample*)std::realloc((void*)_imaginary,_size*sizeof(float));
    _module=(sample*)std::realloc((void*)_module,_size*sizeof(float));
    //le nombre de twidleFator sera toujours de : nbre de points du signal -1
    //les twidleFactor sont aussi des complexes donc il nous faut allouer 2 tableaux
    //dans notre exemple deux tableaux de la taille 15 c'est a dire : [0, 14]
    _twidleFactor=(sample*)std::realloc((void*)_twidleFactor,(_size-1)*sizeof(float));
    _twidleFactorI=(sample*)std::realloc((void*)_twidleFactorI,(_size-1)*sizeof(float));
    //_indexTable contient tous les index inverse
    //ici on se refere aux index du signal remonte a la puissance de 2 superieur
    //dans notre exemple : 9->16 : index de 0 a 15
    _indexTable=(unsigned int*)std::realloc((void*)_indexTable,
                                            _size*sizeof(unsigned int));
  }
  //on initialise les valeurs a 0
  for (unsigned int i=0; i<_size;i++) {
    _real[i]=0;
    _imaginary[i]=0;
    _module[i]=0;
    _input[i]=0;
  }
  
  //calcul des twidleFactor
  //la formule est : exp(-j*2*PI*n/N)
  //si on explose en partie reel et imaginaire
  //Re(W)=cos(-2*PI*n/N)
  //Im(W)=sin(-2*PI*n/N)
  //Pour la FFT, N varie en puissance de 2 en commencant par 2 : 2, 4, 8, ...
  //N s'arrete a la puissance de 2 superieur au nombre de points du signal compris
  //dans notre exemple : 16
  //n varie de [0 a N/2[
  //pour le stockage, on les stocke dans l'ordre
  //W02, W04, W14, W08, W18, W28, W38, ...
  sample* g=_twidleFactor;
  sample* gi=_twidleFactorI;
  for (unsigned int N=2; N<=_size; N<<=1) {
    for (unsigned int n=0; n<(N>>1); n++) {
      *g++=(sample)std::cos(-2.f*3.14159265359*(float)n/(float)N);
      *gi++=(sample)std::sin(-2.f*3.14159265359*(float)n/(float)N);
    }
  }

  //generation du tableau d'index a bit renverse
  //les indexes sont de taille en bits de : puissance de 2 superieur au nombre de 
  //point du signal
  //dans notre exemple : 9 -> 16 : les indexes vont de [0,15]
  //la taille en bit des indexes sont donc de 4=_pow2
  //Pour calculer les indexes renverse, on fait :
  //&######0 -> 0######&
  //#&####0# -> #0####&#
  //...
  //la deuxieme boucle boucle sur la taille en bit d'un index divise par deux
  //en effet, en parcourant la moitie des bits de l'index, on a inverse tous les bits
  //g_init donne la position du bit 0
  //g_fin donne la position du bit &
  //i_delta donne l'ecart entre ces deux bits
    for (unsigned int j=0; j<_size; j++) {
      unsigned int g_init=0, g_fin=_pow2-1, index=0;
      unsigned int i_delta=g_fin-g_init;   
      for (unsigned int i=0; i<(unsigned int)(_pow2>>1); i++) {
        unsigned int b1=0,b2=0;
        b1=(j&(1<<g_init))<<i_delta;
        b2=(j&(1<<g_fin))>>i_delta;
        index=index|(b1|b2);
        g_init++;
        g_fin--;
        i_delta-=2;
      }
      //std::cout << "l'index vaut "<<index << std::endl;
      _indexTable[j]=index;
    }
}


void FFT::compute(const Signal &s) {
  compute(s.samples, Signal::size);
}

//#include <QDebug>

void FFT::compute(const sample* s, unsigned int size) {
  if (size > _size) size = _size;
  unsigned int k=0;
  //Copy AND swap (no need to SWAP for imaginary (always 0))
  for (; k<size; k++) {_real[_indexTable[k]]=s[k]; _imaginary[k]=0;}
  //zero-padding
  for (; k<_size; k++) {_real[_indexTable[k]]=0; _imaginary[k]=0;}

  //qDebug() << "Input fft " << size << " FFT Size " << _size;

  //butterfly
  unsigned int N_sum=0;
  sample c1_reel=0, c1_img=0, c2_reel=0, c2_img=0, W_reel=0, W_img=0;

  for (unsigned int N=2; N<=_size; N<<=1) {
    for(unsigned int i=0; i<(_size/N); i++) {
      for (unsigned int n=0; n < (N>>1) ; n++) {
        //std::cout << "index c1 "<< N*i+n << std::endl;
        c1_reel=_real[N*i+n];
        c1_img=_imaginary[N*i+n];
        //std::cout << "index c2 "<< N*i+n+(N>>1) << std::endl;
        c2_reel=_real[N*i+n+(N>>1)];
        c2_img=_imaginary[N*i+n+(N>>1)];
        //std::cout << "index twidleFactor "<< N_sum+n << std::endl;
        W_reel=_twidleFactor[N_sum+n];
        W_img=_twidleFactorI[N_sum+n];

        _real[N*i+n]=c1_reel + c2_reel*W_reel - c2_img*W_img;
        _imaginary[N*i+n]=c1_img + c2_reel*W_img + c2_img*W_reel;

        _real[N*i+n+(N>>1)]=c1_reel - c2_reel*W_reel + c2_img*W_img;
        _imaginary[N*i+n+(N>>1)]=c1_img - c2_reel*W_img - c2_img*W_reel;
      }
    }
    N_sum+=N>>1;
  }
}

//Desormais normalisé entre 0 et 1.0 !!!!
void FFT::computeModule() {
  if (_real && _imaginary) {
    //static float maxi=0;
    for (unsigned int i=0; i<_size; i++) {
      _module[i]=(sample)2.0*sqrt((_real[i]*_real[i] + _imaginary[i]*_imaginary[i]))/(float)_size;
    }
    //maxi=std::max(maxi,*(std::max_element(_module, _module+_size)));
    //std::cout << maxi << std::endl;
  }
}
