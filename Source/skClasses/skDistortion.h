/*
  ==============================================================================

    distortion.h
    Created: 8 Apr 2025 6:29:22pm
    Author:  jwh93

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

enum mode
{
    soft_clip,
    hard_clip,
    half_wave_rect,
    fuzz,
    tangent,
    triode
};

class SKDistortion
{
public:
    SKDistortion(int setting);

    void setGain(double amount);

    void setMode(int setting);
    
    juce::dsp::AudioBlock<float> clipBlock(juce::dsp::AudioBlock<float> block, int numChannels);

    void setClipThreshold(double forward, double reverse);

    float triodeClip(float input);

private:

    int clipMode;
    double forwardClip, reverseClip;
    double gain;
};

