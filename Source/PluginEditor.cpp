/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

//==============================================================================
UniversalCombFilterAudioProcessorEditor::UniversalCombFilterAudioProcessorEditor (UniversalCombFilterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    /* delay */
    addAndMakeVisible(delaySlider);
    delaySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    delaySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 60, 20);
    delaySlider.setColour(Slider::textBoxTextColourId, Colours::black);
    delaySlider.setTextBoxIsEditable(true);
    delaySlider.setRange(0, 500, 1);
    delaySlider.setValue(0);
    delaySlider.setTextValueSuffix(" ms");
    delaySlider.addListener(this);
    // label
    addAndMakeVisible(delayLabel);
    delayLabel.setText("min. delay", juce::dontSendNotification);
    delayLabel.setJustificationType(juce::Justification::centred);
    delayLabel.attachToComponent(&delaySlider, false);
    
    /* sweep width */
    addAndMakeVisible(widthSlider);
    widthSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    widthSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 60, 20);
    widthSlider.setColour(Slider::textBoxTextColourId, Colours::black);
    widthSlider.setTextBoxIsEditable(true);
    widthSlider.setRange(0.0f, 5.0f, 0.01f); // maxSweepWidth = 5 ms
    widthSlider.setValue(2.0f); // TODO: is this intialized to 0.0003 in slidervaluechanged? probably not...
    widthSlider.setTextValueSuffix(" ms");
    widthSlider.addListener(this);
    // label
    addAndMakeVisible(widthLabel);
    widthLabel.setText("mod depth", juce::dontSendNotification);
    widthLabel.setJustificationType(juce::Justification::centred);
    widthLabel.attachToComponent(&widthSlider, false);
    
    /* lfo frequency */
    addAndMakeVisible(freqSlider);
    freqSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    freqSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 60, 20);
    freqSlider.setColour(Slider::textBoxTextColourId, Colours::black);
    freqSlider.setTextBoxIsEditable(true);
    freqSlider.setRange(0.0f, 250.0f, 0.01f);
    freqSlider.setValue(0.5f);
    freqSlider.setTextValueSuffix(" Hz");
    freqSlider.addListener(this);
    // label
    addAndMakeVisible(freqLabel);
    freqLabel.setText("lfo frequency", juce::dontSendNotification);
    freqLabel.setJustificationType(juce::Justification::centred);
    freqLabel.attachToComponent(&freqSlider, false);
    
    /* bleed */
    addAndMakeVisible(bleedSlider);
    bleedSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    bleedSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 60, 20);
    bleedSlider.setColour(Slider::textBoxTextColourId, Colours::black);
    bleedSlider.setTextBoxIsEditable(true);
    bleedSlider.setRange(0.0f, 1.0f, 0.01f);
    bleedSlider.setValue(0.7f);
    bleedSlider.addListener(this);
    // label
    addAndMakeVisible(bleedLabel);
    bleedLabel.setText("bleed", juce::dontSendNotification);
    bleedLabel.setJustificationType(juce::Justification::centred);
    bleedLabel.attachToComponent(&bleedSlider, false);
    
    /* feedforward */
    addAndMakeVisible(feedforwardSlider);
    feedforwardSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    feedforwardSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 60, 20);
    feedforwardSlider.setColour(Slider::textBoxTextColourId, Colours::black);
    feedforwardSlider.setTextBoxIsEditable(true);
    feedforwardSlider.setRange(0.0f, 1.0f, 0.01f);
    feedforwardSlider.setValue(0.7f);
    feedforwardSlider.addListener(this);
    // label
    addAndMakeVisible(feedforwardLabel);
    feedforwardLabel.setText("feedforward", juce::dontSendNotification);
    feedforwardLabel.setJustificationType(juce::Justification::centred);
    feedforwardLabel.attachToComponent(&feedforwardSlider, false);
    
    /* feedback */
    addAndMakeVisible(feedbackSlider);
    feedbackSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    feedbackSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 60, 20);
    feedbackSlider.setColour(Slider::textBoxTextColourId, Colours::black);
    feedbackSlider.setTextBoxIsEditable(true);
    feedbackSlider.setRange(0.0f, 1.0f, 0.01f);
    feedbackSlider.setValue(0.7f);
    feedbackSlider.addListener(this);
    // label
    addAndMakeVisible(feedbackLabel);
    feedbackLabel.setText("feedback", dontSendNotification);
    feedbackLabel.setJustificationType(Justification::centred);
    feedbackLabel.attachToComponent(&feedbackSlider, false);
    
    addAndMakeVisible(tremoloToggle);
    tremoloToggle.setButtonText("(+ tremolo)");
    tremoloToggle.addListener(this);
    
    addAndMakeVisible(inputLabel);
    inputLabel.setText("x[n]", dontSendNotification);
    inputLabel.setJustificationType(Justification::centred);
    
    addAndMakeVisible(outputLabel);
    outputLabel.setText("y[n]", dontSendNotification);
    outputLabel.setJustificationType(Justification::centred);
    
    addAndMakeVisible(title);
    title.setText("Universal Comb Filter", dontSendNotification);
    title.setJustificationType(Justification::centred);
    title.setFont(Font("Helvetica", 32, Font::FontStyleFlags::italic));
    
    getLookAndFeel().setColour(Label::textColourId, Colours::black);
    getLookAndFeel().setColour(Slider::textBoxTextColourId, Colours::black);
    getLookAndFeel().setColour(Slider::textBoxOutlineColourId, Colours::transparentWhite);
    getLookAndFeel().setColour(Slider::rotarySliderFillColourId, Colours::orange);
    getLookAndFeel().setColour(Slider::trackColourId, Colours::black);
    getLookAndFeel().setColour(Slider::thumbColourId, Colours::transparentBlack);
    getLookAndFeel().setColour(ToggleButton::textColourId, Colours::black);
    getLookAndFeel().setColour(ToggleButton::tickColourId, Colours::black);
    getLookAndFeel().setColour(ToggleButton::tickDisabledColourId, Colours::black);
    
    setSize(800, 600);
}

