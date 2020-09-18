//      _____ _____ _____ __
//     |   __|     |  |  |  |        Auto-generated C++
//     |__   |  |  |  |  |  |__      SOUL Version 0.9.0
//     |_____|_____|_____|_____|     https://soul.dev
//

#pragma once

#ifndef JUCE_AUDIO_PROCESSORS_H_INCLUDED
 #error "This file is designed to be included inside a file in a JUCE project, so that the module headers have already been included before it"
#endif

#ifndef SOUL_CPP_ASSERT
 #define SOUL_CPP_ASSERT(x)
#endif

//==============================================================================
// Generated from graph 'Level', source file: Level.soulpatch
//
struct Level   : public juce::AudioPluginInstance
{
    Level();
    ~Level() override;

    //==============================================================================
    void fillInPluginDescription (juce::PluginDescription&) const override;
    void refreshParameterList() override;

    //==============================================================================
    const juce::String getName() const override;
    juce::StringArray getAlternateDisplayNames() const override;
    bool isBusesLayoutSupported (const BusesLayout&) const override;

    //==============================================================================
    void reset() override;
    void prepareToPlay (double sampleRate, int maxBlockSize) override;
    void releaseResources() override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    bool hasEditor() const override;
    juce::AudioProcessorEditor* createEditor() override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int) override;
    const juce::String getProgramName (int) override;
    void changeProgramName (int, const juce::String&) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int size) override;

    //==============================================================================
    double getTailLengthSeconds() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool supportsMPE() const override;
    bool isMidiEffect() const override;
    void setNonRealtime (bool) noexcept override;

protected:
    //==============================================================================
    struct EditorSize  { int width = 0, height = 0; };
    EditorSize getStoredEditorSize (EditorSize defaultSize);
    void storeEditorSize (EditorSize newSize);

    juce::MidiKeyboardState midiKeyboardState;

    //==============================================================================
private:
    struct Pimpl;
    struct Parameter;
    struct EditorComponent;

    std::unique_ptr<Pimpl> pimpl;
    std::vector<Parameter*> allParameters;
    std::vector<std::unique_ptr<Parameter>> hiddenParams;
    juce::ValueTree lastValidState;

    juce::ValueTree createCurrentState();
    void updateLastState();
    void ensureValidStateExists();
    void applyLastState();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Level)
};

#define SOUL_HEADER_INCLUDED_111672075 1

