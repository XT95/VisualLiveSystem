/****************************************************************************
Nom ......... : signal.cpp
Role ........ : Implémente une classe gérant une unité de son (classe Signal) 
                Ainsi que des modeles abstrait pour le générer ce son...
Auteur ...... : Julien DE LOOR & Nadher ALI
Version ..... : V2.0 olol
Licence ..... : © Copydown™
****************************************************************************/

#include "signal/signal.hpp"
#include "signal/fft.hpp"

volatile unsigned int Signal::frequency=44100;
volatile float Signal::refreshRate=50;
volatile unsigned int Signal::size=Signal::frequency/Signal::refreshRate;
volatile unsigned int Signal::byteSize=sizeof(sample)*Signal::size;
volatile bool Signal::isPow2=false;

std::set<Signal*> Signal::_instances;

Signal::Signal() :
samples(0)
{
  const unsigned int size = Signal::byteSize;
  samples=(sample*) malloc(size);
  memset((void*) samples,0,size);
  _instances.insert(this);
}

Signal::Signal(const Signal& s) :
samples(0)
{
  const unsigned int size = Signal::byteSize;
  samples=(sample*) malloc(size);
  memcpy((void*) samples,(void*) s.samples,size);
  _instances.insert(this);
}

Signal::~Signal()
{
  if (samples)
    free((void*) samples);
  _instances.erase(this);
}

void Signal::mix(const Signal* s, unsigned int n)
{ 
  const unsigned int size = Signal::size;
  for (unsigned int k=0; k < n; k++)
  {
    for (unsigned int i=0; i < size;i++)
    {
      samples[i] *= s[k].samples[i];
    }
  }
}

void Signal::add(const Signal* s, unsigned int n)
{
  const unsigned int size = Signal::size;
  for (unsigned int k=0; k < n; k++)
  {
    for (unsigned int i=0; i < size;i++)
    {
      samples[i] += s[k].samples[i];
    }
  }
}

void Signal::addOffset(sample offset)
{
  const unsigned int size = Signal::size;
  for (unsigned int i=0; i < size;i++)
  {
    samples[i] += offset;
  }
}

void Signal::scale(sample scale)
{
  const unsigned int size = Signal::size;
  for (unsigned int i=0; i < size;i++)
  {
    samples[i] *= scale;
  }
}

void Signal::constant(sample constant)
{
  const unsigned int size = Signal::size;
  for (unsigned int i=0; i < size;i++)
  {
    samples[i] = constant;
  }
}

Signal& Signal::operator=(const Signal& s)
{
  memcpy((void*) samples,(void*) s.samples,Signal::byteSize);
  return *this;
}

void Signal::_reset()
{
  const unsigned int size = Signal::byteSize;
  samples=(sample*) realloc((void*)samples,size);
  memset((void*) samples,0,size);
}

void Signal::globalConfigurationFromFramerate(unsigned int f,  float r)
{
  if (f>=8000)
    Signal::frequency = f;

  if (r > 0)
    Signal::refreshRate = r; 
  Signal::size = Signal::frequency/Signal::refreshRate;
  Signal::byteSize = sizeof(sample)*Signal::size;
  Signal::isPow2 = false;
  for (std::set<Signal*>::iterator it = _instances.begin(); it != _instances.end() ;it++)
  {
    (*it)->_reset();
  }
}

void Signal::globalConfigurationFromPow2(unsigned int f,  unsigned short pow2)
{
  if (f>=8000)
    Signal::frequency = f;

  pow2 = FFT::getSupPow2(pow2);
  Signal::size = 1 << pow2;
  Signal::refreshRate = Signal::frequency/(float)Signal::size;
  Signal::byteSize = sizeof(sample)*Signal::size;
  Signal::isPow2 = true;
  for (std::set<Signal*>::iterator it = _instances.begin(); it != _instances.end() ;it++)
  {
    (*it)->_reset();
  }
}

void Signal::saturate(sample min, sample max)
{
  const unsigned int size = size;
  for (unsigned int i=0; i < Signal::size;i++)
  {
    if (samples[i] > max) samples[i] = max;
    else if (samples[i] < min) samples[i] = min;
  }
}

sample Signal::energy()
{
  sample energy=0;
  const unsigned int size = size;
  for (unsigned int i=0; i < Signal::size;i++)
  {
    energy += samples[i]*samples[i];
  }
  return energy;
}

