/*
  ==============================================================================

	UI.cpp
	Created: 21 Sep 2020 6:06:02am
	Author:  callu

  ==============================================================================
*/

#include "UI.h"
#include "Processor.h"

UI::UI(Processor& processor, LFO& lfo) :
	processor(&processor)
{
	this->plot = std::make_unique<Plot>(lfo.plot);
	addAndMakeVisible(plot.get());
	addAndMakeVisible(controls);

	auto gridParam = processor.apvs->getParameter("grid");
	auto gridDial = std::make_shared<Dial>(*gridParam, [&](int v)
		{
			plot->gridRes = v;
			plot->repaint();
		});
	controls.add(gridDial);
	
	auto speedParam = processor.apvs->getParameter("speed");
	auto speedDial = std::make_shared<Dial>(*speedParam, [&](int value)
		{
			processor.lfo->speed = processor.lfo->speedValues[value];
		});
	controls.add(speedDial);
	
	controls.add(std::make_shared<ModeButton>(
		[&](float mode)
		{
			lfo.setMode(mode);
		}
	));
	
} void UI::resized()
{
	plot->setBounds(0, 0, getWidth(), getHeight() - CTRL_HEIGHT);
	controls.setBounds(0, getHeight() - CTRL_HEIGHT, getWidth(), CTRL_HEIGHT);
}
