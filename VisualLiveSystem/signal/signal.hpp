/****************************************************************************
Nom ......... : signal.hpp
Role ........ : Déclare une classe gérant une unité de son (classe Signal) 
                Ainsi que des modeles abstrait pour le générer ce son...
Auteur ...... : Julien DE LOOR
Version ..... : V1.0 olol
Licence ..... : © Copydown™
****************************************************************************/

#ifndef __SIGNAL
#define __SIGNAL

#include <vector>
#include <cstdlib> //old malloc
#include <cstring> //memset
#include <cmath>
#include <set>

typedef float sample; //type pour un echantillon

class Signal
{
  public:
    Signal();
    Signal(const Signal& s);
    virtual ~Signal(); 

    //Signal take care of allocation of this
    sample* samples;

    void mix(const Signal*, unsigned int n=1);
    void add(const Signal*, unsigned int n=1);
    void addOffset(sample offset);
    void scale(sample scale);
    void constant(sample constant);
    void saturate(sample min, sample max);
    sample energy();
    inline double power() {
      return energy()/(sample)Signal::size;
    }

    sample level();
    void normalize(sample coeff);
    void normalize();

    void tfd(Signal& out_tfd);
    
      inline void reset()
      {
        memset((void*) samples,0,Signal::byteSize);
      }


    Signal& operator=(const Signal&);

    //Constants for all signals
    volatile static unsigned int frequency; //Fe
    volatile static float refreshRate;      //Le but c'est d'avoir 50 ici ! le plus possible sera le mieux
    volatile static unsigned int size;
    volatile static unsigned int byteSize;
    volatile static bool isPow2;

    // /!\ Signal::refreshRate n'est qu'une valeur indicative !


    //be sure to lock ALL instances of Signals before do theses
    static void globalConfigurationFromFramerate( unsigned int f,  float r);
    static void globalConfigurationFromPow2(unsigned int f,  unsigned short pow2);

  private:
    void _reset();
    static std::set<Signal*> _instances;

};

class AbstractSignalModifier
{
  public:
    virtual ~AbstractSignalModifier() {}
    virtual void step(Signal* inout) = 0;
};

class AbstractStereoSignalMixer
{
  public:
    virtual ~AbstractStereoSignalMixer() {}
    virtual void step(const Signal& inleft, const Signal& inright, Signal& outleft,  Signal& outright) = 0;
};

class AbstractSignalGenerator : public AbstractSignalModifier
{
  public:
    AbstractSignalGenerator() : _time(0) {}
    virtual ~AbstractSignalGenerator() {}

    virtual void step(Signal* output) = 0;

    inline Signal* generate()
    {
      this->step(&_output);
      return &_output;
    }

    virtual inline void resetTime()
    {
      _time=0;
    }

  protected:
    Signal _output;
    unsigned int _time; //in (1/fe) secondes
};

class VisualSignal
{
  public:
    VisualSignal();
    virtual ~VisualSignal();

    void update(const Signal& signal);
    
    int noise;
    int clean;
    int distord;
    int linear;
    int triangles;

  protected:
    int samples[1024]; //samples scaled from 0 to 100
    int previous;
    int previous2;
    int dprevious;
    int dprevious2;
};


#endif
