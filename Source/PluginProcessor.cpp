/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "custom-patch-loader.h"
#include "soulpatch.cpp"


//==============================================================================
Processor::Processor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	),
	lfo(std::make_unique<LFO>(/*gain.getParameters()[0])*/))
#endif
{
}

Processor::~Processor()
{
}

//==============================================================================
const juce::String Processor::getName() const
{
	return JucePlugin_Name;
}

bool Processor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool Processor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool Processor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double Processor::getTailLengthSeconds() const
{
	return 0.0;
}

int Processor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int Processor::getCurrentProgram()
{
	return 0;
}

void Processor::setCurrentProgram(int index)
{
}

const juce::String Processor::getProgramName(int index)
{
	return {};
}

void Processor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void Processor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	gain.prepareToPlay(sampleRate, samplesPerBlock);
	//lfo->start();
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
}

void Processor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Processor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
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

void Processor::processBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi)
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

	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
	{
		audio.clear(i, 0, audio.getNumSamples());
	}
	gain.processBlock(audio, midi);

}

//==============================================================================
bool Processor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Processor::createEditor()
{
	//return new ProcessorEditor(*this);
	return nullptr;
}

//==============================================================================
void Processor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void Processor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

struct SimpleEditor : AudioProcessorEditor
{
	SimpleEditor(AudioProcessor& p)
		: AudioProcessorEditor(p), proc(&p)
	{
		setResizable(true, true);
		setSize(300, 200);
	}

	void resized() override
	{

	};

	void mouseDown(const MouseEvent& event) override
	{
		dbg(proc->getParameters().size());
	};

	void paint(Graphics& g) override
	{
		g.drawText("HELLO", 0, 0, getWidth(), getHeight(), Justification::centred);
	};

	AudioProcessor* proc;

};

struct Proc : public soul::patch::CustomPatchLoader<soul::patch::PatchLibraryDLL>
{
	using CustomPatchLoader::CustomPatchLoader;
	void initialise(const char* patchPath)
	{
		this->lfo = std::make_unique<LFO>();
		this->patchReady = [this]()
		{
			this->plugin->createCustomGUI = [this](soul::patch::SOULPatchAudioProcessor& p)
			{
				return new ProcessorEditor(p, *lfo.get());
			};
			dbg(this->plugin->getParameters().size());
			lfo->setEndPoint(this->plugin->getParameters()[0]);
			//lfo->start();
		};
		this->patchUpdating = [this]()
		{
			lfo->updateStartTime();
			//std::this_thread::sleep_for(std::chrono::microseconds(lfo->minInterval));
		};

		this->setPatchURL(patchPath);

	}

	UPtr<LFO> lfo;

};
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	using namespace soul::patch;
	auto patchPath = "c:/users/callu/projects/juce-projects/LFO/source/soul/level.soulpatch";
	auto* patchLoader = new Proc(PatchLibraryDLL());
	patchLoader->initialise(patchPath);


	return patchLoader;
}
