#include "PluginProcessor.h"
#include "PluginEditor.h"

MetaphysicalAudioProcessor::MetaphysicalAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "METAPHYSICAL_STATE", createParameterLayout())
{}

juce::AudioProcessorValueTreeState::ParameterLayout MetaphysicalAudioProcessor::createParameterLayout()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> p;
    p.push_back(std::make_unique<AudioParameterFloat>("width", "Width", NormalisableRange<float>(0.05f, 400.0f, 0.001f, 0.35f), 1.2f, "m"));
    p.push_back(std::make_unique<AudioParameterFloat>("height", "Height", NormalisableRange<float>(0.05f, 400.0f, 0.001f, 0.35f), 0.8f, "m"));
    p.push_back(std::make_unique<AudioParameterFloat>("thickness", "Thickness", NormalisableRange<float>(0.0005f, 0.5f, 0.0001f, 0.4f), 0.012f, "m"));
    p.push_back(std::make_unique<AudioParameterFloat>("impactX", "Impact X", NormalisableRange<float>(0.001f, 0.999f), 0.5f));
    p.push_back(std::make_unique<AudioParameterFloat>("impactY", "Impact Y", NormalisableRange<float>(0.001f, 0.999f), 0.5f));
    p.push_back(std::make_unique<AudioParameterFloat>("velocity", "Rod Velocity", NormalisableRange<float>(0.01f, 30.0f, 0.001f, 0.5f), 2.0f, "m/s"));
    p.push_back(std::make_unique<AudioParameterFloat>("rodMass", "Rod Mass", NormalisableRange<float>(0.005f, 10.0f, 0.001f, 0.5f), 0.18f, "kg"));
    p.push_back(std::make_unique<AudioParameterFloat>("loss", "Modal Loss", NormalisableRange<float>(0.00005f, 0.08f, 0.00001f, 0.5f), 0.0018f));
    p.push_back(std::make_unique<AudioParameterFloat>("output", "Output", NormalisableRange<float>(0.0f, 1.0f), 0.35f));
    return { p.begin(), p.end() };
}

void MetaphysicalAudioProcessor::prepareToPlay(double sampleRate, int)
{
    currentSampleRate = sampleRate;
    plate.prepare(sampleRate);
    plate.reset();
}

void MetaphysicalAudioProcessor::releaseResources() {}

void MetaphysicalAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    const auto get = [this](const char* id) { return parameters.getRawParameterValue(id)->load(); };

    metaphysical::PlateParameters p;
    p.widthMetres = get("width");
    p.heightMetres = get("height");
    p.thicknessMetres = get("thickness");
    p.impactX = get("impactX");
    p.impactY = get("impactY");
    p.rodVelocityMps = get("velocity");
    p.rodMassKg = get("rodMass");
    p.modalLoss = get("loss");
    plate.setParameters(p);

    for (const auto metadata : midi)
        if (metadata.getMessage().isNoteOn())
            plate.strike();
    if (impactRequested.exchange(false))
        plate.strike();

    const float output = get("output");
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        const float value = plate.processSample() * output;
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            buffer.setSample(channel, sample, value);
    }
}

juce::AudioProcessorEditor* MetaphysicalAudioProcessor::createEditor()
{
    return new MetaphysicalAudioProcessorEditor(*this);
}

void MetaphysicalAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, dest);
}

void MetaphysicalAudioProcessor::setStateInformation(const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, size));
    if (xml && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MetaphysicalAudioProcessor();
}
