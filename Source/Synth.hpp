//
//  Synth.hpp
//  GrainPain - VST3
//
//  Created by Joel Keohane on 5/3/24.
//
#pragma once
#define Synth_hpp

#include <stdio.h>
#include "SynthVoice.h"
#include "Sample.h"
#include <array>

class Synth {
public:
    const static int MAX_NUMBER_OF_VOICES = 6;
    const static int NUMBER_OF_NOTES = 127;
    int numberOfVoices;
    std::array<SynthVoice, 6> voices;
    // managing voice triggering: maps a note to its voice, or -1 if its off
    int voiceArray[NUMBER_OF_NOTES];
    
    Synth(int numVoices, Sample& sample);
    void renderNextBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, int numSamples);
    void giveParametersObject(Parameters* parameters);
private:
    void handleNoteOn(juce::AudioBuffer<float>& buffer, int note, int startSample);
    void handleNoteOff(juce::AudioBuffer<float>& buffer, int note, int endSample);
    SynthVoice& getFreeVoice(int midiNumber);
};

