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

struct NodeList;
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
		CtrlParam(Edge& path)
			:path(&path)
		{
			getParentComponent()->addAndMakeVisible(*this);
			this->x = 0.5;
			this->y = 0.5;
		}
		void paint(Graphics& g) override
		{
			auto box = Rectangle<float>(0, 0, 8, 8);
			box.setCentre(getWidth() / 2, getHeight() / 2);
			g.setColour(Colour(0xff53F70D));
			g.drawEllipse(box, 1);
		};

		void setPosition()
		{
			auto w = path->getWidth();
			auto h = path->getHeight();
			auto px = path->getX();
			auto py = path->getY();
			setBounds(px + x * w - 4.0f, y * h - 4.0f, 8, 8);

		}

		void mouseDown(const MouseEvent& event) override
		{
			current = value;
		};

		void mouseDrag(const MouseEvent& event) override
		{
			auto distance = (float)event.getDistanceFromDragStartY() / getParentHeight();
			auto Dy = path->end->y - path->start->y;
			value = (Dy >= 0)
				? std::clamp(current + distance, 0.0f, 1.0f)
				: std::clamp(current -distance, 0.0f, 1.0f);
			path->generatePlot();
		}

		float value = 0.5;
		float x;
		float y;
		float radius = POINT_RADIUS;
		bool isSelected = false;
		Edge* path;

	private:
		float current;
	};


	Edge(NodeList& nodeList, Node& start, Node& end);
	int widthToArrayLength(float Dx) const;
	void setControlPoints();
	void generatePlot();
	std::shared_ptr<CurvePoint> cubicBezier(Node& p0, Point& p1, Point& p2, Node& p3, float t) const;
	void moveControlParam(float y);
	void paint(Graphics& g) override;
	void resized() override;
	void updatePosition();
	void mouseDoubleClick(const MouseEvent& event) override;

	UPtr<CtrlParam> ctrlParam;
	Point ctrl1;
	Point ctrl2;
	Node* start;
	Node* end;
	Array<float> plot;
	Array<std::shared_ptr<CurvePoint>> curvePoints;

private:
	void setCurvePoints();
	void updateCtrlParam();
	NodeList* nodeList;
};
