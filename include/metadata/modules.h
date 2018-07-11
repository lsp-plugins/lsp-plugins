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

#ifndef MOD_LADSPA
    #define MOD_LADSPA(x)
#endif /* MOD_LADSPA */

#ifndef MOD_LV2
    #define MOD_LV2(x)
#endif /* MOD_LV2 */

#ifndef MOD_VST
    #define MOD_VST(x)
#endif /* MOD_VST */

#ifndef MOD_AU
    #define MOD_AU(x)
#endif /* MOD_AU */

#ifndef MOD_GTK2
    #define MOD_GTK2(x)
#endif /* MOD_GTK2 */

#ifndef MOD_GTK3
    #define MOD_GTK3(x)
#endif /* MOD_GTK3 */

#ifndef MOD_QT4
    #define MOD_QT4(x)
#endif /* MOD_QT4 */

#ifndef MOD_QT5
    #define MOD_QT5(x)
#endif /* MOD_QT5 */

// Specify modules

// Phase detector
MOD_PLUGIN(phase_detector)
MOD_LADSPA(phase_detector)
MOD_LV2(phase_detector)
MOD_VST(phase_detector)
MOD_GTK2(phase_detector)

// Delay compensator
MOD_PLUGIN(comp_delay_mono)
MOD_LADSPA(comp_delay_mono)
MOD_LV2(comp_delay_mono)
MOD_VST(comp_delay_mono)
MOD_GTK2(comp_delay_mono)

MOD_PLUGIN(comp_delay_stereo)
MOD_LADSPA(comp_delay_stereo)
MOD_LV2(comp_delay_stereo)
MOD_VST(comp_delay_stereo)
MOD_GTK2(comp_delay_stereo)

MOD_PLUGIN(comp_delay_x2_stereo)
MOD_LADSPA(comp_delay_x2_stereo)
MOD_LV2(comp_delay_x2_stereo)
MOD_VST(comp_delay_x2_stereo)
MOD_GTK2(comp_delay_x2_stereo)

// Spectrum Analyzer
MOD_PLUGIN(spectrum_analyzer_x1)
MOD_LADSPA(spectrum_analyzer_x1)
MOD_LV2(spectrum_analyzer_x1)
MOD_VST(spectrum_analyzer_x1)
MOD_GTK2(spectrum_analyzer_x1)

MOD_PLUGIN(spectrum_analyzer_x2)
MOD_LADSPA(spectrum_analyzer_x2)
MOD_LV2(spectrum_analyzer_x2)
MOD_VST(spectrum_analyzer_x2)
MOD_GTK2(spectrum_analyzer_x2)

MOD_PLUGIN(spectrum_analyzer_x4)
MOD_LADSPA(spectrum_analyzer_x4)
MOD_LV2(spectrum_analyzer_x4)
MOD_VST(spectrum_analyzer_x4)
MOD_GTK2(spectrum_analyzer_x4)

MOD_PLUGIN(spectrum_analyzer_x8)
MOD_LADSPA(spectrum_analyzer_x8)
MOD_LV2(spectrum_analyzer_x8)
MOD_VST(spectrum_analyzer_x8)
MOD_GTK2(spectrum_analyzer_x8)

MOD_PLUGIN(spectrum_analyzer_x12)
MOD_LADSPA(spectrum_analyzer_x12)
MOD_LV2(spectrum_analyzer_x12)
MOD_VST(spectrum_analyzer_x12)
MOD_GTK2(spectrum_analyzer_x12)

MOD_PLUGIN(spectrum_analyzer_x16)
MOD_LADSPA(spectrum_analyzer_x16)
MOD_LV2(spectrum_analyzer_x16)
MOD_VST(spectrum_analyzer_x16)
MOD_GTK2(spectrum_analyzer_x16)

// Sampler
MOD_PLUGIN(sampler_mono)
MOD_LV2(sampler_mono)
MOD_VST(sampler_mono)
MOD_GTK2(sampler_mono)

MOD_PLUGIN(sampler_stereo)
MOD_LV2(sampler_stereo)
MOD_VST(sampler_stereo)
MOD_GTK2(sampler_stereo)

MOD_PLUGIN(multisampler_x12)
MOD_LV2(multisampler_x12)
MOD_VST(multisampler_x12)
MOD_GTK2(multisampler_x12)

