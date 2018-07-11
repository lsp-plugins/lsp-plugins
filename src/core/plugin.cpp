#include <core/plugin.h>
#include <core/debug.h>

namespace lsp
{
    plugin_t::plugin_t(const plugin_metadata_t &mdata)
    {
        pMetadata       = &mdata;
//        pDSP            = NULL;
        fSampleRate     = -1;
        nLatency        = 0;
    }

    plugin_t::~plugin_t()
    {
    }

    void plugin_t::init()
    {
//        pDSP            = dsp::createInstance();
    }

    void plugin_t::set_sample_rate(int sr)
    {
        if (fSampleRate != sr)
        {
            fSampleRate = sr;
            update_sample_rate(sr);
        }
    };

    void plugin_t::update_sample_rate(int sr)
    {
    }

    void plugin_t::activate()
    {
    }

    void plugin_t::deactivate()
    {
    }

    void plugin_t::destroy()
    {
        for (size_t i=0; i < vIntPorts.size(); ++i)
            delete vIntPorts[i];

        vIntPorts.clear();
        vExtPorts.clear();
    }

    void plugin_t::run(size_t samples)
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

                // Call for plugin configuration
                update_settings();

                // Apply changes to all ports
                for (size_t j=0; j<vIntPorts.size(); ++j)
                    vIntPorts[j]->update();
                break;
            }
        }

        // Call the main processing unit
        process(samples);

        // Process external ports for changes
        for (size_t i=0; i<vExtPorts.size(); ++i)
        {
            if (vExtPorts[i] != NULL)
                vExtPorts[i]->post_process();
        }
    }
    
    void plugin_t::update_settings()
    {
    }

    void plugin_t::process(size_t samples)
    {
    }

    bool plugin_t::add_port(IPort *port, bool external)
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

    IPort *plugin_t::port(size_t id, bool external)
    {
        return (external) ? vExtPorts[id] : vIntPorts[id];
    }
}

