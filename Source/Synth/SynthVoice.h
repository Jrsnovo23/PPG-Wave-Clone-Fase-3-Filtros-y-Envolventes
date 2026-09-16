#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include "../DSP/Oscillator.h"
#include "../DSP/WavetableSet.h"
#include "../DSP/Filter.h"
#include "../DSP/Envelope.h"
#include "../Params/OscillatorParameterPointers.h"
#include "../Params/FilterParameterPointers.h"
#include "../Params/EnvelopeParameterPointers.h"

/**
    Voz polifónica: OSC1 + OSC2 (wavetable) -> filtro multimodo -> salida.
    Envelope 1 controla la amplitud (sustituye la rampa anti-click de la
    Fase 2). Envelope 2 modula el cutoff del filtro. Otros destinos de
    Envelope 2 (wave position, pitch) descritos en el documento de diseño
    llegan en la Fase 4 junto con la matriz de modulación, que es el
    mecanismo pensado para rutear una fuente a varios destinos a la vez.
*/
class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice (const WavetableSet& wavetableSetIn,
                OscillatorParameterPointers& osc1ParamsIn,
                OscillatorParameterPointers& osc2ParamsIn,
                FilterParameterPointers& filterParamsIn,
                EnvelopeParameterPointers& env1ParamsIn,
                EnvelopeParameterPointers& env2ParamsIn);

    bool canPlaySound (juce::SynthesiserSound*) override { return true; }

    void startNote (int midiNoteNumber, float velocity,
                     juce::SynthesiserSound*, int pitchWheel) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                           int startSample, int numSamples) override;

private:
    Oscillator osc1, osc2;
    Filter filter;
    Envelope envelope1, envelope2;

    OscillatorParameterPointers& osc1Params;
    OscillatorParameterPointers& osc2Params;
    FilterParameterPointers& filterParams;
    EnvelopeParameterPointers& env1Params;
    EnvelopeParameterPointers& env2Params;

    double baseFrequencyHz = 440.0;
    int currentMidiNote = 60;
    float velocityGain = 1.0f;

    void refreshOscillatorSettings();
    void refreshEnvelopeSettings();
    void refreshFilterStaticSettings();
};
