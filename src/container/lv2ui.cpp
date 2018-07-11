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
        IWidgetFactory *factory     = new LSP_WIDGET_FACTORY(bundle_path);
        plugin_ui *p                = new plugin_ui(plugin_name, *m, factory);

        // Scan for extensions
        LV2Extensions *ext              = new LV2Extensions(features, uri, controller, write_function);
        LV2UIAtomTransport *transport   = NULL;
        if (ext->atom_supported())
        {
            lsp_trace("Creating atom transport");
            transport           = new LV2UIAtomTransport(lv2_atom_ports, ext, p);
        }

        // Initialize plugin
        lsp_trace("Initializing plugin");
        p->init();

        // Perform all port bindings
        size_t port_id  = 0;

        for (const port_t *port = m->ports; (port->id != NULL) && (port->name != NULL); ++port)
        {
            bool out    = (port->flags & F_OUT);

            lsp_trace("bind port id=%s, idx=%d, role = %d, out=%s", port->id, int(port_id), int(port->role), (out) ? "true" : "false");

            switch (port->role)
            {
                case R_AUDIO:
                    p->add_port(new LV2UIPort(port, port_id, ext), true);
                    break;
                case R_METER:
                    p->add_port(new LV2UIPeakPort(port, port_id, ext), true);
                    break;
                case R_CONTROL:
                    p->add_port(new LV2UIFloatPort(port, port_id, ext), true);
                    break;
                case R_UI_SYNC:
                    continue;
                case R_MESH:
                    if (transport != NULL)
                        p->add_port(new LV2UIMeshPort(port, transport), false);
                    else
                        p->add_port(new LV2UIFloatPort(port, port_id, ext), false);
                    continue;
                default:
                    p->add_port(new LV2UIFloatPort(port, port_id, ext), true);
                    break;
            }
            out = !out;

            port_id++;
        }

        // Build plugin
        p->build();

        // Add Atom communication ports
        if (transport != NULL)
        {
            lsp_trace("binding LV2UITransport");
            transport->out()    -> set_id(port_id++);
            transport->in()     -> set_id(port_id++);

            p->add_port(transport->out(), true);
            p->add_port(transport->in(), true);

            transport   -> query_state();
            transport   -> unbind();
        }

        // Add stub for latency reporting
        {
            const port_t *port = &lv2_latency_port;
            if ((port->id != NULL) && (port->name != NULL))
                p->add_port(new LV2UIFloatPort(port, port_id, ext), true);
        }

        // Return UI
        lsp_trace("Return handle");
        ext         -> unbind();

        // Return widget
        *widget     =   reinterpret_cast<LV2UI_Widget>(factory->root_widget());

        return reinterpret_cast<LV2UI_Handle>(p);
    }

    void lv2ui_cleanup(LV2UI_Handle ui)
    {
        lsp_trace("cleanup");
        plugin_ui *p = reinterpret_cast<plugin_ui *>(ui);
        p->destroy();
        delete p;
    }

    void lv2ui_port_event(
        LV2UI_Handle ui,
        uint32_t     port_index,
        uint32_t     buffer_size,
        uint32_t     format,
        const void*  buffer)
    {
        plugin_ui *p = reinterpret_cast<plugin_ui *>(ui);
        IUIPort *port = p->port(port_index);
        if (port != NULL)
        {
            LV2UIPort *lv2port = static_cast<LV2UIPort *>(port);

//            lsp_trace("notify: idx=%d, id=%s", int(port_index), lv2port->metadata()->id);
            lv2port->notify(buffer, format, buffer_size);
        }
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

