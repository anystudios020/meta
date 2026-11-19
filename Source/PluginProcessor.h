#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "PlateModel.h"

class MetaphysicalAudioProcessor final : public juce::AudioProcessor
{
public:
    MetaphysicalAudioProcessor();
    ~MetaphysicalAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "METAPHYSICAL"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 12.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
const juce::String getProgramName(int) override { return "Glass Plate - Default"; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState parameters;
    void triggerImpact() noexcept { impactRequested.store(true); }
    double getFundamentalHz() const noexcept { return plate.getFundamentalHz(); }
    bool isPhysicalModelInRange() const noexcept { return plate.isPlausible(); }

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    metaphysical::PlateModel plate;
    std::atomic<bool> impactRequested { false };
    double currentSampleRate = 44100.0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MetaphysicalAudioProcessor)
};
