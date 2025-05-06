/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PolyAmpAudioProcessorEditor::PolyAmpAudioProcessorEditor (PolyAmpAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (1200, 400);
    setLookAndFeel(&skVibes);

    boostSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "BOOST", mBoostSlider);
    peakSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "PEAK", mPeakSlider);
    bassCutSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "BASSCUT", mBassCutSlider);
    gainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "GAIN", mGainSlider);
    trebleSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "TREBLE", mTrebleSlider);
    bassSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "BASS", mBassSlider);
    midSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "MID", mMidSlider);
    voiceSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "VOICE", mVoiceSlider);
    cabSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "CAB", mCabSlider);
    levelSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "LEVEL", mLevelSlider);

    channelButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.apvts, "CHANNEL", chanButton);
    algorithmButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.apvts, "ALGORITHM", algorithmButton);
    powerButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.apvts, "POWERAMP", powerAmpButton);


    skVibes.createDivisions(getWidth(), getHeight(), 28, 12, 25);
    skVibes.createSubDivisions(4);
    skVibes.getGridCentres();

    unitSize = skVibes.getUnitSize();

    visualWidth = unitSize[0] * 2.0;
    visualHeight = unitSize[1] * 3.0;

    knobSize = 100;
    buttonSize = 50;

    panel1 = skVibes.setupPanels(skVibes.getGridRectTopLeft(0, 0).x, skVibes.getGridRectTopLeft(0, 0).y, unitSize[0] * 7.0, unitSize[1] * 11.0, SKVibes::skBarleyGreen, SKVibes::skPalePurple);
    panel2 = skVibes.setupPanels(skVibes.getGridRectTopLeft(8, 0).x, skVibes.getGridRectTopLeft(8, 0).y, unitSize[0] * 10.0, unitSize[1] * 11.0, SKVibes::skPeach, SKVibes::skBerryPink);
    panel3 = skVibes.setupPanels(skVibes.getGridRectTopLeft(19, 0).x, skVibes.getGridRectTopLeft(19, 0).y, unitSize[0] * 4.0, unitSize[1] * 6.0, SKVibes::skDuckEgg, SKVibes::skBarleyGreen);
    panel4 = skVibes.setupPanels(skVibes.getGridRectTopLeft(24, 5).x, skVibes.getGridRectTopLeft(24, 0).y, unitSize[0] * 4.0, unitSize[1] * 11.0, SKVibes::skMintGreen, SKVibes::skPaleCyan);

    knobPos =
    {
        {juce::Point<float>(skVibes.drawFromEdge(2, 3, knobSize, knobSize))},//bass cut
        {juce::Point<float>(skVibes.drawFromEdge(5, 3, knobSize, knobSize))},//boost
        {juce::Point<float>(skVibes.drawFromEdge(5, 8, knobSize, knobSize))},//peak
        {juce::Point<float>(skVibes.drawFromEdge(10, 3, knobSize, knobSize))},//treble
        {juce::Point<float>(skVibes.drawFromEdge(13, 3, knobSize, knobSize))},//mid
        {juce::Point<float>(skVibes.drawFromEdge(16, 3, knobSize, knobSize))},//bass
        {juce::Point<float>(skVibes.drawFromEdge(13, 8, knobSize, knobSize))},//voice
        {juce::Point<float>(skVibes.drawFromEdge(21, 3, knobSize, knobSize))},//cab
        {juce::Point<float>(skVibes.drawFromEdge(26, 8, knobSize, knobSize))},//gain
        {juce::Point<float>(skVibes.drawFromEdge(26, 3, knobSize, knobSize))}//level
    };

    chanPos = juce::Point<float>(skVibes.drawFromEdge(16, 8, buttonSize, buttonSize)) ;

    for (int i = 0; i < allKnobs.size(); i++)
    {
        allKnobs[i]->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        allKnobs[i]->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        allKnobs[i]->setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(knobCols[i]));

        if (i == 2 || i == 6)
        {
            allKnobs[i]->setDoubleClickReturnValue(true, 0.0);
        }

        else if (i == 0 || i == 7)
        {
            allKnobs[i]->setDoubleClickReturnValue(true, 1.0);
        }

        else
        {
            allKnobs[i]->setDoubleClickReturnValue(true, 0.5);
        }

        allKnobs[i]->addListener(this);
        addAndMakeVisible(allKnobs[i]);

        allLabels[i]->setText(labelText[i], juce::NotificationType::dontSendNotification);
        allLabels[i]->setFont(skVibes.getPluginFont().boldened().withHeight(24.0f));
        allLabels[i]->setJustificationType(juce::Justification::centred);
        allLabels[i]->setColour(juce::Label::ColourIds::textColourId, juce::Colour(juce::Colours::black));
        addAndMakeVisible(allLabels[i]);
    }

    channel->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour(SKVibes::skDaisyYellow));
    channel->setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(SKVibes::skTomato));
    channel->setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colour(juce::Colours::black));
    channel->setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colour(juce::Colours::black));
    channel->setColour(juce::ComboBox::outlineColourId, juce::Colour(SKVibes::skClear));
    channel->setClickingTogglesState(true);
    addAndMakeVisible(channel);

    algo->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour(SKVibes::skClear));
    algo->setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(SKVibes::skClear));
    algo->setColour(juce::ComboBox::outlineColourId, juce::Colour(SKVibes::skClear));
    algo->setClickingTogglesState(true);
    algo->addListener(this);
    addAndMakeVisible(algo);

    power->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour(SKVibes::skClear));
    power->setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(SKVibes::skClear));
    power->setColour(juce::ComboBox::outlineColourId, juce::Colour(SKVibes::skClear));
    power->setClickingTogglesState(true);
    power->addListener(this);
    addAndMakeVisible(power);

    peakVisualPos = juce::Point<float>(skVibes.getGridRectTopLeft(1, 8));
    voiceVisualPos = juce::Point<float>(skVibes.getGridRectTopLeft(9, 8));
    peakVisual = skVibes.setVisualParams(peakVisualPos.x, peakVisualPos.y, visualWidth, visualHeight);
    voiceVisual = skVibes.setVisualParams(voiceVisualPos.x, voiceVisualPos.y, visualWidth, visualHeight);

    peakLabelPos = juce::Point<float>(skVibes.drawFromEdge(2, 10, knobSize, 35));
    addAndMakeVisible(peakLabel);
    peakLabel.setFont(skVibes.getPluginFont().boldened().withHeight(24.0f));
    peakLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colour(juce::Colours::black));
    peakLabel.setJustificationType(juce::Justification::centred);

    voiceLabelPos = juce::Point<float>(skVibes.drawFromEdge(10, 10, knobSize, 35));
    addAndMakeVisible(voiceLabel);
    voiceLabel.setFont(skVibes.getPluginFont().boldened().withHeight(24.0f));
    voiceLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colour(juce::Colours::black));
    voiceLabel.setJustificationType(juce::Justification::centred);

    titlePos1 = juce::Point<float>(skVibes.drawFromEdge(21, 7, unitSize[0] * 6.0f, unitSize[1]).x, skVibes.drawFromCentre(21, 7, unitSize[0] * 6.0f, unitSize[1] * 3.0).y);
    titleLabel1.setText("P O L Y", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(titleLabel1);
    titleLabel1.setFont(skVibes.getPluginFont().boldened().withHeight(38.0f));
    titleLabel1.setJustificationType(juce::Justification::centred);

    titlePos2 = juce::Point<float>(skVibes.drawFromEdge(21, 9, unitSize[0] * 6.0f, unitSize[1]).x, skVibes.drawFromCentre(21, 9, unitSize[0] * 6.0f, unitSize[1] * 3.0).y);
    titleLabel2.setText("S W I T C H", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(titleLabel2);
    titleLabel2.setFont(skVibes.getPluginFont().boldened().withHeight(38.0f));
    titleLabel2.setJustificationType(juce::Justification::centred);

    algoPos = juce::Point<float>(titlePos2.x, skVibes.drawFromCentre(21, 9, unitSize[0] * 6.0f, unitSize[1] * 2.0).y);
    powerPos = juce::Point<float>(titlePos1.x, skVibes.drawFromCentre(21, 7, unitSize[0] * 6.0f, unitSize[1] * 2.0).y);

    updateFrequencyLabel(allKnobs[1]);
    updateFrequencyLabel(allKnobs[2]);
    updateFrequencyLabel(allKnobs[4]);
    updateFrequencyLabel(allKnobs[6]);
}

PolyAmpAudioProcessorEditor::~PolyAmpAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void PolyAmpAudioProcessorEditor::paint (juce::Graphics& g)
{
    skVibes.drawPanel(g, 0.0, 0.0, getWidth(), getHeight(), SKVibes::skLightBlue);

    for (int i = 0; i < panels.size(); i++)
    {
        skVibes.drawDoublePanel(g, panels[i]);
    };

    for (int i = 0; i < knobPos.size(); i++)
    {
        allKnobs[i]->setBounds(knobPos[i].x, knobPos[i].y, knobSize, knobSize);
        allLabels[i]->setBounds(knobPos[i].x, knobPos[i].y + knobSize, knobSize, 25);
    }

    channel->setBounds(chanPos.x, chanPos.y, buttonSize, buttonSize);

    if (channel->getToggleState() == false)
    {
        channel->setButtonText("LESS");
    }
    else
    {
        channel->setButtonText("MORE");
    }

    if (algo->getToggleState())
    {
        titleLabel2.setColour(juce::Label::ColourIds::textColourId, juce::Colour(juce::Colours::black));
    } 
    else
    {
        titleLabel2.setColour(juce::Label::ColourIds::textColourId, juce::Colour(juce::Colours::white));

    }

    if (power->getToggleState())
    {
        titleLabel1.setColour(juce::Label::ColourIds::textColourId, juce::Colour(juce::Colours::black));
    }
    else
    {
        titleLabel1.setColour(juce::Label::ColourIds::textColourId, juce::Colour(juce::Colours::white));

    }

    skVibes.drawMidVisual(g, &peakVisual, peakFreqValue, peakBoostValue);
    skVibes.drawMidVisual(g, &voiceVisual, voiceFreqValue, voiceBoostValue);

    peakLabel.setBounds(peakLabelPos.x, peakLabelPos.y, knobSize, 35);
    peakLabel.setText(peakLabelStr, juce::dontSendNotification);

    voiceLabel.setBounds(voiceLabelPos.x, voiceLabelPos.y, knobSize, 35);
    voiceLabel.setText(voiceLabelStr, juce::dontSendNotification);

    titleLabel1.setBounds(titlePos1.x, titlePos1.y, unitSize[0] * 6.0f, unitSize[1] * 3.0f);
    titleLabel2.setBounds(titlePos2.x, titlePos2.y, unitSize[0] * 6.0f, unitSize[1] * 3.0f);
    titleLabel1.toBehind(power);
    titleLabel2.toBehind(algo);
    algo->setBounds(algoPos.x, algoPos.y, unitSize[0] * 6.0f, unitSize[1] * 2.0f);
    power->setBounds(powerPos.x, powerPos.y, unitSize[0] * 6.0f, unitSize[1] * 2.0f);
}

void PolyAmpAudioProcessorEditor::resized()
{
}

void PolyAmpAudioProcessorEditor::updateFrequencyLabel(juce::Slider* slider)
{
    if (slider == &mPeakSlider)
    {
        double sliderVal = slider->getValue();

        //scale to Hz values and send to label
        double scaled = (sliderVal * 2100) + 400;
        int scaledVal = juce::roundToInt(scaled);
        std::string hz = "Hz";
        peakLabelStr = std::to_string(scaledVal) + hz;

        peakFreqValue = peakVisual.controlMin + (sliderVal * peakVisual.controlWidth);
        repaint();
    }

    if (slider == &mBoostSlider)
    {
        double sliderVal = (slider->getValue() * 2.0) - 1.0;
        peakBoostValue = peakVisual.lineY - (sliderVal * peakVisual.humpHeight);
        repaint();
    }

    if (slider == &mVoiceSlider)
    {
        double sliderVal = slider->getValue();

        //scale to Hz values and send to label
        double scaled = (sliderVal * 450) + 400;
        int scaledVal = juce::roundToInt(scaled);
        std::string hz = "Hz";
        voiceLabelStr = std::to_string(scaledVal) + hz;

        voiceFreqValue = voiceVisual.controlMin + (sliderVal * voiceVisual.controlWidth);
        repaint();
    }

    if (slider == &mMidSlider)
    {
        double sliderVal = (slider->getValue() * 2.0) - 1.0;
        voiceBoostValue = voiceVisual.lineY - (sliderVal * voiceVisual.humpHeight);
        repaint();
    }
}

void PolyAmpAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    updateFrequencyLabel(slider);
}

void PolyAmpAudioProcessorEditor::buttonClicked(juce::Button* button)
{

}
