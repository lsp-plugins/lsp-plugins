/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 2 сент. 2016 г.
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

#ifndef CORE_FILTERS_FILTERBANK_H_
#define CORE_FILTERS_FILTERBANK_H_

#include <core/IStateDumper.h>

namespace lsp
{
    class FilterBank
    {
        private:
            FilterBank & operator = (const FilterBank &);

        protected:
            biquad_t           *vFilters;   // Optimized list of filters
            biquad_x1_t        *vChains;    // List of biquad banks
            size_t              nItems;     // Current number of biquad_x1 filters
            size_t              nMaxItems;  // Maximum number of biquad_x1 filters
            size_t              nLastItems; // Previous number of biquad_x1 filters
            float              *vBackup;    // Delay backup to take online impulse response
            uint8_t            *vData;      // Unaligned data

        protected:
            void                clear_delays();

        public:
            explicit FilterBank();
            ~FilterBank();

            /**
             * Construct the filter bank being a chunk of memory
             */
            void                construct();

            /** Initialize filter bank
             *
             * @param filters number of biquad filters
             * @return true on success
             */
            bool                init(size_t filters);

            /** Destroy filter bank
             *
             */
            void                destroy();

        public:
            /** Start filter bank, clears number of cascades
             *
             */
            inline void         begin()
            {
                nLastItems      = nItems;
                nItems          = 0;
            }

            /** Add cascade to biquad filter
             *
             * @return added cascade
             */
            biquad_x1_t        *add_chain();

            /** Optimize structure of filter bank
             * @param clear force to clear delays
             */
            void                end(bool clear = false);

            /** Process samples
             *
             * @param out output buffer
             * @param in input buffer
             * @param samples number of samples to process
             */
            void                process(float *out, const float *in, size_t samples);

            /** Get impulse response of the bank
             *
             * @param out output buffer to store impulse response
             * @param samples length of buffer in samples
             */
            void                impulse_response(float *out, size_t samples);

            /** Get number of biquad filters
             *
             * @return number of biquad filters
             */
            inline size_t       size() const { return nItems; }

            /** Reset internal state of filters (clear filter memory)
             *
             */
            void                reset();

            /**
             * Dump the state
             * @param dumper dumper
             */
            void                dump(IStateDumper *v) const;
    };
} /* namespace lsp */

#endif /* CORE_FILTERS_FILTERBANK_H_ */
