#include <core/plugin.h>
#include <core/debug.h>

namespace lsp
{
    plugin::plugin(const plugin_metadata_t &mdata)
    {
        pMetadata       = &mdata;
        pDSP            = NULL;
        fSampleRate     = 0;
        nLatency        = 0;
    }

    plugin::~plugin()
    {
    }

    void plugin::init(int sample_rate)
    {
        fSampleRate     = sample_rate;
        pDSP            = dsp::createInstance();
    }

    void plugin::activate()
    {
    }

    void plugin::deactivate()
    {
    }

    void plugin::destroy()
    {
        for (size_t i=0; i < vIntPorts.size(); ++i)
            delete vIntPorts[i];

        vIntPorts.clear();
        vExtPorts.clear();

        if (pDSP != NULL)
        {
            delete pDSP;
            pDSP        = NULL;
        }
    }

    void plugin::run(size_t samples)
    {
        // Process external ports for changes
        for (size_t i=0; i<vExtPorts.size(); ++i)
        {
            if (vExtPorts[i] != NULL)
                vExtPorts[i]->pre_process();
        }

        // Check that input parameters changed
        for (size_t i=0; i<vIntPorts.size(); ++i)
        {
            IPort *port = vIntPorts[i];

            if ((port != NULL) && (port->changed()))
            {
                lsp_trace("Changed port id=%s", port->metadata()->id);

                update_settings();

                // Apply changes
                for (size_t j=0; j<vIntPorts.size(); ++j)
                    vIntPorts[j]->update();
                break;
            }
        }

        process(samples);

        // Process external ports for changes
        for (size_t i=0; i<vExtPorts.size(); ++i)
        {
            if (vExtPorts[i] != NULL)
                vExtPorts[i]->post_process();
        }
    }
    
    void plugin::update_settings()
    {
    }

    void plugin::process(size_t samples)
    {
    }

    bool plugin::add_port(IPort *port, bool external)
    {
        if (!vIntPorts.add(port))
            return false;

        lsp_trace("added internal port id=%s, index=%d", port->metadata()->id, int(vIntPorts.size() - 1));

        if (external)
        {
            if (!vExtPorts.add(port))
                return false;

            lsp_trace("added external port id=%s, index=%d", port->metadata()->id, int(vExtPorts.size() - 1));
        }

        return true;
    }

    IPort *plugin::port(size_t id, bool external)
    {
        return (external) ? vExtPorts[id] : vIntPorts[id];
    }
}