MOD_PLUGIN(multisampler_x12_do)
MOD_LV2(multisampler_x12_do)
MOD_VST(multisampler_x12_do)
MOD_GTK2(multisampler_x12_do)

MOD_PLUGIN(multisampler_x24)
MOD_LV2(multisampler_x24)
MOD_VST(multisampler_x24)
MOD_GTK2(multisampler_x24)

MOD_PLUGIN(multisampler_x24_do)
MOD_LV2(multisampler_x24_do)
MOD_VST(multisampler_x24_do)
MOD_GTK2(multisampler_x24_do)

MOD_PLUGIN(multisampler_x48)
MOD_LV2(multisampler_x48)
MOD_VST(multisampler_x48)
MOD_GTK2(multisampler_x48)

MOD_PLUGIN(multisampler_x48_do)
MOD_LV2(multisampler_x48_do)
MOD_VST(multisampler_x48_do)
MOD_GTK2(multisampler_x48_do)

// Trigger
MOD_PLUGIN(trigger_mono)
MOD_LV2(trigger_mono)
MOD_VST(trigger_mono)
MOD_GTK2(trigger_mono)

MOD_PLUGIN(trigger_stereo)
MOD_LV2(trigger_stereo)
MOD_VST(trigger_stereo)
MOD_GTK2(trigger_stereo)

MOD_PLUGIN(trigger_midi_mono)
MOD_LV2(trigger_midi_mono)
MOD_VST(trigger_midi_mono)
MOD_GTK2(trigger_midi_mono)

MOD_PLUGIN(trigger_midi_stereo)
MOD_LV2(trigger_midi_stereo)
MOD_VST(trigger_midi_stereo)
MOD_GTK2(trigger_midi_stereo)

// Parametric Equalizer
MOD_PLUGIN(para_equalizer_x16_mono)
MOD_LADSPA(para_equalizer_x16_mono)
MOD_LV2(para_equalizer_x16_mono)
MOD_VST(para_equalizer_x16_mono)
MOD_GTK2(para_equalizer_x16_mono)

MOD_PLUGIN(para_equalizer_x32_mono)
MOD_LADSPA(para_equalizer_x32_mono)
MOD_LV2(para_equalizer_x32_mono)
MOD_VST(para_equalizer_x32_mono)
MOD_GTK2(para_equalizer_x32_mono)

MOD_PLUGIN(para_equalizer_x16_stereo)
MOD_LADSPA(para_equalizer_x16_stereo)
MOD_LV2(para_equalizer_x16_stereo)
MOD_VST(para_equalizer_x16_stereo)
MOD_GTK2(para_equalizer_x16_stereo)

MOD_PLUGIN(para_equalizer_x32_stereo)
MOD_LADSPA(para_equalizer_x32_stereo)
MOD_LV2(para_equalizer_x32_stereo)
MOD_VST(para_equalizer_x32_stereo)
MOD_GTK2(para_equalizer_x32_stereo)

MOD_PLUGIN(para_equalizer_x16_lr)
MOD_LADSPA(para_equalizer_x16_lr)
MOD_LV2(para_equalizer_x16_lr)
MOD_VST(para_equalizer_x16_lr)
MOD_GTK2(para_equalizer_x16_lr)

MOD_PLUGIN(para_equalizer_x32_lr)
MOD_LADSPA(para_equalizer_x32_lr)
MOD_LV2(para_equalizer_x32_lr)
MOD_VST(para_equalizer_x32_lr)
MOD_GTK2(para_equalizer_x32_lr)

MOD_PLUGIN(para_equalizer_x16_ms)
MOD_LADSPA(para_equalizer_x16_ms)
MOD_LV2(para_equalizer_x16_ms)
MOD_VST(para_equalizer_x16_ms)
MOD_GTK2(para_equalizer_x16_ms)

MOD_PLUGIN(para_equalizer_x32_ms)
MOD_LADSPA(para_equalizer_x32_ms)
MOD_LV2(para_equalizer_x32_ms)
MOD_VST(para_equalizer_x32_ms)
MOD_GTK2(para_equalizer_x32_ms)

