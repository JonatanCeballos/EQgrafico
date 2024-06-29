/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Fifo.h"
#include "SingleChannelSampleFifo.h"
#include <iostream>
using namespace std;

#define GANANCIA_ID "ganancia"
#define GANANCIA_NAME "Ganancia"

#define MUTEADOR_ID "muteador"
#define MUTEADOR_NAME "muteador"

//arrreglo para todos los ID y frecuencias de cada banda
const juce::String FRECUENCIAS_ID[31] = { "20","25","31,5","40" ,"50" ,"63" ,"80" ,"100" ,"125" ,"160" ,"200" ,"250" ,"315" ,"400" ,"500" ,"630" ,"800" ,"1k" ,"1,25k" ,"1,6k" ,"2k" ,"2,5k" ,"3,15k" ,"4k" ,"5k" ,"6,3k" ,"8k" ,"10k" ,"12,5k" ,"16k" ,"20k" };

//Valor de cada frecuencia
const float frecuenciaValor[31] = { 20.0f, 25.0f, 31.5f, 40.0f, 50.0f, 63.0f, 80.0f, 100.0f, 125.0f, 160.0f, 200.0f, 250.0f, 315.0f, 400.0f, 500.0f, 630.0f, 800.0f, 1000.0f, 1250.0f, 1600.0f, 2000.0f, 2500.0f, 3150.0f, 4000.0f, 5000.0f, 6300.0f, 8000.0f, 10000.0f, 12500.0f, 16000.0f, 20000.0f, };

struct ChainSettings {
    float gain{ 0 };
    float bandas[31]{ 0 };
    bool muteador{ false };
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

using Filter = juce::dsp::IIR::Filter<float>;
using MonoChain = juce::dsp::ProcessorChain < Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter >;

enum ChainPositions {
    f20, f25, f31_5, f40, f50, f63, f80, f100, f125, f160, f200, f250, f315, f400, f500, f630, f800, f1k, f1_25k, f1_6k, f2k, f2_5k, f3_15k, f4k, f5k, f6_3k, f8k, f10k, f12_5k, f16k, f20k
};

using Coefficients = Filter::CoefficientsPtr;

class MyTest : public juce::UnitTest
{
    public:
        MyTest() : juce::UnitTest("MyTest") {}

        void runTest() override
        {
            beginTest("Test MemoryOutputStream");
            juce::MemoryOutputStream output;
            output.writeString("Hello World");
            juce::String data = output.toUTF8();

            if ( data == "Hello World" ) 
            {
                cout << data;
            };
        }
        
};
//==============================================================================
/**
*/
class EQgraficoAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    EQgraficoAudioProcessor(); //Constructor
    ~EQgraficoAudioProcessor() override; //Destructor

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parametros", createParameterLayout() };
    
    //==============================================================================
    using BlockType = juce::AudioBuffer<float>;
    SingleChannelSampleFifo<BlockType> leftChannelFifo{ Channel::Left };
    SingleChannelSampleFifo<BlockType> rightChannelFifo{ Channel::Right };

private:

    MonoChain leftChain, rightChain;
    MyTest test;

    void updateFilters();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQgraficoAudioProcessor)
};
