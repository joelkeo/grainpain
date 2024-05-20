/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Parameters.h"

//==============================================================================
/**
*/
class GrainPainAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Parameters parameters;
    GrainPainAudioProcessorEditor (GrainPainAudioProcessor&);
    ~GrainPainAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GrainPainAudioProcessor& audioProcessor;
    // Params
    void initializeSliders();
    juce::Slider spacing;
    juce::Slider sizeRatio;
    juce::Slider position;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrainPainAudioProcessorEditor)
};
