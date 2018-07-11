#include <core/plugins.h>
#include <stdio.h>
#include <container/vst/defs.h>

#define BUFFER_SIZE 512
#define SAMPLE_RATE 48000

namespace vst_test
{
    using namespace lsp;

    extern "C"
    {
        extern AEffect *VST_CREATE_INSTANCE_NAME(VstInt32 uid, audioMasterCallback callback);
    }

    VstIntPtr callback(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
    {
        return 1;
    }

    int test(int argc, const char **argv)
    {
        using namespace lsp;

        const char *plugin_id       = "fwd3";

//        AEffect *eff    =
        VST_CREATE_INSTANCE_NAME(vst_cconst(plugin_id), callback);

        return 0;
    }
}
