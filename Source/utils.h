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
using Ptr = std::unique_ptr<T>;

template<typename T>
void dbg(T v)
{
	Logger::writeToLog(juce::String(v));
}
