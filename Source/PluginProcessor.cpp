/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SynthVoice.h"
#include "Synth.hpp"

//==============================================================================
GrainPainAudioProcessor::GrainPainAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
)
#endif
, synth(3, sample)
, chooser("Select a Wave file to play...",
          juce::File{},
          "*.wav, *.mp3")
{
    hasFile = false;
    loadFile();
}

GrainPainAudioProcessor::~GrainPainAudioProcessor()
{
}

//==============================================================================
const juce::String GrainPainAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GrainPainAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GrainPainAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GrainPainAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GrainPainAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GrainPainAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GrainPainAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GrainPainAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GrainPainAudioProcessor::getProgramName (int index)
{
    return {};
}

void GrainPainAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GrainPainAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void GrainPainAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GrainPainAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GrainPainAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = 2;
    auto totalNumOutputChannels = 2;
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
    buffer.clear();
    synth.renderNextBlock(buffer, midiMessages, buffer.getNumSamples());
}

//==============================================================================
bool GrainPainAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GrainPainAudioProcessor::createEditor()
{
    return new GrainPainAudioProcessorEditor (*this);
}

//==============================================================================
void GrainPainAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GrainPainAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return (juce::AudioProcessor*) new GrainPainAudioProcessor();
}

void GrainPainAudioProcessor::giveFile(juce::File file) {
    formatReader = formatManager.createReaderFor(file);
    juce::AudioBuffer<float> buffer;
    buffer.setSize(formatReader->numChannels, formatReader->lengthInSamples);
    formatReader->read(&buffer, 0, formatReader->lengthInSamples, 0, true, true);
    sample.setAudio(buffer);
    hasFile = true;
}

void GrainPainAudioProcessor::loadFile() {
    auto chooserFlags = juce::FileBrowserComponent::openMode
                              | juce::FileBrowserComponent::canSelectFiles;
     
    formatManager.registerBasicFormats();
    chooser.launchAsync(chooserFlags, [this](const juce::FileChooser& chooser) {this->giveFile(chooser.getResult());});
}

void GrainPainAudioProcessor::giveParameterObject(Parameters* parameters) {
    synth.giveParametersObject(parameters);
}
