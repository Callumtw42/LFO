
#pragma once
#include "UI.h"

static const int WIDTH = 300;
static const int HEIGHT = 200;

class Editor : public juce::AudioProcessorEditor, public juce::AudioProcessorParameter::Listener
{
public:
	Editor(Processor&);
	~Editor() override;
	void resized() override;
	void parameterValueChanged(int parameterIndex, float newValue) override {};
	void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}
	void connectLFOCallback();
	Processor* processor;
	Label errorText;
	ComponentBoundsConstrainer constrainer;
	UI* ui;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Editor);
};
