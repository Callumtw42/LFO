/*
  ==============================================================================

    node.h
    Created: 4 Sep 2020 2:28:06pm
    Author:  callu

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
struct Edge;
const float POINT_RADIUS = 0.04f;

struct Node :public Component
{
	Node(float x, float y, float radius, bool isBound);
	void move(int x, int y);
	void paint(Graphics& g) override;
	void resized() override;
	void mouseDrag(const MouseEvent& event) override;
	void updatePosition();
	void setX(int x);
	void setY(int y);

	std::unique_ptr<Edge> path;
	Node* rightNeighbour;
	Node* leftNeighbour;
	float x;
	float y;
	float radius;
	bool isSelected;
	bool isBound;

private:
	int parentHeight = 0;
	int parentWidth = 0;
};

