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
			std::chrono::steady_clock::time_point startTime = std::chrono::high_resolution_clock::now();
			std::thread proc([this, startTime]()
				{
					while (mode == free)
					{
						process(startTime);
						std::this_thread::sleep_for(std::chrono::microseconds(minInterval));
					}
				});
			proc.detach();
		}
	}

	//NEXT:: Fix 1 shot Mode
	void trigger(bool isNoteOn)
	{
		std::chrono::steady_clock::time_point startTime = std::chrono::high_resolution_clock::now();
		if (mode == oneshot) {
			std::thread proc([this, startTime]()
				{
					for (int i = 0; i < LFORES; ++i)
					{
						process(startTime);
						std::this_thread::sleep_for(std::chrono::microseconds(minInterval));
					}
				});
			proc.detach();
		}
		else if (mode == latch) {
			noteOn = isNoteOn;
			std::thread proc([this, startTime]()
				{
					while (noteOn)
					{
						process(startTime);
						std::this_thread::sleep_for(std::chrono::microseconds(minInterval));
					}
				});
			proc.detach();
		}
	}

	void process(std::chrono::steady_clock::time_point startTime)
	{
		double microSecondsPerBeat = (60.0 / bpm) * 1000000;
		double barLength = microSecondsPerBeat * 4;
		std::chrono::duration<float> elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
		double microsecondsElapsed = elapsedTime.count() * 1000000;
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
		if (mode == free) start();
	}

	int minInterval = 1000;
	double speed = 1.0;
	std::array<float, LFORES> plot;
	AudioProcessorParameter* endPoint;
	bool noteOn = false;
	//enum { free, oneShot, hold, sync }modes;
	enum { sync, oneshot, latch, free } modes;
	int mode = 0;
	std::function<void(int)> callback;
	int currentIndex = 0;
};
