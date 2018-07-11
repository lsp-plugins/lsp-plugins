#include <sys/types.h>
#include <stddef.h>
#include <string.h>

#include <core/metadata.h>
#include <core/plugins.h>
#include <core/lib.h>

#include <data/cvector.h>

// UI includes
#include <ui/ui.h>
#include <container/lv2ext.h>
#include <container/lv2ui/ports.h>
#include <container/lv2ui/transport.h>
#include <container/lv2ui/vports.h>
#include <container/lv2ui/wrapper.h>

#if defined(LSP_UI_GTK2)
    #include <ui/gtk2/ui.h>
    #define LSP_PACKAGE gtk2
    #define LSP_WIDGET_FACTORY Gtk2WidgetFactory
#elif defined(LSP_UI_GTK3)
    #include <ui/gtk3/ui.h>
    #define LSP_PACKAGE gtk3
    #define LSP_WIDGET_FACTORY Gtk3WidgetFactory
#endif /* LSP_UI_GTK3 */

namespace lsp
{
    //--------------------------------------------------------------------------------------
    // LV2UI routines

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
        const plugin_metadata_t *m = NULL;
        const char *plugin_name = NULL;
        const char *uri = NULL;

        lsp_trace("descriptor->uri = %s", descriptor->URI);

        #define MOD_LV2UI(plugin) \
            if ((!m) && (!strcmp(descriptor->URI, LSP_PLUGIN_UI_URI(lv2, plugin, LSP_PACKAGE)))) \
            { \
                plugin_name = #plugin; \
                m = & plugin ## _metadata::metadata; \
                uri = LSP_PLUGIN_URI(lv2, plugin); \
            }
        #if defined(LSP_UI_GTK2)
            #define MOD_GTK2(plugin)    MOD_LV2UI(plugin)
        #elif defined(LSP_UI_GTK3)
            #define MOD_GTK3(plugin)    MOD_LV2UI(plugin)
        #endif /* LSP_UI_GTK3 */

        #include <core/modules.h>
        #undef MOD_LV2UI

        if (!m)
            return NULL;

        // Create widget factory and UI
        lsp_trace("Creating plugin UI");
        IWidgetFactory *factory         = new LSP_WIDGET_FACTORY(bundle_path);
        plugin_ui *p                    = new plugin_ui(plugin_name, m, factory);

        // Scan for extensions
        LV2Extensions *ext              = new LV2Extensions(features, uri, controller, write_function);
        LV2UIWrapper *w                 = new LV2UIWrapper(p, ext);
        w->init();

        *widget                         = reinterpret_cast<LV2UI_Widget>(factory->root_widget());

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
        LV2UIWrapper *w = reinterpret_cast<LV2UIWrapper *>(ui);
        w->notify(port_index, buffer_size, format, buffer);
    }

    const void* lv2ui_extension_data(const char* uri)
    {
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

        #define MOD_LV2UI(plugin)   lv2ui_descriptors_count++;
        #if defined(LSP_UI_GTK2)
            #define MOD_GTK2(plugin)    MOD_LV2UI(plugin)
        #elif defined(LSP_UI_GTK3)
            #define MOD_GTK3(plugin)    MOD_LV2UI(plugin)
        #endif /* LSP_UI_GTK3 */

        #include <core/modules.h>
        #undef MOD_LV2UI

        lsp_trace("descriptors count=%d", (int)lv2ui_descriptors_count);

        // Now allocate descriptors
        lv2ui_descriptors           = new LV2UI_Descriptor[lv2ui_descriptors_count];
        LV2UI_Descriptor *d         = lv2ui_descriptors;

        #define MOD_LV2UI(plugin)  \
            d->URI                  = LSP_PLUGIN_UI_URI(lv2, plugin, LSP_PACKAGE); \
            d->instantiate          = lv2ui_instantiate;    \
            d->cleanup              = lv2ui_cleanup;        \
            d->port_event           = lv2ui_port_event;     \
            d->extension_data       = lv2ui_extension_data; \
            lsp_trace("generated descriptor URI=%s", d->URI); \
            d++;

        #if defined(LSP_UI_GTK2)
            #define MOD_GTK2(plugin)    MOD_LV2UI(plugin)
        #elif defined(LSP_UI_GTK3)
            #define MOD_GTK3(plugin)    MOD_LV2UI(plugin)
        #endif /* LSP_UI_GTK3 */

        #include <core/modules.h>
        #undef MOD_LV2UI
    };

    void lv2ui_drop_descriptors()
    {
        if (lv2ui_descriptors == NULL)
            return;

        delete [] lv2ui_descriptors;
        lv2ui_descriptors = NULL;
    };

    static StaticFinalizer lv2ui_finalizer(lv2ui_drop_descriptors);
}

LV2_SYMBOL_EXPORT
const LV2UI_Descriptor *lv2ui_descriptor(uint32_t index)
{
    using namespace lsp;

    lv2ui_gen_descriptors();
    return (index < lv2ui_descriptors_count) ? &lv2ui_descriptors[index] : NULL;
}

