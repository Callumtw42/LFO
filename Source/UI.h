#pragma once
#include <JuceHeader.h>
#include "plot.h"
#include "controlbox.h"
#include "mode-button.h"

const int CTRL_HEIGHT = 50;

class UI :public Component
{
public:
	UI(AudioProcessor& processor, LFO& lfo) :
		processor(&processor)
	{
		this->plot = std::make_unique<Plot>(lfo.plot);
		addAndMakeVisible(plot.get());
		addAndMakeVisible(controls);
		controls.add(std::make_shared<Dial>(1, 16, 1, 8, [&](float value)
			{
				plot->gridRes = value;
				plot->repaint();
			}));

		auto speedValues = new Array<float>({
			32.0f, 16.0f, 8.0f,
			4.0f ,2.0f, 1.0f,
			1.0f / 2, 1.0f / 4,1.0f / 8,
			1.0f / 16, 1.0f / 32, 1.0f / 64,
			1.0f/128, 1.0f/256, 1.0f / 512
			});

		auto labels = new Array<String>({
					"32", "16", "8",
					"4", "2", "1",
					"1/2", "1/4", "1/8",
					"1/16", "1/32", "1/64",
					"1/128", "1/256", "fast",
			
			});

		controls.add(std::make_shared<Dial>(*speedValues, *labels, 7, [&](float value)
			{
				lfo.speed = value;
			}));

		controls.add(std::make_shared<ModeButton>(
			[&](float mode)
			{
				lfo.setMode(mode);
			}
		));

	}

	void resized() override
	{
		plot->setBounds(0, 0, getWidth(), getHeight() - CTRL_HEIGHT);
		controls.setBounds(0, getHeight() - CTRL_HEIGHT, getWidth(), CTRL_HEIGHT);
	};

	UPtr<Plot> plot;
	Controls controls;
	AudioProcessor* processor;
};

