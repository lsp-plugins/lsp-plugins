#include <sys/types.h>
#include <time.h>
#include <string.h>

#include <core/types.h>
#include <core/lib.h>
#include <core/protocol/midi.h>
#include <core/protocol/osc.h>
#include <core/resource.h>
#include <dsp/atomic.h>
#include <plugins/plugins.h>

#include <data/cvector.h>

#ifndef LSP_NO_LV2_UI
    #include <ui/ui.h>
#endif /* LSP_NO_LV2_UI */

#include <container/lv2/extensions.h>
#include <container/lv2/types.h>
#include <container/lv2/ports.h>
#include <container/lv2/executor.h>
#include <container/lv2/wrapper.h>

#ifndef LSP_NO_LV2_UI
    #include <container/lv2/ui_ports.h>
    #include <container/lv2/ui_wrapper.h>
#endif

#ifdef LSP_NO_LV2_UI
    /* Generate stub resource placeholders if there is no UI requirement */
    BUILTIN_RESOURCES_STUB
#endif /* LSP_NO_LV2_UI */

namespace lsp
{
    //--------------------------------------------------------------------------------------
    // LV2 routines

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
        lsp_debug_init("lv2");

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

        #define MOD_PLUGIN(plugin, ui) \
            if ((!p) && (plugin::metadata.lv2_uid != NULL) && (!strcmp(descriptor->URI, LSP_PLUGIN_URI(lv2, plugin)))) \
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
        dsp::context_t ctx;
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
//        lsp_trace("handle = %p", instance);

        dsp::context_t ctx;
        LV2_Inline_Display_Image_Surface *result;
        LV2Wrapper *wrapper = reinterpret_cast<LV2Wrapper *>(instance);

        dsp::start(&ctx);
//        lsp_trace("call wrapper for rendering w=%d, h=%d", int(w), int(h));
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
        if (!::strcmp(uri, LV2_STATE__interface))
        {
            lsp_trace("  state_interface = %p", &lv2_state_interface);
            return &lv2_state_interface;
        }
        else if (!::strcmp(uri, LV2_WORKER__interface))
        {
            lsp_trace("  worker_interface = %p", &lv2_worker_interface);
            return &lv2_worker_interface;
        }
#ifndef LSP_NO_LV2_UI
        else if (!::strcmp(uri, LV2_INLINEDISPLAY__interface))
        {
            lsp_trace("  inline_display_interface = %p", &lv2_inline_display_interface);
            return &lv2_inline_display_interface;
        }
#endif

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
        #define MOD_PLUGIN(plugin, ui)  if (plugin::metadata.lv2_uid != NULL) lv2_descriptors_count++;
        #include <metadata/modules.h>

        // Now allocate descriptors
        lv2_descriptors             = new LV2_Descriptor[lv2_descriptors_count];
        LV2_Descriptor *d           = lv2_descriptors;

        #define MOD_PLUGIN(plugin, ui)  \
            if (plugin::metadata.lv2_uid != NULL) \
            { \
                d->URI                  = LSP_PLUGIN_URI(lv2, plugin); \
                d->instantiate          = lv2_instantiate;      \
                d->connect_port         = lv2_connect_port;     \
                d->activate             = lv2_activate;         \
                d->run                  = lv2_run;              \
                d->deactivate           = lv2_deactivate;       \
                d->cleanup              = lv2_cleanup;          \
                d->extension_data       = lv2_extension_data;   \
                d++; \
            }

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

    //--------------------------------------------------------------------------------------
    // LV2UI routines
#ifndef LSP_NO_LV2_UI
    LV2UI_Handle lv2ui_instantiate(
        const struct _LV2UI_Descriptor* descriptor,
        const char*                     plugin_uri,
        const char*                     bundle_path,
        LV2UI_Write_Function            write_function,
        LV2UI_Controller                controller,
        LV2UI_Widget*                   widget,
        const LV2_Feature* const*       features)
    {
        // Find plugin metadata
        lsp_debug_init("lv2");
        lsp_trace("descriptor->uri = %s", descriptor->URI);

        // Initialize dsp (if possible)
        dsp::init();

        // Instantiate UI of plugin
        plugin_ui *p        = NULL;
        LV2Extensions *ext  = NULL;

        #define MOD_PLUGIN(plugin, plugin_ui) \
            lsp_trace("Check URI: %s", LSP_PLUGIN_UI_URI(lv2, plugin)); \
            if ((!p) && (!strcmp(descriptor->URI, LSP_PLUGIN_UI_URI(lv2, plugin)))) \
            { \
                if (plugin::metadata.ui_resource != NULL) \
                { \
                    ext = new LV2Extensions(features, LSP_PLUGIN_URI(lv2, plugin), controller, write_function); \
                    lsp_trace("Creating plugin UI, parent window=%p", ext->parent_window()); \
                    p   = new plugin_ui(&plugin::metadata, ext->parent_window()); \
                } \
            }
        #include <metadata/modules.h>

        if (p == NULL)
        {
            lsp_trace("Plugin UI metadata not found");
            return NULL;
        }

        // Create wrapper
        LV2UIWrapper *w                 = new LV2UIWrapper(p, ext);
        w->init();

        LSPWindow *root                 = p->root_window();
        *widget                         = reinterpret_cast<LV2UI_Widget>((root != NULL) ? root->handle() : NULL);
        lsp_trace("returned widget handle = %p", *widget);

        return reinterpret_cast<LV2UI_Handle>(w);
    }

