/*
 * mb_expander.h
 *
 *  Created on: 27 дек. 2019 г.
 *      Author: sadko
 */

#ifndef PLUGINS_MB_EXPANDER_H_
#define PLUGINS_MB_EXPANDER_H_

#include <metadata/plugins.h>

#include <core/plugin.h>

namespace lsp
{
    class mb_expander_base: public plugin_t
    {
        protected:
            enum c_mode_t
            {
                MBEM_MONO,
                MBEM_STEREO,
                MBEM_LR,
                MBEM_MS
            };

        public:
            explicit mb_expander_base(const plugin_metadata_t &metadata, bool sc, size_t mode);
            virtual ~mb_expander_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);
            virtual void ui_activated();

            virtual void process(size_t samples);
            virtual bool inline_display(ICanvas *cv, size_t width, size_t height);
    };

    //-------------------------------------------------------------------------
    // Different expander implementations
    class mb_expander_mono: public mb_expander_base, public mb_expander_mono_metadata
    {
        public:
            mb_expander_mono();
    };

    class mb_expander_stereo: public mb_expander_base, public mb_expander_stereo_metadata
    {
        public:
            mb_expander_stereo();
    };

    class mb_expander_lr: public mb_expander_base, public mb_expander_lr_metadata
    {
        public:
            mb_expander_lr();
    };

    class mb_expander_ms: public mb_expander_base, public mb_expander_ms_metadata
    {
        public:
            mb_expander_ms();
    };

    class sc_mb_expander_mono: public mb_expander_base, public sc_mb_expander_mono_metadata
    {
        public:
            sc_mb_expander_mono();
    };

    class sc_mb_expander_stereo: public mb_expander_base, public sc_mb_expander_stereo_metadata
    {
        public:
            sc_mb_expander_stereo();
    };

    class sc_mb_expander_lr: public mb_expander_base, public sc_mb_expander_lr_metadata
    {
        public:
            sc_mb_expander_lr();
    };

    class sc_mb_expander_ms: public mb_expander_base, public sc_mb_expander_ms_metadata
    {
        public:
            sc_mb_expander_ms();
    };
}

#endif /* PLUGINS_MB_EXPANDER_H_ */
