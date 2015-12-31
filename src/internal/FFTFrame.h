/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FFTFrame_h
#define FFTFrame_h

#include "LabSound/core/AudioArray.h"

#include "internal/ConfigMacros.h"

#include <vector>

#if OS(DARWIN) && !USE(WEBAUDIO_KISSFFT)
#define USE_ACCELERATE_FFT 1
#else
#define USE_ACCELERATE_FFT 0
#endif

#if USE_ACCELERATE_FFT
#include <Accelerate/Accelerate.h>
#endif // !USE_ACCELERATE_FFT

#if USE(WEBAUDIO_KISSFFT)
#include <kissfft/kiss_fft.hpp>
#include <kissfft/kiss_fftr.hpp>
#endif // USE(WEBAUDIO_KISSFFT)

namespace lab 
{

// Defines the interface for an "FFT frame", an object which is able to perform a forward
// and reverse FFT, internally storing the resultant frequency-domain data.
class FFTFrame 
{

public:

	FFTFrame();	// creates a blank/empty frame for later use with createInterpolatedFrame()
	FFTFrame(uint32_t fftSize);

	 // Copy
    FFTFrame(const FFTFrame& frame);
    ~FFTFrame();
    
    void doFFT(const float* data);
    void doInverseFFT(float* data);
    void multiply(const FFTFrame& frame); // multiplies ourself with frame : effectively operator*=()

    float* realData() const;
    float* imagData() const;

    void print(); // for debugging

    // The remaining public methods have cross-platform implementations:

    // Interpolates from frame1 -> frame2 as x goes from 0.0 -> 1.0
    static std::unique_ptr<FFTFrame> createInterpolatedFrame(const FFTFrame& frame1, const FFTFrame& frame2, double x);

    void doPaddedFFT(const float* data, size_t dataSize); // zero-padding with dataSize <= fftSize
    double extractAverageGroupDelay();
    void addConstantGroupDelay(double sampleFrameDelay);

    unsigned fftSize() const { return m_FFTSize; }
    unsigned log2FFTSize() const { return m_log2FFTSize; }
    
#if USE_ACCELERATE_FFT
    static void cleanup();
#endif
    
private:
    
    unsigned m_FFTSize;
    unsigned m_log2FFTSize;

    void interpolateFrequencyComponents(const FFTFrame& frame1, const FFTFrame& frame2, double x);

#if USE_ACCELERATE_FFT
    DSPSplitComplex& dspSplitComplex() { return m_frame; }
    DSPSplitComplex dspSplitComplex() const { return m_frame; }

    static FFTSetup fftSetupForSize(unsigned fftSize);

    static FFTSetup* fftSetups;

    FFTSetup m_FFTSetup;

    DSPSplitComplex m_frame;
    AudioFloatArray m_realData;
    AudioFloatArray m_imagData;
#else // !USE_ACCELERATE_FFT
    
#if USE(WEBAUDIO_KISSFFT)
    kiss_fftr_cfg mFFT;
    kiss_fftr_cfg mIFFT;

    kiss_fft_cpx* m_cpxInputData;
    kiss_fft_cpx* m_cpxOutputData;

	std::vector<kiss_fft_cpx> mOutputBuffer; 

    AudioFloatArray m_realData;
    AudioFloatArray m_imagData;
#endif

#endif // !USE_ACCELERATE_FFT
};

} // namespace lab

#endif // FFTFrame_h
