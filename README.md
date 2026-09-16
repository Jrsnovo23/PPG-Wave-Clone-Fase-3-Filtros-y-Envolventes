# PPG Wave Clone — Fase 3: Filtros y Envolventes

Implementación original de un instrumento virtual VST3 de síntesis wavetable
inspirado conceptualmente en el PPG Wave 3.3. No contiene ROMs, muestras,
gráficos ni código propietario de terceros.

## Estado de esta entrega

**Fase 1** (arquitectura): listo — compila, carga en Ableton Live, estable en
Mac Intel (x86_64 forzado explícitamente).

**Fase 2** (osciladores): listo — motor de wavetables real, OSC1+OSC2,
morphing, confirmado sonando en hardware real.

**Fase 3** (esta entrega): filtro + envolventes reales.

- `Filter`: multimodo (Low Pass / High Pass / Band Pass) sobre
  `juce::dsp::StateVariableTPTFilter` — cutoff, resonance, key tracking,
  envelope amount.
- `Envelope`: ADSR real con segmentos lineales — reemplaza la rampa
  anti-click de las Fases 1-2.
- `SynthVoice` ahora es: OSC1+OSC2 → Filter → salida. Envelope 1 controla la
  amplitud; Envelope 2 modula el cutoff del filtro (bipolar, +/-4 octavas en
  el extremo del parámetro Env Amount).
- Parámetros nuevos, automatizables: Filter Type/Cutoff/Resonance/Key
  Track/Env Amount, y Attack/Decay/Sustain/Release para cada una de las dos
  envolventes.
- GUI ampliada con paneles de Filter, Env 1 (Amp) y Env 2 (Filter).

**Lo que NO incluye todavía**: LFOs, matriz de modulación (Envelope 2 solo
modula el filtro por ahora — wave position y pitch como destinos llegan con
la matriz en la Fase 4), efectos, gestión de presets, carácter "vintage" y
la interfaz gráfica definitiva.

## Problemas conocidos

- Mismo aliasing en notas agudas que en la Fase 2 (ver README anterior) —
  sigue siendo un comportamiento esperado, no un bug de esta fase.
- El filtro y la envolvente de amplitud se recalculan con resolución por
  muestra (correcto para que el movimiento del ADSR suene suave); los
  parámetros de oscilador y los ajustes "estáticos" del filtro (tipo,
  resonancia) se refrescan una vez por bloque, igual que en fases previas.
- Con Resonance muy alta y Cutoff muy bajo puede oírse auto-oscilación del
  filtro — es un comportamiento típico de un filtro resonante, no un error.

## Compilación sin instalar nada (recomendado)

Este proyecto incluye `.github/workflows/build.yml`: compila el VST3 en un
Mac con Xcode ya instalado, en la nube, gratis, vía GitHub Actions. Sube la
carpeta a un repositorio (con GitHub Desktop, para no perder `.github` por
ser una carpeta oculta), espera la palomita verde en la pestaña "Actions", y
descarga el `.vst3` desde "Artifacts".

## Compilación local (requiere CMake + compilador C++20)

1. Clona JUCE dentro de esta carpeta:
   ```
   git clone --depth 1 --branch 7.0.12 https://github.com/juce-framework/JUCE.git
   ```
   Debe quedar como `PPGWaveClone/JUCE/`.

2. Configura y compila:

   **macOS** (Xcode instalado):
   ```
   cmake -B build -G Xcode
   cmake --build build --config Release
   ```
   El `.vst3` queda en `build/PPGWaveClone_artefacts/Release/VST3/`.
   Cópialo a `~/Library/Audio/Plug-Ins/VST3/` (tu usuario) o
   `/Library/Audio/Plug-Ins/VST3/` (todo el sistema — recomendado si Ableton
   corre en otra sesión de usuario). Si lo bajaste de un navegador:
   ```
   xattr -dr com.apple.quarantine "/ruta/al/PPG Wave Clone.vst3"
   ```

   **Windows** (Visual Studio 2022):
   ```
   cmake -B build -G "Visual Studio 17 2022"
   cmake --build build --config Release
   ```
   El `.vst3` queda en `build/PPGWaveClone_artefacts/Release/VST3/`.
   Cópialo a `C:\Program Files\Common Files\VST3\`.

3. Rescanea plugins en Ableton Live (Preferences → Plug-ins → Rescan).

## Estructura

```
PPGWaveClone/
  CMakeLists.txt
  .github/workflows/build.yml   — compila el VST3 en la nube (macOS, x86_64)
  Source/
    PluginProcessor.h/.cpp      — AudioProcessor, APVTS, Synthesiser
    PluginEditor.h/.cpp         — GUI (Fase 3): OSC1/OSC2 + Filter + Env1/Env2
    Params/
      ParameterIDs.h
      ParameterLayout.h/.cpp
      OscillatorParameterPointers.h
      FilterParameterPointers.h
      EnvelopeParameterPointers.h
    DSP/
      Wavetable.h/.cpp
      WavetableSet.h/.cpp
      WavetableFactory.h/.cpp
      Oscillator.h
      Filter.h/.cpp                — multimodo sobre StateVariableTPTFilter
      Envelope.h/.cpp               — ADSR lineal
    Synth/
      SynthSound.h
      SynthVoice.h/.cpp             — OSC1+OSC2 -> Filter -> salida
```

## Próxima fase (Fase 4 — Modulación)

- `Source/DSP/LFO.h/.cpp`: dos LFOs (sine/triangle/square/saw/random/S&H),
  rate, sync al tempo del host, retrigger, fase, depth.
- `Source/DSP/ModulationMatrix.h/.cpp`: rutas fuente→destino con cantidad y
  curva, incluyendo los destinos de Envelope 2 que quedaron pendientes
  (wave position, pitch).
- Integrar ambos en `SynthVoice`.
- Nuevos parámetros de LFO y de cada ruta de la matriz.

Dime cuándo avanzamos a la Fase 4.
