#include <core/types.h>
#include <core/dsp.h>
#include <core/alloc.h>

#include <container/jack/defs.h>

namespace jack_test
{
    using namespace lsp;

    int test(int argc, const char **argv)
    {
        const char *plugin =
//                "spectrum_analyzer_x1";
//                "spectrum_analyzer_x2";
//                "spectrum_analyzer_x4";
//                "spectrum_analyzer_x8";
//                "spectrum_analyzer_x12";
//                "spectrum_analyzer_x16";
//                "impulse_responses_mono";
//                "impulse_responses_stereo";
//                "impulse_reverb_mono";
//                "impulse_reverb_stereo";
//                "comp_delay_mono";
//                "comp_delay_stereo";
//                "comp_delay_x2_stereo";
//                "slap_delay_mono";
//                "slap_delay_stereo";
//                "sc_compressor_lr";
//                "sc_compressor_mono";
//                "sc_compressor_ms";
//                "sc_compressor_stereo";
//                "compressor_lr";
//                "compressor_mono";
//                "compressor_ms";
//                "compressor_stereo";
//                "sc_expander_lr";
//                "sc_expander_mono";
//                "sc_expander_ms";
//                "sc_expander_stereo";
//                "expander_lr";
//                "expander_mono";
//                "expander_ms";
//                "expander_stereo";
//                "sc_gate_lr";
//                "sc_gate_mono";
//                "sc_gate_ms";
//                "sc_gate_stereo";
//                "gate_lr";
//                "gate_mono";
//                "gate_ms";
//                "gate_stereo";
//                "sc_limiter_mono";
//                "sc_limiter_stereo";
//                "limiter_mono";
//                "limiter_stereo";
//                "sc_dyna_processor_lr";
//                "sc_dyna_processor_mono";
//                "sc_dyna_processor_ms";
//                "sc_dyna_processor_stereo";
//                "dyna_processor_lr";
//                "dyna_processor_mono";
//                "dyna_processor_ms";
//                "dyna_processor_stereo";
//                "graph_equalizer_x16_lr";
//                "graph_equalizer_x16_mono";
//                "graph_equalizer_x16_ms";
//                "graph_equalizer_x16_stereo";
//                "graph_equalizer_x32_lr";
//                "graph_equalizer_x32_mono";
//                "graph_equalizer_x32_ms";
//                "graph_equalizer_x32_stereo";
//                "para_equalizer_x16_lr";
//                "para_equalizer_x16_mono";
//                "para_equalizer_x16_ms";
//                "para_equalizer_x16_stereo";
//                "para_equalizer_x32_lr";
//                "para_equalizer_x32_mono";
//                "para_equalizer_x32_ms";
//                "para_equalizer_x32_stereo";
//                "multisampler_x12_do";
//                "multisampler_x12";
//                "multisampler_x24_do";
//                "multisampler_x24";
//                "multisampler_x48_do";
//                "multisampler_x48";
//                "sampler_mono";
//                "sampler_stereo";
//                "trigger_mono";
//                "trigger_stereo";
//                "trigger_midi_mono";
//                "trigger_midi_stereo";
//                "phase_detector";
//                "oscillator_mono";
//                "latency_meter";
//                "mb_compressor_mono";
//                "mb_compressor_stereo";
//                "mb_compressor_lr";
//                "mb_compressor_ms";
//                "sc_mb_compressor_mono";
//                "sc_mb_compressor_stereo";
//                "sc_mb_compressor_lr";
                "sc_mb_compressor_ms";
//                "test_plugin";

        return JACK_MAIN_FUNCTION(plugin, argc, argv);
    }
    
}
