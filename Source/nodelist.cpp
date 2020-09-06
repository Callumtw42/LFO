/*
  ==============================================================================

	nodelist.cpp
	Created: 27 Aug 2020 3:37:42pm
	Author:  callu

  ==============================================================================
*/

#include "nodelist.h"
#include "edge.h"

NodeList::NodeList()
{
	head->path = std::make_unique<Edge>(*head.get(), *tail.get());
	addAndMakeVisible(head->path.get());
	addAndMakeVisible(head.get());
	addAndMakeVisible(tail.get());
	add(head);
	add(tail);
	head->rightNeighbour = tail.get();
	head->leftNeighbour = nullptr;
	tail->rightNeighbour = nullptr;
	tail->leftNeighbour = head.get();
}

void NodeList::mouseDoubleClick(const MouseEvent& event)
{
	auto mouse = getMouseXYRelative();
	auto x = (float)mouse.getX() / getWidth();
	auto y = (float)mouse.getY() / getHeight();
	auto newNode = std::make_shared<Node>(x, y, POINT_RADIUS, false);
	auto index = findLeftNeighbour(mouse.getX());
	insertAfter(index, std::move(newNode));
	auto* node = getReference(index + 1).get();
	addAndMakeVisible(node);
	node->updatePosition();

}

void NodeList::insertAfter(int index, SPtr<Node> node)
{
	auto* leftNeighbour = getReference(index).get();
	auto* rightNeighbour = leftNeighbour->rightNeighbour;

	node->leftNeighbour = leftNeighbour;
	leftNeighbour->rightNeighbour = node.get();
	node->rightNeighbour = rightNeighbour;
	rightNeighbour->leftNeighbour = node.get();

	leftNeighbour->path = std::make_unique<Edge>(*leftNeighbour, *node);
	addAndMakeVisible(leftNeighbour->path.get(), 0);
	leftNeighbour->path->generatePlot();

	node->path = std::make_unique<Edge>(*node, *rightNeighbour);
	addAndMakeVisible(node->path.get(), 0);
	node->path->generatePlot();

	insert(index + 1, node);
	for (int i = 0; i < size(); ++i)
	{
		getReference(i)->index = i;
	}
	dbg(leftNeighbour->index);
}

void NodeList::removeNode(int index) {
	auto* node = getReference(index).get();
	auto* leftNeighbour = node->leftNeighbour;
	auto* rightNeighbour = node->rightNeighbour;

	rightNeighbour->leftNeighbour = leftNeighbour;
	leftNeighbour->rightNeighbour = rightNeighbour;

	leftNeighbour->path = std::make_unique<Edge>(*leftNeighbour, *rightNeighbour);
	addAndMakeVisible(*leftNeighbour->path, 0);
	leftNeighbour->path->generatePlot();

	remove(index);
	for (int i = 0; i < size(); ++i)
	{
		getReference(i)->index = i;
	}
}

void NodeList::resized()
{
	for (int i = 0; i < size(); ++i)
	{
		auto* node = getReference(i).get();
		node->updatePosition();
		if (node->path)
			node->path->updatePosition();
	}
}

int NodeList::findLeftNeighbour(int mouseX)
{
	for (int i = 0; i < size(); i++)
	{
		auto currentNode = getReference(i).get();
		auto nodeX = currentNode->x * (float)getWidth();
		auto Dx = mouseX - nodeX;
		if (Dx < 0)
		{
			return i - 1;
		}
	}
	return 0;
}
