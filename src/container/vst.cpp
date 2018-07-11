#include <sys/types.h>
#include <stddef.h>
#include <string.h>

#include <core/metadata.h>
#include <core/plugins.h>
#include <core/lib.h>

#include <data/cvector.h>

// VST SDK includes
#include <container/vst/defs.h>
#include <container/vst/helpers.h>

namespace lsp
{
    VstIntPtr VSTCALLBACK vst_dispatcher(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
    {
        return VstIntPtr(NULL);
    }

    void VSTCALLBACK vst_process(AEffect* effect, float** inputs, float** outputs, VstInt32 sampleFrames)
    {
    }

    void VSTCALLBACK vst_set_parameter(AEffect* effect, VstInt32 index, float parameter)
    {
    }

    float VSTCALLBACK vst_get_parameter(AEffect* effect, VstInt32 index)
    {
        return 0.0f;
    }

    AEffect *vst_instantiate(const char *uid, audioMasterCallback callback)
    {
        // Instantiate plugin
        const plugin_metadata_t *m = NULL;

        #define MOD_VST(plugin) \
            if ((!m) && (!strcmp(uid, plugin::metadata.vst_uid))) \
                m   = &plugin::metadata;
        #include <core/modules.h>

        // Return NULL if plugin was not found
        if (!m)
            return NULL;

        // Create effect and clean it up
        AEffect *e = new AEffect;
        if (e == NULL)
            return NULL;
        ::memset(e, 0, sizeof(AEffect));

        // Fill effect with values depending on metadata
        e->magic                        = kEffectMagic;
        e->dispatcher                   = vst_dispatcher;
        #ifndef VST_2_4_EXTENSIONS
            e->process                      = vst_process;
        #endif /* VST_2_4_EXTENSIONS */
        e->setParameter                 = vst_set_parameter;
        e->getParameter                 = vst_get_parameter;
        e->numPrograms                  = 0;
        e->numParams                    = 0;
        e->numInputs                    = 0;
        e->numOutputs                   = 0;
        e->flags                        = effFlagsCanReplacing;
        e->initialDelay                 = 0;
        e->object                       = NULL;
        e->user                         = NULL;
        e->uniqueID                     = vst_cconst(m->vst_uid);
        e->processReplacing             = vst_process;

        // Currently no double-replacing
        #ifdef VST_2_4_EXTENSIONS
            e->processDoubleReplacing       = NULL;
        #endif /* VST_2_4_EXTENSIONS */

        return e;
    }
}

extern "C"
{
    AEffect *VST_CREATE_INSTANCE_NAME(const char *uid, audioMasterCallback callback)
    {
        return lsp::vst_instantiate(uid, callback);
    }
}