UniversalCombFilterAudioProcessorEditor::~UniversalCombFilterAudioProcessorEditor()
{
}

//==============================================================================
void UniversalCombFilterAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(Colours::whitesmoke);
    
    g.setColour(Colours::black);
    g.drawRect(getWidth()/2-120, getHeight()/2-120, 240, 240);
    
    g.drawLine(getWidth()/2-296, getHeight()/2, getWidth()/2-264, getHeight()/2); // input
    g.drawLine(getWidth()/2-240, getHeight()/2, getWidth()/2-120, getHeight()/2);
    g.drawLine(getWidth()/2-180, getHeight()/2, getWidth()/2-180, getHeight()/2-200);
    g.drawEllipse(getWidth()/2-182, getHeight()/2-2, 4, 4, 4);
    drawSum(g, getWidth()/2-252, getHeight()/2);
    
    // feedback line
    g.drawLine(getWidth()/2-252, getHeight()/2+12, getWidth()/2-252, getHeight()/2+200);
    g.drawLine(getWidth()/2-252, getHeight()/2+200, getWidth()/2-146, getHeight()/2+200);
    g.drawLine(getWidth()/2-106, getHeight()/2+200, getWidth()/2, getHeight()/2+200);
    g.drawLine(getWidth()/2, getHeight()/2+120, getWidth()/2, getHeight()/2+200);
    
    // feedforward line
    g.drawLine(getWidth()/2+120, getHeight()/2, getWidth()/2+160, getHeight()/2);
    g.drawLine(getWidth()/2+200, getHeight()/2, getWidth()/2+240, getHeight()/2);
    drawSum(g, getWidth()/2+252, getHeight()/2);
    
    // bleed line
