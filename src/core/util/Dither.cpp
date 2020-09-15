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

    void Dither::dump(IStateDumper *v) const
    {
        v->write("nBits", nBits);
        v->write("fGain", fGain);
        v->write("fDelta", fDelta);
        v->write_object("sRandom", &sRandom);
    }

} /* namespace lsp */
