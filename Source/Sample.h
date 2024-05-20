/*
  ==============================================================================

    Sample.h
    Created: 4 May 2024 9:17:22pm
    Author:  Joel Keohane

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Sample {
public:
    bool hasAudio;
    float* leftChannel;
    float* rightChannel;
    static float padding;
    int length;
    Sample();
    float read(float start, float rate, int numSamples, juce::AudioBuffer<float> &buffer);
    float readSinc(float start, float rate, int outStart, int outEndSample, juce::AudioBuffer<float> &buffer);
    void setAudio(juce::AudioBuffer<float> &audio);
};
