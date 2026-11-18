#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "PluginProcessor.h"

class MetaphysicalAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit MetaphysicalAudioProcessorEditor(MetaphysicalAudioProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    MetaphysicalAudioProcessor& processor;
    juce::Label brand, product, subtitle, objectLabel, statusLabel;
    juce::TextButton strikeButton { "STRIKE OBJECT" };
    juce::Slider width, height, thickness, impactX, impactY, velocity, output;
    juce::Label widthLabel, heightLabel, thicknessLabel, impactXLabel, impactYLabel, velocityLabel, outputLabel;
    std::unique_ptr<SliderAttachment> widthAttachment, heightAttachment, thicknessAttachment,
        impactXAttachment, impactYAttachment, velocityAttachment, outputAttachment;
    void styleSlider(juce::Slider&, juce::Label&, const juce::String&, const juce::String&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MetaphysicalAudioProcessorEditor)
};
