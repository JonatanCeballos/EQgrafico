#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "sliderLookAndFeel.h"

//==============================================================================
/*
*/
class BandasEQ : public juce::Component
{
public:
    BandasEQ(EQgraficoAudioProcessor&); //Constructor
    ~BandasEQ() override; //Destructor

    void paint(juce::Graphics&) override; //Metodo para Pintar, Viene por defecto en Juce
    void resized() override; //Dimenciona y Ubica los componentes, Viene por defecto en Juce

private:

    //llama al procesador de audio
    EQgraficoAudioProcessor& audioProcessor;

    //clase con el diseño de los slider
    sliderLookAndFeel sliderFront;

    //Bandas
    std::array<juce::Slider, 31> bandas; //componentes Slider
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 31> bandasAttachment; // Attachment de las bandas

    //Etiquetas de Frecuencia
    std::array<juce::Label, 31> frcLabelSuperior;
    std::array<juce::Label, 31> frcLabelInferior;

    //Etiquetas de decibelios
    std::array<juce::Label, 9> dbLabelDerecha;
    std::array<juce::Label, 9> dbLabelIzquierda;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BandasEQ)
};