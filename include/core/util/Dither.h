/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 21 дек. 2016 г.
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

#ifndef CORE_UTIL_DITHER_H_
#define CORE_UTIL_DITHER_H_

#include <core/types.h>
#include <core/IStateDumper.h>
#include <core/util/Randomizer.h>

namespace lsp
{
    class Dither
    {
        private:
            Dither &operator = (const Dither &);

        protected:
            size_t      nBits;
            float       fGain;
            float       fDelta;
            Randomizer  sRandom;

        public:
            explicit Dither();
            ~Dither();

        public:
            /** Initialize dither
             *
             */
            inline void init() { sRandom.init(); };

            /** Set number of bits per sample
             *
             * @param bits number of bits per sample
             */
            void set_bits(size_t bits);

            /** Process signal
             *
             * @param out output signal
             * @param in input signal
             * @param count number of samples to process
             */
            void process(float *out, const float *in, size_t count);

            /**
             * Dump the state
             * @param dumper dumper
             */
            void dump(IStateDumper *v) const;
    };

} /* namespace lsp */

#endif /* CORE_UTIL_DITHER_H_ */
