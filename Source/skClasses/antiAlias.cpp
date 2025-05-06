/*
  ==============================================================================

    antiAlias.cpp
    Created: 20 Apr 2025 6:22:59pm
    Author:  jwh93

  ==============================================================================
*/

#include "antiAlias.h"

SKAntiAlias::SKAntiAlias()
{
    oversampler = std::make_unique<juce::dsp::Oversampling<float>>(2, 4, juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR, true, false);
    aaFilter = std::make_unique<skFilters>(lowpass);
    aaFilterOut = std::make_unique<skFilters>(lowpass);
}

SKAntiAlias::~SKAntiAlias()
{
    oversampler = { nullptr };
}


void SKAntiAlias::setup(int blockSamples, double Fs)
{
    oversampler->reset();
    oversampler->initProcessing(blockSamples);
    aaFilter->calc(Fs / 4.0, 1.0, 0.707, Fs * 4.0);
    aaFilterOut->calc(Fs / 4.0, 1.0, 0.707, Fs * 4.0);
}

juce::dsp::AudioBlock<float> SKAntiAlias::processUp(juce::dsp::AudioBlock<float> block)
{
    juce::dsp::AudioBlock<float> osBlock = oversampler->processSamplesUp(block);

    osBlock = aaFilter->processBlock(osBlock, 2);

    return osBlock;
}

juce::dsp::AudioBlock<float> SKAntiAlias::processDown(juce::dsp::AudioBlock<float> osBlock, juce::dsp::AudioBlock<float> block)
{
    osBlock = aaFilterOut->processBlock(osBlock, 2);

    oversampler->processSamplesDown(block);

    return block;
}
