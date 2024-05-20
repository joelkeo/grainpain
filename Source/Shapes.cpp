/*
  ==============================================================================

    Shapes.cpp
    Created: 7 May 2024 5:21:01pm
    Author:  Joel Keohane

  ==============================================================================
*/

#include "Shapes.h"
#include <cmath>
#define PI 3.141592653589793238462643383279502884e+00F

int Shapes::sincLength = 4;
float Shapes::sincResolution = 65536.0f;
int Shapes::sincBufferLength = sincLength * sincResolution;
float* Shapes::sincBuffer = generateSinc();

float Shapes::sinc(float location) {
    // make sure location is in bounds
    jassert(abs(location) < sincLength);
    float i = (location * sincResolution);
    int first = abs(floor(i));
    int next = first + 1;
    // linear interpolation
    float firstAlpha = i - ((float) first);
    float nextAlpha = ((float) next) - i;
    return sincBuffer[first] * firstAlpha + sincBuffer[next] * nextAlpha;
}

float Shapes::internalSinc(float location) {
    if (location == 0.0f) {return 1.0f;}
    return sinf(location *  PI) / (location *  PI);
}

float* Shapes::generateSinc() {
    // 65536 per 1 -> and theres 4
    const int length = sincBufferLength;
    // plus 1 for linear interpolation stuff
    float* sincy = new float[length + 1];
    sincy[0] = 1.0f;
    for (int i = 1; i < length; i++) {
        sincy[i] = internalSinc(((float) i) / sincResolution);
    }
    sincy[length] = 0.0f;
    applyHalfBlackman(sincy, length);
    return sincy;
}

void Shapes::applyHalfBlackman(float* buffer, int length) {
    float N = ((float) length) * 2.0f;
    float a  = .16f;
    float a0 = (1.0f - a) / 2.0f;
    float a1 = 1.0f / 2.0f;
    float a2 = a / 2.0f;
    float x = (float) length;
    for (int i = 0; i < length; i++) {
        // actual window application
        buffer[i] *= (a0 - a1 * cosf(2.0f * PI * x / N) + a2 * cosf(4.0f * PI * x / N));
        x += 1.0f;
    }
}
