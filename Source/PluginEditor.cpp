#include "PluginEditor.h"

namespace
{
const juce::Colour background { 0xff111214 };
const juce::Colour panel { 0xff1a1c1f };
const juce::Colour muted { 0xff858990 };
const juce::Colour ink { 0xfff1f1ee };
const juce::Colour accent { 0xffc8d0d3 };
}

MetaphysicalAudioProcessorEditor::MetaphysicalAudioProcessorEditor(MetaphysicalAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(760, 520);
    auto setupLabel = [](juce::Label& label, const juce::String& text, float size, juce::Colour colour, bool bold = false)
    {
        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::Font(juce::FontOptions(size, bold ? juce::Font::bold : juce::Font::plain)));
        label.setColour(juce::Label::textColourId, colour);
    };
    setupLabel(brand, "ANY STUDIOS", 11.0f, muted, true);
    setupLabel(product, "METAPHYSICAL", 25.0f, ink, true);
    setupLabel(subtitle, "PHYSICAL MODELLING INSTRUMENT     /     GLASS PLATE 001", 10.0f, muted);
    setupLabel(objectLabel, "RECTANGULAR PLATE     ·     SODA-LIME GLASS", 11.0f, accent, true);
    setupLabel(statusLabel, "SIMPLY SUPPORTED PLATE MODEL  /  MODAL RESPONSE", 9.0f, muted);
    for (auto* c : { &brand, &product, &subtitle, &objectLabel, &statusLabel }) addAndMakeVisible(*c);

    styleSlider(width, widthLabel, "WIDTH", "m");
    styleSlider(height, heightLabel, "HEIGHT", "m");
    styleSlider(thickness, thicknessLabel, "THICKNESS", "m");
    styleSlider(impactX, impactXLabel, "IMPACT X", "");
    styleSlider(impactY, impactYLabel, "IMPACT Y", "");
    styleSlider(velocity, velocityLabel, "ROD VELOCITY", "m/s");
    styleSlider(output, outputLabel, "OUTPUT", "");

    widthAttachment = std::make_unique<SliderAttachment>(processor.parameters, "width", width);
    heightAttachment = std::make_unique<SliderAttachment>(processor.parameters, "height", height);
    thicknessAttachment = std::make_unique<SliderAttachment>(processor.parameters, "thickness", thickness);
    impactXAttachment = std::make_unique<SliderAttachment>(processor.parameters, "impactX", impactX);
    impactYAttachment = std::make_unique<SliderAttachment>(processor.parameters, "impactY", impactY);
    velocityAttachment = std::make_unique<SliderAttachment>(processor.parameters, "velocity", velocity);
    outputAttachment = std::make_unique<SliderAttachment>(processor.parameters, "output", output);

    strikeButton.setColour(juce::TextButton::buttonColourId, accent);
    strikeButton.setColour(juce::TextButton::textColourOffId, background);
    strikeButton.setColour(juce::TextButton::buttonOnColourId, ink);
    strikeButton.setConnectedEdges(juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight
                                   | juce::Button::ConnectedOnTop | juce::Button::ConnectedOnBottom);
    strikeButton.onClick = [this] { processor.triggerImpact(); };
    addAndMakeVisible(strikeButton);
}

void MetaphysicalAudioProcessorEditor::styleSlider(juce::Slider& slider, juce::Label& label,
                                                    const juce::String& name, const juce::String& suffix)
{
    label.setText(name, juce::dontSendNotification);
    label.setFont(juce::Font(juce::FontOptions(9.5f, juce::Font::bold)));
    label.setColour(juce::Label::textColourId, muted);
    addAndMakeVisible(label);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 86, 20);
    slider.setTextValueSuffix(suffix.isNotEmpty() ? " " + suffix : "");
    slider.setColour(juce::Slider::rotarySliderFillColourId, accent);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff3b3e43));
    slider.setColour(juce::Slider::thumbColourId, ink);
    slider.setColour(juce::Slider::textBoxTextColourId, ink);
    slider.setColour(juce::Slider::textBoxBackgroundColourId, panel);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);
}

void MetaphysicalAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(background);
    g.setColour(panel);
    g.fillRoundedRectangle(24.0f, 112.0f, 712.0f, 170.0f, 5.0f);
    g.setColour(juce::Colour(0xff34383d));
    g.drawRoundedRectangle(24.0f, 112.0f, 712.0f, 170.0f, 5.0f, 1.0f);

    // Minimal technical plate illustration: outline, support edges and impact point.
    auto plateBounds = juce::Rectangle<float>(205.0f, 150.0f, 350.0f, 100.0f);
    g.setColour(juce::Colour(0xffb8c4c8).withAlpha(0.10f));
    g.fillRoundedRectangle(plateBounds, 2.0f);
    g.setColour(juce::Colour(0xffcbd5d8).withAlpha(0.75f));
    g.drawRoundedRectangle(plateBounds, 2.0f, 1.0f);
    g.setColour(juce::Colour(0xff9da9ad).withAlpha(0.45f));
    for (int i = 0; i < 18; ++i)
    {
        const float x = plateBounds.getX() + 12.0f + i * 19.0f;
        g.drawVerticalLine(static_cast<int>(x), plateBounds.getY() + 5.0f, plateBounds.getBottom() - 5.0f);
    }
    const auto px = plateBounds.getX() + plateBounds.getWidth() * static_cast<float>(impactX.getValue());
    const auto py = plateBounds.getY() + plateBounds.getHeight() * static_cast<float>(impactY.getValue());
    g.setColour(ink);
    g.fillEllipse(px - 3.5f, py - 3.5f, 7.0f, 7.0f);
    g.setColour(ink.withAlpha(0.35f));
    g.drawEllipse(px - 10.0f, py - 10.0f, 20.0f, 20.0f, 1.0f);

    g.setColour(juce::Colour(0xff34383d));
    g.drawHorizontalLine(300, 302.0f, 736.0f);
}

void MetaphysicalAudioProcessorEditor::resized()
{
    brand.setBounds(28, 20, 180, 18);
    product.setBounds(26, 39, 300, 36);
    subtitle.setBounds(29, 77, 500, 18);
    objectLabel.setBounds(42, 122, 450, 18);
    statusLabel.setBounds(42, 258, 500, 14);
    strikeButton.setBounds(570, 226, 142, 36);

    const int top = 320;
    const int colW = 100;
    const int gap = 8;
    const int start = 34;
    juce::Component* sliders[] = { &width, &height, &thickness, &impactX, &impactY, &velocity, &output };
    juce::Label* labels[] = { &widthLabel, &heightLabel, &thicknessLabel, &impactXLabel, &impactYLabel, &velocityLabel, &outputLabel };
    for (int i = 0; i < 7; ++i)
    {
        const int x = start + i * (colW + gap);
        labels[i]->setBounds(x, top, colW, 18);
        sliders[i]->setBounds(x, top + 18, colW, 145);
    }
}
