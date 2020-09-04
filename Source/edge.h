/*
  ==============================================================================

	edge.h
	Created: 4 Sep 2020 2:28:14pm
	Author:  callu

  ==============================================================================
*/

#pragma once
#include "lfo.h"
#include "node.h"
#include "utils.h"
#include <algorithm>

const int VALUES_PER_CURVE_POINT = 4;

struct CurvePoint : juce::Point<float>
{
	int targetIndex;
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
			relY = std::clamp(mouseY, 0.0f, 1.0f);
			auto Dy = path->end->y - path->start->y;
			//relY = y;
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
	void mouseDoubleClick(const MouseEvent& event) override;

	Ptr<CtrlParam> ctrlParam;
	Point ctrl1;
	Point ctrl2;
	Node* start;
	Node* end;
	Array<float> plot;
	Array<std::shared_ptr<CurvePoint>> curvePoints;
};
