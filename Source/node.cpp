/*
  ==============================================================================

	node.cpp
	Created: 27 Aug 2020 3:48:25pm
	Author:  callu

  ==============================================================================
*/

#include "nodelist.h"


Node::Node(float x, float y, float radius, bool isBound)
{
	this->x = x;
	this->y = y;
	//setXY(x, y);
	this->radius = radius;
	this->isSelected = false;
	this->isBound = isBound;
	this->leftNeighbour = nullptr;
	this->rightNeighbour = nullptr;
	this->path = nullptr;
}

void Node::move(float x, float y)
{
	float clampY = std::clamp(y, 0.0f, (float)getParentHeight());
	float clampX = (leftNeighbour && rightNeighbour)
		? std::clamp(x, leftNeighbour->x, rightNeighbour->x)
		: x;

	y = clampY;
	if (!isBound)
		x = clampX;
	if (path) path->updateControlParam();
	if (leftNeighbour) leftNeighbour->path->updateControlParam();
}

void Node::paint(Graphics& g)
{
	float height = getHeight();
	float width = getWidth();
	g.setColour(Colours::green);
	g.fillEllipse(0, 0, width, height);
}

void Node::resized()
{
}
