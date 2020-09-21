/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "Processor.h"
#include "PluginEditor.h"


Processor::Processor() : AudioProcessor(BusesProperties()
	.withInput("Input", juce::AudioChannelSet::stereo(), true)
	.withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
	this->lfo = new LFO();

	std::function <String(int index, int maxStringLength)> stringFromIndex =
		[&](int index, int maxStringLength)
	{
		return  lfo->speedLabels[index];
	};

	std::function <int(const String&)> indexFromString =
		[&](const String& choice)
	{
		return  lfo->speedLabels.indexOf(choice);
	};

	this->apvs = new AudioProcessorValueTreeState(*this, nullptr, Identifier("LFO"), {
		std::make_unique<AudioParameterChoice>("speed", "speed", lfo->speedLabels, 7,String(), stringFromIndex, indexFromString),
		std::make_unique<AudioParameterInt>("grid", "grid", 1, 16, 8)
		});
}

void Processor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{

}

void Processor::processBlock(AudioBuffer<float>& audio, MidiBuffer& midi)
{
	setBpm();
	handleMidiMessages(midi);

	auto sampleRate = getSampleRate();
	auto offset = 0.0;
	auto freq = 200.0;

	auto samplesPerCycle = getSamplesPerCycle();

	for (int s = 0; s < getBlockSize(); ++s)
	{
		lfo->currentIndex = calculateIndex(samplesPerCycle, s);

		offset += freq / sampleRate;
		for (int i = 0; i < audio.getNumChannels(); ++i)
		{
			audio.getWritePointer(i, 0)[s] = std::sin(2 * std::_Pi * offset) * lfo->plot[lfo->currentIndex];

		}
	}
}

int Processor::calculateFree(double samplesPerCycle)
{
	samplesElapsed++;
	auto samplePosition = std::fmod(samplesElapsed, samplesPerCycle);
	auto relPosition = samplePosition / samplesPerCycle;
	auto index = (int)std::floor(relPosition * LFORES);
	return index;
}

int Processor::calculateSync(double samplesPerCycle)
{
	samplesElapsed = (playheadPosition.timeInSamples > 0) ? playheadPosition.timeInSamples : 0;
	auto samplePosition = std::fmod(samplesElapsed, samplesPerCycle);
	auto relPosition = samplePosition / samplesPerCycle;
	auto index = (int)std::floor(relPosition * LFORES);
	return index;
}


double Processor::getSamplesPerCycle()
{
	auto secondsPerBeat = 60 / bpm;
	auto samplesPerBeat = secondsPerBeat * getSampleRate();
	auto samplesPerBar = samplesPerBeat * 4;
	auto samplesPerCycle = samplesPerBar * lfo->speed;
	return samplesPerCycle;
}

int Processor::calculateOneshot(double samplesPerCycle)
{
	if (samplesElapsed < samplesPerCycle && triggered)
		samplesElapsed++;
	else
	{
		triggered = false;
		samplesElapsed = samplesPerCycle;
	}

	auto samplePosition = std::fmod(samplesElapsed, samplesPerCycle);
	auto relPosition = samplePosition / samplesPerCycle;
	auto index = (int)std::floor(relPosition * LFORES);
	return index;
}

int Processor::calculateLatch(double samplesPerCycle)
{
	if (lastMessage.isNoteOn) samplesElapsed++;

	auto samplePosition = std::fmod(samplesElapsed, samplesPerCycle);
	auto relPosition = samplePosition / samplesPerCycle;
	auto index = (int)std::floor(relPosition * LFORES);
	return index;
}

void Processor::handleMidiMessages(MidiBuffer& midi)
{

	for (MidiMessageMetadata data : midi)
	{
		if (data.numBytes < 4 &&
			(lfo->mode == LFO::oneshot || lfo->mode == LFO::latch))
		{
			MidiMessage message = data.getMessage();
			if (message.isNoteOn())
			{
				triggered = true;
				lastMessage.isNoteOn++;
				lastMessage.samplePosition = message.getTimeStamp();
				samplesElapsed = 0;
			}
			else if (message.isNoteOff())
			{
				lastMessage.isNoteOn--;
				lastMessage.samplePosition = message.getTimeStamp();
				if (lfo->mode == LFO::latch)samplesElapsed = 0;
			}
		}
	}
}

void Processor::setBpm()
{
	if (playHead.load() && playHead.load()->getCurrentPosition(playheadPosition))
	{
		bpm = playheadPosition.bpm;
	}
}

int Processor::calculateIndex(double samplesPerCycle, int currentBlockPosition)
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
		index = calculateOneshot(samplesPerCycle);
		break;
	case LFO::latch:
		index = calculateLatch(samplesPerCycle);
		break;
	default: index = 0;
	}

	return index;

}
AudioProcessorEditor* Processor::createEditor() { return new ProcessorEditor(*this, *lfo); }

void Processor::getStateInformation(juce::MemoryBlock& destData)
{
	std::unique_ptr<XmlElement> xml(apvs->state.createXml());

	if (xml != nullptr)
		copyXmlToBinary(*xml, destData);
}

void Processor::setStateInformation(const void* data, int sizeInBytes)
{
	std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

	if (xml != nullptr)
		apvs->state = ValueTree::fromXml(*xml);
}


const String Processor::getName() const { return "LFO"; }
void Processor::releaseResources() { }
double Processor::getTailLengthSeconds() const { return getTailLengthSeconds(); }
bool Processor::acceptsMidi() const { return true; }
bool Processor::producesMidi() const { return false; }
bool Processor::hasEditor() const { return true; }
int Processor::getNumPrograms() { return 1; }
int Processor::getCurrentProgram() { return 0; }
void Processor::setCurrentProgram(int index) { }
const String Processor::getProgramName(int index) { return "LFO"; }
void Processor::changeProgramName(int index, const String& newName) { }

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	using namespace soul::patch;
	auto patchPath = "c:/users/callu/projects/juce-projects/LFO/source/soul/level.soulpatch";
	auto* patchLoader = new Processor();

	return patchLoader;
}



