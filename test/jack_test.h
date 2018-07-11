#include <core/types.h>
#include <core/dsp.h>
#include <core/alloc.h>

#include <container/jack/defs.h>

namespace jack_test
{
    using namespace lsp;

    int test(int argc, const char **argv)
    {
        return JACK_MAIN_FUNCTION("trigger_mono", argc, argv);
//        return JACK_MAIN_FUNCTION("para_equalizer_x32_ms", argc, argv);
//        return JACK_MAIN_FUNCTION("phase_detector", argc, argv);
    }
    
}
