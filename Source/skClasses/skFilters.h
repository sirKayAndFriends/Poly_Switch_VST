/*
  ==============================================================================

    skFilters.h
    Created: 9 Apr 2025 2:16:22pm
    Author:  jwh93

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "math.h"

enum type
{
    highpass,
    lowpass,
    peak,
    highshelf,
    lowshelf
};

class skFilters
{
public:
    skFilters(int type);
    
    //void calc(float& freq, float& gain, float& q, float Fs);
    void calc(double freq, double gain, double q, double Fs);

    float process(float input);

    juce::dsp::AudioBlock<float> processBlock(juce::dsp::AudioBlock<float> block);

private:
    int filterType;
    float b0, b1, b2, a0, a1, a2;
    float z1, z2;
};