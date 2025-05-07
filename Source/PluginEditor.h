/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "skClasses/skVibes.h"

//==============================================================================
/**
*/
class PolyAmpAudioProcessorEditor : public juce::AudioProcessorEditor,
                                    public juce::Slider::Listener,
                                    public juce::Button::Listener

{
public:
    PolyAmpAudioProcessorEditor (PolyAmpAudioProcessor&);
    ~PolyAmpAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;
    void updateFrequencyLabel(juce::Slider* slider);

private:

    float buttonSize;
    float knobSize;
    float visualWidth;
    float visualHeight;

    float peakFreqValue, peakBoostValue;
    float voiceFreqValue, voiceBoostValue;

    std::vector<float> unitSize;

    panelParams panel1;
    panelParams panel2;
    panelParams panel3;
    panelParams panel4;

    std::vector<panelParams*> panels
    {
        &panel1,
        &panel2,
        &panel3,
        &panel4
    };

    std::vector<std::vector<int>> panelCols;
    std::vector<juce::Point<float>> panelPos;

    juce::Slider mBoostSlider, mPeakSlider, mBassCutSlider, mGainSlider, mTrebleSlider, mBassSlider, mMidSlider, mVoiceSlider, mCabSlider,mLevelSlider;
    std::vector<juce::Point<float>> knobPos;

    std::vector<juce::Slider*> allKnobs
    {
        &mBassCutSlider,
        &mBoostSlider,
        &mPeakSlider,
        &mTrebleSlider,
        &mMidSlider,
        &mBassSlider,
        &mVoiceSlider,
        &mCabSlider,
        &mGainSlider,
        &mLevelSlider
    };

    std::vector<int> knobCols
    {
        SKVibes::skColours::skLightBlue,
        SKVibes::skColours::skDaisyYellow,
        SKVibes::skColours::skPrincessPink,
        SKVibes::skColours::skLightBlue,
        SKVibes::skColours::skLightBlue,
        SKVibes::skColours::skLightBlue,
        SKVibes::skColours::skPrincessPink,
        SKVibes::skColours::skForestGreen,
        SKVibes::skColours::skBerryPink,
        SKVibes::skColours::skDaisyYellow
        
    };

    juce::TextButton chanButton{ "" }, algorithmButton{ "" }, powerAmpButton{ "" };
    juce::Label boost, peak, basscut, gain, treble, mid, bass, voice, level, cab;

    juce::Button* channel = &chanButton;
    juce::Button* algo = &algorithmButton;
    juce::Button* power = &powerAmpButton;

    juce::Label peakLabel;
    juce::Label voiceLabel;
    juce::Label titleLabel1{ "P O L Y" };
    juce::Label titleLabel2{ "S W I T C H" };

    std::string peakLabelStr;
    std::string voiceLabelStr;

    juce::Point<float> chanPos;
    juce::Point<float> titlePos1;
    juce::Point<float> titlePos2;
    juce::Point<float> algoPos;
    juce::Point<float> powerPos;
    juce::Point<float> peakLabelPos;
    juce::Point<float> voiceLabelPos;

    std::vector<juce::Label*> allLabels
    {
        &basscut,
        &boost,
        &peak,
        &treble,
        &mid,
        &bass,
        &voice,
        &cab,
        &gain,
        &level
    };

    std::vector<juce::String> labelText
    {
        "BASS CUT",
        "BOOST",
        "PEAK",
        "TREBLE",
        "MID",
        "BASS",
        "VOICE",
        "CAB",
        "GAIN",
        "LEVEL",
    };


    visualParams peakVisual;
    visualParams voiceVisual;

    juce::Point<float> peakVisualPos;
    juce::Point<float> voiceVisualPos;

    std::vector<juce::Rectangle<float>> grid;
    std::vector<juce::Rectangle<float>> subGrid;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> boostSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peakSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bassCutSliderAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> trebleSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bassSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voiceSliderAttachment;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cabSliderAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> levelSliderAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> channelButtonAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> algorithmButtonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> powerButtonAttachment;

    SKVibes skVibes;

    PolyAmpAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolyAmpAudioProcessorEditor)
};
