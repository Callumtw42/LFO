#include "PluginEditor.h"

juce::String message;

Editor::Editor(Processor& p)
	: AudioProcessorEditor(&p), processor(&p)
{
	constrainer.setMinimumWidth(WIDTH);
	constrainer.setMinimumHeight(HEIGHT);
	setConstrainer(&constrainer);
	setResizable(true, true);
	setSize(WIDTH, HEIGHT);

	juce::File UIPath("C:\\Users\\callu\\Desktop\\projects\\LFO\\Source\\jsui\\build\\js\\main.js");
	jassert(UIPath.existsAsFile());
	appRoot.enableHotReloading();
	//updateParams();
	//for (Param* param : processor->params)
	bindNativeCallbacks();
	//processor->lfo->callback = &updateCurrentIndex;
	appRoot.evaluate(UIPath);
	connectLFOCallback();
	processor->lfo->start();
	addAndMakeVisible(appRoot);
}

Editor::~Editor() { }

void Editor::connectLFOCallback()
{
	processor->lfo->callback = [&](int index)
	{
		Component::SafePointer<blueprint::ReactApplicationRoot> safeAppRoot(&appRoot);

		juce::MessageManager::callAsync([=]() {
			if (blueprint::ReactApplicationRoot* root = safeAppRoot.getComponent())
				root->dispatchEvent("updateCurrentIndex",
					index
				);
			});
	};
}

void Editor::bindNativeCallbacks()
{
	appRoot.engine.registerNativeMethod(
		"sendPlot",
		[](void* stash, const var::NativeFunctionArgs& args) {
			auto* self = reinterpret_cast<Editor*>(stash);
			const juce::String& paramId = args.arguments[0].toString();
			//AudioProcessorParameter* param = self->processor->getParameters()[0];
			//param->enableLFOs();
			for (int i = 0; i < LFORES; i++) {
				double inVal = static_cast<double>(args.arguments[1][i]);
				//Logger::writeToLog(juce::String(inVal));
				self->processor->lfo->plot[i] = inVal;
			}
			return var::undefined();
		},
		(void*)this
			);

	appRoot.engine.registerNativeMethod(
		"sendLFOSpeed",
		[](void* stash, const var::NativeFunctionArgs& args) {
			auto* self = reinterpret_cast<Editor*>(stash);
			const juce::String& paramId = args.arguments[0].toString();
			//Param* param = self->processor->params.getReference(paramId);

			self->processor->lfo->speed = args.arguments[1];
			return var::undefined();
		},
		(void*)this
			);

	appRoot.engine.registerNativeMethod(
		"setMode",
		[](void* stash, const var::NativeFunctionArgs& args) {
			auto* self = reinterpret_cast<Editor*>(stash);
			const juce::String& paramId = args.arguments[0].toString();
			//Param* param = self->processor->params.getReference(paramId);

			self->processor->lfo->mode = args.arguments[1];
			return var::undefined();
		},
		(void*)this
			);

	appRoot.engine.registerNativeMethod(
		"log",
		[](void* stash, const juce::var::NativeFunctionArgs& args) {
			Logger::writeToLog(args.arguments[0].toString());
			return juce::var::undefined();
		},
		(void*)this
			);

	appRoot.engine.registerNativeMethod(
		"getMouseX",
		[](void* stash, const juce::var::NativeFunctionArgs& args) {
			auto* self = reinterpret_cast<Editor*>(stash);
			return var(self->getMouseXYRelative().getX());
		},
		(void*)this
			);

	appRoot.engine.registerNativeMethod(
		"getMouseY",
		[](void* stash, const juce::var::NativeFunctionArgs& args) {
			auto* self = reinterpret_cast<Editor*>(stash);
			return var(self->getMouseXYRelative().getY());
		},
		(void*)this
			);

}


void Editor::parameterValueChanged(int parameterIndex, float newValue)
{
	//const auto& p = processor->pluginInstances[MAXVOICES - 1]->getParameters()[parameterIndex];
	const auto& p = processor->getParameters()[parameterIndex];
	juce::String id = p->getName(100);

	if (auto* x = dynamic_cast<AudioProcessorParameterWithID*>(p))
		id = x->paramID;

	float defaultValue = p->getDefaultValue();
	juce::String stringValue = p->getText(newValue, 0);

	Component::SafePointer<blueprint::ReactApplicationRoot> safeAppRoot(&appRoot);

	juce::MessageManager::callAsync([=]() {
		if (blueprint::ReactApplicationRoot* root = safeAppRoot.getComponent())
			root->dispatchEvent("parameterValueChange",
				parameterIndex,
				id,
				defaultValue,
				newValue,
				stringValue);
		});
}

void Editor::resized()
{
	appRoot.setBounds(getLocalBounds());
	errorText.setBounds(getLocalBounds());
	errorText.centreWithSize(getParentHeight(), getParentWidth());
}


