/*
  ==============================================================================

	mode-button.h
	Created: 8 Sep 2020 4:10:07pm
	Author:  callu

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "lfo.h"

struct ModeButton : public Component
{
	ModeButton(std::function<void(int)> callback)
	{
		this->callback = callback;
		this->mode = LFO::free;
		callback(mode);
		
		colors[LFO::sync] = 0xff76FF00;
		colors[LFO::oneshot] = 0xffFDFF58;
		colors[LFO::latch] = 0xff46D7FF;
		colors[LFO::free] = 0xffE478FF;

		labels[LFO::sync] = "sync";
		labels[LFO::oneshot] = "1shot";
		labels[LFO::latch] = "latch";
		labels[LFO::free] = "free";
		callback(mode);
	}
	void paint(Graphics& g) override
	{
		auto radius = (float)juce::jmin(getWidth() / 2, getHeight() / 2) - 4.0f;
		auto centreX = (float)getWidth() * 0.5f;
		auto centreY = (float)getHeight() * 0.5f;
		auto rx = centreX - radius;
		auto ry = centreY - radius;
		auto rw = radius * 2.0f;

		g.setColour(Colour(colors[mode]));
		g.fillEllipse(rx, ry, rw, rw);
		g.setColour(Colour(0xFF000000));
		g.drawText(String(labels[mode]), 0, 0, getWidth(), getHeight(), Justification::centred);
	};

	void mouseDown(const MouseEvent& event) override
	{
		mode++;
		mode %= 4;
		repaint();
		callback(mode);
	};

	std::array<int, 4> colors;
	std::array<String, 4> labels;
	int mode;
	std::function<void(int)> callback;
};
