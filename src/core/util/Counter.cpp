/*
 * Counter.cpp
 *
 *  Created on: 27 нояб. 2018 г.
 *      Author: sadko
 */

#include <core/util/Counter.h>

namespace lsp
{
    Counter::Counter()
    {
        nCurrent        = DEFAULT_SAMPLE_RATE;
        nInitial        = DEFAULT_SAMPLE_RATE;
        nSampleRate     = DEFAULT_SAMPLE_RATE;
        fFrequency      = 1.0f;
        nFlags          = 0;
    }
    
    Counter::~Counter()
    {
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
} /* namespace lsp */
