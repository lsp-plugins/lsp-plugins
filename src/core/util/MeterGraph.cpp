/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 20 мая 2016 г.
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
#include <core/util/MeterGraph.h>

namespace lsp
{
    MeterGraph::MeterGraph()
    {
        fCurrent    = 0.0f;
        nCount      = 0;
        nPeriod     = 1;
        bMinimize   = false;
    }

    MeterGraph::~MeterGraph()
    {
        destroy();
    }

    bool MeterGraph::init(size_t frames, size_t period)
    {
        if (period <= 0)
            return false;

        if (!sBuffer.init(frames * 4, frames))
            return false;

        fCurrent    = 0.0f;
        nCount      = 0;
        nPeriod     = period;
        return true;
    }

    void MeterGraph::destroy()
    {
        sBuffer.destroy();
    }

    void MeterGraph::process(float sample)
    {
        // Make sample positive
        if (sample < 0)
            sample      = - sample;

        if (bMinimize)
        {
            // Update current sample
            if (nCount == 0)
                fCurrent    = sample;
            else if (fCurrent < sample)
                fCurrent    = sample;
        }
        else
        {
            // Update current sample
            if (nCount == 0)
                fCurrent    = sample;
            else if (fCurrent > sample)
                fCurrent    = sample;
        }

        // Increment number of samples processed
        if ((++nCount) >= nPeriod)
        {
            // Append current sample to buffer
            sBuffer.shift();
            sBuffer.append(fCurrent);

            // Update counter
            nCount      = 0;
        }
    }

    void MeterGraph::process(const float *s, size_t n)
    {
        if (bMinimize)
        {
            while (n > 0)
            {
                // Determine amount of samples to process
                ssize_t can_do      = nPeriod - nCount;
                if (can_do > ssize_t(n))
                    can_do          = n;

                // Process the samples
                if (can_do > 0)
                {
                    // Get maximum sample
                    float sample        = dsp::abs_min(s, can_do);
                    if (nCount == 0)
                        fCurrent        = sample;
                    else if (fCurrent > sample)
                        fCurrent        = sample;

                    // Update counters and pointers
                    nCount             += can_do;
                    n                  -= can_do;
                    s                  += can_do;
                }

                // Check that need to switch to next sample
                if (nCount >= nPeriod)
                {
                    // Append current sample to buffer
                    sBuffer.shift();
                    sBuffer.append(fCurrent);

                    // Update counter
                    nCount      = 0;
                }
            }
        }
        else
        {
            while (n > 0)
            {
                // Determine amount of samples to process
                ssize_t can_do      = nPeriod - nCount;
                if (can_do > ssize_t(n))
                    can_do          = n;

                // Process the samples
                if (can_do > 0)
                {
                    // Get maximum sample
                    float sample        = dsp::abs_max(s, can_do);
                    if (nCount == 0)
                        fCurrent        = sample;
                    else if (fCurrent < sample)
                        fCurrent        = sample;

                    // Update counters and pointers
                    nCount             += can_do;
                    n                  -= can_do;
                    s                  += can_do;
                }

                // Check that need to switch to next sample
                if (nCount >= nPeriod)
                {
                    // Append current sample to buffer
                    sBuffer.shift();
                    sBuffer.append(fCurrent);

                    // Update counter
                    nCount      = 0;
                }
            }
        }
    }

    void MeterGraph::dump(IStateDumper *v) const
    {
        v->write_object("sBuffer", &sBuffer);
        v->write("fCurrent", fCurrent);
        v->write("nCount", nCount);
        v->write("nPeriod", nPeriod);
        v->write("bMinimize", bMinimize);
    }
}



