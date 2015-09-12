#include <cstring>
#include "FFTAnalyzer.h"

// Return true if power of 2.
inline bool isPowerOf2(size_t i)
{
    assert(i >= 0);
    return (i != 0) && ((i & (i - 1)) == 0);
}

// return x so that (1 << x) >= i
int iFloorLog2(size_t i)
{
    assert(i > 0);

    /*#if defined(_WIN32)
        unsigned long y;
        _BitScanReverse(&y, i);
        return y;
    #else*/
        int result = 0;
        while (i > 1)
        {
            i = i / 2;
            result = result + 1;
        }
        return result;
    //#endif
}




void Window::generate(Type type, size_t N, float out[])
{
    for (size_t i = 0; i < N; ++i)
    {
        out[i] = static_cast<float>(eval(type, i, N));
    }
}

double Window::eval(Type type, size_t n, size_t N)
{
    switch(type)
    {
    case TYPE_RECT:
        return 1.0;

    case TYPE_HANN:
        return 0.5 - 0.5 * cos((2 * M_PI * n) / (N - 1));

    default:
        assert(false);
    }
}


template<typename T>
void FFT(std::complex<T>* inout, size_t size, FFTDirection direction)
{
    assert(isPowerOf2(size));
    int m = iFloorLog2(size);

    // do the bit reversal
    int i2 = size / 2;
    int j = 0;
    for (int i = 0; i < (int)size - 1; ++i)
    {
        if (i < j)
            std::swap(inout[i], inout[j]);

        int k = i2;
        while(k <= j)
        {
            j = j - k;
            k = k / 2;
        }
        j += k;
    }

    // compute the FFT
    std::complex<T> c = -1;
    int l2 = 1;
    for (int l = 0; l < m; ++l)
    {
        int l1 = l2;
        l2 = l2 * 2;
        std::complex<T> u = 1;
        for (int j2 = 0; j2 < l1; ++j2)
        {
            int i = j2;
            while (i < (int)size)
            {
                int i1 = i + l1;
                std::complex<T> t1 = u * inout[i1];
                inout[i1] = inout[i] - t1;
                inout[i] += t1;
                i += l2;
            }
            u = u * c;
        }

        T newImag = sqrt((1 - real(c)) / 2);
        if (direction == FFT_FORWARD)
            newImag = -newImag;
        T newReal = sqrt((1 + real(c)) / 2);
        c = std::complex<T>(newReal, newImag);
    }

    // scaling for forward transformation
    if (direction == FFT_FORWARD)
    {
        for (int i = 0; i < (int)size; ++i)
            inout[i] = inout[i] / std::complex<T>((T)size, 0);
    }
}



// if zeroPhaseWindowing = true, "zero phase" windowing is used
// (center of window is at first sample, zero-padding happen at center)
FFTAnalyzer::FFTAnalyzer(Window::Type windowType, bool zeroPhaseWindowing, bool correctWindowLoss)
{
    _windowType = windowType;
    _zeroPhaseWindowing = zeroPhaseWindowing;
    _setupWasCalled = false;
    _correctWindowLoss = correctWindowLoss;
}

size_t FFTAnalyzer::windowSize() const
{
    return _windowSize;
}

size_t FFTAnalyzer::analysisPeriod() const
{
    return _analysisPeriod;
}

// To call at initialization and whenever samplerate changes
// windowSize = size of analysis window, expressed in samples
// fftSize = size of FFT. Must be power-of-two and >= windowSize. Missing samples are zero-padded in time domain.
// analysisPeriod = period of analysis results, allow to be more precise frequentially, expressed in samples
// Basic overlap is achieved with windowSize = 2 * analysisPeriod
void FFTAnalyzer::setup(size_t windowSize, size_t fftSize, size_t analysisPeriod)
{
    assert(isPowerOf2(fftSize));
    assert(fftSize >= windowSize);
    _setupWasCalled = true;

    assert(windowSize != 1);
    assert(analysisPeriod <= windowSize); // no support for zero overlap

    // 1-sized FFT support
    if (analysisPeriod == 0)
        analysisPeriod = 1;

    _windowSize = windowSize;
    _fftSize = fftSize;
    _analysisPeriod = analysisPeriod;

    // clear input delay
    _audioBuffer.resize(_windowSize);
    _index = 0;

    _windowBuffer.resize(_windowSize);
    Window::generate(_windowType, _windowSize, _windowBuffer.data());

    _windowGainCorrFactor = 0;
    for (size_t i = 0; i < _windowSize; ++i)
        _windowGainCorrFactor += _windowBuffer[i];
    _windowGainCorrFactor = _windowSize / _windowGainCorrFactor;

    if (_correctWindowLoss)
    {
        for (size_t i = 0; i < _windowSize; ++i)
            _windowBuffer[i] *= _windowGainCorrFactor;
    }

}

