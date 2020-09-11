/*
  ==============================================================================

	lfo.h
	Created: 12 Jul 2020 11:03:20pm
	Author:  callu

  ==============================================================================
*/

//static const int DEFAULT_TEMPO = 128;

#pragma once
#define _USE_MATH_DEFINES
#include <array>
#include <JuceHeader.h>
#include <math.h>
#include <functional>

#include "utils.h"

inline AudioPlayHead::CurrentPositionInfo playheadPosition;
inline double bpm = 128;
static const int LFORES = 1024;

class LFO : Timer
{
public:
	LFO(AudioProcessorParameter* endPoint)
		:endPoint(endPoint)
	{
		plot.fill(0);
		Timer::startTimer(16);
	};

	~LFO() {};

	void timerCallback() override
	{
		callback(currentIndex);
	};

	void start()
	{

		if (mode == free) {
			startTime = std::chrono::high_resolution_clock::now();
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
			startTime = std::chrono::high_resolution_clock::now();
			//auto _this = ReferenceCountedObjectPtr<LFO>(this);
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
			startTime = std::chrono::high_resolution_clock::now();
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
			startTime = std::chrono::high_resolution_clock::now();
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
			//std::chrono::high_resolution_clock::now().time_since_epoch().count()
			//+ playheadPosition.timeInSeconds * 1000000000
			//- startTime.time_since_epoch().count();
		double microsecondsElapsed = elapsedTime / 1000;
		double barsElapsed = microsecondsElapsed / barLength;
		double intervalsElapsed = barsElapsed / speed;
		int rounded = std::round(intervalsElapsed * LFORES);
		int index = rounded % LFORES;
		currentIndex = index;
		double lfoVal = plot[index];
		double outVal = std::clamp(lfoVal, 0.0, 1.0);
		endPoint->setValue(outVal);
		dbg(lfoVal);
	}

	void process(std::chrono::steady_clock::time_point startTime)
	{
		double microSecondsPerBeat = (60.0 / bpm) * 1000000;
		double barLength = microSecondsPerBeat * 4;
		auto elapsedTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() - startTime.time_since_epoch().count();
		double microsecondsElapsed = elapsedTime / 1000;
		double barsElapsed = microsecondsElapsed / barLength;
		double intervalsElapsed = barsElapsed / speed;
		int rounded = std::round(intervalsElapsed * LFORES);
		int index = rounded % LFORES;
		currentIndex = index;
		double lfoVal = plot[index];
		double outVal = std::clamp(lfoVal, 0.0, 1.0);
		endPoint->setValue(outVal);
		dbg(lfoVal);
	}

	void setMode(int mode)
	{
		this->mode = mode;
		if (mode == free || mode == sync) start();
	}

	int minInterval = 1000;
	double speed = 1.0;
	std::array<float, LFORES> plot;
	AudioProcessorParameter* endPoint;
	bool noteOn = false;
	enum { sync, oneshot, latch, free } modes;
	int mode = 0;
	std::function<void(int)> callback;
	int currentIndex = 0;
	std::chrono::steady_clock::time_point startTime;

};
