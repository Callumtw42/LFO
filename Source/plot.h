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
#include "nodelist.h"

const float REL_PLOTHEIGHT = 0.75;

struct Plot : public Component
{
	Plot(std::array<float, LFORES>& lfoPlot)
	{
		nodeList = std::make_unique<NodeList>(lfoPlot);
		addAndMakeVisible(playhead);
		addAndMakeVisible(nodeList.get());
	}

	void paint(Graphics& g) override
	{
		drawBorder(g);
		drawGrid(g);
	}


	void resized() override
	{
		playhead.setBoundsRelative(0, 0, 1, 1);
		nodeList->setBoundsRelative(0, 0, 1, 1);
	};

	void drawBorder(Graphics& g)
	{
		g.setColour(Colours::green);
		g.drawRect(0.0, 0.0, getWidth(), getHeight());
	}

	void drawGrid(Graphics& g)
	{
		auto height = (float)getHeight();
		auto width = (float)getWidth();
		g.setColour(GREY);
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
	Playhead playhead;
	UPtr<NodeList> nodeList;


};
