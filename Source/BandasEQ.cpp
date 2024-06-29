/*
  ==============================================================================

    BandasEQ.cpp
    Created: 10 Sep 2022 9:12:51am
    Author:  Jonatan

  ==============================================================================
*/

#include <JuceHeader.h>
#include "BandasEQ.h"

//==============================================================================
//Constructor
BandasEQ::BandasEQ( EQgraficoAudioProcessor& p) : audioProcessor(p)
{

    //Crea las especificaciones de las banda
    auto crearBanda = [this](juce::Slider& slider, int i)
    {
        slider.setSliderStyle(juce::Slider::LinearVertical);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setPopupDisplayEnabled(true, true, this);
        slider.setTextValueSuffix(" db");
        slider.setLookAndFeel(&sliderFront);
        addAndMakeVisible(slider);

        bandasAttachment[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts,
            FRECUENCIAS_ID[i],
            slider);
    };

    //Crea las bandas
    int i = 0;
    for (auto& banda : bandas)
    {
        crearBanda(banda, i);
        i++;
    };

    //Etiquetas de Frecuencia Superiores
    i = 0;
    for (auto& label : frcLabelSuperior)
    {
        label.setText(FRECUENCIAS_ID[i], juce::dontSendNotification);
        label.setColour(juce::Label::textColourId, juce::Colours::white);
        label.setFont(juce::Font(13.0f));
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
        i++;
    };

    //Etiquetas de Frecuencia Inferiores
    i = 0;
    for (auto& label : frcLabelInferior)
    {
        label.setText(FRECUENCIAS_ID[i], juce::dontSendNotification);
        label.setColour(juce::Label::textColourId, juce::Colours::white);
        label.setFont(juce::Font(13.0f));
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
        i++;
    };

    //Etiquetas de desibelios Derecha
    i = 16;
    for (auto& label : dbLabelDerecha)
    {
        label.setText((juce::String)i, juce::dontSendNotification);
        label.setColour(juce::Label::textColourId, juce::Colours::white);
        label.setFont(juce::Font(13.0f));
        label.setJustificationType(juce::Justification::centredRight);
        addAndMakeVisible(label);
        i = i - 4;
    };

    //Etiquetas de decibelios Izquierda
    i = 16;
    for (auto& label : dbLabelIzquierda)
    {
        label.setText((juce::String)i, juce::dontSendNotification);
        label.setColour(juce::Label::textColourId, juce::Colours::white);
        label.setFont(juce::Font(13.0f));
        label.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(label);
        i = i - 4;
    };
    
}

//Destructor
BandasEQ::~BandasEQ()
{
    setLookAndFeel(nullptr);//destrulle la clase de diseños de los slider
}

//Metodo para Pintar, Viene por defecto en Juce
void BandasEQ::paint(juce::Graphics& g)
{
    //Crea el fondo de la seccion
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    //crea las lineas alrededor del la seccion
    g.setColour(juce::Colours::orange);
    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

    //crea las lineas de indentificacion de el valor de desibelios (db)
    g.setColour(juce::Colours::orange.contrasting(0.2f));
    float cont = -3.0f;
    for (int i = 0; i <= 8; i++) {
        g.drawLine(getX()+2, getY() + (5.0f * cont), getWidth() - 24.0f, getY() + (5.0f * cont), 1);
        cont = cont + 4.45f;
    }
}

//Dimenciona y Ubica los componentes, Viene por defecto en Juce
void BandasEQ::resized()
{   
    auto crearGrid = [this](auto& items, float margen, juce::String horientacion, int x, int y, int width, int height)
    {
        juce::Grid grid;//Crea el componente grid

        //Espesificaciones y diviciones internas del grid
        grid.templateRows = { juce::Grid::TrackInfo(juce::Grid::Fr(1)) };
        grid.templateColumns = { juce::Grid::TrackInfo(juce::Grid::Fr(1)) };

        //colaca la horientacion automatica.
        if (horientacion == "vertical") {
            grid.autoRows = juce::Grid::TrackInfo(juce::Grid::Fr(1));
            grid.autoFlow = juce::Grid::AutoFlow::row;//Ajusta automaticamente deacuerdo el espacio de cada divicion
        }
        else if (horientacion == "horizontal") {
            grid.autoColumns = juce::Grid::TrackInfo(juce::Grid::Fr(1));
            grid.autoFlow = juce::Grid::AutoFlow::column;//Ajusta automaticamente deacuerdo el espacio de cada divicion
        }
        else {
            grid.autoRows = juce::Grid::TrackInfo(juce::Grid::Fr(1));
            grid.autoColumns = juce::Grid::TrackInfo(juce::Grid::Fr(1));

            //Ajusta el automaticamente deacuerdo el espacio de cada divicion
            grid.autoFlow = juce::Grid::AutoFlow::row;
            grid.autoFlow = juce::Grid::AutoFlow::column;
        }

        //asigna los items a el grid
        for (auto& item : items)
            grid.items.add(juce::GridItem(item).withMargin({ margen }));

        //Da el tamaño y la ubicacion del grig.
        grid.performLayout(juce::Rectangle<int>(x, y, width, height));
    };

    // Cuadricula de la Etiquetas de frecuencias de la parte superior.
    crearGrid(frcLabelSuperior, -3, "horizontal", 20, 7, getWidth() - 40, 10);

    // Cuadricula de los Slider de las bandas de frecuencias. 
    crearGrid(bandas, 3, "horizontal", 20, 17, getWidth() - 40, getHeight() - 36);

    // Cuadricula de la Etiquetas de frecuencias de la parte Inferior.
    crearGrid(frcLabelInferior, -3, "horizontal", 20, getHeight() - 17, getWidth() - 40, 10);

    // Cuadricula de la Etiquetas de decibelios de la parte Derecha.
    crearGrid(dbLabelIzquierda, 2.4, "vertical", -2, 17, 30, getHeight() - 36);

    // Cuadricula de la Etiquetas de decibelios de la parte Izquierda.
    crearGrid(dbLabelDerecha, 2.3, "vertical", getWidth() - 28, 17, 30, getHeight() - 36);
}
