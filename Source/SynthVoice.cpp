//
//  SynthVoice.cpp
//  GrainPain
//
//  Created by Joel Keohane on 5/2/24.
//

#include <stdio.h>
#include "SynthVoice.h"
#include <cmath>
#include <JuceHeader.h>

SynthVoice::SynthVoice (Sample& sample) : sample(sample) {
    position = 0.0f;
    note = 72; // default to C
    rate = 1.0f;
    bufferStartSample = 0;
    active = false;
    alive = false;
    initializeGrains();
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, int startSample) {
    note = midiNoteNumber;
    rate = pow(2.0f, (((float) note) - 72.0f) / 12.0f);
    bufferStartSample = startSample;
    active = true;
    alive = true;
    initializeGrains();
}

void SynthVoice::stopNote() {
    bufferStartSample = -1;
    position = 0.0f;
    active = false;
    // FOR NOW, until release is implemented
    alive = false;
    
};

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int endSample) {
    renderActiveGrains(outputBuffer, endSample);
    while (populateNextGrain(outputBuffer, endSample)) {}
    // outputBuffer.getWritePointer(0)[0] = 1.0f;
    // continuously create its children
    // position = sample.readSinc(position, rate, endSample, outputBuffer);
}

int SynthVoice::getSampleRate() {
    return 44100;
}

void SynthVoice::nextBlock() {
    if (bufferStartSample != -1) {
        bufferStartSample = 0;
    }
}

bool SynthVoice::isActive() {
    return active;
}

bool SynthVoice::isAlive() {
    return alive;
}

int SynthVoice::getCurrentNote() {
    return note;
}

void SynthVoice::setSample(Sample& sample) {
    this->sample = sample;
}


// Grain Stuff ----------------------------------

void SynthVoice::renderGrain(juce::AudioBuffer<float>& outputBuffer, int endSample, int i) {
    Grain& currentGrain = grains[i];
    if (! currentGrain.isActive) {
        return;
    }
    int readEndSample;
    int grainEndInt = (int) floor(currentGrain.length + currentGrain.start);
    float samplePosition = currentGrain.samplePosition;
    float grainStart = currentGrain.start;
    float rate = currentGrain.sampleReadRate;
    
    // fractional grain handling
    float grainStartCeil = ceil(grainStart);
    float grainShift = grainStartCeil - grainStart;
    
    // if we are going to read the whole thing
    if (grainEndInt <= endSample) {
        readEndSample = grainEndInt;
        currentGrain.isActive = false;
    }
    else {
        readEndSample = endSample;
        float amountRead = ((float) endSample) - grainStart;
        currentGrain.length -= amountRead;
        currentGrain.start = 0.0f;
    }
    int grainStartCeilInt = (int) grainStartCeil;
    // int readLength = readEndSample - grainStartCeilInt;
    currentGrain.samplePosition
    = sample.readSinc(samplePosition + grainShift,
                      rate,
                      grainStartCeilInt,
                      readEndSample,
                      outputBuffer);
}

// return true if you can keep populating
bool SynthVoice::populateNextGrain(juce::AudioBuffer<float>& outputBuffer, int endSample) {
    Grain& currentGrain = grains[previousGrain];
    // see if we can put it in this buffer
    // yes we can: generate the next grain
    float nextGrainStart = currentGrain.childStart;
    if (((int) ceil(nextGrainStart)) < endSample) {
        int grainLocation = nextAvailableGrainspace();
        if (grainLocation == -1) {return false;}
        Grain& nextGrain = grains[grainLocation];
        nextGrain.isActive = true;
        float interOnsetTime = (44100 / (rate * middleCHz)) * parameters->spacing;
        nextGrain.childStart = nextGrainStart + interOnsetTime;
        nextGrain.samplePosition = parameters->position * (float) sample.length;
        nextGrain.sampleReadRate = rate;
        nextGrain.length = (parameters->sizeRatio) * interOnsetTime;
        nextGrain.start = nextGrainStart;
        renderGrain(outputBuffer, endSample, grainLocation);
        previousGrain = grainLocation;
        return true;
    }
    else {
        // update IOT for next buffer
        // float startToEnd = (((float) endSample) - currentGrain.start);
        currentGrain.childStart -= ((float) endSample);
        currentGrain.start = 0.0f;
        return false;
    }
}

void SynthVoice::renderActiveGrains(juce::AudioBuffer<float>& outputBuffer, int endSample) {
    for (int i = 0; i < numberOfGrains; i++) {
        renderGrain(outputBuffer, endSample, i);
    }
}

int SynthVoice::nextAvailableGrainspace() {
    Grain& currentGrain = grains[previousGrain];
    if (! currentGrain.isActive) {
        return previousGrain;
    }
    for (int i = 0; i < numberOfGrains; i++) {
        currentGrain = grains[i];
        if (! currentGrain.isActive) {
            return i;
        }
    }
    return -1;
}

void SynthVoice::initializeGrains() {
    Grain& firstGrain = grains[previousGrain];
    firstGrain.start = 0.0f;
    firstGrain.length = 0.0f;
    firstGrain.isActive = true;
    firstGrain.pan = 0.0f;
    firstGrain.samplePosition = 0.0f;
    firstGrain.sampleReadRate = 1.0f;
    firstGrain.windowPosition = 0.0f;
    firstGrain.windowRate = 1.0f;
    firstGrain.childStart = 0.0f;
    for (int i = 1; i < numberOfGrains; i++) {
        Grain& currentGrain = grains[i];
        currentGrain.isActive = false;
    }
}
               
