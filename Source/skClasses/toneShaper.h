/*
  ==============================================================================

    toneShaper.h
    Created: 6 May 2025 7:08:45pm
    Author:  jwh93

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "skFilters.h"

struct ToneParams
{
    double trebleGain;
    double bassGain;
    double midGain;

    double trebleFreq;
    double bassFreq;
    double midFreq;

    double inputLPFreq;
    double inputHPFreq;

    double midBoostFreq;
    double midBoostGain;
    
    double cabFilterHPFreq;
    double cabFilterLPFreq;

    double genericQ;

    double sampleRate;
};

class ToneShaper
{
public:
    ToneShaper();

    void setFs(double Fs);

    void update();

    std::shared_ptr<ToneParams> getParams();

    juce::dsp::AudioBlock<float> processPre(juce::dsp::AudioBlock<float> block);

    juce::dsp::AudioBlock<float> processPost(juce::dsp::AudioBlock<float> block);

private:

    std::shared_ptr<ToneParams> params = std::make_shared<ToneParams>();

    std::unique_ptr<skFilters> inputLP = std::make_unique<skFilters>(lowpass);
    std::unique_ptr<skFilters> inputHP = std::make_unique<skFilters>(highpass);
    
    std::unique_ptr<skFilters> midBoost = std::make_unique<skFilters>(peak);

    std::unique_ptr<skFilters> treble = std::make_unique<skFilters>(highshelf);
    std::unique_ptr<skFilters> bass = std::make_unique<skFilters>(lowshelf);
    std::unique_ptr<skFilters> mid = std::make_unique<skFilters>(peak);
    
    std::unique_ptr<skFilters> cabLP = std::make_unique<skFilters>(lowpass);

    std::unique_ptr<skFilters> cabHP = std::make_unique<skFilters>(highpass);
};