// Process one sample, eventually return the result of short-term FFT
// in a given Buffer
bool FFTAnalyzer::feed(float x, std::vector< std::complex<float> >* fftData)
{
    assert(_setupWasCalled);
    _audioBuffer[_index] = x;
    _index = _index + 1;
    if (_index >= _windowSize)
    {
        fftData->resize(_fftSize);

        if (_zeroPhaseWindowing)
        {
            // "Zero Phase" windowing
            // Through clever reordering, phase of ouput coefficients will relate to the
            // center of the window
            //_
            // \_                   _/
            //   \                 /
            //    \               /
            //     \_____________/____
            size_t center = (_windowSize - 1) / 2; // position of center bin
            size_t nLeft = _windowSize - center;
            for (size_t i = 0; i < nLeft; ++i)
                (*fftData)[i] = _audioBuffer[center + i] * _windowBuffer[center + i];

            size_t nPadding = _fftSize - _windowSize;
            for (size_t i = 0; i < nPadding; ++i)
                (*fftData)[nLeft + i] = 0.0f;

            for (size_t i = 0; i < center; ++i)
                (*fftData)[nLeft + nPadding + i] = _audioBuffer[i] * _windowBuffer[i];
        }
        else
        {
            // "Normal" windowing
            // Phase of ouput coefficient will relate to the start of the buffer
            //      _
            //    _/ \_
            //   /     \
            //  /       \
            //_/         \____________

            // fill FFT buffer and multiply by window
            for (size_t i = 0; i < _windowSize; ++i)
                (*fftData)[i] = _audioBuffer[i] * _windowBuffer[i];

            // zero-padding
            for (size_t i = _windowSize; i < _fftSize; ++i)
                (*fftData)[i] = 0.0f;
        }

        // perform forward FFT on this slice
        FFT(fftData->data(), _fftSize, FFT_FORWARD);

        // rotate buffer
        {
            size_t const samplesToDrop = _analysisPeriod;
            assert(0 < samplesToDrop && samplesToDrop <= _windowSize);
            size_t remainingSamples = _windowSize - samplesToDrop;

            // TODO: use ring buffer instead of copy
            memmove(_audioBuffer.data(), _audioBuffer.data() + samplesToDrop, sizeof(float) * remainingSamples);
            _index = remainingSamples;

        }
        return true;
    }
    else
    {
        return false;
    }
}


bool FFTAnalyzer::pushStereoBuffer(float* buffer , unsigned int size , std::vector< std::complex<float> >* fftData)
{
    assert(_setupWasCalled);

    unsigned int trash_size = _index + (size>>1) > _windowSize ? _windowSize - _index : (size>>1); //VERY UGLY NEED TO CHANGE THAT BECAUSE IT MAY TRUNCATE AUDIOS BUFFERS
    //memcpy((void*) &(buffer[_index]), (void*) buffer, sizeof(float)*trash_size);
    //Trash memcpy avant de trouver une solution...
    for (unsigned int i=0; i < trash_size; i++)
    {
        _audioBuffer[_index++]=*buffer++;
        buffer++; //mono
    }

    if (_index >= _windowSize)
    {
        fftData->resize(_fftSize);

        if (_zeroPhaseWindowing)
        {
            // "Zero Phase" windowing
            // Through clever reordering, phase of ouput coefficients will relate to the
            // center of the window
            //_
            // \_                   _/
            //   \                 /
            //    \               /
            //     \_____________/____
            size_t center = (_windowSize - 1) / 2; // position of center bin
            size_t nLeft = _windowSize - center;
            for (size_t i = 0; i < nLeft; ++i)
                (*fftData)[i] = _audioBuffer[center + i] * _windowBuffer[center + i];

            size_t nPadding = _fftSize - _windowSize;
            for (size_t i = 0; i < nPadding; ++i)
                (*fftData)[nLeft + i] = 0.0f;

            for (size_t i = 0; i < center; ++i)
                (*fftData)[nLeft + nPadding + i] = _audioBuffer[i] * _windowBuffer[i];
        }
        else
        {
            // "Normal" windowing
            // Phase of ouput coefficient will relate to the start of the buffer
            //      _
            //    _/ \_
            //   /     \
            //  /       \
            //_/         \____________

            // fill FFT buffer and multiply by window
            for (size_t i = 0; i < _windowSize; ++i)
                (*fftData)[i] = _audioBuffer[i] * _windowBuffer[i];

            // zero-padding
            for (size_t i = _windowSize; i < _fftSize; ++i)
                (*fftData)[i] = 0.0f;
        }

        // perform forward FFT on this slice
        FFT(fftData->data(), _fftSize, FFT_FORWARD);

        // rotate buffer
        {
            size_t const samplesToDrop = _analysisPeriod;
            assert(0 < samplesToDrop && samplesToDrop <= _windowSize);
            size_t remainingSamples = _windowSize - samplesToDrop;

            // TODO: use ring buffer instead of copy
            memmove(_audioBuffer.data(), _audioBuffer.data() + samplesToDrop, sizeof(float) * remainingSamples);
            _index = remainingSamples;

        }
        return true;
    }
    else
    {
        return false;
    }
}
