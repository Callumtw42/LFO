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

struct NodeList : public Component, Array<Node*>
{
	NodeList();
	void insertAfter(int index, Node& node);
	void removeNode(int index);
	void resized() override;
	int findLeftNeighbour(int mouseX);

	Node head = Node(0, 1, POINT_RADIUS, true);
	Node tail = Node(1, 1, POINT_RADIUS, true);
};


