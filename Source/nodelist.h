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

struct Edge;

struct CurvePoint : juce::Point<float>
{
	int targetIndex;
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
	float x;
	float y;
	float radius;
	bool isSelected;
	bool isBound;

private:
	int parentHeight = 0;
	int parentWidth = 0;
};

struct Edge : public Component
{
	using Point = juce::Point<float>;

	struct CtrlParam : Component
	{
		CtrlParam(Edge& path, float x, float y)
			:path(&path)
		{
			this->x = x;
			this->y = y;
		}
		void paint(Graphics& g) override
		{
			auto box = Rectangle<float>(0, 0, 8, 8);
			box.setCentre(getWidth() / 2, getHeight() / 2);
			//auto box = Rectangle<float>(0,0, getWidth(), getHeight());
			g.setColour(Colour(0xff53F70D));
			g.drawEllipse(box, 1);
		};

		void setPosition()
		{

			setBoundsRelative(x - radius, y - radius, radius * 2, radius * 2);
			//setBounds(50, 50, 50, 50);
		}

		void mouseDrag(const MouseEvent& event) override
		{
			juce::Point<int> mouse = getParentComponent()->getMouseXYRelative();
			float mouseY = (float)mouse.getY() / getParentHeight();
			y = std::clamp(mouseY, path->start->y, path->end->y);
			relY = y;
			setPosition();
			path->generatePlot();
		};

		float relY = 0.5;
		float x;
		float y;
		float radius = POINT_RADIUS;
		bool isSelected = false;
		Edge* path;
	};


	Edge(Node& start, Node& end);
	int widthToArrayLength(float Dx) const;
	void setControlPoints();
	void generatePlot();
	void updateControlParam();
	std::shared_ptr<CurvePoint> cubicBezier(Node& p0, Point& p1, Point& p2, Node& p3, float t) const;
	void moveControlParam(float y);
	void paint(Graphics& g) override;
	void resized() override;
	void updatePosition();

	Ptr<CtrlParam> ctrlParam;
	Point ctrl1;
	Point ctrl2;
	Node* start;
	Node* end;
	Array<float> plot;
	Array<std::shared_ptr<CurvePoint>> curvePoints;
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


