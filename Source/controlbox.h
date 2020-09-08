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

	struct Controls : public Component, std::vector<SPtr<Component>>
	{
		void add(SPtr<Component> ctrl)
		{
			push_back(ctrl);
			addAndMakeVisible(ctrl.get());
		}

		void resized() override
		{
			const auto ctrlSize = getHeight();
			for (int i = 0; i < size(); ++i)
			{
				float disp = i * ctrlSize;
				this->at(i)->setBounds(disp, 0, ctrlSize, ctrlSize);
			}
		}
	};

