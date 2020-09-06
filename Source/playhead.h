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
		Playhead()
		{
			getParentComponent()->addAndMakeVisible(*this);
			setBoundsRelative(0, 0, 1, 1);
		}
		void paint(Graphics& g) override
		{
			float position = relPosition * getWidth();
			//Logger::writeToLog(String(relPosition));
			g.setColour(Colours::yellow);
			g.drawLine(position, 0.0, position, getHeight());
		};

		void setPosition(float p)
		{
			relPosition = p;
			repaint();
		}

		float relPosition = 0.1f;
	};
