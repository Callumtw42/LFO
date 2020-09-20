/*
  ==============================================================================

	node.cpp
	Created: 27 Aug 2020 3:48:25pm
	Author:  callu

  ==============================================================================
*/

#include "node.h"
#include "edge.h"
#include "nodelist.h"
#include "plot.h"

float snap(float v, float oldMin, float oldMax, float newMin, float newMax)
{
	auto oldRange = oldMax - oldMin;
	auto vRel = (v - oldMin) / oldRange;
	auto newRange = newMax - newMin;
	auto newVal = std::round(newRange * vRel) + newMin;
	return newVal;
}

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
	setWantsKeyboardFocus(true);
}

Point<float> Node::snapToGrid(int x, int y, int gridRes)
{
	auto maxX = getParentWidth();
	auto minX = 0;
	auto snappedX = snap(x, minX, maxX, 0, gridRes);

	auto maxY = getParentHeight();
	auto minY = 0;
	auto snappedY = snap(y, minY, maxY, 0, gridRes);

	auto newX = snappedX / gridRes * maxX;
	auto newY = snappedY / gridRes * maxY;

	Point<float> out = Point(newX, newY);
	return out;
}

void Node::move(int mouseX, int mouseY)
{


	int clampY = std::clamp(mouseY, 0, getParentHeight());
	int clampX = (leftNeighbour && rightNeighbour)
		? std::clamp((float)mouseX, (leftNeighbour->x * getParentWidth()), (rightNeighbour->x * getParentWidth()))
		: mouseX;
	if (isBound) {
		setY(clampY);
	}
	else {
		setX(clampX);
		setY(clampY);
	}

	updatePosition();

	if (path)
		path->generatePlot();
	if (leftNeighbour)
		leftNeighbour->path->generatePlot();
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
	if (path) path->updatePosition();
}

void Node::updatePosition()
{
	auto p = getParentComponent();
	auto w = p->getWidth();
	auto h = p->getHeight();
	auto px = p->getX();
	auto py = p->getY();
	setBounds(px + x * w - 4.0f, y * h - 4.0f, 8, 8);
}


void Node::setX(int x)
{
	this->x = (float)x / getParentWidth();
}

void Node::setY(int y)
{
	this->y = (float)y / getParentHeight();
}

void Node::createPath()
{
	//getParentComponent()->addAndMakeVisible(path.get(), 0);
	//path->updatePosition();
	//path->generatePlot();
	//repaint();
}


void Node::mouseDrag(const MouseEvent& event)
{
	Point<int> mouse = getParentComponent()->getMouseXYRelative();
	if (event.mods.isAltDown())
	{
		move(mouse.getX(), mouse.getY());
	}
	else
	{
		auto* plot = dynamic_cast<Plot*>(getParentComponent()->getParentComponent());
		auto snapped = snapToGrid(mouse.getX(), mouse.getY(), plot->gridRes);
		move(snapped.getX(), snapped.getY());
	}
}

bool Node::keyPressed(const KeyPress& key)
{
	//if (key.getModifiers().isAltDown())
	return true;
}

void Node::mouseDoubleClick(const MouseEvent& event)
{
	auto parent = dynamic_cast<NodeList*>(getParentComponent());
	if (!isBound) parent->removeNode(index);
}
