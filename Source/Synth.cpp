//
//  Synth.cpp
//  GrainPain - VST3
//
//  Created by Joel Keohane on 5/3/24.
//

#include "Synth.hpp"
#include <JuceHeader.h>
#include "SynthVoice.h"

void Synth::renderNextBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, int numSamples) {
    // reset voice starts to 0
    for (int i = 0 ; i < numberOfVoices ; i++) {
        SynthVoice& currentVoice = voices[i];
        currentVoice.nextBlock();
    }
    // render alive but inactive voices
    for (int i = 0; i < numberOfVoices; i++) {
        SynthVoice& currentVoice = voices[i];
        if (currentVoice.isAlive() && (! currentVoice.isActive())) {
            currentVoice.renderNextBlock(buffer, numSamples);
        }
    }
    // iterate through MIDI messages
    for (const auto meta : midiMessages) {
        int sample = meta.samplePosition;
        juce::MidiMessage message = meta.getMessage();
        int note = message.getNoteNumber();
        if (message.isNoteOn()) {
            handleNoteOn(buffer, note, sample);
        }
        if (message.isNoteOff()) {
            handleNoteOff(buffer, note, sample);
        }
    }
    // render until end for voices still, not yet rendered
    for (int i = 0 ; i < numberOfVoices ; i++) {
        SynthVoice& currentVoice = voices[i];
        if (currentVoice.isActive()) {
            currentVoice.renderNextBlock(buffer, numSamples);
        }
    }
}

void Synth::handleNoteOn(juce::AudioBuffer<float>& buffer, int note, int startSample) {
    bool isAlreadyOn = (voiceArray[note] != -1);
    jassert(! isAlreadyOn);
    SynthVoice& nextVoice = getFreeVoice(note);
    nextVoice.startNote(note, 100, 0);
}

void Synth::handleNoteOff(juce::AudioBuffer<float>& buffer, int note, int endSample) {
    int voiceIndex = voiceArray[note];
    bool isOn = (voiceIndex != -1);
    if (isOn) {
        SynthVoice& currentVoice = voices[voiceIndex];
        currentVoice.renderNextBlock(buffer, endSample);
        currentVoice.stopNote();
        voiceArray[note] = -1;
    }
}

Synth::Synth(int numVoices, Sample& sample) : voices({{sample, sample, sample, sample, sample, sample}}) {
    jassert(numberOfVoices < 6);
    numberOfVoices = numVoices;
    for (int i = 0; i < NUMBER_OF_NOTES; i++) {
        voiceArray[i] = -1;
    }
}

SynthVoice& Synth::getFreeVoice(int midiNumber) {
    for (int i = 0 ; i < numberOfVoices ; i++) {
        SynthVoice& currentVoice = voices[i];
        if(! currentVoice.isActive()) {
            voiceArray[midiNumber] = i;
            return currentVoice;
        }
    }
    // if there are no free voices: steal
    // TODO: make this actually good
    SynthVoice& currentVoice = voices[0];
    int previousNote = currentVoice.getCurrentNote();
    currentVoice.stopNote();
    voiceArray[previousNote] = -1;
    voiceArray[midiNumber] = 0;
    return voices[0];
}

void Synth::giveParametersObject(Parameters* parameters) {
    for (int i = 0; i < numberOfVoices; i++) {
        SynthVoice& currentVoice = voices[i];
        currentVoice.parameters = parameters;
    }
}
