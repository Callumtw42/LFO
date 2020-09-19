/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


Processor::Processor(const char* patchPath) : CustomPatchLoader(soul::patch::PatchLibraryDLL(), patchPath)
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

PluginProcessor::PluginProcessor() : AudioProcessor(BusesProperties()
	.withInput("Input", juce::AudioChannelSet::stereo(), true)
	.withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
	this->lfo = new LFO();
}

void PluginProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{

}

void PluginProcessor::processBlock(AudioBuffer<float>& audio, MidiBuffer& midi)
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
				lastMessage.isNoteOn = true;
				lastMessage.samplePosition = message.getTimeStamp();
				lastMessage.mode = lfo->mode;
				samplesElapsed = 0;
			}
			else if (message.isNoteOff())
			{
				lastMessage.isNoteOn = false;
				lastMessage.samplePosition = message.getTimeStamp();
				lastMessage.mode = lfo->mode;
			}
		}
	}

	auto numChannels = audio.getNumChannels();
	auto sampleRate = getSampleRate();
	auto blockSize = getBlockSize();
	auto offset = 0.0;
	auto freq = 200.0;

	auto secondsPerBeat = 60 / bpm;
	auto samplesPerBeat = secondsPerBeat * getSampleRate();
	auto samplesPerBar = samplesPerBeat * 4;
	auto samplesPerCycle = samplesPerBar * lfo->speed;


	//double barLength = microSecondsPerBeat * 4;
	//auto elapsedTime =
	//	std::chrono::high_resolution_clock::now().time_since_epoch().count() - startTime.time_since_epoch().count();
	//double microsecondsElapsed = elapsedTime / 1000;
	//double barsElapsed = microsecondsElapsed / barLength;
	//double intervalsElapsed = barsElapsed / speed;

	//int rounded = std::round(intervalsElapsed * LFORES);
	//int index = rounded % LFORES;
	//currentIndex = index;
	//double lfoVal = plot[index];

	//auto dB = 20.0f * std::log10(lfoVal);
	//auto outVal = (std::clamp(dB, -40.0, 0.0) + 40) / 40;

	for (int s = 0; s < getBlockSize(); ++s)
	{
		auto index = 0;
		switch (lfo->mode)
		{
		case LFO::free:
			index = calculateFree(samplesPerCycle);
			break;
		case LFO::sync:
			index = calculateSync(samplesPerCycle);
			break;
		case LFO::oneshot:
			index = calculateOneshot(samplesPerCycle, s, midi);
			break;
		case LFO::latch:
			index = calculateLatch(samplesPerCycle, s, midi);
			break;
		default: index = 0;
		}
		lfo->currentIndex = index;

		offset += freq / sampleRate;
		for (int i = 0; i < audio.getNumChannels(); ++i)
		{
			audio.getWritePointer(i, 0)[s] = std::sin(2 * std::_Pi * offset) * lfo->plot[index];

		}
	}
}

int PluginProcessor::calculateFree(double samplesPerCycle)
{
	samplesElapsed++;
	auto samplePosition = std::fmod(samplesElapsed, samplesPerCycle);
	auto relPosition = samplePosition / samplesPerCycle;
	auto index = (int)std::floor(relPosition * LFORES);
	return index;
}

int PluginProcessor::calculateSync(double samplesPerCycle)
{
	samplesElapsed = (playheadPosition.timeInSamples > 0) ? playheadPosition.timeInSamples : 0;
	auto samplePosition = std::fmod(samplesElapsed, samplesPerCycle);
	auto relPosition = samplePosition / samplesPerCycle;
	auto index = (int)std::floor(relPosition * LFORES);
	return index;
}

int PluginProcessor::calculateOneshot(double samplesPerCycle, int currentBlockPosition, MidiBuffer& midi)
{
	if (samplesElapsed < samplesPerCycle && lastMessage.mode == lfo->mode)
		samplesElapsed++;

	auto samplePosition = std::fmod(samplesElapsed, samplesPerCycle);
	auto relPosition = samplePosition / samplesPerCycle;
	auto index = (int)std::floor(relPosition * LFORES);
	return index;
}

int PluginProcessor::calculateLatch(double samplesPerCycle, int currentBlockPosition, MidiBuffer& midi)
{
	if (lastMessage.isNoteOn) samplesElapsed++;
	else samplesElapsed = 0;

	auto samplePosition = std::fmod(samplesElapsed, samplesPerCycle);
	auto relPosition = samplePosition / samplesPerCycle;
	auto index = (int)std::floor(relPosition * LFORES);
	return index;
}






AudioProcessorEditor* PluginProcessor::createEditor() { return new ProcessorEditor(*this, *lfo); }

const String PluginProcessor::getName() const { return "LFO"; }
void PluginProcessor::releaseResources() { }
double PluginProcessor::getTailLengthSeconds() const { return getTailLengthSeconds(); }
bool PluginProcessor::acceptsMidi() const { return true; }
bool PluginProcessor::producesMidi() const { return false; }
bool PluginProcessor::hasEditor() const { return true; }
int PluginProcessor::getNumPrograms() { return 1; }
int PluginProcessor::getCurrentProgram() { return 0; }
void PluginProcessor::setCurrentProgram(int index) { }
const String PluginProcessor::getProgramName(int index) { return "LFO"; }
void PluginProcessor::changeProgramName(int index, const String& newName) { }
void PluginProcessor::getStateInformation(juce::MemoryBlock& destData) { }
void PluginProcessor::setStateInformation(const void* data, int sizeInBytes) { }

//-----------------------------------------------------------------------	----------------------------------------





//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	using namespace soul::patch;
	auto patchPath = "c:/users/callu/projects/juce-projects/LFO/source/soul/level.soulpatch";
	auto* patchLoader = new PluginProcessor();

	return patchLoader;
}



