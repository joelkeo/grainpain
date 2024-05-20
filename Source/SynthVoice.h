//
//  SynthVoice.h
//  GrainPain
//
//  Created by Joel Keohane on 5/2/24.
//
#pragma once
#define SynthVoice_h
#include <JuceHeader.h>
#include "Sample.h"
#include "Grain.h"
#include "Parameters.h"

class SynthVoice {
public:
    SynthVoice (Sample& sample);
    void startNote(int midiNoteNumber, float velocity, int startSample);
    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int endSample);
    void stopNote();
    int getSampleRate();
    int getCurrentNote();
    void nextBlock();
    bool isActive();
    bool isAlive();
    void setSample(Sample& sample);
    void setParameters(Parameters& parameters);
    bool hasRendered();
    Parameters* parameters;
private:
    float position;
    int note;
    float rate;
    const float middleCHz = 523.2511;
    int bufferStartSample;
    bool active;
    bool alive;
    Sample& sample;
    bool rendered;
    // grain handling
    int grainProcessingBufferLength = 88200;
    juce::AudioBuffer<float> grainProcessingBuffer = juce::AudioBuffer<float>(2, 88200);
    const int numberOfGrains = 100;
    Grain grains[100];
    int previousGrain;
    void renderActiveGrains(juce::AudioBuffer<float>& outputBuffer, int endSample);
    void renderGrain(juce::AudioBuffer<float>& outputBuffer, int endSample, int i);
    bool populateNextGrain(juce::AudioBuffer<float>& outputBuffer, int endSample);
    int nextAvailableGrainspace();
    void initializeGrains();
};
