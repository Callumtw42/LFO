/*
  ==============================================================================

	nodelist.cpp
	Created: 27 Aug 2020 3:37:42pm
	Author:  callu

  ==============================================================================
*/

#include "nodelist.h"

NodeList::NodeList(Node& head, Node& tail)
{
	this->thing = new Thing();
	addAndMakeVisible(thing);
	head.path = std::make_unique<Edge>(head, tail);
	addAndMakeVisible(&head);
	add(&head);
	addAndMakeVisible(&tail);
	add(&tail);
	head.rightNeighbour = &tail;
	head.leftNeighbour = nullptr;
	tail.rightNeighbour = nullptr;
	tail.leftNeighbour = &head;
}


void NodeList::insertAfter(int index, Node& node)
{
	auto* leftNeighbour = getReference(index);
	auto* rightNeighbour = leftNeighbour->rightNeighbour;
	insert(index + 1, &node);
	node.leftNeighbour = leftNeighbour;
	leftNeighbour->rightNeighbour = &node;
	node.rightNeighbour = rightNeighbour;
	rightNeighbour->leftNeighbour = &node;
	leftNeighbour->path = std::make_unique<Edge>(*leftNeighbour, node);
	leftNeighbour->path->updateControlParam();
	node.path = std::make_unique<Edge>(node, *rightNeighbour);
	node.path->updateControlParam();
}


void NodeList::removeNode(int index) {
	Node* point = getReference(index);
	Node* leftNeighbour = point->leftNeighbour;
	Node* rightNeighbour = point->rightNeighbour;
	leftNeighbour->path = std::make_unique<Edge>(*leftNeighbour, *rightNeighbour);
	remove(index);
	getReference(index)->leftNeighbour = leftNeighbour;
	leftNeighbour->path->updateControlParam();
	getReference(index - 1)->rightNeighbour = rightNeighbour;
}

void NodeList::resized()
{
	height = getHeight();
	width = getWidth();

	for (int i = 0; i < size(); ++i)
	{
		auto* node = getReference(i);
		auto radius = height * POINT_RADIUS;
		auto diameter = radius * 2;
		auto x = std::round(node->x * width) - radius;
		auto y = std::round(node->y * height) - radius;
		node->setBounds(x, y, diameter, diameter);
	}

	//thing->setBounds(0, 0, height, width);

}

void NodeList::paint(Graphics& g)
{

	//g.setColour(Colours::red);
	//g.drawRect(0.0f, 0.0f, width, height);
}

//forEach(...args) {
	//return this.nodes.forEach(...args);
//}

//indexOf(...args) {
//    return this.nodes.indexOf(...args);
//}

//splice(...args) {
//    return this.nodes.splice(...args);
//}

//map(...args) {
//    return this.nodes.map(...args);
//}

//get() {
//    return this.nodes;
//}
