/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 07 дек. 2015 г.
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

#include <core/util/Bypass.h>
#include <core/debug.h>

namespace lsp
{
    
    Bypass::Bypass()
    {
        construct();
    }
    
    Bypass::~Bypass()
    {
    }

    void Bypass::construct()
    {
        nState      = S_OFF;
        fDelta      = 0;
        fGain       = 0;
    }

    bool Bypass::set_bypass(bool bypass)
    {
        // Trigger state change
        switch (nState)
        {
            case S_ON:
                if (bypass)
                    return false;
                nState  = S_ACTIVE;
                break;
            case S_OFF:
                if (!bypass)
                    return false;
                nState  = S_ACTIVE;
                break;
            case S_ACTIVE:
            {
                bool off    = (fDelta < 0.0f);
                if (bypass == off)
                    return false;
                break;
            }
            default:
                return false;
        }

        // Change sign of the applying delta
        fDelta  = -fDelta;
        return true;
    }

    bool Bypass::bypassing() const
    {
        switch (nState)
        {
            case S_ON: return true;
            case S_OFF: return false;
            case S_ACTIVE: return fDelta < 0.0f;
            default: return false;
        }
    }

    void Bypass::init(int sample_rate, float time)
    {
        // Off by default
        float length    = sample_rate * time;
        if (length < 1.0f)
            length          = 1.0f;
        nState          = S_OFF;
        fDelta          = 1.0 / length;
        fGain           = 1.0;
    }

    void Bypass::process(float *dst, const float *dry, const float *wet, size_t count)
    {
        // Skip empty buffers
        if (count == 0)
            return;

        if (dry != NULL)
        {
            // Analyze direction
            if (fDelta > 0.0f)
            {
                // Process transition
                while (fGain < 1.0)
                {
                    *dst    =   *dry + (*wet - *dry) * fGain;

                    fGain   +=  fDelta;
                    dry     ++;
                    wet     ++;
                    dst     ++;

                    if ((--count) <= 0) // Last sample?
                        return;
                }

                // Copy wet data
                fGain   = 1.0;
                nState  = S_OFF;
                if (count > 0)
                    dsp::copy(dst, wet, count);
            }
            else
            {
                // Process transition
                while (fGain > 0.0)
                {
                    *dst    =   *dry + (*wet - *dry) * fGain;

                    fGain   +=  fDelta;
                    dry     ++;
                    wet     ++;
                    dst     ++;

                    if ((--count) <= 0) // Last sample?
                        return;
                }

                // Copy dry data
                fGain   = 0.0;
                nState  = S_ON;
                if (count > 0)
                    dsp::copy(dst, dry, count);
            }
        }
        else
        {
            // Analyze direction
            if (fDelta > 0.0f)
            {
                // Process transition
                while (fGain < 1.0)
                {
                    *dst    =   (*wet) * fGain;

                    fGain   +=  fDelta;
                    wet     ++;
                    dst     ++;

                    if ((--count) <= 0) // Last sample?
                        return;
                }

                // Copy wet data
                fGain   = 1.0;
                nState  = S_OFF;
                if (count > 0)
                    dsp::copy(dst, wet, count);
            }
            else
            {
                // Process transition
                while (fGain > 0.0)
                {
                    *dst    =   (*wet) * fGain;

                    fGain   +=  fDelta;
                    wet     ++;
                    dst     ++;

                    if ((--count) <= 0) // Last sample?
                        return;
                }

                // Copy dry data
                fGain   = 0.0;
                nState  = S_ON;
                if (count > 0)
                    dsp::fill_zero(dst, count);
            }
        }
    }

    void Bypass::dump(IStateDumper *v) const
    {
        v->write("nState", nState);
        v->write("fDelta", fDelta);
        v->write("fGain", fGain);
    }

} /* namespace lsp */
