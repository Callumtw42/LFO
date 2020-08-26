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
		addAndMakeVisible(plot);
		controls.addDial(new Dial(1, 16, 1, 8, [&](float value)
			{
				plot.gridRes = value;
				plot.repaint();
			}));
		//NEXT: Create an alternative dial constructor which accepts an array of values
		std::array<float, (int)9>* speedValues = new std::array<float, 9>({
			4.0f ,2.0f, 1.0f,
			1.0f / 2, 1.0f / 4,1.0f / 8,
		1.0f / 16, 1.0f / 32, 1.0f / 64 });

		controls.addDial(new Dial(*speedValues, 2, [&](float value)
			{
				processor.lfo->speed = value;
			}));

		addAndMakeVisible(controls);
	}

	void resized() override
	{
		const float width = getWidth(), height = getHeight();
		const Rectangle<int> plotBounds = Rectangle<int>(
			0,
			0,
			width,
			3.0f / 4 * height
			);

		const Rectangle<int> controlBounds = Rectangle<int>(
			0,
			plotBounds.getHeight(),
			width,
			height - plotBounds.getHeight()
			);

		plot.setBounds(plotBounds);
		controls.setBounds(controlBounds);
	};

	Plot plot;
	Controls controls;
	Processor* processor;
};

class Editor : public juce::AudioProcessorEditor, public juce::AudioProcessorParameter::Listener
{
public:
	Editor(Processor&);
	~Editor() override;
	void resized() override;
	void parameterValueChanged(int parameterIndex, float newValue) override;
	void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}
	void updateParams();
	void bindNativeCallbacks();
	void connectLFOCallback();
	Processor* processor;
	blueprint::ReactApplicationRoot appRoot;
	Label errorText;
	ComponentBoundsConstrainer constrainer;
	UI* ui;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Editor);
};
