/*
  ==============================================================================

	utils.h
	Created: 27 Aug 2020 6:34:18pm
	Author:  callu

  ==============================================================================
*/

#pragma once
#include <memory>
#include <juceheader.h>

template<typename T>
using UPtr = std::unique_ptr<T>;

template<typename T>
using SPtr = std::shared_ptr<T>;

template<typename T>
void dbg(T v)
{
	Logger::writeToLog(juce::String(v));
}

static const Colour GREEN = Colour(0xff77ff00);
static const Colour GREY = Colour(0xff777777);
