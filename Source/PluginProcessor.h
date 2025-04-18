/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "skClasses/skTriode.h"
#include "skClasses/skDistortion.h"

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

    void updateAAF();

    void updateParameters();

    juce::dsp::AudioBlock<float> gainScale(juce::dsp::AudioBlock<float> block, int numChannels);

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
    
    //triode objects
    SKTriode* triode1 = new SKTriode();
    SKTriode* triode2 = new SKTriode();
    SKTriode* triode3 = new SKTriode();
    SKTriode* triode4 = new SKTriode();

    //SK distortion algorithm
    SKDistortion* skOverdrive = new SKDistortion(tangent);

    //power amp object
    SKPowerAmp* powerAmp = new SKPowerAmp();

    //filter objects
    skFilters* midBoost = new skFilters(peak);
    skFilters* inputFilterHP = new skFilters(highpass);
    skFilters* inputFilterLP = new skFilters(lowpass);
    skFilters* postDriveFilterLP = new skFilters(lowpass);
    skFilters* trebleFilterHS = new skFilters(highshelf);
    skFilters* bassFilterLS = new skFilters(lowshelf);
    skFilters* midFilterEQ = new skFilters(peak);
    skFilters* cabFilterLP = new skFilters(lowpass);
    skFilters* cabFilterHP = new skFilters(highpass);

    juce::dsp::Oversampling<float> oversampler;

    juce::dsp::ProcessorDuplicator<juce::dsp::FIR::Filter<float>, juce::dsp::FIR::Coefficients<float>> aafilterIn;
    juce::dsp::ProcessorDuplicator<juce::dsp::FIR::Filter<float>, juce::dsp::FIR::Coefficients<float>> aafilterOut;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolyAmpAudioProcessor)
};