// Graphic Equalizer
MOD_PLUGIN(graph_equalizer_x16_mono)
MOD_LADSPA(graph_equalizer_x16_mono)
MOD_LV2(graph_equalizer_x16_mono)
MOD_VST(graph_equalizer_x16_mono)
MOD_GTK2(graph_equalizer_x16_mono)

MOD_PLUGIN(graph_equalizer_x16_stereo)
MOD_LADSPA(graph_equalizer_x16_stereo)
MOD_LV2(graph_equalizer_x16_stereo)
MOD_VST(graph_equalizer_x16_stereo)
MOD_GTK2(graph_equalizer_x16_stereo)

MOD_PLUGIN(graph_equalizer_x16_lr)
MOD_LADSPA(graph_equalizer_x16_lr)
MOD_LV2(graph_equalizer_x16_lr)
MOD_VST(graph_equalizer_x16_lr)
MOD_GTK2(graph_equalizer_x16_lr)

MOD_PLUGIN(graph_equalizer_x16_ms)
MOD_LADSPA(graph_equalizer_x16_ms)
MOD_LV2(graph_equalizer_x16_ms)
MOD_VST(graph_equalizer_x16_ms)
MOD_GTK2(graph_equalizer_x16_ms)

MOD_PLUGIN(graph_equalizer_x32_mono)
MOD_LADSPA(graph_equalizer_x32_mono)
MOD_LV2(graph_equalizer_x32_mono)
MOD_VST(graph_equalizer_x32_mono)
MOD_GTK2(graph_equalizer_x32_mono)

MOD_PLUGIN(graph_equalizer_x32_stereo)
MOD_LADSPA(graph_equalizer_x32_stereo)
MOD_LV2(graph_equalizer_x32_stereo)
MOD_VST(graph_equalizer_x32_stereo)
MOD_GTK2(graph_equalizer_x32_stereo)

MOD_PLUGIN(graph_equalizer_x32_lr)
MOD_LADSPA(graph_equalizer_x32_lr)
MOD_LV2(graph_equalizer_x32_lr)
MOD_VST(graph_equalizer_x32_lr)
MOD_GTK2(graph_equalizer_x32_lr)

MOD_PLUGIN(graph_equalizer_x32_ms)
MOD_LADSPA(graph_equalizer_x32_ms)
MOD_LV2(graph_equalizer_x32_ms)
MOD_VST(graph_equalizer_x32_ms)
MOD_GTK2(graph_equalizer_x32_ms)

// Compressor
MOD_PLUGIN(compressor_mono)
MOD_LADSPA(compressor_mono)
MOD_LV2(compressor_mono)
MOD_VST(compressor_mono)
MOD_GTK2(compressor_mono)

MOD_PLUGIN(compressor_stereo)
MOD_LADSPA(compressor_stereo)
MOD_LV2(compressor_stereo)
MOD_VST(compressor_stereo)
MOD_GTK2(compressor_stereo)

MOD_PLUGIN(compressor_lr)
MOD_LADSPA(compressor_lr)
MOD_LV2(compressor_lr)
MOD_VST(compressor_lr)
MOD_GTK2(compressor_lr)

MOD_PLUGIN(compressor_ms)
MOD_LADSPA(compressor_ms)
MOD_LV2(compressor_ms)
MOD_VST(compressor_ms)
MOD_GTK2(compressor_ms)

MOD_PLUGIN(sc_compressor_mono)
MOD_LADSPA(sc_compressor_mono)
MOD_LV2(sc_compressor_mono)
MOD_VST(sc_compressor_mono)
MOD_GTK2(sc_compressor_mono)

MOD_PLUGIN(sc_compressor_stereo)
MOD_LADSPA(sc_compressor_stereo)
MOD_LV2(sc_compressor_stereo)
MOD_VST(sc_compressor_stereo)
MOD_GTK2(sc_compressor_stereo)

MOD_PLUGIN(sc_compressor_lr)
MOD_LADSPA(sc_compressor_lr)
MOD_LV2(sc_compressor_lr)
MOD_VST(sc_compressor_lr)
MOD_GTK2(sc_compressor_lr)

MOD_PLUGIN(sc_compressor_ms)
MOD_LADSPA(sc_compressor_ms)
MOD_LV2(sc_compressor_ms)
MOD_VST(sc_compressor_ms)
MOD_GTK2(sc_compressor_ms)

