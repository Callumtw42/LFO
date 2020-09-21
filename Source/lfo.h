/*
  ==============================================================================

	lfo.h
	Created: 12 Jul 2020 11:03:20pm
	Author:  callu

  ==============================================================================
*/


#pragma once
#define _USE_MATH_DEFINES
#include <array>
#include <JuceHeader.h>
#include <math.h>
#include <functional>

#include "utils.h"

inline AudioPlayHead::CurrentPositionInfo playheadPosition;
inline double bpm = 128;
static const int LFORES = 4096;
const int INIT_SPEED = 1.0f / 4;

class LFO : Timer
{
public:
	LFO()
		:endPoint(nullptr)
	{
		plot.fill(0);
		Timer::startTimer(16);

		speedValues = Array<float>({
			32.0f, 16.0f, 8.0f,
			4.0f ,2.0f, 1.0f,
			1.0f / 2, 1.0f / 4,1.0f / 8,
			1.0f / 16, 1.0f / 32, 1.0f / 64,
			1.0f / 128, 1.0f / 256, 1.0f / 512
			});

		speedLabels = StringArray({
			"32", "16", "8",
			"4", "2", "1",
			"1/2", "1/4", "1/8",
			"1/16", "1/32", "1/64",
			"1/128", "1/256", "fast",
			});
	};

	~LFO() {};

	void setEndPoint(AudioProcessorParameter* endPoint)
	{
		this->endPoint = endPoint;
	}

	void timerCallback() override
	{
		callback(currentIndex);
	};

	void start()
	{

		if (mode == free) {
			updateStartTime();
			std::thread proc([this]()
				{
					auto start = startTime.time_since_epoch().count();
					while (mode == free && isUpToDate(start))
					{
						process(startTime);
						std::this_thread::sleep_for(std::chrono::microseconds(minInterval));
					}
					currentIndex = 0;
				});
			proc.detach();
		}

		else if (mode == sync && playheadPosition.timeInSeconds > -1)
		{
			updateStartTime();
			std::thread proc([this]()
				{
					auto start = startTime.time_since_epoch().count();
					while (mode == sync && isUpToDate(start))
					{
						processSync(startTime);
						std::this_thread::sleep_for(std::chrono::microseconds(minInterval));
					}
					currentIndex = 0;
				});
			proc.detach();
		}
	}

	void trigger(bool isNoteOn)
	{

		if (mode == oneshot && isNoteOn) {
			updateStartTime();
			std::thread proc([this]()
				{
					auto start = startTime.time_since_epoch().count();
					double microSecondsPerBeat = (60.0 / bpm) * 1000000;
					double barLength = microSecondsPerBeat * 4;
					auto playDuration = barLength * speed;
					auto microsecondsElapsed = 0;

					while (mode == oneshot && microsecondsElapsed < playDuration && isUpToDate(start))
					{
						auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
						microsecondsElapsed = (float)elapsedTime.count() / 1000;
						process(startTime);
						std::this_thread::sleep_for(std::chrono::microseconds(minInterval));
					}
					currentIndex = 0;
				});
			proc.detach();
		}

		else if (mode == latch) {
			updateStartTime();
			noteOn = isNoteOn;
			std::thread proc([this]()
				{
					auto start = startTime.time_since_epoch().count();
					while (mode == latch && noteOn && isUpToDate(start))
					{
						process(startTime);
						std::this_thread::sleep_for(std::chrono::microseconds(minInterval));
					}
					currentIndex = 0;
				});
			proc.detach();
		}
	}

	void updateStartTime()
	{
		startTime = std::chrono::high_resolution_clock::now();
	}

	bool isUpToDate(long long start)
	{
		return startTime.time_since_epoch().count() == start;
	}

	void processSync(std::chrono::steady_clock::time_point startTime)
	{
		double microSecondsPerBeat = (60.0 / bpm) * 1000000;
		double barLength = microSecondsPerBeat * 4;
		auto elapsedTime =
			playheadPosition.timeInSeconds * 1000000000;
		double microsecondsElapsed = elapsedTime / 1000;
		double barsElapsed = microsecondsElapsed / barLength;
		double intervalsElapsed = barsElapsed / speed;
		int rounded = std::round(intervalsElapsed * LFORES);
		int index = rounded % LFORES;
		currentIndex = index;
		double lfoVal = plot[index];

		auto dB = 20.0f * std::log10(lfoVal);
		auto outVal = (std::clamp(dB, -40.0, 0.0) + 40) / 40;

		if (endPoint)endPoint->setValue(lfoVal);
	}

	void process(std::chrono::steady_clock::time_point startTime)
	{
		double microSecondsPerBeat = (60.0 / bpm) * 1000000;
		double barLength = microSecondsPerBeat * 4;
		auto elapsedTime =
			std::chrono::high_resolution_clock::now().time_since_epoch().count() - startTime.time_since_epoch().count();
		double microsecondsElapsed = elapsedTime / 1000;
		double barsElapsed = microsecondsElapsed / barLength;
		double intervalsElapsed = barsElapsed / speed;
		int rounded = std::round(intervalsElapsed * LFORES);
		int index = rounded % LFORES;
		currentIndex = index;
		double lfoVal = plot[index];

		auto dB = 20.0f * std::log10(lfoVal);
		auto outVal = (std::clamp(dB, -40.0, 0.0) + 40) / 40;

		if (endPoint)endPoint->setValue(lfoVal);
	}

	void setMode(int mode)
	{
		this->mode = mode;
		if (endPoint != nullptr && (mode == free || mode == sync)) start();
	}

	int minInterval = 1000;
	double speed = 1.0;
	std::array<float, LFORES> plot;
	AudioProcessorParameter* endPoint;
	bool noteOn = false;
	enum { sync, oneshot, latch, free } modes;
	int mode = 0;
	std::function<void(int)> callback = [](int i) {};
	int currentIndex = 0;
	std::chrono::steady_clock::time_point startTime;
	Array<float> speedValues;
	StringArray speedLabels;
};
