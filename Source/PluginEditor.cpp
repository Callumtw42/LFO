#include "PluginEditor.h"

juce::String message;


Editor::Editor(Processor& p)
	: AudioProcessorEditor(&p), processor(&p), ui(new UI(p))
{
	constrainer.setMinimumWidth(WIDTH);
	constrainer.setMinimumHeight(HEIGHT);
	setConstrainer(&constrainer);
	setResizable(true, true);

	connectLFOCallback();
	processor->lfo->start();
	addAndMakeVisible(ui);
	setSize(WIDTH, HEIGHT);
}

Editor::~Editor() { }

void Editor::connectLFOCallback()
{
	processor->lfo->callback = [&](int index)
	{
		float position = (float)index / LFORES;
		ui->plot->playhead.setPosition(position);
	};
}


void Editor::resized()
{
	ui->setBoundsRelative(0, 0, 1, 1);
}