void Signal::tfd(Signal& out_tfd)
{
  out_tfd.reset();
  const unsigned int size = Signal::size;
  for (unsigned int k=0; k < size;k++)
  {
    for (unsigned int n=0; n < size;n++)
    {
      double a=2*3.14159*k*n/(double)(size);
      double cos_a=cos(a);
      double sin_a=sin(a);
      out_tfd.samples[k] += (sample) (abs(samples[n])*(sqrt(abs(cos_a*cos_a+sin_a*sin_a))));  
    }
  }
}

sample Signal::level(){
  sample lvl=0;
  const unsigned int size = Signal::size;
  for (unsigned int k=0; k < size;k++)
  {
    const sample fs = fabs(samples[k]);
    if (fs > lvl) lvl = fs;
  }
  return lvl;
}

void Signal::normalize() {
  scale(1.0/(level()+0.001));
}




VisualSignal::VisualSignal() :
noise(0),
clean(0),
linear(0),
distord(0),
triangles(0),
previous(50),
previous2(50),
dprevious(0),
dprevious2(0)
{
  memset(samples,0,1024);
}
VisualSignal::~VisualSignal() {
}


void VisualSignal::update(const Signal& signal) {
  const unsigned int size = Signal::size > 1024 ? 1024 : Signal::size;
  for (unsigned int i = 0; i < size; i++){
    samples[i] = signal.samples[i]*50+50;
  }

  noise=0;
  clean=0;
  distord=0;
  linear=0;
  triangles=0;

  const int previous_save=previous;
  for (unsigned int i = 0; i < size; i++){
    const int d31 = previous - samples[i];
    const int d32 = samples[i] - previous;
    const int d3 = d31 >= 0 ? d31 : d32;
    const int d3i = d31 < 0 ? d31 : d32;

    const int d3p = dprevious > 0 ? dprevious : -dprevious; 
    const int d3pp = dprevious2 > 0 ? dprevious2 : -dprevious2; 

    const int dd31 = dprevious - d31;
    const int dd32 = d31 - dprevious;
    const int dd3 = dd31 >= 0 ? dd31 : dd32;
    const int dd3i = dd31 < 0 ? dd31 : dd32;


    //if ((d31 > 0 && dprevious < 0)||(d31 < 0 && dprevious > 0))
    //  noise += d3;
    if (d3>5) {
      noise += d3;    }
    else if ((d31 >= 0 && dprevious >= 0) || ( d31 < 0 && dprevious < 0)) {
      if (d3p <=3 && d3pp <=3 && ((d31 >= 0 && dprevious2 >= 0) || ( d31 < 0 && dprevious2 < 0)))
      {
        clean+=6;
      }
      else clean++;
    }

    if (dd3 > 3) distord += dd3;
    if (d31 == dprevious && dprevious == dprevious2) linear += 3;

    previous2=previous;
    previous=samples[i];
    dprevious2=dprevious;
    dprevious=d31;
  }

  noise = noise*1024/size;
  clean = clean*1024/size;
  distord = distord*1024/size;
  linear = linear*1024/size;

  previous=previous_save;
  int min=100;
  int max=0;
  unsigned int x1=0;
  unsigned int x2=0;
  // (x1,max)  
  // /\
  //   \
  //    \  /
  //     \/
  //     (x2,min)
  for (unsigned int i = 0; i < size; i++){
    if (samples[i] > previous && min==100) {
      x1=i;
      max = samples[i]; //Find the max
    }
    else if (samples[i] < previous) {
      x2=i;
      min = samples[i]; //next find the min
    }
    else if (samples[i] > previous)
    {
      //testons la linéarité de la pente
      const int d = x2 - x1;
      const int m = (max - min);
      if (d > 30 && m > 3) {
        const float c = m/(float)d;
        float y = max;
        unsigned bruits = 0;
        for (unsigned int i=x1; i <= x2;i++)
        {
          y-=c;
          if (fabs(y-(float)samples[i]) > 2) { //suffisament linéaire ?
            bruits++;
          }
        }
        if (bruits < m/3) {
          triangles+=1;
        }
      }
      min=100;
      max=0;
    }
   
    previous=samples[i];
  }

  triangles = triangles*1024/size;

}
