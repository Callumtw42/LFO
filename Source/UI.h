#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "plot.h"
#include "controlbox.h"
#include "mode-button.h"

class UI :public Component
{
public:
	UI(Processor& processor) :
		processor(&processor)
	{
		this->plot = std::make_unique<Plot>(processor.lfo->plot);
		addAndMakeVisible(plot.get());
		addAndMakeVisible(controls);
		controls.add(std::make_shared<Dial>(1, 16, 1, 8, [&](float value)
			{
				plot->gridRes = value;
				plot->repaint();
			}));

		std::array<float, (int)9>* speedValues = new std::array<float, 9>({
			4.0f ,2.0f, 1.0f,
			1.0f / 2, 1.0f / 4,1.0f / 8,
		1.0f / 16, 1.0f / 32, 1.0f / 64 });

		controls.add(std::make_shared<Dial>(*speedValues, 2, [&](float value)
			{
				processor.lfo->speed = value;
			}));

		controls.add(std::make_shared<ModeButton>(
			[&](float mode)
			{
				processor.lfo->setMode(mode);
			}
		));

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

