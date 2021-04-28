/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OctaneAudioProcessorEditor::OctaneAudioProcessorEditor (OctaneAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (720, 640);
}

OctaneAudioProcessorEditor::~OctaneAudioProcessorEditor()
{
}

//==============================================================================
void OctaneAudioProcessorEditor::paint (juce::Graphics& g)
{
    
}

void OctaneAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
