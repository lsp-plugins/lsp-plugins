/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 11 июл. 2020 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CORE_FILES_HYDROGEN_H_
#define CORE_FILES_HYDROGEN_H_

#include <core/types.h>
#include <core/status.h>
#include <core/io/Path.h>
#include <core/io/File.h>
#include <core/io/IInStream.h>
#include <core/io/IInSequence.h>
#include <data/cvector.h>

namespace lsp
{
    namespace hydrogen
    {
        typedef struct layer_t
        {
            private:
                layer_t & operator = (const layer_t &);

            public:
                LSPString   file_name;
                float       min;
                float       max;
                float       gain;
                float       pitch;

            public:
                explicit layer_t();
                ~layer_t();
        } layer_t;

        typedef struct instrument_t
        {
            private:
                instrument_t & operator = (const instrument_t &);

            public:
                ssize_t     id;
                LSPString   file_name;
                LSPString   name;
                float       volume;
                bool        muted;
                bool        locked;
                float       pan_left;
                float       pan_right;
                float       random_pitch_factor;
                float       gain;
                bool        filter_active;
                float       filter_cutoff;
                float       filter_resonance;
                float       attack;
                float       decay;
                float       sustain;
                float       release;
                ssize_t     mute_group;
                bool        stop_note;
                ssize_t     midi_out_channel;
                ssize_t     midi_out_note;
                ssize_t     midi_in_channel;
                ssize_t     midi_in_note;
                float       fx1_level;
                float       fx2_level;
                float       fx3_level;
                float       fx4_level;

                cvector<layer_t>   layers;

            public:
                explicit instrument_t();
                ~instrument_t();
        } instrument_t;

        typedef struct drumkit_t
        {
            private:
                drumkit_t & operator = (const drumkit_t &);

            public:
                LSPString           name;
                LSPString           author;
                LSPString           info;
                LSPString           license;
                cvector<instrument_t>   instruments;

            public:
                explicit drumkit_t();
                ~drumkit_t();

                void    swap(drumkit_t *dst);
        } drumkit_t;


        /**
         * Load drumkit file
         * @param path UTF-8 encoded location of the file
         * @param dst pointer to store drumkit data
         * @return status of operation
         */
        status_t load(const char *path, drumkit_t *dst);

        /**
         * Load drumkit file
         * @param path location of the file
         * @param dst pointer to store drumkit data
         * @return status of operation
         */
        status_t load(const LSPString *path, drumkit_t *dst);

        /**
         * Load drumkit file
         * @param path location of the file
         * @param dst pointer to store drumkit data
         * @return status of operation
         */
        status_t load(const io::Path *path, drumkit_t *dst);

        /**
         * Load drumkit from character input stream
         * @param input stream pointer
         * @param dst pointer to store drumkit data
         * @return status of operation
         */
        status_t load(io::IInStream *is, drumkit_t *dst);

        /**
         * Load drumkit from character input stream
         * @param input sequence pointer
         * @param dst pointer to store drumkit data
         * @return status of operation
         */
        status_t load(io::IInSequence *is, drumkit_t *dst);
    }
}

#endif /* CORE_FILES_HYDROGEN_H_ */
