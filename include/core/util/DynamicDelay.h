/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 1 дек. 2020 г.
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

#ifndef CORE_UTIL_DYNAMICDELAY_H_
#define CORE_UTIL_DYNAMICDELAY_H_

#include <core/types.h>
#include <core/IStateDumper.h>

namespace lsp
{
    class DynamicDelay
    {
        private:
            DynamicDelay & operator = (const DynamicDelay &);

        protected:
            float      *vDelay;
            size_t      nHead;
            size_t      nCapacity;
            ssize_t     nMaxDelay;
            uint8_t    *pData;

        public:
            explicit DynamicDelay();
            ~DynamicDelay();

            void        construct();
            void        destroy();

        public:
            /**
             * Obtain the maximum possible value for the delay
             * @return the maximum possible value for the delay in samples
             */
            inline size_t max_delay() const     { return nMaxDelay;     }

            /**
             * Obtain the overall delay capacity
             * @return the overall delay capacity in samples
             */
            inline size_t capacity() const      { return nMaxDelay;     }

            /**
             * Initialize delay
             * @param max_size maximum delay size
             * @return status of operation
             */
            status_t    init(size_t max_size);

            /**
             * Process the signal using dynamic settings of delay and feedback
             * @param out output buffer
             * @param in input buffer
             * @param delay the delay values
             * @param fback feedback gain values
             * @param fdelay feedback delay values
             * @param samples number of samples to process
             */
            void        process(float *out, const float *in, const float *delay, const float *fback, const float *fdelay, size_t samples);

            /**
             * Clear delay state
             */
            void        clear();

            /**
             * Copy the contents of the dynamic delay
             * @param s delay to copy contents from
             */
            void        copy(DynamicDelay *s);

            /**
             * Swap contents with some another delay
             * @param d delay to swap contents
             */
            void        swap(DynamicDelay *d);

            /**
             * Dump internal state
             * @param v state dumper
             */
            void        dump(IStateDumper *v) const;
    };
}


#endif /* CORE_UTIL_DYNAMICDELAY_H_ */
