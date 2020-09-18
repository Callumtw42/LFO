/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


Processor::Processor(const char* patchPath): CustomPatchLoader(soul::patch::PatchLibraryDLL(), patchPath)
{
	this->lfo = std::make_unique<LFO>();
}

void Processor::patchReady()
{
		this->plugin->createCustomGUI = [this](soul::patch::SOULPatchAudioProcessor& p)
		{
			return new ProcessorEditor(p, *lfo.get());
		};
		dbg(this->plugin->getParameters().size());
		lfo->setEndPoint(this->plugin->getParameters()[0]);
}

void Processor::patchUpdating()
{
		lfo->updateStartTime();
}

void Processor::preProcessBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi)
{
	if (playHead.load() && playHead.load()->getCurrentPosition(playheadPosition))
	{
		bpm = playheadPosition.bpm;
	}

	for (MidiMessageMetadata data : midi)
	{
		int msgCount = 0;
		if (data.numBytes < 4)
		{
			MidiMessage message = data.getMessage();
			if (message.isNoteOn())
			{
				lfo->trigger(true);
			}
			else if (message.isNoteOff())
			{
				lfo->trigger(false);
			}
		}
	}

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	using namespace soul::patch;
	auto patchPath = "c:/users/callu/projects/juce-projects/LFO/source/soul/level.soulpatch";
	auto* patchLoader = new Processor(patchPath);

	return patchLoader;
}



