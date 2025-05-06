/*
  ==============================================================================

    antiAlias.h
    Created: 20 Apr 2025 6:23:08pm
    Author:  jwh93

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "skFilters.h"

class SKAntiAlias
{
public:
    SKAntiAlias();

    ~SKAntiAlias();

    void setup(int blockSamples, double Fs);

    juce::dsp::AudioBlock<float> processUp(juce::dsp::AudioBlock<float> block);
    
    juce::dsp::AudioBlock<float> processDown(juce::dsp::AudioBlock<float> osBlock, juce::dsp::AudioBlock<float> block);

private:
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;
    std::unique_ptr<skFilters> aaFilter;
    std::unique_ptr<skFilters> aaFilterOut;
};
