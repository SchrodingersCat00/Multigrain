#include "AdsrComponent.h"

AdsrComponent::AdsrComponent(AudioPluginAudioProcessor& processorRef, Parameters parameters)
    : processorRef(processorRef),
      attackSlider(*processorRef.apvts.getParameter(parameters.attackParameter), "ms"),
      decaySlider(*processorRef.apvts.getParameter(parameters.decayParameter), "ms"),
      sustainSlider(*processorRef.apvts.getParameter(parameters.sustainParameter), "%"),
      releaseSlider(*processorRef.apvts.getParameter(parameters.releaseParameter), "ms"),

      attackSliderAttachment(processorRef.apvts, processorRef.apvts.getParameter(parameters.attackParameter)->paramID, attackSlider),
      decaySliderAttachment(processorRef.apvts, processorRef.apvts.getParameter(parameters.decayParameter)->paramID, decaySlider),
      sustainSliderAttachment(processorRef.apvts, processorRef.apvts.getParameter(parameters.sustainParameter)->paramID, sustainSlider),
      releaseSliderAttachment(processorRef.apvts, processorRef.apvts.getParameter(parameters.releaseParameter)->paramID, releaseSlider),

      parameters(parameters),
      visualComponent(parameters, processorRef.apvts)
{
    attackSlider.addListener(this);
    decaySlider.addListener(this);
    sustainSlider.addListener(this);
    releaseSlider.addListener(this);

    addAndMakeVisible(visualComponent);
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(decaySlider);
    addAndMakeVisible(sustainSlider);
    addAndMakeVisible(releaseSlider);
}

AdsrComponent::~AdsrComponent()
{
    attackSlider.removeListener(this);
    decaySlider.removeListener(this);
    sustainSlider.removeListener(this);
    releaseSlider.removeListener(this);
}

void AdsrComponent::sliderValueChanged(juce::Slider* slider)
{
    visualComponent.repaint();
}

void AdsrComponent::resized()
{
    auto bounds = getLocalBounds();
    auto visualArea = bounds.removeFromTop(getHeight()*0.66);
    auto sliderArea = bounds;

    visualComponent.setBounds(visualArea);

    attackSlider.setBounds(sliderArea.removeFromLeft(sliderArea.getWidth()*.25));
    decaySlider.setBounds(sliderArea.removeFromLeft(sliderArea.getWidth()*.33));
    sustainSlider.setBounds(sliderArea.removeFromLeft(sliderArea.getWidth()*.5));
    releaseSlider.setBounds(sliderArea);
}

void AdsrComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(getHeight()*0.66);
    g.setColour(juce::Colours::white);
    g.drawRect(bounds);
}

AdsrComponent::AdsrVisualComponent::AdsrVisualComponent(AdsrComponent::Parameters parameters, APVTS& apvts)
    : parameters(parameters),
      apvts(apvts)
{}

void AdsrComponent::AdsrVisualComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    g.setColour(juce::Colours::white);
    g.fillRect(bounds);

    auto attackMs = apvts.getRawParameterValue(parameters.attackParameter)->load();
    auto decayMs = apvts.getRawParameterValue(parameters.decayParameter)->load();
    auto releaseMs = apvts.getRawParameterValue(parameters.releaseParameter)->load();
    auto sustain = apvts.getParameter(parameters.sustainParameter)->getValue();

    auto totalMs = attackMs + decayMs + releaseMs;
    drawGrid(g, totalMs);

    auto attackRatio = (float) attackMs / (float) totalMs;
    auto decayRatio = (float) decayMs / (float) totalMs;
    auto releaseRatio = (float) releaseMs / (float) totalMs;


    g.setColour(juce::Colours::black);
    g.drawLine(
        0,
        bounds.getHeight(),
        getWidth()*attackRatio,
        1,
        2.f
    );

    g.drawLine(
        getWidth()*attackRatio,
        0,
        getWidth()*(attackRatio + decayRatio),
        bounds.getHeight()*(1-sustain) + 1,
        2.f
    );

    g.drawLine(
        getWidth()*(attackRatio + decayRatio),
        bounds.getHeight()*(1-sustain) + 1,
        getWidth(),
        bounds.getHeight(),
        2.f
    );
}

void AdsrComponent::AdsrVisualComponent::drawGrid(juce::Graphics& g, float ms)
{
    g.setColour(juce::Colours::grey);
    ms /= 1000;
    int msFloor = (int) ms;
    float ratio = (float) msFloor / ms;
    float increment = (getWidth()*ratio)/(float) msFloor;
    float xIterator = 0;
    int counter = 0;
    while(xIterator <= getWidth())
    {
        g.drawLine(
            xIterator,
            0,
            xIterator,
            getHeight()
        );
        juce::String str;
        str << counter;
        str << " s";
        auto labelArea = juce::Rectangle<int>(xIterator + 4, getHeight() - 10, 30, 10);
        g.drawText(
            str,
            labelArea,
            juce::Justification::centred
        );
        xIterator += increment;
        counter++;
    }
}