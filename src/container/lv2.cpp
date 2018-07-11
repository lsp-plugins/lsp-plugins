#include <sys/types.h>
#include <stddef.h>
#include <string.h>

#include <core/metadata.h>
#include <core/plugins.h>
#include <core/lib.h>

#include <data/cvector.h>

#include <container/lv2ext.h>
#include <container/lv2/ports.h>
#include <container/lv2/transport.h>
#include <container/lv2/vports.h>
#include <container/lv2/wrapper.h>

namespace lsp
{
    void lv2_activate(LV2_Handle instance)
    {
        lsp_trace("instance = %p", instance);
        LV2Wrapper *w = reinterpret_cast<LV2Wrapper *>(instance);
        w->activate();
//        plugin_t *p = reinterpret_cast<plugin_t *>(instance);
//        p->activate();
    }

    void lv2_cleanup(LV2_Handle instance)
    {
        lsp_trace("instance = %p", instance);
        LV2Wrapper *w = reinterpret_cast<LV2Wrapper *>(instance);
        w->destroy();
        delete w;
//        plugin_t *p = reinterpret_cast<plugin_t *>(instance);
//        lsp_trace("cleanup plugin = %p (%s)", p, p->get_metadata()->name);
//
//        p->destroy();
//        delete p;
    }

    void lv2_connect_port(
        LV2_Handle instance,
        uint32_t   port,
        void      *data_location )
    {
//        lsp_trace("port = %d, data_location=%p", int(port), data_location);
        LV2Wrapper *w = reinterpret_cast<LV2Wrapper *>(instance);
        w->connect(port, data_location);

//        plugin_t  *p      = reinterpret_cast<plugin_t *>(instance);
//        IPort   *dst    = p->port(port);
//        if (dst != NULL)
//        {
//            LV2Port *p_dst  = static_cast<LV2Port *>(dst);
//            p_dst->bind(data_location);
//        }
    }

    void lv2_deactivate(LV2_Handle instance)
    {
        lsp_trace("instance = %p", instance);
        LV2Wrapper *w = reinterpret_cast<LV2Wrapper *>(instance);
        w->deactivate();

//        plugin_t *p = reinterpret_cast<plugin_t *>(instance);
//        p->deactivate();
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
//        const plugin_metadata_t *m = NULL;
        const char *uri = NULL;

        #define MOD_LV2(plugin) \
            if ((!p) && (!strcmp(descriptor->URI, LSP_PLUGIN_URI(lv2, plugin)))) \
            { \
                p   = new plugin(); \
                if (p == NULL) \
                    return NULL; \
                /* m   = &plugin::metadata; \ */ \
                uri = LSP_PLUGIN_URI(lv2, plugin); \
            }
        #include <core/modules.h>

        // Check that plugin instance is available
        if (p == NULL)
            return NULL;

        lsp_trace("lv2_instantiate uri=%s, sample_rate=%f", uri, sample_rate);

//        // Scan for extensions
        LV2Extensions *ext          = new LV2Extensions(features, uri);
        LV2Wrapper *w               = new LV2Wrapper(p, ext);

        w->init(sample_rate);

        return reinterpret_cast<LV2_Handle>(w);

//        LV2AtomTransport *transport = NULL;
//        if (ext->atom_supported())
//        {
//            lsp_trace("Creating atom transport");
//            transport       = new LV2AtomTransport(lv2_atom_ports, ext, p);
//        }
//
//        // Bind ports
//        lsp_trace("Binding ports");
//        for (const port_t *port = m->ports; (port->id != NULL) && (port->name != NULL); ++port)
//        {
//            bool out = (port->flags & F_OUT);
//
//            switch (port->role)
//            {
//                case R_UI_SYNC:
//                    p->add_port(new LV2Port(port, ext), false);
//                    continue;
//                case R_MESH:
//                    if (transport != NULL)
//                        p->add_port(new LV2MeshPort(port, transport), false);
//                    else
//                        p->add_port(new LV2Port(port, ext), false);
//                    continue;
//
//                case R_AUDIO:
//                    p->add_port(new LV2AudioPort(port, ext), true);
//                    break;
//                case R_CONTROL:
//                case R_METER:
//                default:
//                    if (out)
//                        p->add_port(new LV2OutputPort(port, ext), true);
//                    else
//                        p->add_port(new LV2InputPort(port, ext), true);
//                    break;
//            }
//        }
//
//        // Add state ports (if supported)
//        if (transport != NULL)
//        {
//            lsp_trace("binding LV2Transport");
//            p->add_port(transport->in(), true);
//            p->add_port(transport->out(), true);
//
//            transport   -> unbind();
//        }
//
//        // Add latency port
//        {
//            const port_t *port = &lv2_latency_port;
//            if ((port->id != NULL) && (port->name != NULL))
//            {
//                lsp_trace("binding Latency Port");
//                p->add_port(new LV2LatencyPort(port, ext, p), true);
//            }
//        }
//
//        // Dereference extensions
//        ext         ->  unbind();
//
//        // Initialize plugin
//        lsp_trace("Initializing plugin");
//        p->init();
//        p->set_sample_rate(sample_rate);

//        return reinterpret_cast<LV2_Handle>(p);
    }

    void lv2_run(LV2_Handle instance, uint32_t sample_count)
    {
        LV2Wrapper *w = reinterpret_cast<LV2Wrapper *>(instance);
        w->run(sample_count);
//        plugin_t *p = reinterpret_cast<plugin_t *>(instance);
//        p->run(sample_count);
    }

    const void *lv2_extension_data(const char * uri)
    {
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
        #include <core/modules.h>

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

        #include <core/modules.h>
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