// Dynamic processor
MOD_PLUGIN(dyna_processor_mono)
MOD_LADSPA(dyna_processor_mono)
MOD_LV2(dyna_processor_mono)
MOD_VST(dyna_processor_mono)
MOD_GTK2(dyna_processor_mono)

MOD_PLUGIN(dyna_processor_stereo)
MOD_LADSPA(dyna_processor_stereo)
MOD_LV2(dyna_processor_stereo)
MOD_VST(dyna_processor_stereo)
MOD_GTK2(dyna_processor_stereo)

MOD_PLUGIN(dyna_processor_lr)
MOD_LADSPA(dyna_processor_lr)
MOD_LV2(dyna_processor_lr)
MOD_VST(dyna_processor_lr)
MOD_GTK2(dyna_processor_lr)

MOD_PLUGIN(dyna_processor_ms)
MOD_LADSPA(dyna_processor_ms)
MOD_LV2(dyna_processor_ms)
MOD_VST(dyna_processor_ms)
MOD_GTK2(dyna_processor_ms)

MOD_PLUGIN(sc_dyna_processor_mono)
MOD_LADSPA(sc_dyna_processor_mono)
MOD_LV2(sc_dyna_processor_mono)
MOD_VST(sc_dyna_processor_mono)
MOD_GTK2(sc_dyna_processor_mono)

MOD_PLUGIN(sc_dyna_processor_stereo)
MOD_LADSPA(sc_dyna_processor_stereo)
MOD_LV2(sc_dyna_processor_stereo)
MOD_VST(sc_dyna_processor_stereo)
MOD_GTK2(sc_dyna_processor_stereo)

MOD_PLUGIN(sc_dyna_processor_lr)
MOD_LADSPA(sc_dyna_processor_lr)
MOD_LV2(sc_dyna_processor_lr)
MOD_VST(sc_dyna_processor_lr)
MOD_GTK2(sc_dyna_processor_lr)

MOD_PLUGIN(sc_dyna_processor_ms)
MOD_LADSPA(sc_dyna_processor_ms)
MOD_LV2(sc_dyna_processor_ms)
MOD_VST(sc_dyna_processor_ms)
MOD_GTK2(sc_dyna_processor_ms)

// Expander
MOD_PLUGIN(expander_mono)
MOD_LADSPA(expander_mono)
MOD_LV2(expander_mono)
MOD_VST(expander_mono)
MOD_GTK2(expander_mono)

MOD_PLUGIN(expander_stereo)
MOD_LADSPA(expander_stereo)
MOD_LV2(expander_stereo)
MOD_VST(expander_stereo)
MOD_GTK2(expander_stereo)

MOD_PLUGIN(expander_lr)
MOD_LADSPA(expander_lr)
MOD_LV2(expander_lr)
MOD_VST(expander_lr)
MOD_GTK2(expander_lr)

MOD_PLUGIN(expander_ms)
MOD_LADSPA(expander_ms)
MOD_LV2(expander_ms)
MOD_VST(expander_ms)
MOD_GTK2(expander_ms)

MOD_PLUGIN(sc_expander_mono)
MOD_LADSPA(sc_expander_mono)
MOD_LV2(sc_expander_mono)
MOD_VST(sc_expander_mono)
MOD_GTK2(sc_expander_mono)

MOD_PLUGIN(sc_expander_stereo)
MOD_LADSPA(sc_expander_stereo)
MOD_LV2(sc_expander_stereo)
MOD_VST(sc_expander_stereo)
MOD_GTK2(sc_expander_stereo)

MOD_PLUGIN(sc_expander_lr)
MOD_LADSPA(sc_expander_lr)
MOD_LV2(sc_expander_lr)
MOD_VST(sc_expander_lr)
MOD_GTK2(sc_expander_lr)

MOD_PLUGIN(sc_expander_ms)
MOD_LADSPA(sc_expander_ms)
MOD_LV2(sc_expander_ms)
MOD_VST(sc_expander_ms)
MOD_GTK2(sc_expander_ms)

// Gate
MOD_PLUGIN(gate_mono)
MOD_LADSPA(gate_mono)
MOD_LV2(gate_mono)
MOD_VST(gate_mono)
MOD_GTK2(gate_mono)

