/*
  ==============================================================================

    controlbox.h
    Created: 26 Aug 2020 1:53:39pm
    Author:  callu

  ==============================================================================
*/

#pragma once
#include<juceheader.h>
#include "dial.h"

	struct Controls : public Component
	{
		Controls()
		{
		}

		void addDial(Dial* dial)
		{
			dials.push_back(dial);
			addAndMakeVisible(*dial);
		}

		void resized() override
		{
			width = getWidth();
			height = getHeight();
			const float dialSize = height;
			for (int i = 0; i < dials.size(); ++i)
			{
				float disp = i * dialSize;
				dials[i]->setBounds(disp, 0, dialSize, dialSize);
			}
		}
		std::vector<Dial*> dials;
		float width;
		float height;
	};

