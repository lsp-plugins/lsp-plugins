/*
 * corellator.cpp
 *
 *  Created on: 28 сент. 2015 г.
 *      Author: sadko
 */

#include <core/dsp.h>

#include <plugins/phase_detector.h>
#include <core/debug.h>

#include <string.h>

namespace lsp
{
    phase_detector::phase_detector() : plugin(metadata)
    {
        fTimeInterval       = NAN;
        fReactivity         = NAN;

        vFunction           = NULL;
        vAccumulated        = NULL;
        vNormalized         = NULL;

        nMaxVectorSize      = 0;
        nVectorSize         = 0;
        nFuncSize           = 0;

        nGapSize            = 0;
        nMaxGapSize         = 0;
        nGapOffset          = 0;

        vA.nSize            = 0;
        vA.pData            = NULL;
        vB.nSize            = 0;
        vB.pData            = NULL;

        fTau                = 0.0f;
        fSelector           = NAN;
        bBypass             = false;
    }

    phase_detector::~phase_detector()
    {
    }

    size_t phase_detector::fillGap(const float *a, const float *b, size_t count)
    {
        size_t fill         = nMaxGapSize - nGapSize;

        if (fill <= 0)
        {
            if (nGapOffset < nGapSize)
                return 0;

            pDSP->copy(vA.pData, &vA.pData[nGapSize], vA.nSize);
            pDSP->copy(vB.pData, &vB.pData[nGapSize], vB.nSize);
            nGapSize            = 0;
            nGapOffset          = 0;
            fill                = nMaxGapSize;
        }

        if (count < fill)
            fill                = count;

        pDSP->copy(&vA.pData[vA.nSize + nGapSize], a, fill);
        pDSP->copy(&vB.pData[vB.nSize + nGapSize], b, fill);
        nGapSize           += fill;

        return fill;
    }
    
    void phase_detector::update_settings()
    {
        bool clear          = false;
        bool old_bypass     = bBypass;

        lsp_trace("update settings");

        // Read parameters
        float bypass        = vIntPorts[BYPASS]     -> getValue();
        float reset         = vIntPorts[RESET]      -> getValue();
        fSelector           = vIntPorts[SELECTOR]   -> getValue();

        lsp_trace("bypass = %.3f, reset = %.3f, selector=%.3f", bypass, reset, fSelector);
        bBypass             = (bypass >= 0.5f) || (reset > 0.5f);

        if ((old_bypass != bBypass) && (bBypass))
            clear               = true;

        if (setTimeInterval(vIntPorts[TIME]->getValue()))
            clear = true;
        setReactiveInterval(vIntPorts[REACTIVITY]->getValue());

        if (clear)
            clearBuffers();
    }

    void phase_detector::process(size_t samples)
    {
        // Store pointers to buffers
        float *in_a         = reinterpret_cast<float *>(vIntPorts[IN_A]    -> getBuffer());
        float *in_b         = reinterpret_cast<float *>(vIntPorts[IN_B]    -> getBuffer());
        float *out_a        = reinterpret_cast<float *>(vIntPorts[OUT_A]   -> getBuffer());
        float *out_b        = reinterpret_cast<float *>(vIntPorts[OUT_B]   -> getBuffer());

        // Bypass the original signal
        pDSP->copy(out_a, in_a, samples);
        pDSP->copy(out_b, in_b, samples);

        if (bBypass)
        {
            vIntPorts[BEST_TIME]       -> setValue(0.0f);
            vIntPorts[BEST_SAMPLES]    -> setValue(0.0f);
            vIntPorts[BEST_DISTANCE]   -> setValue(0.0f);
            vIntPorts[BEST_VALUE]      -> setValue(0.0f);

            vIntPorts[WORST_TIME]      -> setValue(0.0f);
            vIntPorts[WORST_SAMPLES]   -> setValue(0.0f);
            vIntPorts[WORST_DISTANCE]  -> setValue(0.0f);
            vIntPorts[WORST_VALUE]     -> setValue(0.0f);

            vIntPorts[SEL_TIME]        -> setValue(0.0f);
            vIntPorts[SEL_SAMPLES]     -> setValue(0.0f);
            vIntPorts[SEL_DISTANCE]    -> setValue(0.0f);
            vIntPorts[SEL_VALUE]       -> setValue(0.0f);

            return;
        }

        // Make calculations
        while (samples > 0)
        {
            ssize_t filled   = fillGap(in_a, in_b, samples);
            samples -= filled;

            // Subtract values
            while (nGapOffset < nGapSize)
            {
                // Subtract oldest sample from all functions
                pDSP->sub_multiplied(vFunction, &vB.pData[nGapOffset], vA.pData[nGapOffset], nFuncSize);

                // Add newest sample to all functions
                pDSP->add_multiplied(vFunction, &vB.pData[nGapOffset + nVectorSize], vA.pData[nGapOffset + nVectorSize], nFuncSize);

                // Accumulate peak function value
                pDSP->integrate(vAccumulated, vFunction, fTau, nFuncSize);

                // Increment gap offset: move to next sample
                nGapOffset++;
            }
        }

        // Now analyze average function in the time
        ssize_t best    = nVectorSize, worst = nVectorSize;
        ssize_t sel     = nFuncSize * (1.0 - (fSelector + SELECTOR_MAX) / (SELECTOR_MAX - SELECTOR_MIN));
        if (sel >= ssize_t(nFuncSize))
            sel             = nFuncSize - 1;
        else if (sel < 0)
            sel             = 0;

        pDSP->normalize(vNormalized, vAccumulated, nFuncSize);

        for (size_t i=0; i<nFuncSize; ++i)
        {
            if (vNormalized[i] > vNormalized[best])
                best        = i;
            if (vNormalized[i] < vNormalized[worst])
                worst       = i;
        }

        // Output values
        ssize_t sel_samples     = ssize_t(nVectorSize) - sel;
        ssize_t best_samples    = ssize_t(nVectorSize) - best;
        ssize_t worst_samples   = ssize_t(nVectorSize) - worst;

        vIntPorts[BEST_TIME]       -> setValue(samples_to_millis(best_samples));
        vIntPorts[BEST_SAMPLES]    -> setValue(best_samples);
        vIntPorts[BEST_DISTANCE]   -> setValue(samples_to_centimeters(best_samples));
        vIntPorts[BEST_VALUE]      -> setValue(vNormalized[best]);

        vIntPorts[WORST_TIME]      -> setValue(samples_to_millis(worst_samples));
        vIntPorts[WORST_SAMPLES]   -> setValue(worst_samples);
        vIntPorts[WORST_DISTANCE]  -> setValue(samples_to_centimeters(worst_samples));
        vIntPorts[WORST_VALUE]     -> setValue(vNormalized[worst]);

        vIntPorts[SEL_TIME]        -> setValue(samples_to_millis(sel_samples));
        vIntPorts[SEL_SAMPLES]     -> setValue(sel_samples);
        vIntPorts[SEL_DISTANCE]    -> setValue(samples_to_centimeters(sel_samples));
        vIntPorts[SEL_VALUE]       -> setValue(vNormalized[sel]);

        // Output mesh if specified
        mesh_t *mesh            = reinterpret_cast<mesh_t *>(vIntPorts[FUNCTION]->getBuffer());
        if (mesh != NULL)
        {
            // Generate function times
            float *dst  = mesh->pvData[0];
            float delta = samples_to_millis(1);
            for (size_t i=0; i<nFuncSize; ++i)
                *(dst++)        = delta * ((ssize_t(nVectorSize)) - ssize_t(i));

            // Copy function values
            pDSP->copy(mesh->pvData[1], vNormalized, nFuncSize);

            // Store mesh size and dimensions
            mesh->nItems    = nFuncSize;
            mesh->nBuffers  = 2;
        }
    }

