/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GrainPainAudioProcessorEditor::GrainPainAudioProcessorEditor (GrainPainAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    parameters = Parameters();
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    setSize (400, 300);
    // params
    audioProcessor.giveParameterObject(&parameters);
    initializeSliders();
}

GrainPainAudioProcessorEditor::~GrainPainAudioProcessorEditor()
{
}

//==============================================================================
void GrainPainAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("paraMETERS", getLocalBounds(), juce::Justification::centred, 1);

}

void GrainPainAudioProcessorEditor::resized()
{
    spacing.setBounds (120, 30, 20, getHeight() - 60);
    sizeRatio.setBounds (40, 30, 20, getHeight() - 60);
    position.setBounds (80, 30, 20, getHeight() - 60);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void GrainPainAudioProcessorEditor::initializeSliders() {
    // Spacing
    spacing.setSliderStyle (juce::Slider::LinearBarVertical);
    spacing.setRange (1.0, 1000.0f, 1.0);
    spacing.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    spacing.setPopupDisplayEnabled (true, false, this);
    spacing.setTextValueSuffix ("Inter Onset Type");
    spacing.setValue(1.0);
    addAndMakeVisible (&spacing);
    spacing.onValueChange = [this] {parameters.spacing = spacing.getValue();};
    // Size Ratio
    sizeRatio.setSliderStyle (juce::Slider::LinearBarVertical);
    sizeRatio.setRange (0.0, 8.0, 0.0);
    sizeRatio.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    sizeRatio.setPopupDisplayEnabled (true, false, this);
    sizeRatio.setTextValueSuffix ("Grain Size vs. Rate");
    sizeRatio.setValue(1.0);
    addAndMakeVisible (&sizeRatio);
    sizeRatio.onValueChange = [this] {parameters.sizeRatio = sizeRatio.getValue();};
    // Position
    position.setSliderStyle (juce::Slider::LinearBarVertical);
    position.setRange (0.0f, 1.0f, 0.0);
    position.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    position.setPopupDisplayEnabled (true, false, this);
    position.setTextValueSuffix ("Sample Position");
    position.setValue(690.0f);
    addAndMakeVisible (&position);
    position.onValueChange = [this] {parameters.position = position.getValue();};
}
