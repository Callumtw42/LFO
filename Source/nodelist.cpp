/*
  ==============================================================================

	nodelist.cpp
	Created: 27 Aug 2020 3:37:42pm
	Author:  callu

  ==============================================================================
*/

#include "nodelist.h"
#include "edge.h"

NodeList::NodeList(std::array<float, LFORES>& lfoPlot)
{
	this->lfoPlot = &lfoPlot;
	head->path = std::make_unique<Edge>(*this, *head.get(), *tail.get());
	addAndMakeVisible(head->path.get(), 0);
	addAndMakeVisible(head->path->ctrlParam.get(), 1);
	addAndMakeVisible(head.get());
	addAndMakeVisible(tail.get());
	add(head);
	add(tail);
	head->rightNeighbour = tail.get();
	head->leftNeighbour = nullptr;
	tail->rightNeighbour = nullptr;
	tail->leftNeighbour = head.get();
	auto newNode = std::make_shared<Node>(0.5, 0, POINT_RADIUS, false);
	addAndMakeVisible(newNode.get());
	insertAfter(0, std::move(newNode));
	updatePlot();
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

void NodeList::updatePlot()
{
	int i = 0;
	for (int j = 0; j < size(); ++j)
	{
		auto node = getReference(j);
		if (node->path)
		{
			auto plot = node->path->plot;
			auto size = plot.size();
			for (int k = 0; k < size; ++k)
			{
				if (i >= LFORES) break;
				auto value = plot.getReference(k);
				lfoPlot->at(i) = 1.0f - value;
				//dbg(lfoPlot[i);
				i++;
			}
		}
	}
}

void NodeList::insertAfter(int index, SPtr<Node> node)
{
	auto* leftNeighbour = getReference(index).get();
	auto* rightNeighbour = leftNeighbour->rightNeighbour;

	node->leftNeighbour = leftNeighbour;
	leftNeighbour->rightNeighbour = node.get();
	node->rightNeighbour = rightNeighbour;
	rightNeighbour->leftNeighbour = node.get();

	leftNeighbour->path = std::make_unique<Edge>(*this, *leftNeighbour, *node);
	addAndMakeVisible(leftNeighbour->path.get(), 0);
	addAndMakeVisible(leftNeighbour->path->ctrlParam.get(), 1);
	leftNeighbour->path->generatePlot();

	node->path = std::make_unique<Edge>(*this, *node, *rightNeighbour);
	addAndMakeVisible(node->path.get(), 0);
	addAndMakeVisible(node->path->ctrlParam.get(), 1);
	node->path->generatePlot();

	insert(index + 1, node);
	for (int i = 0; i < size(); ++i)
	{
		getReference(i)->index = i;
	}
	//dbg(leftNeighbour->index);
}

void NodeList::removeNode(int index) {
	auto* node = getReference(index).get();
	auto* leftNeighbour = node->leftNeighbour;
	auto* rightNeighbour = node->rightNeighbour;

	rightNeighbour->leftNeighbour = leftNeighbour;
	leftNeighbour->rightNeighbour = rightNeighbour;

	leftNeighbour->path = std::make_unique<Edge>(*this, *leftNeighbour, *rightNeighbour);
	addAndMakeVisible(*leftNeighbour->path, 0);
	addAndMakeVisible(*leftNeighbour->path->ctrlParam, 1);
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
