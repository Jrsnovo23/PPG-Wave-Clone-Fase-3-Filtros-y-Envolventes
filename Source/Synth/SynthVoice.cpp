#include "SynthVoice.h"
#include <cmath>

SynthVoice::SynthVoice (const WavetableSet& wavetableSetIn,
                         OscillatorParameterPointers& osc1ParamsIn,
                         OscillatorParameterPointers& osc2ParamsIn,
                         FilterParameterPointers& filterParamsIn,
                         EnvelopeParameterPointers& env1ParamsIn,
                         EnvelopeParameterPointers& env2ParamsIn)
    : osc1Params (osc1ParamsIn), osc2Params (osc2ParamsIn),
      filterParams (filterParamsIn), env1Params (env1ParamsIn), env2Params (env2ParamsIn)
{
    osc1.setWavetableSet (&wavetableSetIn);
    osc2.setWavetableSet (&wavetableSetIn);
}

void SynthVoice::refreshOscillatorSettings()
{
    osc1.setWavetableIndex ((int) osc1Params.wavetableIndex->load());
    osc1.setWavePosition (osc1Params.wavePosition->load());
    osc1.setLevel (osc1Params.level->load());

    osc2.setWavetableIndex ((int) osc2Params.wavetableIndex->load());
    osc2.setWavePosition (osc2Params.wavePosition->load());
    osc2.setLevel (osc2Params.level->load());

    const auto computeFreq = [this] (const OscillatorParameterPointers& p) -> float
    {
        const float octave = p.octave->load();
        const float coarse = p.coarseTune->load();
        const float fine   = p.fineTune->load();
        const float semitoneOffset = (octave * 12.0f) + coarse + (fine / 100.0f);
        return (float) (baseFrequencyHz * std::pow (2.0, (double) semitoneOffset / 12.0));
    };

    osc1.setSampleRate (getSampleRate());
    osc2.setSampleRate (getSampleRate());
    osc1.setFrequency (computeFreq (osc1Params));
    osc2.setFrequency (computeFreq (osc2Params));
}

void SynthVoice::refreshEnvelopeSettings()
{
    envelope1.setSampleRate (getSampleRate());
    envelope1.setAttackSeconds (env1Params.attack->load());
    envelope1.setDecaySeconds (env1Params.decay->load());
    envelope1.setSustainLevel (env1Params.sustain->load());
    envelope1.setReleaseSeconds (env1Params.release->load());

    envelope2.setSampleRate (getSampleRate());
    envelope2.setAttackSeconds (env2Params.attack->load());
    envelope2.setDecaySeconds (env2Params.decay->load());
    envelope2.setSustainLevel (env2Params.sustain->load());
    envelope2.setReleaseSeconds (env2Params.release->load());
}

void SynthVoice::refreshFilterStaticSettings()
{
    filter.setSampleRate (getSampleRate());

    const int typeIndex = (int) filterParams.type->load();
    filter.setType (typeIndex == 1 ? Filter::Type::highPass
                   : typeIndex == 2 ? Filter::Type::bandPass
                                    : Filter::Type::lowPass);

    filter.setResonance (filterParams.resonance->load());
}

void SynthVoice::startNote (int midiNoteNumber, float velocity,
                             juce::SynthesiserSound*, int /*pitchWheel*/)
{
    currentMidiNote = midiNoteNumber;
    baseFrequencyHz = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
    velocityGain = velocity;

    osc1.resetPhase();
    osc2.resetPhase();
    refreshOscillatorSettings();

    refreshFilterStaticSettings();
    filter.reset();

    refreshEnvelopeSettings();
    envelope1.noteOn();
    envelope2.noteOn();
}

void SynthVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    if (! allowTailOff)
        clearCurrentNote();

    envelope1.noteOff();
    envelope2.noteOff();
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                                   int startSample, int numSamples)
{
    // Ajustes que no necesitan resolución por muestra: una vez por bloque
    // (mismo criterio que los parámetros de oscilador desde la Fase 2).
    refreshOscillatorSettings();
    refreshEnvelopeSettings();
    refreshFilterStaticSettings();

    const float baseCutoff = filterParams.cutoffHz->load();
    const float keyTrack   = filterParams.keyTrack->load();
    const float envAmount  = filterParams.envAmount->load();

    // Key tracking: más brillante en notas agudas, relativo a C4 (nota 60).
    const float keyTrackOctaves = keyTrack * (float) (currentMidiNote - 60) / 12.0f;
    const float keyTrackMultiplier = std::pow (2.0f, keyTrackOctaves);

    while (--numSamples >= 0)
    {
        const float ampEnv    = envelope1.getNextSample();
        const float filterEnv = envelope2.getNextSample();

        // Cutoff final: base * key tracking, desplazado por la envolvente
        // del filtro (+/- 4 octavas en el extremo del Env Amount).
        const float envOctaves = envAmount * filterEnv * 4.0f;
        const float finalCutoff = juce::jlimit (20.0f, 20000.0f,
            baseCutoff * keyTrackMultiplier * std::pow (2.0f, envOctaves));
        filter.setCutoffHz (finalCutoff);

        const float oscMix   = (osc1.renderSample() + osc2.renderSample()) * 0.5f;
        const float filtered = filter.processSample (oscMix);
        const float sample   = filtered * ampEnv * velocityGain;

        for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
            outputBuffer.addSample (ch, startSample, sample);

        ++startSample;

        if (! envelope1.isActive())
        {
            clearCurrentNote();
            break;
        }
    }
}
