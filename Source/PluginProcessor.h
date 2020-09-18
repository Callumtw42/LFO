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

struct Processor : public soul::patch::CustomPatchLoader<soul::patch::PatchLibraryDLL>
{
	Processor(const char* patchPath);
	void preProcessBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) override;
	void patchReady() override;
	void patchUpdating() override;

	UPtr<LFO> lfo;

};