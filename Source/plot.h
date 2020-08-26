/*
  ==============================================================================

	plot.h
	Created: 26 Aug 2020 1:53:08pm
	Author:  callu

  ==============================================================================
*/

#pragma once
#include<juceheader.h>

#include "playhead.h"

struct Plot : public Component {

	Plot()
	{
		addAndMakeVisible(playhead);
	};

	void paint(Graphics& g) override
	{
		drawBorder(g);
		drawGrid(g);
	}

	void resized() override
	{
		width = getWidth();
		height = getHeight();
		playhead.setBounds(0, 0, width, height);
	};

	void drawBorder(Graphics& g)
	{
		g.setColour(Colours::green);
		g.drawRect(0.0, 0.0, width, height);
	}

	void drawGrid(Graphics& g)
	{
		g.setColour(Colours::white);
		float dy = height / gridRes,
			dx = width / gridRes,
			y = 0,
			x = 0;
		for (int i = 0; i < gridRes; ++i)
		{
			x += dx;
			g.drawLine(x, 0, x, height);
			y += dy;
			g.drawLine(0, y, width, y);
		}
	}

	int gridRes = 16;
	float height;
	float width;
	Playhead playhead;
};
