/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

using namespace juce;

//==============================================================================
/**
*/
class UniversalCombFilterAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                                 public juce::Slider::Listener,
                                                 public juce::Button::Listener
{
public:
    UniversalCombFilterAudioProcessorEditor (UniversalCombFilterAudioProcessor&);
    ~UniversalCombFilterAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(Slider*) override;
    void buttonClicked(Button*) override;
    void buttonStateChanged(Button*) override;
    void drawSum(juce::Graphics&, float, float);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    UniversalCombFilterAudioProcessor& audioProcessor;
    
    float delayInterval;
    
    Slider delaySlider;
    Label delayLabel;
    
    Slider widthSlider;
    Label widthLabel;
    
    Slider freqSlider;
    Label freqLabel;
    
    Slider bleedSlider;
    Label bleedLabel;
    
    Slider feedforwardSlider;
    Label feedforwardLabel;
    
    Slider feedbackSlider;
    Label feedbackLabel;
    
    ToggleButton tremoloToggle;
    
    Label inputLabel;
    Label outputLabel;
    Label title;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UniversalCombFilterAudioProcessorEditor)
};
