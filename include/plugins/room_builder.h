/*
 * room_builder.h
 *
 *  Created on: 25 апр. 2019 г.
 *      Author: sadko
 */

#ifndef PLUGINS_ROOM_BUILDER_H_
#define PLUGINS_ROOM_BUILDER_H_

#include <core/plugin.h>

#include <metadata/plugins.h>

namespace lsp
{

    class room_builder_base: public plugin_t
    {
        protected:
            size_t                  nInputs;

        public:
            room_builder_base(const plugin_metadata_t &metadata, size_t inputs);
            virtual ~room_builder_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);

            virtual void process(size_t samples);
    };

    class room_builder_mono: public room_builder_base, public room_builder_mono_metadata
    {
        public:
            room_builder_mono();
            virtual ~room_builder_mono();
    };

    class room_builder_stereo: public room_builder_base, public room_builder_stereo_metadata
    {
        public:
            room_builder_stereo();
            virtual ~room_builder_stereo();
    };

}


#endif /* PLUGINS_ROOM_BUILDER_H_ */
