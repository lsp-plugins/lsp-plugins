/*
 * Dither.cpp
 *
 *  Created on: 21 дек. 2016 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <core/util/Dither.h>

#define DITHER_8BIT         0.00390625  /* 1 / 256 */

namespace lsp
{
    Dither::Dither()
    {
        nBits   = 0;
        fGain   = 1.0f;
        fDelta  = 0.0f;
    }

    Dither::~Dither()
    {
    }

    void Dither::set_bits(size_t bits)
    {
        nBits   = bits;
        if (bits <= 0)
            return;

        fDelta  = 4.0f; // 4 = 2 to compensate random in range -0.5 .. 0.5  *  2 to compensate (-1.0 .. 1.0 = 2.0) polarity
        while (bits >= 8)
        {
            fDelta     *= DITHER_8BIT;
            bits       -= 8;
        }
        if (bits > 0)
            fDelta     /= float(1 << bits);
        fGain   = 1.0f - 0.5f * fDelta;
    }

    void Dither::process(float *out, const float *in, size_t count)
    {
        if (!nBits)
        {
            dsp::copy(out, in, count);
            return;
        }

        while (count--)
            *(out++) = *(in++) * fGain + (sRandom.random(RND_TRIANGLE) - 0.5f) * fDelta;
    }

} /* namespace lsp */
