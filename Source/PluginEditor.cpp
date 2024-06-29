/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
//Constructor
EQgraficoAudioProcessorEditor::EQgraficoAudioProcessorEditor (EQgraficoAudioProcessor& p)
    : AudioProcessorEditor (&p), bandas(p), monitorOut(p), audioProcessor(p)
{
    // Tamaño de la ventana
    setSize(1000, 300);
    
    crearTitulo();

    addAndMakeVisible(&bandas);//Hace el llamado a la vista de las bandas de EQ
    addAndMakeVisible(&monitorOut);//Hace el llamado a la vista que munitorea la salida

}

//Destructor
EQgraficoAudioProcessorEditor::~EQgraficoAudioProcessorEditor()
{
}

//==============================================================================
//Metodo para Pintar, Viene por defecto en Juce
void EQgraficoAudioProcessorEditor::paint (juce::Graphics& g)
{
    //Color de la ventana
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

//Dimenciona y Ubica los componentes, Viene por defecto en Juce
void EQgraficoAudioProcessorEditor::resized()
{
    bandas.setBounds(20, 45, 840, 240);
    monitorOut.setBounds(870, 45, 115, 240);
    tituloLabel.setBounds(20, 5, 70, 40);
    subtituloLabel.setBounds(69, 8, 80, 50);
}

void EQgraficoAudioProcessorEditor::crearTitulo()
{
    tituloLabel.setText("EQ", juce::dontSendNotification);
    tituloLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
    tituloLabel.setFont(juce::Font(40.0f));
    tituloLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(tituloLabel);

    subtituloLabel.setText("Grafico", juce::dontSendNotification);
    subtituloLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    subtituloLabel.setFont(juce::Font(20.0f));
    subtituloLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(subtituloLabel);
}
