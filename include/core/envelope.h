/*
 * envelope.h
 *
 *  Created on: 20 февр. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_ENVELOPE_H_
#define CORE_ENVELOPE_H_

#include <core/types.h>

namespace lsp
{
    namespace envelope
    {
        extern const char *envelopes[];

        enum envelope_t
        {
            VIOLET_NOISE,
            BLUE_NOISE,
            WHITE_NOISE,
            PINK_NOISE,
            BROWN_NOISE,
            MINUS_4_5_DB,
            PLUS_4_5_DB,

            // Special variables
            TOTAL,
            FIRST = VIOLET_NOISE,
            LAST = TOTAL - 1
        };

        void noise(float *dst, size_t n, envelope_t type);

        void reverse_noise(float *dst, size_t n, envelope_t type);

        void white_noise(float *dst, size_t n);

        void pink_noise(float *dst, size_t n);

        void brown_noise(float *dst, size_t n);

        void blue_noise(float *dst, size_t n);

        void violet_noise(float *dst, size_t n);
    }
}

#endif /* CORE_ENVELOPE_H_ */
