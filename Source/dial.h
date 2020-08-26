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
	template<typename T>
	Dial(T min, T max, T step, T init, std::function<void(float)> callback) :
		callback(callback)
	{
		static_assert((std::is_same<T, float>::value || std::is_same<T, int>::value), "floats and integers only");

		slider.setLookAndFeel(&style);
		slider.setRange(min, max, step);
		slider.onValueChange = [&]()
		{
			float value = slider.getValue();
			valueString.setText(slider.getTextFromValue((T)value));
			this->callback(value);
		};
		slider.setValue(init);
		callback(init);
		addAndMakeVisible(slider);
		valueString.setText(slider.getTextFromValue((T)slider.getValue()));
		addAndMakeVisible(valueString);
	}

	template<int L, typename T>
	Dial(std::array<T, L>& values, int initIndex, std::function<void(float)>callback) :
		callback(callback)
	{
		static_assert((std::is_same<T, float>::value || std::is_same<T, int>::value), "floats and integers only");
		slider.setLookAndFeel(&style);
		slider.setRange(0, L - 1, 1);
		slider.onValueChange = [&]()
		{
			int index = slider.getValue();
			valueString.setText(String(values[index]));
			this->callback(values[index]);
		};
		slider.setValue(initIndex);
		callback(values[initIndex]);
		addAndMakeVisible(slider);
		valueString.setText(slider.getTextFromValue((T)slider.getValue()));
		addAndMakeVisible(valueString);
	};

	void resized() override
	{
		width = getWidth();
		height = getHeight();
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
	float width;
	float height;
	DrawableText valueString;
	std::function<void(float)> callback;
};
