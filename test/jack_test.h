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
//                "graph_equalizer_x16_mono";
//                "graph_equalizer_x16_stereo";
//                "graph_equalizer_x16_lr";
//                "graph_equalizer_x16_ms";
//                "graph_equalizer_x32_mono";
//                "graph_equalizer_x32_stereo";
//                "graph_equalizer_x32_lr";
//                "graph_equalizer_x32_ms";
//                "para_equalizer_x16_stereo";
                "para_equalizer_x16_mono";
//                "trigger_mono";
//                "trigger_stereo";
//                "phase_detector";
//                "spectrum_analyzer_x2";
//                "sampler_mono";
//                "comp_delay_x2_stereo";

        return JACK_MAIN_FUNCTION(plugin, argc, argv);
    }
    
}
