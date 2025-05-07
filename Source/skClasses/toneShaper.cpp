/*
  ==============================================================================

    toneShaper.cpp
    Created: 6 May 2025 7:08:52pm
    Author:  jwh93

  ==============================================================================
*/

#include "toneShaper.h"

ToneShaper::ToneShaper()
{
    params->inputLPFreq = 3500.0;
    params->inputHPFreq = 25.0;

    params->cabFilterHPFreq = 100.0;
    params->cabFilterLPFreq = 2000.0;

    params->midBoostFreq = 400.0;
    params->midBoostGain = 1.0;

    params->trebleGain = 1.0;
    params->bassGain = 1.0;
    params->midGain = 1.0;

    params->trebleFreq = 2000.0;
    params->bassFreq = 250.0;
    params->midFreq = 400.0;

    params->genericQ = 0.707;

    params->sampleRate = 44100.0;

    update();
}

void ToneShaper::setFs(double Fs)
{
    params->sampleRate = Fs;
    update();
}


void ToneShaper::update()
{
    inputLP->calc(params->cabFilterLPFreq, 1.0, params->genericQ, params->sampleRate);
    inputHP->calc(params->cabFilterHPFreq, 1.0, params->genericQ, params->sampleRate);

    midBoost->calc(params->midBoostFreq, params->midBoostGain, params->genericQ, params->sampleRate);

    treble->calc(params->trebleFreq, params->trebleGain, 1.0, params->sampleRate);
    bass->calc(params->bassFreq, params->bassGain, 1.0, params->sampleRate);
    mid->calc(params->midFreq, params->midGain, params->genericQ, params->sampleRate);

    cabLP->calc(params->cabFilterLPFreq, 1.0, params->genericQ, params->sampleRate);
    cabHP->calc(params->cabFilterHPFreq, 1.0, params->genericQ, params->sampleRate);
}

std::shared_ptr<ToneParams> ToneShaper::getParams()
{
    return params;
}

juce::dsp::AudioBlock<float> ToneShaper::processPre(juce::dsp::AudioBlock<float> block)
{
    block = inputHP->processBlock(block);

    block = inputLP->processBlock(block);

    block = midBoost->processBlock(block);

    return block;
}

juce::dsp::AudioBlock<float> ToneShaper::processPost(juce::dsp::AudioBlock<float> block)
{
    block = bass->processBlock(block);

    block = treble->processBlock(block);

    block = mid->processBlock(block);

    block = cabLP->processBlock(block);

    block = cabHP->processBlock(block);

    return block;
}
