#include <sys/types.h>
#include <stddef.h>
#include <string.h>

#include <core/lib.h>
#include <core/midi.h>
#include <plugins/plugins.h>

#include <data/cvector.h>

#include <container/lv2ext.h>
#include <container/lv2/types.h>
#include <container/lv2/ports.h>
#include <container/lv2/executor.h>
#include <container/lv2/wrapper.h>

namespace lsp
{
    void lv2_activate(LV2_Handle instance)
    {
        lsp_trace("instance = %p", instance);
        LV2Wrapper *w = reinterpret_cast<LV2Wrapper *>(instance);
        w->activate();
    }

    void lv2_cleanup(LV2_Handle instance)
    {
        lsp_trace("instance = %p", instance);
        LV2Wrapper *w = reinterpret_cast<LV2Wrapper *>(instance);
        w->destroy();
        delete w;
    }

    void lv2_connect_port(
        LV2_Handle instance,
        uint32_t   port,
        void      *data_location )
    {
//        lsp_trace("port = %d, data_location=%p", int(port), data_location);
        LV2Wrapper *w = reinterpret_cast<LV2Wrapper *>(instance);
        w->connect(port, data_location);
    }

    void lv2_deactivate(LV2_Handle instance)
    {
        lsp_trace("instance = %p", instance);
        LV2Wrapper *w = reinterpret_cast<LV2Wrapper *>(instance);
        w->deactivate();
    }

    LV2_Handle lv2_instantiate(
        const struct _LV2_Descriptor * descriptor,
        double                         sample_rate,
        const char *                   bundle_path,
        const LV2_Feature *const *     features)
    {
        // Check sample rate
        if (sample_rate > MAX_SAMPLE_RATE)
        {
            lsp_error("Unsupported sample rate: %f, maximum supported sample rate is %ld", float(sample_rate), long(MAX_SAMPLE_RATE));
            return NULL;
        }

        // Initialize DSP
        dsp::init();

        // Instantiate plugin
        plugin_t *p = NULL;
        const char *uri = NULL;

        #define MOD_LV2(plugin) \
            if ((!p) && (!strcmp(descriptor->URI, LSP_PLUGIN_URI(lv2, plugin)))) \
            { \
                p   = new plugin(); \
                if (p == NULL) \
                    return NULL; \
                uri = LSP_PLUGIN_URI(lv2, plugin); \
            }
        #include <metadata/modules.h>

        // Check that plugin instance is available
        if (p == NULL)
            return NULL;

        lsp_trace("lv2_instantiate uri=%s, sample_rate=%f", uri, sample_rate);

        // Scan for extensions
        LV2Extensions *ext          = new LV2Extensions(features, uri, NULL, NULL);
        LV2Wrapper *w               = new LV2Wrapper(p, ext);

        w->init(sample_rate);

        return reinterpret_cast<LV2_Handle>(w);
    }

    void lv2_run(LV2_Handle instance, uint32_t sample_count)
    {
        dsp_context_t ctx;
        LV2Wrapper *w = reinterpret_cast<LV2Wrapper *>(instance);

        // Call the plugin for processing
        dsp::start(&ctx);
        w->run(sample_count);
        dsp::finish(&ctx);
    }

    LV2_State_Status lv2_save_state(
        LV2_Handle                 instance,
        LV2_State_Store_Function   store,
        LV2_State_Handle           handle,
        uint32_t                   flags,
        const LV2_Feature *const * features)
    {
        lsp_trace("handle = %p", instance);
        LV2Wrapper *w = reinterpret_cast<LV2Wrapper *>(instance);
        w->save_state(store, handle, flags, features);

        return LV2_STATE_SUCCESS;
    }

    LV2_State_Status lv2_restore_state(
        LV2_Handle                  instance,
        LV2_State_Retrieve_Function retrieve,
        LV2_State_Handle            handle,
        uint32_t                    flags,
        const LV2_Feature *const *  features)
    {
        lsp_trace("handle = %p", instance);
        LV2Wrapper *w = reinterpret_cast<LV2Wrapper *>(instance);
        w->restore_state(retrieve, handle, flags, features);

        return LV2_STATE_SUCCESS;
    }

