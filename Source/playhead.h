/*
  ==============================================================================

    playhead.h
    Created: 26 Aug 2020 3:37:30pm
    Author:  callu

  ==============================================================================
*/

#pragma once
#include<juceheader.h>

	struct Playhead : public Component
	{
		void paint(Graphics& g) override
		{
			float position = relPosition * width;
			//Logger::writeToLog(String(relPosition));
			g.setColour(Colours::yellow);
			g.drawLine(position, 0.0, position, height);
		};

		void resized() override
		{
			width = getWidth();
			height = getHeight();
		};

		void setPosition(float p)
		{
			relPosition = p;
			repaint();
		}

		float width = 0.0f;
		float height = 0.0f;
		float relPosition = 0.1f;
	};
