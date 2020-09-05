/*
  ==============================================================================

	nodelist.cpp
	Created: 27 Aug 2020 3:37:42pm
	Author:  callu

  ==============================================================================
*/

#include "nodelist.h"

NodeList::NodeList()
{
	head->path = std::make_unique<Edge>(*head.get(), *tail.get());
	addAndMakeVisible(head->path.get());
	addAndMakeVisible(head.get());
	add(head);
	addAndMakeVisible(tail.get());
	add(tail);
	head->rightNeighbour = tail.get();
	head->leftNeighbour = nullptr;
	tail->rightNeighbour = nullptr;
	tail->leftNeighbour = head.get();
}

void NodeList::mouseDoubleClick(const MouseEvent& event)
{
	auto mouse = event.getMouseDownPosition();
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
	
	leftNeighbour->createPath();
	node->createPath();
	
	insert(index + 1, node);
	for (int i = 0; i < size(); ++i)
	{
		getReference(i)->index = i;
	}
}

void NodeList::removeNode(int index) {
	auto* node = getReference(index).get();
	auto* leftNeighbour = node->leftNeighbour;
	auto* rightNeighbour = node->rightNeighbour;
	
	rightNeighbour->leftNeighbour = leftNeighbour;
	leftNeighbour->rightNeighbour = rightNeighbour;
	leftNeighbour->createPath();
	
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
		auto Dx = mouseX - currentNode->getX();
		if (Dx < 0)
		{
			return i - 1;
		}
	}
	return 0;
}