    bool phase_detector::setTimeInterval(float interval)
    {
        lsp_trace("interval = %.3f", interval);
        interval            = limit(interval, DETECT_TIME_MIN, DETECT_TIME_MAX);

        // Calculate parameters
        if (fTimeInterval != interval)
        {
            fTimeInterval   = interval;
            nVectorSize     = (size_t(millis_to_samples(interval)) >> 2) << 2; // Make number of samples multiple of SSE register size
            nFuncSize       = nVectorSize << 1;
            vA.nSize        = nFuncSize;
            vB.nSize        = nFuncSize + nVectorSize;
            nMaxGapSize     = (nMaxVectorSize * 3) - nFuncSize;
            nGapSize        = 0;
            nGapOffset      = 0;

            // Yep, clear all buffers
            return true;
        }

        return false;
    }

    void phase_detector::setReactiveInterval(float interval)
    {
        lsp_trace("interval = %.3f", interval);
        interval            = limit(interval, REACT_TIME_MIN, REACT_TIME_MAX);

        if (fReactivity != interval)
        {
            // Calculate Reduction
            fReactivity     = interval;
            fTau            = 1.0f - expf(logf(1.0 - M_SQRT1_2) / seconds_to_samples(interval));
        }
    }

    void phase_detector::clearBuffers()
    {
        pDSP->fill_zero(vA.pData, nGapSize + nMaxVectorSize * 2);
        pDSP->fill_zero(vB.pData, nGapSize + nMaxVectorSize * 3);
        pDSP->fill_zero(vFunction, nMaxVectorSize * 2);
        pDSP->fill_zero(vAccumulated, nMaxVectorSize * 2);
    }

    void phase_detector::init(int sample_rate)
    {
        /*
                          +---------+---------+---------+
         A:               | Gap     | A Data  | Lookup  |
                          +---------+---------+---------+
                               /                       |
                +---------+---------+---------+---------+
         B:     | Gap     | B Delay | B Data  | Lookup  |
                +---------+---------+---------+---------+
                           |                      /
                          +---------+---------+
         F:               | Correlation funcs |
                          +---------+---------+
        */

        plugin::init(sample_rate);

        nMaxVectorSize  = millis_to_samples(DETECT_TIME_MAX);
        vA.pData        = new float[nMaxVectorSize * 3];
        vB.pData        = new float[nMaxVectorSize * 4];
        vFunction       = new float[nMaxVectorSize * 2];
        vAccumulated    = new float[nMaxVectorSize * 2];
        vNormalized     = new float[nMaxVectorSize * 2];

        setTimeInterval(DETECT_TIME_DFL);
        setReactiveInterval(REACT_TIME_DFL);
        fSelector       = SELECTOR_DFL;

        clearBuffers();
    }

    void phase_detector::destroy()
    {
        if (vA.pData != NULL)
        {
            lsp_trace("delete []   vA.pData");
            delete []   vA.pData;
            vA.pData    = NULL;
        }
        if (vB.pData != NULL)
        {
            lsp_trace("delete []   vB.pData");
            delete []   vB.pData;
            vB.pData    = NULL;
        }
        if (vFunction != NULL)
        {
            lsp_trace("delete []   vFunction");
            delete []   vFunction;
            vFunction   = NULL;
        }
        if (vAccumulated != NULL)
        {
            lsp_trace("delete []   vAccumulated");
            delete []   vAccumulated;
            vAccumulated= NULL;
        }
        if (vNormalized != NULL)
        {
            lsp_trace("delete []   vNormalized");
            delete []   vNormalized;
            vNormalized = NULL;
        }

        plugin::destroy();
    }

} /* namespace ddb */


