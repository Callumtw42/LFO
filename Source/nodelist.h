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
const float POINT_RADIUS = 0.04f;
const int VALUES_PER_CURVE_POINT = 4;

struct Node;

struct Edge : public Component
{
	using Point = juce::Point<float>;

	struct CtrlParam
	{
		CtrlParam(float x, float y)
		{
			this->x = x;
			this->y = y;
		}

		float relY = 0.5;
		float x;
		float y;
		float radius = POINT_RADIUS;
		bool isSelected = false;
	};

	struct CurvePoint : Point
	{
		int targetIndex;
	};

	Edge(Node& start, Node& end);
	int widthToArrayLength(float Dx);
	void setControlPoints();
	void generatePlot();
	void updateControlParam();
	CurvePoint* cubicBezier(Node& p0, Point& p1, Point& p2, Node& p3, float t);
	void moveControlParam(float y);
	void paint(Graphics& g) override;
	void resized() override;

	Ptr<CtrlParam> ctrlParam;
	Point ctrl1;
	Point ctrl2;
	Node* start;
	Node* end;
	Ptr<Array<float>> plot = std::make_unique<Array<float>>();
	Ptr<Array<CurvePoint*>> curvePoints = std::make_unique <Array<CurvePoint*>>();
};

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
	int x;
	int y;
	float relX;
	float relY;
	float radius;
	bool isSelected;
	bool isBound;

private:
	int parentHeight = 0;
	int parentWidth = 0;
};

struct NodeList : public Component, Array<Node*>
{
	NodeList();
	void insertAfter(int index, Node& node);
	void removeNode(int index);
	void resized() override;

	Node head = Node(0, 1, POINT_RADIUS, true);
	Node tail = Node(1, 1, POINT_RADIUS, true);
};


