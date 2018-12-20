/*
 * modules.h
 *
 *  Created on: 06 окт. 2015 г.
 *      Author: sadko
 */

// Define capabilities
#ifndef MOD_PLUGIN
    #define MOD_PLUGIN(x)
#endif /* MOD_PLUGIN */

// Specify modules

// Phase detector
MOD_PLUGIN(phase_detector)

// Delay compensator
MOD_PLUGIN(comp_delay_mono)
MOD_PLUGIN(comp_delay_stereo)
MOD_PLUGIN(comp_delay_x2_stereo)

// Spectrum Analyzer
MOD_PLUGIN(spectrum_analyzer_x1)
MOD_PLUGIN(spectrum_analyzer_x2)
MOD_PLUGIN(spectrum_analyzer_x4)
MOD_PLUGIN(spectrum_analyzer_x8)
MOD_PLUGIN(spectrum_analyzer_x12)
MOD_PLUGIN(spectrum_analyzer_x16)

// Sampler
MOD_PLUGIN(sampler_mono)
MOD_PLUGIN(sampler_stereo)
MOD_PLUGIN(multisampler_x12)
MOD_PLUGIN(multisampler_x12_do)
MOD_PLUGIN(multisampler_x24)
MOD_PLUGIN(multisampler_x24_do)
MOD_PLUGIN(multisampler_x48)
MOD_PLUGIN(multisampler_x48_do)

// Trigger
MOD_PLUGIN(trigger_mono)
MOD_PLUGIN(trigger_stereo)
MOD_PLUGIN(trigger_midi_mono)
MOD_PLUGIN(trigger_midi_stereo)

// Parametric Equalizer
MOD_PLUGIN(para_equalizer_x16_mono)
MOD_PLUGIN(para_equalizer_x32_mono)
MOD_PLUGIN(para_equalizer_x16_stereo)
MOD_PLUGIN(para_equalizer_x32_stereo)
MOD_PLUGIN(para_equalizer_x16_lr)
MOD_PLUGIN(para_equalizer_x32_lr)
MOD_PLUGIN(para_equalizer_x16_ms)
MOD_PLUGIN(para_equalizer_x32_ms)

// Graphic Equalizer
MOD_PLUGIN(graph_equalizer_x16_mono)
MOD_PLUGIN(graph_equalizer_x16_stereo)
MOD_PLUGIN(graph_equalizer_x16_lr)
MOD_PLUGIN(graph_equalizer_x16_ms)
MOD_PLUGIN(graph_equalizer_x32_mono)
MOD_PLUGIN(graph_equalizer_x32_stereo)
MOD_PLUGIN(graph_equalizer_x32_lr)
MOD_PLUGIN(graph_equalizer_x32_ms)

// Compressor
MOD_PLUGIN(compressor_mono)
MOD_PLUGIN(compressor_stereo)
MOD_PLUGIN(compressor_lr)
MOD_PLUGIN(compressor_ms)
MOD_PLUGIN(sc_compressor_mono)
MOD_PLUGIN(sc_compressor_stereo)
MOD_PLUGIN(sc_compressor_lr)
MOD_PLUGIN(sc_compressor_ms)

// Dynamic processor
MOD_PLUGIN(dyna_processor_mono)
MOD_PLUGIN(dyna_processor_stereo)
MOD_PLUGIN(dyna_processor_lr)
MOD_PLUGIN(dyna_processor_ms)
MOD_PLUGIN(sc_dyna_processor_mono)
MOD_PLUGIN(sc_dyna_processor_stereo)
MOD_PLUGIN(sc_dyna_processor_lr)
MOD_PLUGIN(sc_dyna_processor_ms)

// Expander
MOD_PLUGIN(expander_mono)
MOD_PLUGIN(expander_stereo)
MOD_PLUGIN(expander_lr)
MOD_PLUGIN(expander_ms)
MOD_PLUGIN(sc_expander_mono)
MOD_PLUGIN(sc_expander_stereo)
MOD_PLUGIN(sc_expander_lr)
MOD_PLUGIN(sc_expander_ms)

// Gate
MOD_PLUGIN(gate_mono)
MOD_PLUGIN(gate_stereo)
MOD_PLUGIN(gate_lr)
MOD_PLUGIN(gate_ms)
MOD_PLUGIN(sc_gate_mono)
MOD_PLUGIN(sc_gate_stereo)
MOD_PLUGIN(sc_gate_lr)
MOD_PLUGIN(sc_gate_ms)

// Limiter
MOD_PLUGIN(limiter_mono)
MOD_PLUGIN(limiter_stereo)
MOD_PLUGIN(sc_limiter_mono)
MOD_PLUGIN(sc_limiter_stereo)

// Impulse responses
MOD_PLUGIN(impulse_responses_mono)
MOD_PLUGIN(impulse_responses_stereo)

// Impulse reverb
MOD_PLUGIN(impulse_reverb_mono)
MOD_PLUGIN(impulse_reverb_stereo)

// Slap delay
MOD_PLUGIN(slap_delay_mono)
MOD_PLUGIN(slap_delay_stereo)

// Oscillator
MOD_PLUGIN(oscillator_mono)

// Latency Meter
MOD_PLUGIN(latency_meter)

// Multiband compressors
MOD_PLUGIN(mb_compressor_mono)
MOD_PLUGIN(mb_compressor_stereo)
MOD_PLUGIN(mb_compressor_lr)
MOD_PLUGIN(mb_compressor_ms)
MOD_PLUGIN(sc_mb_compressor_mono)
MOD_PLUGIN(sc_mb_compressor_stereo)
MOD_PLUGIN(sc_mb_compressor_lr)
MOD_PLUGIN(sc_mb_compressor_ms)

// Profiler
MOD_PLUGIN(profiler_mono)
MOD_PLUGIN(profiler_stereo)

// Nonlinear Convolver
// MOD_PLUGIN(nonlinear_convolver_mono)

#ifndef LSP_NO_EXPERIMENTAL
    // Test plugin
    MOD_PLUGIN(test_plugin)
#endif

// Undefine capabilities
#undef MOD_PLUGIN

