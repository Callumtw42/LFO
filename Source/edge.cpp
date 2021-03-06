/*
  ==============================================================================

	edge.cpp
	Created: 27 Aug 2020 3:48:31pm
	Author:  callu

  ==============================================================================
*/

#include "edge.h"

#include "nodelist.h"

Edge::Edge(NodeList& nodeList, Node& start, Node& end)
{
	this->start = &start;
	this->end = &end;
	this->nodeList = &nodeList;
	auto newSize = widthToArrayLength(end.x - start.x);
	plot.resize(newSize);
	this->ctrlParam = std::make_unique<CtrlParam>(*this);
	generatePlot();
}

int Edge::widthToArrayLength(float Dx) const
{
	const auto length = Dx * LFORES;
	return std::round(length);
}

void Edge::setControlPoints() {
	auto Dy = end->y - start->y;
	auto Dx = end->x - start->x;
	auto relY = ctrlParam->value;
	auto x = end->x - Dx * relY;

	auto y = start->y + Dy * relY;

	ctrl1.setXY(x, y);
	ctrl2.setXY(x, y);
}

void Edge::setCurvePoints()
{
	const int curveRes = std::round(plot.size() / VALUES_PER_CURVE_POINT);
	curvePoints.resize(curveRes);
	for (int i = 0; i < curveRes; i++) {
		auto t = (float)i / curveRes;
		auto p = cubicBezier(*start, ctrl1, ctrl2, *end, t);
		auto relPosition = ((p->x - start->x) / (end->x - start->x));
		p->targetIndex = std::ceil(relPosition * (plot.size()));
		curvePoints.getReference(i) = std::move(p);
	}
}


void Edge::updateCtrlParam()
{
	auto size = plot.size();
	if (size > 2)
	{
		const int centreIndex = std::round(size / 2);
		ctrlParam->y = plot.getReference(centreIndex);
		//ctrlParam->y = ctrlParam->value;
	}
	else
	{
		ctrlParam->y = start->y + ctrlParam->value * (end->y - start->y);
	}
}

void Edge::generatePlot() {
	auto newSize = widthToArrayLength(end->x - start->x);
	plot.resize(newSize);
	setControlPoints();
	setCurvePoints();

	int current = 0;
	int next = 1;
	int i = 0;
	while (next < curvePoints.size())
	{
		const auto startIndex = curvePoints.getReference(current)->targetIndex;
		const auto targetIndex = curvePoints.getReference(next)->targetIndex;
		const auto startY = curvePoints.getReference(current)->y;
		const auto targetY = curvePoints.getReference(next)->y;
		const auto Dx = targetIndex - startIndex;
		const auto Dy = (targetY - startY);
		const auto dy = Dy / (Dx | 1);
		for (int j = 0; j < Dx; j++) {
			if (i < newSize)
			{
				plot.getReference(i) = curvePoints.getReference(current)->y + j * dy;
				i++;
			}
		}
		current++;
		next++;
	}
	if (curvePoints.size() != 0) {
		auto targetY = end->y;
		auto Dx = plot.size() - i;
		auto Dy = (targetY - curvePoints.getReference(current)->y);
		auto dy = Dy / (Dx | 1);
		for (int j = 0; j < Dx; j++) {
			plot.getReference(i) = curvePoints.getReference(current)->y + j * dy;
			i++;
		}
	}
	nodeList->updatePlot();
	updateCtrlParam();
	updatePosition();
	ctrlParam->setPosition();
}


std::shared_ptr<CurvePoint> Edge::cubicBezier(Node& p0, Point& p1, Point& p2, Node& p3, float t) const
{
	auto pFinal = std::make_shared<CurvePoint>();
	pFinal->x = std::pow(1 - t, 3) * p0.x +
		std::pow(1 - t, 2) * 3 * t * p1.x +
		(1 - t) * 3 * t * t * p2.x +
		t * t * t * p3.x;
	pFinal->y = std::pow(1 - t, 3) * p0.y +
		std::pow(1 - t, 2) * 3 * t * p1.y +
		(1 - t) * 3 * t * t * p2.y +
		t * t * t * p3.y;
	return std::move(pFinal);
}

void Edge::moveControlParam(float y)
{
	auto clampY = std::clamp<float>(y, getWidth(), getHeight());
	//auto dy = end->y - start->y;
	//auto yInBounds = clampY - end->y;
	ctrlParam->value = (-clampY * 2 / getHeight()) - 1;
	generatePlot();
}



void Edge::paint(Graphics& g)
{
	auto h = getHeight();
	auto w = getWidth();
	auto pw = getParentWidth();
	auto maxY = std::max(start->y, end->y);
	auto minY = std::min(start->y, end->y);
	g.setColour(GREEN);
	auto startPoint = juce::Point<float>(start->x, start->y);
	auto endPoint = juce::Point<float>(end->x, end->y);
	auto ctrl1Point = juce::Point<float>(ctrl1.getX(), ctrl1.getY());
	auto ctrl2Point = juce::Point<float>(ctrl2.getX(), ctrl2.getY());

	if (start->y == end->y)
		g.fillRect(0.0f, minY * getHeight(), (float)getWidth(), 1.0f);
	else if (start->x == end->x)
		g.fillRect(0.0f, minY * getHeight(), 1.0f, (maxY-minY) * getHeight());
	else {
		auto path = Path();
		path.startNewSubPath(startPoint);
		path.cubicTo(ctrl1Point, ctrl2Point, endPoint);
		path.scaleToFit(0, minY * getHeight(), getWidth(), (maxY - minY) * getHeight(), false);
		auto strokeType = PathStrokeType(1);
		g.strokePath(path, strokeType);
	}

	//for (int i = 0; i < plot.size(); ++i)
	//{
	//	auto currentX = (float)i / plot.size() * w;
	//	auto currentY = plot.getReference(i) * h;
	//	g.fillEllipse(currentX, currentY, 2.0f, 2.0f);
	//}

	/*##############--DEBUGGING--####################
	g.setColour(Colours::white);
	g.fillRect(ctrl1.x * pw, ctrl1.y * h, 5.0f, 5.0f);
	g.setColour(Colours::pink);
	g.fillRect(ctrl2.x * pw, ctrl2.y * h, 3.0f, 3.0f);
	auto* txt = new String(ctrlParam->value);
	g.drawText(*txt, 0, 0, getWidth(), getHeight(), Justification::centred);
	*/
}

void Edge::resized()
{
	ctrlParam->setPosition();
}

void Edge::updatePosition()
{
	auto minY = std::min(start->y, end->y);
	auto maxY = std::max(start->y, end->y);
	setBoundsRelative(start->x, 0, end->x - start->x, 1);
	if (start->x == end->x)setSize(getWidth() + 1, getHeight());
	if (start->y == end->y)setSize(getWidth(), getHeight() + 1);
}

void Edge::mouseDoubleClick(const MouseEvent& event)
{
	getParentComponent()->mouseDoubleClick(event);
}


