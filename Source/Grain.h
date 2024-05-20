/*
  ==============================================================================

    grain.h
    Created: 10 May 2024 12:16:20pm
    Author:  Joel Keohane

  ==============================================================================
*/

#pragma once
class Grain {
public:
    float start;
    // the length of the grain
    float length;
    // the position to read in the sample
    float samplePosition;
    // the rate to move through the sample (PITCH)
    float sampleReadRate;
    // the grains pan
    float pan;
    // the position to read in the window
    float windowPosition;
    // the rate to move through the window
    float windowRate;
    // time until next grain
    float childStart;
    // whether its active
    bool isActive;
    Grain();
};
