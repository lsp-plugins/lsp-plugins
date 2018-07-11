/*
 * plugin_list.h
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
// !!! Do not edit the order of several plugins !!!
// This may break LADSPA_ID numbering.

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

#ifndef LSP_NO_EXPERMIENTAL
    // Impulse responses
    MOD_PLUGIN(impulse_responses_mono)
    MOD_LV2(impulse_responses_mono)
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
