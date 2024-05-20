/*
  ==============================================================================

    Sample.cpp
    Created: 4 May 2024 9:17:29pm
    Author:  Joel Keohane

  ==============================================================================
*/

#include "Sample.h"
#include "Shapes.h"


float Sample::padding = 4;

Sample::Sample() {hasAudio=false;}

void Sample::setAudio(juce::AudioBuffer<float> &buffer) {
    // pad the beginning and end with 0s to aid with sinc interpolation
    length = buffer.getNumSamples() + 2 * ((int) padding);
    leftChannel = (float*) malloc(sizeof(float) * length);
    rightChannel = (float*) malloc(sizeof(float) * length);
    const float* originalLeft = buffer.getReadPointer(0);
    const float* originalRight = buffer.getReadPointer(1);
    int paddingInt = (int) padding;
    // beginning padding
    for (int i = 0; i < paddingInt; i++) {
        leftChannel[i] = 0.0f;
        rightChannel[i] = 0.0f;
    }
    for (int i = paddingInt; i < (length - paddingInt); i++) {
        leftChannel[i] = originalLeft[i - paddingInt];
        rightChannel[i] = originalRight[i - paddingInt];
    }
    for (int i = length - paddingInt; i < length; i++) {
        leftChannel[i] = 0.0f;
        rightChannel[i] = 0.0f;
    }
    hasAudio = true;
}

// linear interpolation
float Sample::read(float start, float rate, int numSamples, juce::AudioBuffer<float> &buffer) {
    if (! hasAudio) {return -1;}
    float* outLeft = buffer.getWritePointer(0);
    float* outRight = buffer.getWritePointer(1);
    float end = (float) (length - 1);
    // TODO implement with pointer arithmetic
    // Currently a simple linear interpoloation
    float currentPoint = start;
    for (int i = 0; (i < numSamples) && (currentPoint < end); i++) {
        float beforeSampleLocation = floor(currentPoint);
        float afterSampleLocation = beforeSampleLocation + 1;
        float beforeAlpha = currentPoint - beforeSampleLocation;
        float afterAlpha = afterSampleLocation - currentPoint;
        outLeft[i] += beforeAlpha * leftChannel[(int) beforeSampleLocation] + afterAlpha * leftChannel[(int) afterSampleLocation];
        outRight[i] += beforeAlpha * rightChannel[(int) beforeSampleLocation] + afterAlpha * rightChannel[(int) afterSampleLocation];
        currentPoint += rate;
    }
    return currentPoint;
}

float Sample::readSinc(float start, float rate, int outStart, int outEndSample, juce::AudioBuffer<float> &buffer) {
    if (! hasAudio) {return 0.0f;}
    int sincPoints = 4;
    float currentPoint = start + padding;
    float* outLeft = buffer.getWritePointer(0);
    float* outRight = buffer.getWritePointer(1);
    float end = (float) (length);
    for (int i = outStart; (i < outEndSample) && (currentPoint < end); i++) {
        float before = floor(currentPoint);
        float after = before + 1;
        float leftSample = 0.0f;
        float rightSample = 0.0f;
        for (int sincI = 0; sincI < sincPoints; sincI++) {
            float beforeDistance = currentPoint - before;
            float afterDistance = after - currentPoint;
            float beforeSinc = Shapes::sinc(beforeDistance);
            float afterSinc = Shapes::sinc(afterDistance);
            leftSample += beforeSinc * leftChannel[(int) before] + afterSinc * leftChannel[(int) after];
            rightSample += beforeSinc * rightChannel[(int) before] + afterSinc * rightChannel[(int) after];
            before -= 1.0f;
            after += 1.0f;
        }
        outLeft[i] += leftSample;
        outRight[i] += rightSample;
        currentPoint += rate;
    }
    return currentPoint - padding;
}
