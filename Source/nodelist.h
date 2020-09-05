/*
  ==============================================================================

	nodelist.h
	Created: 26 Aug 2020 3:39:07pm
	Author:  callu

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "utils.h"
#include "edge.h"

struct NodeList : public Component, Array<SPtr<Node>>
{
	NodeList();
	void insertAfter(int index, SPtr<Node> node);
	void removeNode(int index);
	void resized() override;
	int findLeftNeighbour(int mouseX);
	void mouseDoubleClick(const MouseEvent& event) override;

	SPtr<Node> head = std::make_unique<Node>(0.0f, 1.0f, POINT_RADIUS, true);
	SPtr<Node> tail = std::make_unique<Node>(1.0f, 1.0f, POINT_RADIUS, true);
};


