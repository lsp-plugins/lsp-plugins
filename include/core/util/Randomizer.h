/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 23 марта 2016 г.
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

#ifndef CORE_UTIL_RANDOMIZER_H_
#define CORE_UTIL_RANDOMIZER_H_

#include <core/types.h>
#include <core/IStateDumper.h>

namespace lsp
{
    enum random_function_t
    {
        RND_LINEAR,
        RND_EXP,
        RND_TRIANGLE
    };

    class Randomizer
    {
        private:
            Randomizer &operator = (const Randomizer &);

        private:
            static const uint32_t vMul1[];
            static const uint32_t vMul2[];
            static const uint32_t vAdders[];

            typedef struct randgen_t
            {
                uint32_t    vLast;
                uint32_t    vMul1;
                uint32_t    vMul2;
                uint32_t    vAdd;
            } randgen_t;

            randgen_t   vRandom[4];
            size_t      nBufID;

        public:
            explicit Randomizer();

            void construct();

        public:
            /** Initialize random generator
             *
             * @param seed seed
             */
            void init(uint32_t seed);

            /** Initialize random generator, take current time as seed
             */
            void init();

            /** Generate float random number in range of [0..1) - excluding 1.0f
             * The guaranteed tolerance is 1e-6 or 0.000001
             *
             * @param func function
             * @return random number
             */
            float random(random_function_t func = RND_LINEAR);

            /**
             * Dump the state
             * @param dumper dumper
             */
            void dump(IStateDumper *v) const;
    };

} /* namespace lsp */

#endif /* CORE_UTIL_RANDOMIZER_H_ */
