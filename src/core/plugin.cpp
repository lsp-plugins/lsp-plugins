#include <core/plugin.h>
#include <core/debug.h>

namespace lsp
{
    plugin_t::plugin_t(const plugin_metadata_t &mdata)
    {
        pMetadata       = &mdata;
        fSampleRate     = -1;
        nLatency        = 0;
    }

    plugin_t::~plugin_t()
    {
    }

    void plugin_t::init()
    {
    }

    void plugin_t::set_sample_rate(long sr)
    {
        if (fSampleRate != sr)
        {
            fSampleRate = sr;
            update_sample_rate(sr);
        }
    };

    void plugin_t::update_sample_rate(long sr)
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
        for (size_t i=0; i < vPorts.size(); ++i)
            delete vPorts[i];

        vPorts.clear();
    }

    void plugin_t::run(size_t samples)
    {
        bool update     = false;

        // Process external ports for changes
        for (size_t i=0; i<vPorts.size(); ++i)
        {
            // Get port
            IPort *port = vPorts[i];
            if (port == NULL)
                continue;

            // Pre-process data in port
            if (port->pre_process())
            {
                lsp_trace("port changed: %s", port->metadata()->id);
                update = true;
            }
        }

        // Check that input parameters have changed
        if (update)
        {
            lsp_trace("updating settings");
            update_settings();
        }

        // Call the main processing unit
        process(samples);

        // Process external ports for changes
        for (size_t i=0; i<vPorts.size(); ++i)
        {
            if (vPorts[i] != NULL)
                vPorts[i]->post_process();
        }
    }
    
    void plugin_t::update_settings()
    {
    }

    void plugin_t::process(size_t samples)
    {
    }

}

