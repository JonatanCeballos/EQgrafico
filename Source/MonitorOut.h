/*
  ==============================================================================

    MonitorOut.h
    Created: 14 Oct 2022 10:43:15am
    Author:  Jonatan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "sliderLookAndFeel.h"

//==============================================================================
/*
*/
class MonitorOut : public juce::Component
{
public:
    MonitorOut( EQgraficoAudioProcessor&); //Constructor
    ~MonitorOut() override; //Destructor

    void paint(juce::Graphics&) override; //Metodo para Pintar, Viene por defecto en Juce
    void resized() override; //Dimenciona y Ubica los componentes, Viene por defecto en Juce

private:
    //llama al procesador de audio
    EQgraficoAudioProcessor& audioProcessor;

    //clase con el diseño del slider
    sliderLookAndFeel sliderFront;

    //componentes
    juce::Slider ganancia;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gananciaAttachment;

    juce::Label gainlabel, dbInicio, dbFin;

    juce::TextButton muteador{ "Mutear" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> botonAttachment;

    //variable para activacion del boton
    bool boton = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MonitorOut)
};
