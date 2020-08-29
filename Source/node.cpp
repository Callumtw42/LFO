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
	this->relX = x;
	this->relY = y;
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
		? std::clamp(x, leftNeighbour->x, rightNeighbour->x)
		: x;
	if (isBound) {
		setY(clampY);
	}
	else {
		setX(clampX);
		setY(clampY);
	}
	updatePosition();
	//if (path) path->updateControlParam();
	//if (leftNeighbour) leftNeighbour->path->updateControlParam();
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
	setBoundsRelative(relX - radius, relY - radius, radius * 2, radius * 2);
}


void Node::setX(int x)
{
	this->x = x;
	this->relX = (float)x / getParentWidth();
}

void Node::setY(int y)
{
	this->y = y;
	this->relY = (float)y / getParentHeight();
}

void Node::mouseDrag(const MouseEvent& event)
{
	Point<int> mouse = getParentComponent()->getMouseXYRelative();
	move(mouse.getX(), mouse.getY());
}
