/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
PolyAmpAudioProcessor::PolyAmpAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
    aaf = std::make_unique<SKAntiAlias>();

    triode1 = std::make_unique<SKTriode>();
    triode2 = std::make_unique<SKTriode>();
    triode3= std::make_unique<SKTriode>();
    triode4 = std::make_unique<SKTriode>();

    skOverdrive = std::make_unique<SKDistortion>(tangent);

    powerAmp = std::make_unique<SKPowerAmp>();

    midBoost = std::make_unique<skFilters>(peak);
    inputFilterHP = std::make_unique<skFilters>(highpass);
    inputFilterLP = std::make_unique<skFilters>(lowpass);
    postDriveFilterLP = std::make_unique<skFilters>(lowpass);
    trebleFilterHS = std::make_unique<skFilters>(highshelf);
    bassFilterLS = std::make_unique<skFilters>(lowshelf);
    midFilterEQ = std::make_unique<skFilters>(peak);
    cabFilterLP = std::make_unique<skFilters>(lowpass);
    cabFilterHP = std::make_unique<skFilters>(highpass);
}

PolyAmpAudioProcessor::~PolyAmpAudioProcessor()
{
}

//==============================================================================
const juce::String PolyAmpAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PolyAmpAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PolyAmpAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PolyAmpAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PolyAmpAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PolyAmpAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PolyAmpAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PolyAmpAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PolyAmpAudioProcessor::getProgramName (int index)
{
    return {};
}

void PolyAmpAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PolyAmpAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();

    double overSampleRate = getSampleRate() * 4.0;

    aaf->setup(samplesPerBlock, getSampleRate());

    triode1->reset(overSampleRate);
    triode2->reset(overSampleRate);
    triode3->reset(overSampleRate);
    triode4->reset(overSampleRate);

    powerAmp->reset(overSampleRate);
   
    inputFilterLP->calc(3500.0, 1.0, 0.707, getSampleRate());
    postDriveFilterLP->calc(2000.0, 1.0, 0.707, getSampleRate());
    cabFilterHP->calc(25.0, 1.0, 0.707, getSampleRate());
    
    updateParameters();
}

void PolyAmpAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PolyAmpAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PolyAmpAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    buffer.copyFrom(1, 0, buffer.getReadPointer(0), buffer.getNumSamples());
    auto inputBlock = juce::dsp::AudioBlock<float>(buffer);
    auto overBlock = juce::dsp::AudioBlock<float>(buffer);

    updateParameters();

    inputBlock = inputFilterHP->processBlock(inputBlock, 2);

    inputBlock = inputFilterLP->processBlock(inputBlock, 2);

    inputBlock = midBoost->processBlock(inputBlock, 2);

    overBlock = aaf->processUp(inputBlock);

    if (algo)
    {
        if(channel)
        { 
            skOverdrive->setMode(triode);
        }
        else
        {
            skOverdrive->setMode(tangent);
            
        }
        overBlock = skOverdrive->clipBlock(overBlock, 2);
    }

    else
    {
        overBlock = triode1->clipBlock(overBlock, 2);
        overBlock = triode2->clipBlock(overBlock, 2);
        overBlock = triode3->clipBlock(overBlock, 2);
        overBlock = triode4->clipBlock(overBlock, 2);
    }

    if (power)
    {
        overBlock = powerAmp->processBlock(overBlock, 2);
    }

    inputBlock = aaf->processDown(overBlock, inputBlock);

    inputBlock = postDriveFilterLP->processBlock(inputBlock, 2);

    inputBlock = bassFilterLS->processBlock(inputBlock, 2);
    inputBlock = trebleFilterHS->processBlock(inputBlock, 2);

    inputBlock = midFilterEQ->processBlock(inputBlock, 2);

    inputBlock = cabFilterLP->processBlock(inputBlock, 2);
    inputBlock = cabFilterHP->processBlock(inputBlock, 2);

    inputBlock = gainScale(inputBlock, volume, 2);
}

juce::dsp::AudioBlock<float> PolyAmpAudioProcessor::gainScale(juce::dsp::AudioBlock<float> block, double gain, int numChannels)
{
    for (int sample = 0; sample < block.getNumSamples(); sample++)
    {
        for (int channel = 0; channel < numChannels; channel++)
        {
            float* data = block.getChannelPointer(channel);

            data[sample] *= 0.25f;

            data[sample] *= gain;
        }
    }
    return block;
}

void PolyAmpAudioProcessor::updateParameters()
{
    boostFreq = apvts.getRawParameterValue("PEAK")->load() * 2100.0 + 400.0;
    boostGain = apvts.getRawParameterValue("BOOST")->load() * 24.0 - 12.0;
    bassCut = 300.0 - (apvts.getRawParameterValue("BASSCUT")->load() * 275.0 + 25.0);
   
    overdriveGain = apvts.getRawParameterValue("GAIN")->load();
    
    trebleGain = apvts.getRawParameterValue("TREBLE")->load() * 24.0 - 24.0;
    bassGain = apvts.getRawParameterValue("BASS")->load() * 24.0 - 12.0;
    midFreq = apvts.getRawParameterValue("VOICE")->load() * 450 + 400.0;
    midGain = apvts.getRawParameterValue("MID")->load() * 24.0 - 12.0;

    cabFreq = apvts.getRawParameterValue("CAB")->load() * 3000.0 + 1000.0;
 
    volume = apvts.getRawParameterValue("LEVEL")->load();

    channel = apvts.getRawParameterValue("CHANNEL")->load();
    algo = apvts.getRawParameterValue("ALGORITHM")->load();
    power = apvts.getRawParameterValue("POWERAMP")->load();

    midBoost->calc(boostFreq, boostGain, 0.707, getSampleRate());
    inputFilterHP->calc(bassCut, 1.0, 0.707, getSampleRate());

    trebleFilterHS->calc(2000.0, trebleGain, 1.0, getSampleRate());
    bassFilterLS->calc(250.0, bassGain, 1.0, getSampleRate());
    midFilterEQ->calc(midFreq, midGain, 0.707, getSampleRate());
    cabFilterLP->calc(cabFreq, 1.0, 0.707, getSampleRate());

    triode1->setGain(overdriveGain * 5.0);
    triode2->setGain(overdriveGain * 2.0);

    triode1->setSaturation(channel);
    triode2->setSaturation(channel);
    triode3->setSaturation(channel);
    triode4->setSaturation(channel);

    skOverdrive->setGain(overdriveGain * 25.0);
}

juce::AudioProcessorValueTreeState::ParameterLayout PolyAmpAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("BOOST", "boost", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PEAK", "peak", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("BASSCUT", "basscut", 0.0f, 1.0f, 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("TREBLE", "treble", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("BASS", "bass", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("MID", "mid", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("VOICE", "voice", 0.0f, 1.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("CAB", "cab", 0.0f, 1.0f, 1.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN", "gain", 0.0f, 1.0f, 0.25f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LEVEL", "level", 0.0f, 1.0f, 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterBool>("CHANNEL", "channel", false));

    params.push_back(std::make_unique<juce::AudioParameterBool>("ALGORITHM", "algorithm", false));

    params.push_back(std::make_unique<juce::AudioParameterBool>("POWERAMP", "poweramp", true));

    return { params.begin(), params.end() };
}

//==============================================================================
bool PolyAmpAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PolyAmpAudioProcessor::createEditor()
{
    return new PolyAmpAudioProcessorEditor (*this);
}

//==============================================================================
void PolyAmpAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PolyAmpAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PolyAmpAudioProcessor();
}
