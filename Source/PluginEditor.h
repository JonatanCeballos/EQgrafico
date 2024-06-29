/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "BandasEQ.h"
#include "MonitorOut.h"

//==============================================================================
/**
*/
class EQgraficoAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    EQgraficoAudioProcessorEditor (EQgraficoAudioProcessor&); //Constructor
    ~EQgraficoAudioProcessorEditor() override; //Destructor

    //==============================================================================
    void paint (juce::Graphics&) override; //Metodo para Pintar, Viene por defecto en Juce
    void resized() override; //Dimenciona y Ubica los componentes, Viene por defecto en Juce
    void crearTitulo();

private:
    
    //llama al procesador de audio
    EQgraficoAudioProcessor& audioProcessor;

    //llama a las diferentes secciones del plugin
    BandasEQ bandas;
    MonitorOut monitorOut;

    juce::Label tituloLabel, subtituloLabel; //Etiqueta de titulo principal

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQgraficoAudioProcessorEditor)
};