MOD_PLUGIN(gate_stereo)
MOD_LADSPA(gate_stereo)
MOD_LV2(gate_stereo)
MOD_VST(gate_stereo)
MOD_GTK2(gate_stereo)

MOD_PLUGIN(gate_lr)
MOD_LADSPA(gate_lr)
MOD_LV2(gate_lr)
MOD_VST(gate_lr)
MOD_GTK2(gate_lr)

MOD_PLUGIN(gate_ms)
MOD_LADSPA(gate_ms)
MOD_LV2(gate_ms)
MOD_VST(gate_ms)
MOD_GTK2(gate_ms)

MOD_PLUGIN(sc_gate_mono)
MOD_LADSPA(sc_gate_mono)
MOD_LV2(sc_gate_mono)
MOD_VST(sc_gate_mono)
MOD_GTK2(sc_gate_mono)

MOD_PLUGIN(sc_gate_stereo)
MOD_LADSPA(sc_gate_stereo)
MOD_LV2(sc_gate_stereo)
MOD_VST(sc_gate_stereo)
MOD_GTK2(sc_gate_stereo)

MOD_PLUGIN(sc_gate_lr)
MOD_LADSPA(sc_gate_lr)
MOD_LV2(sc_gate_lr)
MOD_VST(sc_gate_lr)
MOD_GTK2(sc_gate_lr)

MOD_PLUGIN(sc_gate_ms)
MOD_LADSPA(sc_gate_ms)
MOD_LV2(sc_gate_ms)
MOD_VST(sc_gate_ms)
MOD_GTK2(sc_gate_ms)

// Limiter
MOD_PLUGIN(limiter_mono)
MOD_LADSPA(limiter_mono)
MOD_LV2(limiter_mono)
MOD_VST(limiter_mono)
MOD_GTK2(limiter_mono)

MOD_PLUGIN(limiter_stereo)
MOD_LADSPA(limiter_stereo)
MOD_LV2(limiter_stereo)
MOD_VST(limiter_stereo)
MOD_GTK2(limiter_stereo)

MOD_PLUGIN(sc_limiter_mono)
MOD_LADSPA(sc_limiter_mono)
MOD_LV2(sc_limiter_mono)
MOD_VST(sc_limiter_mono)
MOD_GTK2(sc_limiter_mono)

MOD_PLUGIN(sc_limiter_stereo)
MOD_LADSPA(sc_limiter_stereo)
MOD_LV2(sc_limiter_stereo)
MOD_VST(sc_limiter_stereo)
MOD_GTK2(sc_limiter_stereo)

// Impulse responses
MOD_PLUGIN(impulse_responses_mono)
MOD_LV2(impulse_responses_mono)
MOD_VST(impulse_responses_mono)
MOD_GTK2(impulse_responses_mono)

MOD_PLUGIN(impulse_responses_stereo)
MOD_LV2(impulse_responses_stereo)
MOD_VST(impulse_responses_stereo)
MOD_GTK2(impulse_responses_stereo)

// Impulse reverb
MOD_PLUGIN(impulse_reverb_mono)
MOD_LV2(impulse_reverb_mono)
MOD_VST(impulse_reverb_mono)
MOD_GTK2(impulse_reverb_mono)

MOD_PLUGIN(impulse_reverb_stereo)
MOD_LV2(impulse_reverb_stereo)
MOD_VST(impulse_reverb_stereo)
MOD_GTK2(impulse_reverb_stereo)

// Slap delay
MOD_PLUGIN(slap_delay_mono)
MOD_LADSPA(slap_delay_mono)
MOD_LV2(slap_delay_mono)
MOD_VST(slap_delay_mono)
MOD_GTK2(slap_delay_mono)

MOD_PLUGIN(slap_delay_stereo)
MOD_LADSPA(slap_delay_stereo)
MOD_LV2(slap_delay_stereo)
MOD_VST(slap_delay_stereo)
MOD_GTK2(slap_delay_stereo)

#ifndef LSP_NO_EXPERIMENTAL

#endif

// Undefine capabilities
#undef MOD_PLUGIN
#undef MOD_LADSPA
#undef MOD_LV2
#undef MOD_VST
#undef MOD_AU
#undef MOD_GTK2
#undef MOD_GTK3
#undef MOD_QT4
#undef MOD_QT5
