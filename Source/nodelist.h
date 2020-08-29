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

//private:
//	float plotWidth = (float)getParentWidth();
};

struct Node :public Component
{
	Node(float x, float y, float radius, bool isBound);
	void move(float x, float y);
	void paint(Graphics& g) override;
	void resized() override;

	std::unique_ptr<Edge> path;
	Node* rightNeighbour;
	Node* leftNeighbour;
	float x;
	float y;
	float radius;
	bool isSelected;
	bool isBound;
};

struct Thing : Component
{
	void paint(Graphics& g) override
	{
		g.setColour(Colours::red);
		g.fillRect(0, 0, getHeight(), getWidth());
	};	
};

struct NodeList : public Component, Array<Node*>
{
	NodeList(Node& head, Node& tail);
	void insertAfter(int index, Node& node);
	void removeNode(int index);
	void resized() override;
	void paint(Graphics& g) override;

	float height = 0.0f;
	float width = 0.0f;
	Thing* thing;

	//forEach(...args) 
	//indexOf(...args) 
	//splice(...args) 
	//map(...args) 
	//get()
	//juce::Array<Node*> nodes;
};


