

#pragma once

#include "PluginProcessor.h"
#include "plot.h"
#include "controlbox.h"

static const int WIDTH = 300;
static const int HEIGHT = 200;

class UI :public Component
{
public:
	UI(Processor& processor) :
		processor(&processor)
	{
		this->plot = std::make_unique<Plot>(processor.lfo->plot);
		addAndMakeVisible(plot.get());
		controls.addDial(new Dial(1, 16, 1, 8, [&](float value)
			{
				plot->gridRes = value;
				plot->repaint();
			}));
		
		addAndMakeVisible(controls);
		std::array<float, (int)9>* speedValues = new std::array<float, 9>({
			4.0f ,2.0f, 1.0f,
			1.0f / 2, 1.0f / 4,1.0f / 8,
		1.0f / 16, 1.0f / 32, 1.0f / 64 });

		controls.addDial(new Dial(*speedValues, 2, [&](float value)
			{
				processor.lfo->speed = value;
			}));

	}

	void resized() override
	{
		plot->setBoundsRelative(0, 0, 1, 0.75f);
		controls.setBoundsRelative(0, 0.75f, 1, 0.25f);
	};

	UPtr<Plot> plot;
	Controls controls;
	Processor* processor;
};

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
