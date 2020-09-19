/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <array>
#include "lfo.h"
#include "c:/SOUL/include/soul/patch/helper_classes/soul_patch_Utilities.h"
#include "custom-patch-loader.h"

class Processor : public AudioProcessor
{
public:
	Processor();
	const String getName() const override;
	void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
	void releaseResources() override;
	void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;
	double getTailLengthSeconds() const override;
	bool acceptsMidi() const override;
	bool producesMidi() const override;
	AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const String getProgramName(int index) override;
	void changeProgramName(int index, const String& newName) override;
	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	int calculateFree(double samplesPerCycle);
	int calculateSync(double samplesPerCycle);
	int calculateOneshot(double samplesPerCycle);
	int calculateLatch(double samplesPerCycle);
	void handleMidiMessages(MidiBuffer& midi);
	void setBpm();
	int calculateIndex(double samplesPerCycle, int currentBlockPosition);
	double getSamplesPerCycle();

	struct LastMessage
	{
		int isNoteOn = false;
		double samplePosition = 0;
	};

	LFO* lfo;
	int64 samplesElapsed = 0;
	LastMessage lastMessage;
	bool triggered = false;


};