
#pragma once
#include "UI.h"

static const int WIDTH = 300;
static const int HEIGHT = 200;

class ProcessorEditor : public juce::AudioProcessorEditor, public juce::AudioProcessorParameter::Listener
{
public:
	ProcessorEditor(Processor& p, LFO& lfo);
	~ProcessorEditor() override;
	void resized() override;
	void parameterValueChanged(int parameterIndex, float newValue) override {};
	void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}
	void connectLFOCallback();
	Processor* processor;
	LFO* lfo;
	Label errorText;
	ComponentBoundsConstrainer constrainer;
	UPtr<UI> ui;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessorEditor);
};
