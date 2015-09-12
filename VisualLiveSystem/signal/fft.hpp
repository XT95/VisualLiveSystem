/****************************************************************************
Nom ......... : fft.hpp
Role ........ : Déclare une classe permettant de faire une transformé de 
                fourrier continue...
Auteur ...... : Kwon-Young CHOI & Julien DE LOOR
Version ..... : V1.7 olol
Licence ..... : © Copydown™
****************************************************************************/

#ifndef __FASTFURIOUS8TRANSFORM
#define __FASTFURIOUS8TRANSFORM

#include "signal/signal.hpp"
#include <algorithm>
#include <QMutex>


class FFT
{
  public:
    FFT(unsigned int size);
    ~FFT();

    inline unsigned int size() {
      return _size;
    }

    //get nearest inferior power of 2
    static unsigned short getInfPow2(unsigned int v);

    //get nearest superior power of 2
    static unsigned short getSupPow2(unsigned int v);

    //push signal and compute when have enough samples
    void pushSignal(const Signal &s);
    
    
    //Direct immediate compute from samples (with truncate or zero padding...)
    void compute(const sample* s, unsigned int size);
    void compute(const Signal &s);

    //Realloc 
    void realloc(unsigned int size);

    inline sample* getReal() {
      return _real;
    }
    inline sample* getImaginary() {
      return _imaginary;
    }

    inline sample* getModule() {
      return _module;
    }
    
    //Compute module if needed !
    void computeModule();

    QMutex mutex;

  private:
    sample* _input; //Real input buffer;
    sample* _real;
    sample* _imaginary;
    sample* _module;
    unsigned int* _indexTable;
    sample* _twidleFactor;
    sample* _twidleFactorI;
    unsigned int _size;
    unsigned int _inputOffset;
    unsigned short _pow2;


};


#endif

