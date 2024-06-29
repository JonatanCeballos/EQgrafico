/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
//Constructor
EQgraficoAudioProcessor::EQgraficoAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

//Destructor
EQgraficoAudioProcessor::~EQgraficoAudioProcessor()
{
}

//==============================================================================
const juce::String EQgraficoAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EQgraficoAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool EQgraficoAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool EQgraficoAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double EQgraficoAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EQgraficoAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int EQgraficoAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EQgraficoAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String EQgraficoAudioProcessor::getProgramName (int index)
{
    return {};
}

void EQgraficoAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void EQgraficoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    updateFilters();

    leftChannelFifo.prepare(samplesPerBlock);
    rightChannelFifo.prepare(samplesPerBlock);
}

void EQgraficoAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EQgraficoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void EQgraficoAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    updateFilters();

    juce::dsp::AudioBlock<float> block(buffer);
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);

    leftChannelFifo.update(buffer);
    rightChannelFifo.update(buffer);

    auto chainSettings = getChainSettings(apvts);
    buffer.applyGain(juce::Decibels::decibelsToGain(chainSettings.gain));
}

//==============================================================================
bool EQgraficoAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EQgraficoAudioProcessor::createEditor()
{
    return new EQgraficoAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);

}

//==============================================================================
void EQgraficoAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void EQgraficoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid()) {
        apvts.replaceState(tree);
        updateFilters();
    }
}
 


ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts) {
    ChainSettings settings;

    settings.gain = apvts.getRawParameterValue(GANANCIA_ID)->load();

    int i = 0;
    for (auto& frc : FRECUENCIAS_ID)
    {
        settings.bandas[i] = apvts.getRawParameterValue(frc)->load();
        i++;
    }

    settings.muteador = apvts.getRawParameterValue(MUTEADOR_ID)->load() > 0.5f;

    return settings;
}