//    g.drawLine(getWidth()/2-180, getHeight()/2-200, getWidth()/2+252, getHeight()/2-200); // centre: 36
    g.drawLine(getWidth()/2-180, getHeight()/2-200, getWidth()/2+16, getHeight()/2-200);
    g.drawLine(getWidth()/2+56, getHeight()/2-200, getWidth()/2+252, getHeight()/2-200);
    g.drawLine(getWidth()/2+252, getHeight()/2-200, getWidth()/2+252, getHeight()/2-12);
    
    g.drawLine(getWidth()/2+264, getHeight()/2, getWidth()/2+294, getHeight()/2); // output
}

void UniversalCombFilterAudioProcessorEditor::resized()
{
    delaySlider.setBounds(getWidth()/2-40, getHeight()/2-90, 80, 80);
    widthSlider.setBounds(getWidth()/2-100, getHeight()/2+20, 80, 80);
    freqSlider.setBounds(getWidth()/2+10, getHeight()/2+20, 100, 80);
    
    feedbackSlider.setBounds(getWidth()/2-126-40, getHeight()/2+200-30, 80, 80);
    feedforwardSlider.setBounds(getWidth()/2+180-40, getHeight()/2-30, 80, 80);
    bleedSlider.setBounds(getWidth()/2+36-40, getHeight()/2-200-30, 80, 80);
    
    inputLabel.setBounds(getWidth()/2-344, getHeight()/2-12, 48, 24);
    outputLabel.setBounds(getWidth()/2+296, getHeight()/2-12, 48, 24);
    
    Font font = Font("Helvetica", 32, Font::FontStyleFlags::italic);
    int titleWidth = font.getStringWidth("Universal Comb Filter");
    int titleHeight = font.getHeight();
    title.setBounds(getWidth()/2+16, getHeight()/2+160-titleHeight/2, titleWidth, titleHeight);
    title.setJustificationType(Justification::centredLeft);
    
    tremoloToggle.setBounds(getWidth()/2+21, getHeight()/2+166, 120, 40);
}

void UniversalCombFilterAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    auto& audioParams = audioProcessor.getParameters();
    if (slider == &widthSlider) {
        AudioParameterFloat* widthParam = (AudioParameterFloat*) audioParams.getUnchecked(0);
        *widthParam = (float)widthSlider.getValue()/1000.0f;
    } else if (slider == &freqSlider) {
        AudioParameterFloat* freqParam = (AudioParameterFloat*) audioParams.getUnchecked(1);
        *freqParam = freqSlider.getValue();
    } else if (slider == &bleedSlider) {
        AudioParameterFloat* bleedParam = (AudioParameterFloat*) audioParams.getUnchecked(2);
        *bleedParam = bleedSlider.getValue();
    }  else if (slider == &feedforwardSlider) {
        AudioParameterFloat* feedforwardParam = (AudioParameterFloat*) audioParams.getUnchecked(3);
        *feedforwardParam = feedforwardSlider.getValue();
    }  else if (slider == &feedbackSlider) {
        AudioParameterFloat* feedbackParam = (AudioParameterFloat*) audioParams.getUnchecked(4);
        *feedbackParam = feedbackSlider.getValue();
    }  else if (slider == &delaySlider) {
        AudioParameterFloat* delayParam = (AudioParameterFloat*) audioParams.getUnchecked(5);
        *delayParam = (float)delaySlider.getValue()/1000.0f;
    }
}

void UniversalCombFilterAudioProcessorEditor::buttonClicked(Button* button)
{
    return;
}

void UniversalCombFilterAudioProcessorEditor::buttonStateChanged(Button* button)
{
    auto& audioParams = audioProcessor.getParameters();
    if (button == &tremoloToggle) {
        AudioParameterBool* tremoloParam = (AudioParameterBool*) audioParams.getUnchecked(6);
        *tremoloParam = tremoloToggle.getToggleState();
    }
}

void UniversalCombFilterAudioProcessorEditor::drawSum(juce::Graphics& g, float x, float y)
{
    g.drawEllipse(x-12, y-12, 24, 24, 1);
    g.drawLine(x-6, y, x+6, y);
    g.drawLine(x, y-6, x, y+6);
}
