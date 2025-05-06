/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "skClasses/skTriode.h"
#include "skClasses/skDistortion.h"
#include "skClasses/antiAlias.h"

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
    double boostFreq;
    double boostGain;
    double bassCut;
    double overdriveGain;
    double trebleGain;
    double bassGain;
    double midGain;
    double midFreq;
    double cabFreq;
    double volume;    
    bool channel;
    bool algo;
    bool power;

    //anti-aliasing
    std::unique_ptr<SKAntiAlias> aaf;
    
    //triode objects
    std::unique_ptr<SKTriode> triode1;
    std::unique_ptr<SKTriode> triode2;
    std::unique_ptr<SKTriode> triode3;
    std::unique_ptr<SKTriode> triode4;

    //SK distortion algorithm
    std::unique_ptr<SKDistortion> skOverdrive;

    //power amp object
    std::unique_ptr<SKPowerAmp> powerAmp;

    //filter objects
    std::unique_ptr<skFilters> midBoost;
    std::unique_ptr<skFilters> inputFilterHP;
    std::unique_ptr<skFilters> inputFilterLP;
    std::unique_ptr<skFilters> postDriveFilterLP;
    std::unique_ptr<skFilters> trebleFilterHS;
    std::unique_ptr<skFilters> bassFilterLS;
    std::unique_ptr<skFilters> midFilterEQ;
    std::unique_ptr<skFilters> cabFilterLP;
    std::unique_ptr<skFilters> cabFilterHP;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolyAmpAudioProcessor)
};
