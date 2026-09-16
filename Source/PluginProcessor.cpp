#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Params/ParameterLayout.h"
#include "Params/ParameterIDs.h"
#include "DSP/WavetableFactory.h"

namespace
{
    constexpr int numVoicesPhase3 = 8; // polifonía configurable real llega en Fase 10

    OscillatorParameterPointers makeOscParams (juce::AudioProcessorValueTreeState& apvts, int oscNumber)
    {
        OscillatorParameterPointers p;
        p.wavetableIndex = apvts.getRawParameterValue (ParamIDs::oscWavetableIndex (oscNumber));
        p.wavePosition   = apvts.getRawParameterValue (ParamIDs::oscWavePosition (oscNumber));
        p.octave         = apvts.getRawParameterValue (ParamIDs::oscOctave (oscNumber));
        p.coarseTune     = apvts.getRawParameterValue (ParamIDs::oscCoarseTune (oscNumber));
        p.fineTune       = apvts.getRawParameterValue (ParamIDs::oscFineTune (oscNumber));
        p.level          = apvts.getRawParameterValue (ParamIDs::oscLevel (oscNumber));
        return p;
    }

    FilterParameterPointers makeFilterParams (juce::AudioProcessorValueTreeState& apvts)
    {
        FilterParameterPointers p;
        p.type      = apvts.getRawParameterValue (ParamIDs::filterType);
        p.cutoffHz  = apvts.getRawParameterValue (ParamIDs::filterCutoff);
        p.resonance = apvts.getRawParameterValue (ParamIDs::filterResonance);
        p.keyTrack  = apvts.getRawParameterValue (ParamIDs::filterKeyTrack);
        p.envAmount = apvts.getRawParameterValue (ParamIDs::filterEnvAmount);
        return p;
    }

    EnvelopeParameterPointers makeEnvParams (juce::AudioProcessorValueTreeState& apvts, int envNumber)
    {
        EnvelopeParameterPointers p;
        p.attack  = apvts.getRawParameterValue (ParamIDs::envAttack (envNumber));
        p.decay   = apvts.getRawParameterValue (ParamIDs::envDecay (envNumber));
        p.sustain = apvts.getRawParameterValue (ParamIDs::envSustain (envNumber));
        p.release = apvts.getRawParameterValue (ParamIDs::envRelease (envNumber));
        return p;
    }
}

PPGWaveCloneAudioProcessor::PPGWaveCloneAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", ParameterLayout::createParameterLayout()),
      wavetableSet (WavetableFactory::createDefaultSet()),
      osc1Params (makeOscParams (apvts, 1)),
      osc2Params (makeOscParams (apvts, 2)),
      filterParams (makeFilterParams (apvts)),
      env1Params (makeEnvParams (apvts, 1)),
      env2Params (makeEnvParams (apvts, 2))
{
    for (int i = 0; i < numVoicesPhase3; ++i)
        synth.addVoice (new SynthVoice (*wavetableSet, osc1Params, osc2Params,
                                         filterParams, env1Params, env2Params));

    synth.addSound (new SynthSound());
}

void PPGWaveCloneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
    juce::ignoreUnused (samplesPerBlock);
}

bool PPGWaveCloneAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void PPGWaveCloneAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                                juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());

    const float volumeDb = apvts.getRawParameterValue (ParamIDs::masterVolume)->load();
    buffer.applyGain (juce::Decibels::decibelsToGain (volumeDb));
}

juce::AudioProcessorEditor* PPGWaveCloneAudioProcessor::createEditor()
{
    return new PPGWaveCloneAudioProcessorEditor (*this);
}

void PPGWaveCloneAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void PPGWaveCloneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PPGWaveCloneAudioProcessor();
}
