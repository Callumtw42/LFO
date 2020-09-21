/*
  ==============================================================================

	dial.h
	Created: 26 Aug 2020 1:53:49pm
	Author:  callu

  ==============================================================================
*/

#pragma once
#include<JuceHeader.h>

struct Dial : Component
{
	Dial(RangedAudioParameter& param, std::function<void(float)> callback)
	{
		this->attachment = std::make_unique<SliderParameterAttachment>(param, this->slider);
		this->callback = callback;
		slider.setLookAndFeel(&style);
		slider.onValueChange = [&]()
		{
			float value = slider.getValue();
			valueString.setText(slider.getTextFromValue(value));
			this->callback(value);
		};
		addAndMakeVisible(slider);
		valueString.setText(slider.getTextFromValue(slider.getValue()));
		addAndMakeVisible(valueString);
	}

	void resized() override
	{
		auto width = getWidth();
		auto height = getHeight();
		slider.setSize(height, height);
		valueString.setBoundingBox(Parallelogram<float>(
			Rectangle<float>(
				0.0f, height / 3,
				width,
				valueString.getFontHeight()
				)));
		valueString.setJustification(Justification::centred);
	}

	struct Style : LookAndFeel_V4
	{
		void drawRotarySlider(Graphics& g, int x, int y, int width, int height,
			float sliderPosProportional, float rotaryStartAngle,
			float rotaryEndAngle, Slider&) override
		{
			auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
			auto centreX = (float)x + (float)width * 0.5f;
			auto centreY = (float)y + (float)height * 0.5f;
			auto rx = centreX - radius;
			auto ry = centreY - radius;
			auto rw = radius * 2.0f;
			auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

			// fill
			g.setColour(juce::Colours::orange);
			g.fillEllipse(rx, ry, rw, rw);

			// outline
			g.setColour(juce::Colours::red);
			g.drawEllipse(rx, ry, rw, rw, 1.0f);

			juce::Path p;
			auto pointerLength = radius * 0.33f;
			auto pointerThickness = 2.0f;
			p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
			p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

			// pointer
			g.setColour(juce::Colours::yellow);
			g.fillPath(p);
		}
	};
	Style style;
	Slider slider = Slider(Slider::RotaryVerticalDrag, Slider::NoTextBox);
	DrawableText valueString;
	std::function<void(float)> callback;
	UPtr<SliderParameterAttachment> attachment;
};
