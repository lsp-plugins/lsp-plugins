#include <core/plugin.h>
#include <core/debug.h>

namespace lsp
{
    plugin_t::plugin_t(const plugin_metadata_t &mdata)
    {
        pMetadata       = &mdata;
        pWrapper        = NULL;
        fSampleRate     = -1;
        nLatency        = 0;
        bActivated      = false;
        bUIActive       = true;
    }

    plugin_t::~plugin_t()
    {
    }

    void plugin_t::init(IWrapper *wrapper)
    {
        pWrapper        = wrapper;
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

    void plugin_t::activated()
    {
    }

    void plugin_t::deactivated()
    {
    }

    void plugin_t::ui_activated()
    {
    }

    void plugin_t::ui_deactivated()
    {
    }

    void plugin_t::destroy()
    {
        vPorts.clear();
        bActivated      = false;
    }
    
    void plugin_t::update_settings()
    {
    }

    void plugin_t::process(size_t samples)
    {
    }

    bool plugin_t::inline_display(ICanvas *cv, size_t width, size_t height)
    {
        return false;
    }

}

