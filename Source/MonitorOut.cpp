/*
  ==============================================================================

    MonitorOut.cpp
    Created: 14 Oct 2022 10:43:15am
    Author:  Jonatan

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MonitorOut.h"

//==============================================================================
//Constructor
MonitorOut::MonitorOut(EQgraficoAudioProcessor& p) : audioProcessor(p)
{
    //creacion del slider de ganancia(volumen)
    ganancia.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
    ganancia.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    ganancia.setPopupDisplayEnabled(true, true, this);
    ganancia.setTextValueSuffix(" db");
    ganancia.setLookAndFeel(&sliderFront);
    addAndMakeVisible(ganancia);

    gananciaAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts,
        GANANCIA_ID,
        ganancia);

    
    muteador.setClickingTogglesState(true);
    muteador.setColour(juce::TextButton::buttonColourId, juce::Colours::black.withAlpha(0.8f));
    muteador.setColour(juce::TextButton::textColourOffId, juce::Colours::orange.withAlpha(0.8f));
    addAndMakeVisible(muteador); 
    
    botonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts,
        MUTEADOR_ID,
        muteador);

    
    gainlabel.setText("Ganancia", juce::dontSendNotification);
    gainlabel.setColour(juce::Label::textColourId, juce::Colours::white);
    gainlabel.setFont(juce::Font(22.0f));
    gainlabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(gainlabel);

    dbInicio.setText("-16", juce::dontSendNotification);
    dbInicio.setColour(juce::Label::textColourId, juce::Colours::white);
    dbInicio.setFont(juce::Font(12.0f));
    dbInicio.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(dbInicio);

    dbFin.setText("16", juce::dontSendNotification);
    dbFin.setColour(juce::Label::textColourId, juce::Colours::white);
    dbFin.setFont(juce::Font(12.0f));
    dbFin.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(dbFin);

}

//Destructor
MonitorOut::~MonitorOut()
{
    setLookAndFeel(nullptr);
}

//Metodo para Pintar, Viene por defecto en Juce
void MonitorOut::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour(juce::Colours::orange);
    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

    g.setColour(juce::Colours::red);
    g.drawLine(1, 1, 1, 1, 3);

}

//Dimenciona y Ubica los componentes, Viene por defecto en Juce
void MonitorOut::resized()
{
    //Crea un componente grid que contendra todos los componentes.
    juce::Grid monitorSalida;

    //Espesificaciones y diviciones internas del monitorSalida
    monitorSalida.templateRows = { juce::Grid::TrackInfo(juce::Grid::Fr(1)),
                                  juce::Grid::TrackInfo(juce::Grid::Fr(1)),
                                  juce::Grid::TrackInfo(juce::Grid::Fr(1)) };

    monitorSalida.templateColumns = { juce::Grid::TrackInfo(juce::Grid::Fr(1)),
                                      juce::Grid::TrackInfo(juce::Grid::Fr(1)) };

    //asigna los componentes a monitorSalida
    monitorSalida.items = { juce::GridItem(gainlabel).withMargin({ -2 }).withArea(1,1,1,3),
                            juce::GridItem(dbInicio).withMargin(juce::GridItem::Margin(60,30,0,0)).withArea(2,1,2,1),
                            juce::GridItem(dbFin).withMargin(juce::GridItem::Margin(60,0,0,30)).withArea(2,2,2,2),
                            juce::GridItem(ganancia).withMargin({ 0 }).withArea(2,1,2,3),
                            juce::GridItem(muteador).withMargin({ 11 }).withArea(3,1,3,3)};

    //Da el tamaño y la ubicacion a grid, el tamaño por defecto y la ubicacion es la misma de la seccion 
    monitorSalida.performLayout(getLocalBounds());
}