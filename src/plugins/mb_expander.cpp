/*
 * mb_expander.cpp
 *
 *  Created on: 27 дек. 2019 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <plugins/mb_expander.h>

namespace lsp
{
    mb_expander_base::mb_expander_base(const plugin_metadata_t &metadata, bool sc, size_t mode):
        plugin_t(metadata)
    {
        // TODO
    }

    mb_expander_base::~mb_expander_base()
    {
        // TODO
    }

    void mb_expander_base::init(IWrapper *wrapper)
    {
        plugin_t::init(wrapper);
    }

    void mb_expander_base::destroy()
    {
        plugin_t::destroy();
    }

    void mb_expander_base::update_settings()
    {
        // TODO
    }

    void mb_expander_base::update_sample_rate(long sr)
    {
        // TODO
    }

    void mb_expander_base::ui_activated()
    {
        // TODO
    }

    void mb_expander_base::process(size_t samples)
    {
        // TODO
    }

    bool mb_expander_base::inline_display(ICanvas *cv, size_t width, size_t height)
    {
        // TODO
        return false;
    }

    //-------------------------------------------------------------------------
    // Compressor derivatives
    mb_expander_mono::mb_expander_mono() : mb_expander_base(metadata, false, MBEM_MONO)
    {
    }

    mb_expander_stereo::mb_expander_stereo() : mb_expander_base(metadata, false, MBEM_STEREO)
    {
    }

    mb_expander_lr::mb_expander_lr() : mb_expander_base(metadata, false, MBEM_LR)
    {
    }

    mb_expander_ms::mb_expander_ms() : mb_expander_base(metadata, false, MBEM_MS)
    {
    }

    sc_mb_expander_mono::sc_mb_expander_mono() : mb_expander_base(metadata, true, MBEM_MONO)
    {
    }

    sc_mb_expander_stereo::sc_mb_expander_stereo() : mb_expander_base(metadata, true, MBEM_STEREO)
    {
    }

    sc_mb_expander_lr::sc_mb_expander_lr() : mb_expander_base(metadata, true, MBEM_LR)
    {
    }

    sc_mb_expander_ms::sc_mb_expander_ms() : mb_expander_base(metadata, true, MBEM_MS)
    {
    }
}

