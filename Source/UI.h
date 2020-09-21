#pragma once
#include <JuceHeader.h>
#include "plot.h"
#include "controlbox.h"
#include "mode-button.h"

struct Processor;
const int CTRL_HEIGHT = 50;
const int CTRL_WIDTh = 200;


class UI :public Component
{
public:
	UI(Processor& processor, LFO& lfo);
	void resized() override;;

	UPtr<Plot> plot;
	Controls controls;
	Processor* processor;
	UPtr<SliderParameterAttachment> speedAttachment;
};

