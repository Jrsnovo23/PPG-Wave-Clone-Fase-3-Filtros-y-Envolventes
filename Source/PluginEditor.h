#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

/**
    Editor de la Fase 3: agrega paneles de Filtro y de las dos envolventes
    sobre la base de la Fase 2. Sigue siendo una interfaz genérica de JUCE,
    no la estética final inspirada en el hardware — eso es la Fase 7.
*/
class PPGWaveCloneAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit PPGWaveCloneAudioProcessorEditor (PPGWaveCloneAudioProcessor&);
    ~PPGWaveCloneAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct OscillatorControls
    {
        juce::Label title;
        juce::ComboBox wavetableBox;
        juce::Slider positionSlider, octaveSlider, coarseSlider, fineSlider, levelSlider;
        juce::Label positionLabel, octaveLabel, coarseLabel, fineLabel, levelLabel;

        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> wavetableAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
            positionAttachment, octaveAttachment, coarseAttachment, fineAttachment, levelAttachment;
    };

    struct FilterControls
    {
        juce::Label title;
        juce::ComboBox typeBox;
        juce::Slider cutoffSlider, resonanceSlider, keyTrackSlider, envAmountSlider;
        juce::Label cutoffLabel, resonanceLabel, keyTrackLabel, envAmountLabel;

        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> typeAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
            cutoffAttachment, resonanceAttachment, keyTrackAttachment, envAmountAttachment;
    };

    struct EnvelopeControls
    {
        juce::Label title;
        juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
        juce::Label attackLabel, decayLabel, sustainLabel, releaseLabel;

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
            attackAttachment, decayAttachment, sustainAttachment, releaseAttachment;
    };

    void setupOscillatorControls (OscillatorControls& controls, int oscNumber);
    void setupFilterControls();
    void setupEnvelopeControls (EnvelopeControls& controls, int envNumber, const juce::String& titleText);

    void layoutOscillatorControls (OscillatorControls& controls, juce::Rectangle<int> area);
    void layoutFilterControls (juce::Rectangle<int> area);
    void layoutEnvelopeControls (EnvelopeControls& controls, juce::Rectangle<int> area);

    PPGWaveCloneAudioProcessor& processorRef;

    juce::Label titleLabel;
    juce::Slider masterVolumeSlider;
    juce::Label  masterVolumeLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterVolumeAttachment;

    OscillatorControls osc1Controls, osc2Controls;
    FilterControls filterControls;
    EnvelopeControls env1Controls, env2Controls;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PPGWaveCloneAudioProcessorEditor)
};
