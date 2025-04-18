/*
  ==============================================================================

    skTriode.h
    Created: 15 Apr 2025 3:22:16pm
    Author:  jwh93

  ==============================================================================
*/

#pragma once

/*
  ==============================================================================

    distortion.h
    Created: 8 Apr 2025 6:29:22pm
    Author:  jwh93

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "math.h"
#include "skFilters.h"
#include "pirkleZVA.h"

struct TriodeParameters
{
    double lfShelfFreq;
    double lfShelfGain;

    double dcBlockFreq;

    double millerFreq;

    double integratorFc;

    double clipPointPositive;
    double clipPointNegative;
    double gridThresh;

    double dcShiftCoeff;
    double waveSaturation;

    double inputGain;
    double outputGain;

    double amount;
};

struct powerAmpParameters
{
    double inputGain;
    double outputGain;
    
    double fixedBiasVoltage;
    double clipPointPositive;
    
    double dcShiftCoeff;
    double waveSaturation;

    //waveshaper
    double asymWaveShaper_g;
    double asymWaveShaper_Lp;
    double asymWaveShaper_Ln;

    double symWaveShaper_g;
    double symWaveShaper_LpLn;
};

class SKTriode
{
public:
    SKTriode();

    void setGain(double amt);

    juce::dsp::AudioBlock<float> clipBlock(juce::dsp::AudioBlock<float> block, int numChannels);

    void reset(double Fs);

    double gridConduction(double input, double thresh);

    double valveEmulation(double input, double k, double gridConductionThresh, double dcOffset, double clipPointPos, double clipPointNeg);

private:
    double amount;

    TriodeParameters params;

    ZVAFilter lossyIntegrator[2];

    skFilters* lowShelvingFilter = new skFilters(lowshelf);
    skFilters* upperBandwidthFilter = new skFilters(lowpass);

    double sampleRate = 44100.0;
};

class SKPowerAmp
{
public:
    SKPowerAmp();

    void reset(double Fs);

    juce::dsp::AudioBlock<float> processBlock(juce::dsp::AudioBlock<float> block, int numChannels);

    double doPirkleWaveshaper(double input, double g, double fixedDC, double variableDC);
    double doPolettiWaveShaper(double input, double g, double Ln, double Lp);

    double gridConduction(double input, double thresh);

private:

    powerAmpParameters params;
    ZVAFilter lossyIntegrator[2];

    double sampleRate;
};