    void lv2ui_cleanup(LV2UI_Handle ui)
    {
        lsp_trace("cleanup");
        LV2UIWrapper *w = reinterpret_cast<LV2UIWrapper *>(ui);
        w->destroy();
    }

    void lv2ui_port_event(
        LV2UI_Handle ui,
        uint32_t     port_index,
        uint32_t     buffer_size,
        uint32_t     format,
        const void*  buffer)
    {
        if ((buffer_size == 0) || (buffer == NULL))
            return;
        LV2UIWrapper *w = reinterpret_cast<LV2UIWrapper *>(ui);
        w->notify(port_index, buffer_size, format, buffer);
    }

    int lv2ui_idle(LV2UI_Handle ui)
    {
        LV2UIWrapper *w = reinterpret_cast<LV2UIWrapper *>(ui);
        return w->idle();
    }

    static LV2UI_Idle_Interface idle_iface =
    {
        lv2ui_idle
    };

    int lv2ui_resize(LV2UI_Feature_Handle ui, int width, int height)
    {
        LV2UIWrapper *w = reinterpret_cast<LV2UIWrapper *>(ui);
        return w->resize_ui(width, height);
    }

    static LV2UI_Resize resize_iface =
    {
        NULL,
        lv2ui_resize
    };

    const void* lv2ui_extension_data(const char* uri)
    {
        lsp_trace("requested extension data = %s", uri);
        if (!strcmp(uri, LV2_UI__idleInterface))
        {
            lsp_trace("  idle_interface = %p", &idle_iface);
            return &idle_iface;
        }
        else if (!strcmp(uri, LV2_UI__resize))
        {
            lsp_trace("  resize_interface = %p", &resize_iface);
            return &resize_iface;
        }
        return NULL;
    }

    LV2UI_Descriptor *lv2ui_descriptors     = NULL;
    size_t lv2ui_descriptors_count          = 0;

    void lv2ui_gen_descriptors()
    {
        if (lv2ui_descriptors != NULL)
            return;

        // Calculate number of plugins
        lv2ui_descriptors_count     = 0;

        #define MOD_PLUGIN(plugin, ui)  if ((plugin::metadata.lv2_uid != NULL) && (plugin::metadata.ui_resource != NULL)) lv2ui_descriptors_count++;
        #include <metadata/modules.h>

        lsp_trace("descriptors count=%d", (int)lv2ui_descriptors_count);

        // Now allocate descriptors
        lv2ui_descriptors           = new LV2UI_Descriptor[lv2ui_descriptors_count];
        LV2UI_Descriptor *d         = lv2ui_descriptors;

        #define MOD_PLUGIN(plugin, ui)  \
            if ((plugin::metadata.lv2_uid != NULL) && (plugin::metadata.ui_resource != NULL)) \
            { \
                d->URI                  = LSP_PLUGIN_UI_URI(lv2, plugin); \
                d->instantiate          = lv2ui_instantiate;    \
                d->cleanup              = lv2ui_cleanup;        \
                d->port_event           = lv2ui_port_event;     \
                d->extension_data       = lv2ui_extension_data; \
                lsp_trace("generated descriptor URI=%s", d->URI); \
                d++; \
            }
        #include <metadata/modules.h>
    };

    void lv2ui_drop_descriptors()
    {
        if (lv2ui_descriptors == NULL)
            return;

        delete [] lv2ui_descriptors;
        lv2ui_descriptors = NULL;
    };

    static StaticFinalizer lv2ui_finalizer(lv2ui_drop_descriptors);
#endif
}

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    LV2_SYMBOL_EXPORT
    const LV2_Descriptor *lv2_descriptor(uint32_t index)
    {
        using namespace lsp;
        lsp_debug_init("lv2");

        lv2_gen_descriptors();
        return (index < lv2_descriptors_count) ? &lv2_descriptors[index] : NULL;
    }

#ifndef LSP_NO_LV2_UI
    LV2_SYMBOL_EXPORT
    const LV2UI_Descriptor *lv2ui_descriptor(uint32_t index)
    {
        using namespace lsp;
        lsp_debug_init("lv2");

        lv2ui_gen_descriptors();
        return (index < lv2ui_descriptors_count) ? &lv2ui_descriptors[index] : NULL;
    }
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
