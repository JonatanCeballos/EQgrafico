/*
  ==============================================================================

    sliderLookAndFeel.h
    Created: 15 Oct 2022 12:45:28am
    Author:  Jonatan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class sliderLookAndFeel : public juce::LookAndFeel_V4
{
public:
    sliderLookAndFeel() {}
    virtual ~sliderLookAndFeel() {}

    //diseño del slider redondo
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override
    {
        auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        //fill color
        g.setColour(juce::Colours::orange);
        g.fillEllipse(rx, ry, rw, rw);

        //outline color
        g.setColour(juce::Colours::black);
        g.drawEllipse(rx, ry, rw, rw, 2.0f);

        juce::Path p;
        auto pointerLength = radius * 0.43f;
        auto pointerThickness = 3.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        g.setColour(juce::Colours::black);
        g.fillPath(p);
    }

    //diseño del slider vertical para las banda
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider& slider)
    {
        if (!slider.isBar())
        {
            auto isTwoVal = (style == juce::Slider::SliderStyle::TwoValueVertical || style == juce::Slider::SliderStyle::TwoValueHorizontal);
            auto isThreeVal = (style == juce::Slider::SliderStyle::ThreeValueVertical || style == juce::Slider::SliderStyle::ThreeValueHorizontal);

            auto trackWidth = juce::jmin(6.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);

            juce::Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
                slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

            juce::Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
                slider.isHorizontal() ? startPoint.y : (float)y);

            juce::Path backgroundTrack;
            backgroundTrack.startNewSubPath(startPoint);
            backgroundTrack.lineTo(endPoint);
            g.setColour(juce::Colours::black);
            g.strokePath(backgroundTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });



            auto thumbWidth = getSliderThumbRadius(slider);

            if (!isTwoVal)
            {
                bool isDownOrDragging = slider.isEnabled() && (slider.isMouseOverOrDragging() || slider.isMouseButtonDown());

                float kx, ky;

                if (style == juce::Slider::LinearVertical)
                {
                    kx = (float)x + (float)width * 0.5f;
                    ky = sliderPos;
                    g.setColour(juce::Colours::orange);
                    g.fillRect(juce::Rectangle<float>(kx - thumbWidth, ky - 10.0f, thumbWidth * 2.0f, 20.0f));

                    g.setColour(juce::Colours::white);
                    g.fillRect(juce::Rectangle<float>(kx - thumbWidth, ky - 4.5f, thumbWidth * 2.0f, 2.1f));

                    g.setColour(juce::Colours::black);
                    g.fillRect(juce::Rectangle<float>(kx - thumbWidth, ky - 2.5f, thumbWidth * 2.0f, 5.0f));

                    g.setColour(juce::Colours::white);
                    g.fillRect(juce::Rectangle<float>(kx - thumbWidth, ky + 2.5f, thumbWidth * 2.0f, 2.1f));
                }
            }
        }
    }

private:

};