    LV2_Inline_Display_Image_Surface *lv2_render_inline_display(
                                       LV2_Handle instance,
                                       uint32_t w, uint32_t h)
    {
        lsp_trace("handle = %p", instance);

        dsp_context_t ctx;
        LV2_Inline_Display_Image_Surface *result;
        LV2Wrapper *wrapper = reinterpret_cast<LV2Wrapper *>(instance);

        dsp::start(&ctx);
        lsp_trace("call wrapper for rendering w=%d, h=%d", int(w), int(h));
        result              = wrapper->render_inline_display(w, h);
        dsp::finish(&ctx);

        return result;
    }

    static const LV2_State_Interface lv2_state_interface =
    {
        lv2_save_state,
        lv2_restore_state
    };

    static const LV2_Inline_Display_Interface lv2_inline_display_interface =
    {
        lv2_render_inline_display
    };

    LV2_Worker_Status lv2_work_work(
        LV2_Handle                  instance,
        LV2_Worker_Respond_Function respond,
        LV2_Worker_Respond_Handle   handle,
        uint32_t                    size,
        const void*                 data)
    {
//        lsp_trace("handle = %p", instance);
        LV2Wrapper *w = reinterpret_cast<LV2Wrapper *>(instance);
        w->job_run(handle, respond, size, data);
        return LV2_WORKER_SUCCESS;
    }

    LV2_Worker_Status lv2_work_response(
        LV2_Handle  instance,
        uint32_t    size,
        const void* body)
    {
//        lsp_trace("handle = %p", instance);
        LV2Wrapper *w = reinterpret_cast<LV2Wrapper *>(instance);
        w->job_response(size, body);
        return LV2_WORKER_SUCCESS;
    }

    LV2_Worker_Status lv2_work_end(LV2_Handle instance)
    {
//        lsp_trace("handle = %p", instance);
        LV2Wrapper *w = reinterpret_cast<LV2Wrapper *>(instance);
        w->job_end();
        return LV2_WORKER_SUCCESS;
    }

    static const LV2_Worker_Interface lv2_worker_interface =
    {
        lv2_work_work,
        lv2_work_response,
        lv2_work_end
    };

    const void *lv2_extension_data(const char * uri)
    {
        lsp_trace("requested extension data = %s", uri);
        if (!strcmp(uri, LV2_STATE__interface))
        {
            lsp_trace("  state_interface = %p", &lv2_state_interface);
            return &lv2_state_interface;
        }
        else if (!strcmp(uri, LV2_WORKER__interface))
        {
            lsp_trace("  worker_interface = %p", &lv2_worker_interface);
            return &lv2_worker_interface;
        }
        else if (!strcmp(uri, LV2_INLINEDISPLAY__interface))
        {
            lsp_trace("  inline_display_interface = %p", &lv2_inline_display_interface);
            return &lv2_inline_display_interface;
        }

        return NULL;
    }

    LV2_Descriptor *lv2_descriptors = NULL;
    size_t lv2_descriptors_count    = 0;

    void lv2_gen_descriptors()
    {
        if (lv2_descriptors != NULL)
            return;

        // Calculate number of plugins
        lv2_descriptors_count       = 0;
        #define MOD_LV2(plugin)     lv2_descriptors_count++;
        #include <metadata/modules.h>

        // Now allocate descriptors
        lv2_descriptors             = new LV2_Descriptor[lv2_descriptors_count];
        LV2_Descriptor *d           = lv2_descriptors;

        #define MOD_LV2(plugin)  \
            d->URI                  = LSP_PLUGIN_URI(lv2, plugin); \
            d->instantiate          = lv2_instantiate;      \
            d->connect_port         = lv2_connect_port;     \
            d->activate             = lv2_activate;         \
            d->run                  = lv2_run;              \
            d->deactivate           = lv2_deactivate;       \
            d->cleanup              = lv2_cleanup;          \
            d->extension_data       = lv2_extension_data;   \
            d++;

        #include <metadata/modules.h>
    };

    void lv2_drop_descriptors()
    {
        if (lv2_descriptors == NULL)
            return;

        delete [] lv2_descriptors;
        lv2_descriptors = NULL;
    };

    static StaticFinalizer lv2_finalizer(lv2_drop_descriptors);
}


LV2_SYMBOL_EXPORT
const LV2_Descriptor *lv2_descriptor(uint32_t index)
{
    using namespace lsp;

    lv2_gen_descriptors();
    return (index < lv2_descriptors_count) ? &lv2_descriptors[index] : NULL;
}
