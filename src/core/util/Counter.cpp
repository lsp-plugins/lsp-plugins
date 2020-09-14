/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 27 нояб. 2018 г.
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

#include <core/util/Counter.h>

namespace lsp
{
    Counter::Counter()
    {
        construct();
    }

    Counter::~Counter()
    {
    }

    void Counter::construct()
    {
        nCurrent        = DEFAULT_SAMPLE_RATE;
        nInitial        = DEFAULT_SAMPLE_RATE;
        nSampleRate     = DEFAULT_SAMPLE_RATE;
        fFrequency      = 1.0f;
        nFlags          = 0;
    }

    void Counter::set_sample_rate(size_t sr, bool reset)
    {
        nSampleRate     = sr;

        if (nFlags & F_INITIAL)
            fFrequency      = float(nSampleRate) / float(nInitial);
        else
            nInitial        = nSampleRate / fFrequency;

        if (reset)
            nCurrent    = nInitial;
    }

    void Counter::set_frequency(float freq, bool reset)
    {
        nFlags         &= ~F_INITIAL;
        fFrequency      = freq;
        nInitial        = nSampleRate / fFrequency;

        if (reset)
            nCurrent    = nInitial;
    }

    void Counter::set_initial_value(size_t value, bool reset)
    {
        nFlags         |= F_INITIAL;
        nInitial        = value;
        fFrequency      = float(nSampleRate) / float(nInitial);

        if (reset)
            nCurrent    = nInitial;
    }

    bool Counter::commit()
    {
        bool res        = nFlags & F_FIRED;
        nFlags         &= ~F_FIRED;
        return res;
    }

    bool Counter::reset()
    {
        bool res        = nFlags & F_FIRED;
        nCurrent        = nInitial;
        return res;
    }

    bool Counter::submit(size_t samples)
    {
        ssize_t left    = ssize_t(nCurrent) - ssize_t(samples);
        if (left <= 0)
        {
            nCurrent        = nInitial + (left % ssize_t(nInitial));
            nFlags         |= F_FIRED;
        }
        else
            nCurrent        = left;

        return nFlags & F_FIRED;
    }

    void Counter::dump(IStateDumper *v) const
    {
        v->write("nCurrent", nCurrent);
        v->write("nInitial", nInitial);
        v->write("nSampleRate", nSampleRate);
        v->write("fFrequency", fFrequency);
        v->write("nFlags", nFlags);
    }

} /* namespace lsp */
