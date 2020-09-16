#include "PluginEditor.h"


juce::String message;


ProcessorEditor::ProcessorEditor(AudioProcessor& p, LFO& lfo)
	: AudioProcessorEditor(&p), processor(&p), lfo(&lfo), ui(std::make_unique<UI>(p, lfo))
{
	constrainer.setMinimumWidth(WIDTH);
	constrainer.setMinimumHeight(HEIGHT);
	setConstrainer(&constrainer);
	setResizable(true, true);

	connectLFOCallback();
	addAndMakeVisible(ui.get());
	setSize(WIDTH, HEIGHT);
}

ProcessorEditor::~ProcessorEditor() { }

void ProcessorEditor::connectLFOCallback()
{
	lfo->callback = [&](int index)
	{
		float position = (float)index / LFORES;
		if (isShowing())
			ui->plot->playhead.setPosition(position);
	};
}


void ProcessorEditor::resized()
{
	ui->setBoundsRelative(0, 0, 1, 1);
}


