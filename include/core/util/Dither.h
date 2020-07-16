/*
 * Dither.h
 *
 *  Created on: 21 дек. 2016 г.
 *      Author: sadko
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