juce::AudioProcessorValueTreeState::ParameterLayout EQgraficoAudioProcessor::createParameterLayout()
{

    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>(GANANCIA_ID, GANANCIA_NAME, juce::NormalisableRange<float>(-16.f, 16.f, 0.5f, 1.f), 0.0f));

    for (auto& frc : FRECUENCIAS_ID)
        layout.add(std::make_unique<juce::AudioParameterFloat>(frc, frc, juce::NormalisableRange<float>(-16.f, 16.f, 0.5f, 1.f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterBool>(MUTEADOR_ID, MUTEADOR_NAME, false));
    return layout;
}

void EQgraficoAudioProcessor::updateFilters () {

    auto chainSettings = getChainSettings(apvts);

    Coefficients peakCoefficients[31];

    int i = 0;
    for(auto frc : frecuenciaValor)
    {
        peakCoefficients[i] = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), frc, 4.32f, juce::Decibels::decibelsToGain(chainSettings.bandas[i]));
        i++;
    }

    leftChain.setBypassed<ChainPositions::f20>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f20>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f20>().coefficients = *peakCoefficients[0];
    *rightChain.get<ChainPositions::f20>().coefficients = *peakCoefficients[0];

    leftChain.setBypassed<ChainPositions::f25>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f25>(chainSettings.muteador);

    leftChain.get<ChainPositions::f25>().coefficients = *peakCoefficients[1];
    rightChain.get<ChainPositions::f25>().coefficients = *peakCoefficients[1];

    leftChain.setBypassed<ChainPositions::f31_5>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f31_5>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f31_5>().coefficients = *peakCoefficients[2];
    *rightChain.get<ChainPositions::f31_5>().coefficients = *peakCoefficients[2];

    leftChain.setBypassed<ChainPositions::f40>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f40>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f40>().coefficients = *peakCoefficients[3];
    *rightChain.get<ChainPositions::f40>().coefficients = *peakCoefficients[3];

   leftChain.setBypassed<ChainPositions::f50>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f50>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f50>().coefficients = *peakCoefficients[4];
    *rightChain.get<ChainPositions::f50>().coefficients = *peakCoefficients[4];

    leftChain.setBypassed<ChainPositions::f63>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f63>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f63>().coefficients = *peakCoefficients[5];
    *rightChain.get<ChainPositions::f63>().coefficients = *peakCoefficients[5];

    leftChain.setBypassed<ChainPositions::f80>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f80>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f80>().coefficients = *peakCoefficients[6];
    *rightChain.get<ChainPositions::f80>().coefficients = *peakCoefficients[6];

    leftChain.setBypassed<ChainPositions::f100>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f100>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f100>().coefficients = *peakCoefficients[7];
    *rightChain.get<ChainPositions::f100>().coefficients = *peakCoefficients[7];

    leftChain.setBypassed<ChainPositions::f125>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f125>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f125>().coefficients = *peakCoefficients[8];
    *rightChain.get<ChainPositions::f125>().coefficients = *peakCoefficients[8];

    leftChain.setBypassed<ChainPositions::f160>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f160>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f160>().coefficients = *peakCoefficients[9];
    *rightChain.get<ChainPositions::f160>().coefficients = *peakCoefficients[9];

    leftChain.setBypassed<ChainPositions::f200>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f200>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f200>().coefficients = *peakCoefficients[10];
    *rightChain.get<ChainPositions::f200>().coefficients = *peakCoefficients[10];

    leftChain.setBypassed<ChainPositions::f250>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f250>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f250>().coefficients = *peakCoefficients[11];
    *rightChain.get<ChainPositions::f250>().coefficients = *peakCoefficients[11];

    leftChain.setBypassed<ChainPositions::f315>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f315>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f315>().coefficients = *peakCoefficients[12];
    *rightChain.get<ChainPositions::f315>().coefficients = *peakCoefficients[12];

    leftChain.setBypassed<ChainPositions::f400>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f400>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f400>().coefficients = *peakCoefficients[13];
    *rightChain.get<ChainPositions::f400>().coefficients = *peakCoefficients[13];

    leftChain.setBypassed<ChainPositions::f500>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f500>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f500>().coefficients = *peakCoefficients[14];
    *rightChain.get<ChainPositions::f500>().coefficients = *peakCoefficients[14];

    leftChain.setBypassed<ChainPositions::f630>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f630>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f630>().coefficients = *peakCoefficients[15];
    *rightChain.get<ChainPositions::f630>().coefficients = *peakCoefficients[15];

    leftChain.setBypassed<ChainPositions::f800>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f800>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f800>().coefficients = *peakCoefficients[16];
    *rightChain.get<ChainPositions::f800>().coefficients = *peakCoefficients[16];

    leftChain.setBypassed<ChainPositions::f1k>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f1k>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f1k>().coefficients = *peakCoefficients[17];
    *rightChain.get<ChainPositions::f1k>().coefficients = *peakCoefficients[17]; 

    leftChain.setBypassed<ChainPositions::f1_25k>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f1_25k>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f1_25k>().coefficients = *peakCoefficients[18];
    *rightChain.get<ChainPositions::f1_25k>().coefficients = *peakCoefficients[18];

    leftChain.setBypassed<ChainPositions::f1_6k>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f1_6k>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f1_6k>().coefficients = *peakCoefficients[19];
    *rightChain.get<ChainPositions::f1_6k>().coefficients = *peakCoefficients[19];

    leftChain.setBypassed<ChainPositions::f2k>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f2k>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f2k>().coefficients = *peakCoefficients[20];
    *rightChain.get<ChainPositions::f2k>().coefficients = *peakCoefficients[20];

    leftChain.setBypassed<ChainPositions::f2_5k>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f2_5k>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f2_5k>().coefficients = *peakCoefficients[21];
    *rightChain.get<ChainPositions::f2_5k>().coefficients = *peakCoefficients[21];

    leftChain.setBypassed<ChainPositions::f3_15k>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f3_15k>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f3_15k>().coefficients = *peakCoefficients[22];
    *rightChain.get<ChainPositions::f3_15k>().coefficients = *peakCoefficients[22];

    leftChain.setBypassed<ChainPositions::f4k>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f4k>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f4k>().coefficients = *peakCoefficients[23];
    *rightChain.get<ChainPositions::f4k>().coefficients = *peakCoefficients[23];

    leftChain.setBypassed<ChainPositions::f5k>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f5k>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f5k>().coefficients = *peakCoefficients[24];
    *rightChain.get<ChainPositions::f5k>().coefficients = *peakCoefficients[24];

    leftChain.setBypassed<ChainPositions::f6_3k>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f6_3k>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f6_3k>().coefficients = *peakCoefficients[25];
    *rightChain.get<ChainPositions::f6_3k>().coefficients = *peakCoefficients[25];

    leftChain.setBypassed<ChainPositions::f8k>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f8k>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f8k>().coefficients = *peakCoefficients[26];
    *rightChain.get<ChainPositions::f8k>().coefficients = *peakCoefficients[26];

    leftChain.setBypassed<ChainPositions::f10k>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f10k>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f10k>().coefficients = *peakCoefficients[27];
    *rightChain.get<ChainPositions::f10k>().coefficients = *peakCoefficients[27];

    leftChain.setBypassed<ChainPositions::f12_5k>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f12_5k>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f12_5k>().coefficients = *peakCoefficients[28];
    *rightChain.get<ChainPositions::f12_5k>().coefficients = *peakCoefficients[28];

    leftChain.setBypassed<ChainPositions::f16k>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f16k>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f16k>().coefficients = *peakCoefficients[29];
    *rightChain.get<ChainPositions::f16k>().coefficients = *peakCoefficients[29];

    leftChain.setBypassed<ChainPositions::f20k>(chainSettings.muteador);
    rightChain.setBypassed<ChainPositions::f20k>(chainSettings.muteador);

    *leftChain.get<ChainPositions::f20k>().coefficients = *peakCoefficients[30];
    *rightChain.get<ChainPositions::f20k>().coefficients = *peakCoefficients[30];

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EQgraficoAudioProcessor();
}
