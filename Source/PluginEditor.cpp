#include "PluginEditor.h"

juce::String message;

static const int WIDTH = 300;
static const int HEIGHT = 200;

Editor::Editor(Processor& p)
	: AudioProcessorEditor(&p), processor(&p), ui(new UI(p))
{
	constrainer.setMinimumWidth(WIDTH);
	constrainer.setMinimumHeight(HEIGHT);
	setConstrainer(&constrainer);
	setResizable(true, true);
	setSize(WIDTH, HEIGHT);

	connectLFOCallback();
	processor->lfo->start();
	addAndMakeVisible(ui);
}

Editor::~Editor() { }

void Editor::connectLFOCallback()
{
	processor->lfo->callback = [&](int index)
	{
		float position = (float)index / LFORES;
		ui->plot.playhead.setPosition(position);
	};
}


void Editor::resized()
{
	ui->setSize(getWidth(), getHeight());
}


