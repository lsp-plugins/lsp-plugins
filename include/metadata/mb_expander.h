/*
 * mb_expander.h
 *
 *  Created on: 27 дек. 2019 г.
 *      Author: sadko
 */

#ifndef METADATA_MB_EXPANDER_H_
#define METADATA_MB_EXPANDER_H_

namespace lsp
{
    //-------------------------------------------------------------------------
    // Multiband expander
    struct mb_expander_base_metadata
    {
    };

    struct mb_expander_mono_metadata: public mb_expander_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct mb_expander_stereo_metadata: public mb_expander_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct mb_expander_lr_metadata: public mb_expander_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct mb_expander_ms_metadata: public mb_expander_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct sc_mb_expander_mono_metadata: public mb_expander_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct sc_mb_expander_stereo_metadata: public mb_expander_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct sc_mb_expander_lr_metadata: public mb_expander_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct sc_mb_expander_ms_metadata: public mb_expander_base_metadata
    {
        static const plugin_metadata_t metadata;
    };
}

#endif /* METADATA_MB_EXPANDER_H_ */
