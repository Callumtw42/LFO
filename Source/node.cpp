/*
  ==============================================================================

	node.cpp
	Created: 27 Aug 2020 3:48:25pm
	Author:  callu

  ==============================================================================
*/

#include "node.h"
#include "edge.h"

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

void Node::move(int x, int y)
{
	int clampY = std::clamp(y, 0, getParentHeight());
	int clampX = (leftNeighbour && rightNeighbour)
		? std::clamp(x, (int)leftNeighbour->x * getParentWidth(), (int)rightNeighbour->x * getParentWidth())
		: x;
	if (isBound) {
		setY(clampY);
	}
	else {
		setX(clampX);
		setY(clampY);
	}
	updatePosition();
	if (path) path->updateControlParam();
	if (leftNeighbour) leftNeighbour->path->updateControlParam();
}

void Node::paint(Graphics& g)
{
	int height = getHeight();
	int width = getWidth();
	auto box = Rectangle<float>(0, 0, 10, 10);
	box.setCentre((float)width / 2, (float)height / 2);
	g.setColour(Colour(0xff53F70D));
	g.fillEllipse(box);
}

void Node::resized()
{
	updatePosition();
}

void Node::updatePosition()
{
	setBoundsRelative(x - radius, y - radius, radius * 2, radius * 2);
}


void Node::setX(int x)
{
	this->x = (float)x / getParentWidth();
}

void Node::setY(int y)
{
	this->y = (float)y / getParentHeight();
}

void Node::mouseDrag(const MouseEvent& event)
{
	Point<int> mouse = getParentComponent()->getMouseXYRelative();
	move(mouse.getX(), mouse.getY());
	//if (path) path->generatePlot();
}
