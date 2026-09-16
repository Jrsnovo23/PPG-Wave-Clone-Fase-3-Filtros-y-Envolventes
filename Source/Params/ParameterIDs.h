#pragma once

#include <juce_core/juce_core.h>

/**
    IDs de parámetros del plugin.

    IMPORTANTE: una vez publicado un preset o proyecto de Ableton Live que use
    un ID, ese ID no debe cambiar de nombre — Ableton Live guarda automatizaciones
    y estado del plugin referenciando estos strings. Nuevos parámetros de fases
    futuras se agregan aquí, nunca se renombran los existentes.
*/
namespace ParamIDs
{
    static const juce::String masterVolume { "masterVolume" };

    // Osciladores (Fase 2)
    inline juce::String oscWavetableIndex (int oscNumber) { return "osc" + juce::String (oscNumber) + "WavetableIndex"; }
    inline juce::String oscWavePosition   (int oscNumber) { return "osc" + juce::String (oscNumber) + "WavePosition"; }
    inline juce::String oscOctave         (int oscNumber) { return "osc" + juce::String (oscNumber) + "Octave"; }
    inline juce::String oscCoarseTune     (int oscNumber) { return "osc" + juce::String (oscNumber) + "CoarseTune"; }
    inline juce::String oscFineTune       (int oscNumber) { return "osc" + juce::String (oscNumber) + "FineTune"; }
    inline juce::String oscLevel          (int oscNumber) { return "osc" + juce::String (oscNumber) + "Level"; }

    // Filtro (Fase 3)
    static const juce::String filterType      { "filterType" };
    static const juce::String filterCutoff    { "filterCutoff" };
    static const juce::String filterResonance { "filterResonance" };
    static const juce::String filterKeyTrack  { "filterKeyTrack" };
    static const juce::String filterEnvAmount { "filterEnvAmount" };

    // Envolventes (Fase 3): env 1 = amplificador, env 2 = filtro
    inline juce::String envAttack  (int envNumber) { return "env" + juce::String (envNumber) + "Attack"; }
    inline juce::String envDecay   (int envNumber) { return "env" + juce::String (envNumber) + "Decay"; }
    inline juce::String envSustain (int envNumber) { return "env" + juce::String (envNumber) + "Sustain"; }
    inline juce::String envRelease (int envNumber) { return "env" + juce::String (envNumber) + "Release"; }

    // --- Fases futuras --------------------------------------------------
    // Fase 4: LFO 1/2, Modulation Matrix (incluye Env2 -> Wave Position, Pitch)
    // Fase 5: Chorus, Delay, Reverb, Drive
    // Fase 8: Vintage Character
}
