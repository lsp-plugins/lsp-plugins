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
            PURPLE_NOISE,
            BLUE_NOISE,
            WHITE_NOISE,
            PINK_NOISE,
            BROWN_NOISE,

            // Special variables
            TOTAL,
            FIRST = PURPLE_NOISE,
            LAST = TOTAL - 1
        };

        void noise(float *dst, size_t n, envelope_t type);

        void reverse_noise(float *dst, size_t n, envelope_t type);

        void white_noise(float *dst, size_t n);

        void pink_noise(float *dst, size_t n);

        void brown_noise(float *dst, size_t n);

        void blue_noise(float *dst, size_t n);

        void purple_noise(float *dst, size_t n);
    }
}

#endif /* CORE_ENVELOPE_H_ */
