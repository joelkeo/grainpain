/*
  ==============================================================================

    Shapes.h
    Created: 7 May 2024 5:20:46pm
    Author:  Joel Keohane

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Shapes {
public:
    static float sinc(float location);
    static float* generateSinc();
private:
    static float* sincBuffer;
    static int sincBufferLength;
    static int sincLength;
    static float sincResolution;
    static float internalSinc(float location);
    static void applyHalfBlackman(float* buffer, int length);
};
