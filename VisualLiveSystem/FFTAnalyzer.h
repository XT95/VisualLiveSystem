#ifndef FFTANALYZER_H
#define FFTANALYZER_H


#include <complex>
#include <vector>
#include <algorithm>
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>

enum FFTDirection
{
    FFT_FORWARD = 0,
    FFT_REVERSE = 1
};


class Window
{
public:
    enum Type
    {
        TYPE_RECT,
        TYPE_HANN
    };

    static void generate(Type type, size_t N, float out[]);
    static double eval(Type type, size_t n, size_t N);
};



// Send Short term FFT data to a listener.
// Variable overlap.
// Introduced approx. windowSize/2 samples delay.
class FFTAnalyzer
{
public:

    // if zeroPhaseWindowing = true, "zero phase" windowing is used
    // (center of window is at first sample, zero-padding happen at center)
    FFTAnalyzer(Window::Type windowType, bool zeroPhaseWindowing, bool correctWindowLoss);

    size_t windowSize() const;
    size_t analysisPeriod() const;

    // To call at initialization and whenever samplerate changes
    // windowSize = size of analysis window, expressed in samples
    // fftSize = size of FFT. Must be power-of-two and >= windowSize. Missing samples are zero-padded in time domain.
    // analysisPeriod = period of analysis results, allow to be more precise frequentially, expressed in samples
    // Basic overlap is achieved with windowSize = 2 * analysisPeriod
    void setup(size_t windowSize, size_t fftSize, size_t analysisPeriod);

    // Process one sample, eventually return the result of short-term FFT
    // in a given Buffer
    bool feed(float x, std::vector< std::complex<float> >* fftData);


    //Copy/Paste a buffer table
    bool pushStereoBuffer(float* buffer , unsigned int size , std::vector< std::complex<float> >* fftData);

private:
    std::vector<float> _audioBuffer;
    std::vector<float> _windowBuffer;

    size_t _fftSize;        // in samples
    size_t _windowSize;     // in samples
    size_t _analysisPeriod; // in samples

    Window::Type _windowType;
    bool _zeroPhaseWindowing;

    size_t _index;
    bool _setupWasCalled;

    // should we multiply by _windowGainCorrFactor?
    bool _correctWindowLoss;
    // the factor by which to multiply transformed data to get in range results
    float _windowGainCorrFactor;
};

#endif // FFTANALYZER_H
