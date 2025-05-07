/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "skClasses/skTriode.h"
#include "skClasses/antiAlias.h"
#include "skClasses/toneShaper.h"

//==============================================================================
/**
*/
class PolyAmpAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    PolyAmpAudioProcessor();
    ~PolyAmpAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void updateParameters();

    juce::dsp::AudioBlock<float> gainScale(juce::dsp::AudioBlock<float> block, double amount, int numChannels);

    juce::AudioProcessorValueTreeState apvts;

private:
    //==============================================================================

    //parameters
    double overdriveGain;
    double volume;    
    bool channel;
    bool compButton;
    bool powerAmpButton;

    //number of triodes you want :)
    int numTriodes = 4;

    //anti-aliasing
    std::unique_ptr<SKAntiAlias> aaf = std::make_unique<SKAntiAlias>();
    
    //triode objects
    std::unique_ptr<SKTriode> triode = std::make_unique<SKTriode>(numTriodes);

    //power amp object
    std::unique_ptr<SKPowerAmp> powerAmp = std::make_unique<SKPowerAmp>();

    //filter objects
    std::unique_ptr<ToneShaper> toneShaper = std::make_unique<ToneShaper>();
    std::shared_ptr<ToneParams> toneparams = std::make_shared<ToneParams>();

    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolyAmpAudioProcessor)
};
