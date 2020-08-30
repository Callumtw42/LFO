/*
  ==============================================================================

	edge.cpp
	Created: 27 Aug 2020 3:48:31pm
	Author:  callu

  ==============================================================================
*/

#include "lfo.h"
#include "nodelist.h"

Edge::Edge(Node& start, Node& end)
{
	this->start = &start;
	this->end = &end;
	//this->plot = Array<float>();
	plot.resize(widthToArrayLength(end.x - start.x));
	//this->curvePoints = [];
	const auto ctrlX = start.x + (end.x - start.x) / 2;
	const auto ctrlY = start.y + (start.y - end.y) / 2;
	this->ctrlParam = std::make_unique<CtrlParam>(*this, ctrlX, ctrlY);
	addAndMakeVisible(ctrlParam.get());
}

int Edge::widthToArrayLength(float Dx) const
{
	const auto length = Dx * LFORES;
	return std::round(length);
}

void Edge::setControlPoints() {
	auto dy = end->y - start->y;
	auto dx = end->x - start->x;
	auto relY = ctrlParam->relY;

	auto x1 = end->x - dx * relY;
	auto y1 = start->y + dy * relY;
	ctrl1.setXY(x1, y1);

	auto x2 = end->x - dx * relY;
	auto y2 = start->y + dy * relY;
	ctrl2.setXY(x2, y2);

}

void Edge::generatePlot() {
	//auto curvePoints = this->curvePoints.get();
	plot.resize(widthToArrayLength(end->x - start->x));
	//const{ start, end, plot, cubicBezier } = this;
//const { ctrl1, ctrl2 } = this.getControlPoints();
	setControlPoints();
	const int curveRes = std::round(plot.size() / VALUES_PER_CURVE_POINT);
	curvePoints.resize(curveRes);
	for (int i = 0; i < curveRes; i++) {
		auto t = (float)i / curveRes;
		auto p = cubicBezier(*start, ctrl1, ctrl2, *end, t);
		auto relPosition = ((p->x - start->x) / (end->x - start->x));
		p->targetIndex = std::ceil(relPosition * (plot.size()));
		curvePoints.getReference(i) = std::move(p);
	}

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
			plot.getReference(i) = curvePoints.getReference(current)->y + j * dy;
			i++;
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
	const int centreIndex = std::round(plot.size() / 2);
	ctrlParam->y = plot.getReference(centreIndex);
	//ctrlParam->setPosition();
	repaint();
}

void Edge::updateControlParam() {
	//const { start, end, ctrlParam } = this;
	auto dx = end->x - start->x;
	auto dy = end->y - start->y;
	auto relY = ctrlParam->relY;

	ctrlParam->x = start->x + dx * 0.5;
	ctrlParam->y = start->y + dy * std::abs(relY);
	generatePlot();
	updatePosition();
}

std::shared_ptr<CurvePoint> Edge::cubicBezier(Node& p0, Point& p1, Point& p2, Node& p3, float t) const
{
	//pFinal = pFinal || {};
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
	//const { start, end } = this;
	auto maxY = std::max(start->y, end->y);
	auto minY = std::min(start->y, end->y);
	auto clampY = std::clamp<float>(y, minY, maxY);
	auto dy = end->y - start->y;
	auto yInBounds = clampY - end->y;
	ctrlParam->relY = (-yInBounds * 2 / dy) - 1;
	generatePlot();
}



void Edge::paint(Graphics& g)
{
	//g.setColour(Colours::yellow);
	//g.drawRect(0, 0, getWidth(), getHeight());
	auto h = getHeight();
	auto w = getWidth();
	g.setColour(Colours::red);
	for (int i = 0; i < plot.size(); ++i)
	{
		if (i % 32 == 0) {
			auto xPosition = (float)i / plot.size() * w;
			auto yPosition = plot.getReference(i) * h;
			g.fillRect(xPosition, yPosition, 1.0f, 1.0f);
		}
	}
	g.setColour(Colours::white);
	g.fillRect(ctrl1.x * w, ctrl1.y * h, 5.0f, 5.0f);
	g.setColour(Colours::pink);
	g.fillRect(ctrl2.x * w, ctrl2.y * h, 3.0f, 3.0f);



	//g.fillRect(ctrlParam->x*getWidth()-5, ctrlParam->y*getHeight()-5, 10.0, 10.0);





}

void Edge::resized()
{
	ctrlParam->setPosition();
}

void Edge::updatePosition()
{
	setBoundsRelative(start->x, 0, end->x, 1);
}

