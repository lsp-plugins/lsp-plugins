/*
 * filter.cpp
 *
 *  Created on: 28 июня 2016 г.
 *      Author: sadko
 */

#include <math.h>
#include <dsp/dsp.h>
#include <core/debug.h>
#include <core/filters/Filter.h>

namespace lsp
{
    Filter::Filter()
    {
        pBank               = NULL;
        sParams.nType       = FLT_NONE;
        sParams.fFreq       = 0;
        sParams.fFreq2      = 0;
        sParams.fGain       = 0.0f;
        sParams.nSlope      = 0;
        sParams.fQuality    = 0.0f;

        nSampleRate         = 0;
        nMode               = FM_BYPASS;
        nLatency            = 0;
        nItems              = 0;

        vItems              = NULL;
        vData               = NULL;
        nFlags              = FF_REBUILD | FF_CLEAR;
    }

    Filter::~Filter()
    {
        destroy();
    }

    bool Filter::init(FilterBank *fb)
    {
        filter_params_t fp;

        fp.nType            = FLT_NONE;
        fp.fFreq            = 1000.0f;
        fp.fFreq2           = 1000.0f;
        fp.fGain            = 1.0f;
        fp.fQuality         = 0;
        fp.nSlope           = 1;

        if (fb != NULL)
            pBank           = fb;
        else
        {
            // Try to allocate filter bank
            pBank           = new FilterBank();
            if (pBank == NULL)
                return false;

            // Update flags that we hawe own filter bank
            nFlags         |= FF_OWN_BANK;

            // Try to initialize own filter bank
            if (!pBank->init(FILTER_CHAINS_MAX))
                return false;
        }

        if (vData == NULL)
        {
            size_t cascade_size = ALIGN_SIZE(sizeof(cascade_t) * FILTER_CHAINS_MAX, DEFAULT_ALIGN);

            size_t allocate     = cascade_size + DEFAULT_ALIGN; // + filters_size;
            vData               = new uint8_t[allocate];
            if (vData == NULL)
                return false;

            uint8_t *ptr        = ALIGN_PTR(vData, DEFAULT_ALIGN);
            vItems              = reinterpret_cast<cascade_t *>(ptr);
            ptr                += cascade_size;
        }

        update(48000, &fp);
        nFlags             |= FF_REBUILD | FF_CLEAR;

        return true;
    }

    void Filter::destroy()
    {
        if (vData != NULL)
        {
            delete  [] vData;
            vItems  = NULL;
            vData   = NULL;
        }

        if (pBank != NULL)
        {
            // Destroy filter bank if it's our own filter bank
            if (nFlags & FF_OWN_BANK)
            {
                pBank->destroy();
                delete pBank;
            }

            pBank       = NULL;
        }

        nFlags      = 0;
    }

    void Filter::update(size_t sr, const filter_params_t *params)
    {
        // Init direct filter chain
        filter_params_t *fp     = &sParams;
        size_t type             = fp->nType;
        size_t slope            = fp->nSlope;

        // Reset parameters
        nSampleRate             = sr;
        nMode                   = FM_BYPASS;
        nLatency                = 0;

        // Copy and limit parameters
        *fp                     = *params;
        if (fp->nSlope < 1)
            fp->nSlope              = 1;
        else if (fp->nSlope > FILTER_CHAINS_MAX)
            fp->nSlope              = FILTER_CHAINS_MAX;
        if (fp->fFreq < SPEC_FREQ_MIN)
            fp->fFreq               = SPEC_FREQ_MIN;
        else if (fp->fFreq > SPEC_FREQ_MAX)
            fp->fFreq               = SPEC_FREQ_MAX;
        if (fp->fFreq >= (0.49f * nSampleRate))
            fp->fFreq               = 0.49f * nSampleRate;
        if (fp->fFreq2 < SPEC_FREQ_MIN)
            fp->fFreq2              = SPEC_FREQ_MIN;
        else if (fp->fFreq2 > SPEC_FREQ_MAX)
            fp->fFreq2              = SPEC_FREQ_MAX;
        if (fp->fFreq2 >= (0.49f * nSampleRate))
            fp->fFreq2              = 0.49f * nSampleRate;

        nFlags                 |= FF_REBUILD;
        if ((type != fp->nType) || (slope != fp->nSlope))
            nFlags                 |= FF_CLEAR;
    }

    void Filter::get_params(filter_params_t *params)
    {
        if (params != NULL)
            *params =    sParams;
    }

    Filter::cascade_t *Filter::add_cascade()
    {
        // Get cascade
        cascade_t *c = (nItems >= FILTER_CHAINS_MAX) ?
            &vItems[FILTER_CHAINS_MAX-1] :
            &vItems[nItems];

        // Increment number of chains
        if (nItems < FILTER_CHAINS_MAX)
            nItems++;

        // Initialize cascade
        for (size_t i=0; i<4; ++i)
        {
            c->t[i] = 0.0;
            c->b[i] = 0.0;
        }

        // Return cascade
        return c;
    }

    float Filter::bilinear_relative(float f1, float f2)
    {
        double nf   = M_PI / double(nSampleRate);
        return tan(f1 * nf) / tan(f2 * nf);
    }

    void Filter::rebuild()
    {
        // Clear bank if it is internal bank
        if (nFlags & FF_OWN_BANK)
            pBank->begin();

        // Reset number of cascades
        nItems                  = 0;

        filter_params_t fp      = sParams;

        // Calculate filter
        switch (sParams.nType)
        {
            case FLT_BT_AMPLIFIER:
            case FLT_BT_RLC_LOPASS:
            case FLT_BT_RLC_HIPASS:
            case FLT_BT_RLC_LOSHELF:
            case FLT_BT_RLC_HISHELF:
            case FLT_BT_RLC_BELL:
            case FLT_BT_RLC_RESONANCE:
            case FLT_BT_RLC_NOTCH:
            case FLT_BT_RLC_LADDERPASS:
            case FLT_BT_RLC_LADDERREJ:
            case FLT_BT_RLC_BANDPASS:
            case FLT_BT_RLC_ENVELOPE:
            {
                // Calculate filter parameters
                fp.fFreq2           = bilinear_relative(fp.fFreq, fp.fFreq2);    // Normalize frequency
                calc_rlc_filter(sParams.nType, &fp);
                nMode               = FM_BILINEAR;
                break;
            }

            case FLT_MT_AMPLIFIER:
            case FLT_MT_RLC_LOPASS:
            case FLT_MT_RLC_HIPASS:
            case FLT_MT_RLC_LOSHELF:
            case FLT_MT_RLC_HISHELF:
            case FLT_MT_RLC_BELL:
            case FLT_MT_RLC_RESONANCE:
            case FLT_MT_RLC_NOTCH:
            case FLT_MT_RLC_LADDERPASS:
            case FLT_MT_RLC_LADDERREJ:
            case FLT_MT_RLC_BANDPASS:
            case FLT_MT_RLC_ENVELOPE:
            {
                // Calculate filter parameters
                fp.fFreq2           = fp.fFreq / fp.fFreq2;    // Normalize frequency
                calc_rlc_filter(sParams.nType - 1, &fp);
                nMode               = FM_MATCHED;
                break;
            }

            case FLT_BT_BWC_LOPASS:
            case FLT_BT_BWC_HIPASS:
            case FLT_BT_BWC_LOSHELF:
            case FLT_BT_BWC_HISHELF:
            case FLT_BT_BWC_BELL:
            case FLT_BT_BWC_LADDERPASS:
            case FLT_BT_BWC_LADDERREJ:
            case FLT_BT_BWC_BANDPASS:
            {
                // Calculate filter parameters
                fp.fFreq2           = bilinear_relative(fp.fFreq, fp.fFreq2);    // Normalize frequency
                calc_bwc_filter(sParams.nType, &fp);
                nMode               = FM_BILINEAR;
                break;
            }

            case FLT_MT_BWC_LOPASS:
            case FLT_MT_BWC_HIPASS:
            case FLT_MT_BWC_LOSHELF:
            case FLT_MT_BWC_HISHELF:
            case FLT_MT_BWC_BELL:
            case FLT_MT_BWC_LADDERPASS:
            case FLT_MT_BWC_LADDERREJ:
            case FLT_MT_BWC_BANDPASS:
            {
                // Calculate filter parameters
                fp.fFreq2           = fp.fFreq / fp.fFreq2;    // Normalize frequency
                calc_bwc_filter(sParams.nType - 1, &fp);
                nMode               = FM_MATCHED;
                break;
            }

            case FLT_BT_LRX_LOPASS:
            case FLT_BT_LRX_HIPASS:
            case FLT_BT_LRX_LOSHELF:
            case FLT_BT_LRX_HISHELF:
            case FLT_BT_LRX_BELL:
            case FLT_BT_LRX_LADDERPASS:
            case FLT_BT_LRX_LADDERREJ:
            case FLT_BT_LRX_BANDPASS:
            {
                // Calculate filter parameters
                fp.fFreq2           = bilinear_relative(fp.fFreq, fp.fFreq2);    // Normalize frequency
                calc_lrx_filter(sParams.nType, &fp);
                nMode               = FM_BILINEAR;
                break;
            }

            case FLT_MT_LRX_LOPASS:
            case FLT_MT_LRX_HIPASS:
            case FLT_MT_LRX_LOSHELF:
            case FLT_MT_LRX_HISHELF:
            case FLT_MT_LRX_BELL:
            case FLT_MT_LRX_LADDERPASS:
            case FLT_MT_LRX_LADDERREJ:
            case FLT_MT_LRX_BANDPASS:
            {
                // Calculate filter parameters
                fp.fFreq2           = fp.fFreq / fp.fFreq2;    // Normalize frequency
                calc_lrx_filter(sParams.nType - 1, &fp);
                nMode               = FM_MATCHED;
                break;
            }

            case FLT_NONE:
            default:
                nMode           = FM_BYPASS;
                break;
        }

        if (nMode == FM_BILINEAR)
            bilinear_transform();
        else if (nMode == FM_MATCHED)
            matched_transform();

        // Complete bank if it is internal bank
        if (nFlags & FF_OWN_BANK)
            pBank->end(nFlags & FF_CLEAR);

        nFlags     &= FF_OWN_BANK; // Clear all flags except FF_OWN_BANK
    }

    void Filter::complex_transfer_calc(float *re, float *im, double f)
    {
        double f2       = f * f; // f ^ 2
        double r_re     = 1.0;
        double r_im     = 0.0;

        for (size_t i=0; i<nItems; ++i)
        {
            cascade_t *c    = &vItems[i];

            // Calculate top and bottom transfer parts
            double t_re     = c->t[0] - f2 * c->t[2];
            double t_im     = c->t[1]*f;
            double b_re     = c->b[0] - f2 * c->b[2];
            double b_im     = c->b[1]*f;

            // Calculate top / bottom
            double w        = 1.0 / (b_re * b_re + b_im * b_im);
            double w_re     = (t_re * b_re + t_im * b_im) * w;
            double w_im     = (t_im * b_re - t_re * b_im) * w;

            // Update transfer function
            b_re            = r_re*w_re - r_im*w_im;
            b_im            = r_re*w_im + r_im*w_re;

            // Commit result
            r_re            = b_re;
            r_im            = b_im;
        }

        *re             = r_re;
        *im             = r_im;
    }

    void Filter::freq_chart(float *re, float *im, const float *f, size_t count)
    {
        // Calculate frequency chart

        switch (nMode)
        {
            case FM_BILINEAR:
            {
                double nf   = M_PI / double(nSampleRate);
                double kf   = 1.0/tan(sParams.fFreq * nf);
                double lf   = nSampleRate * 0.499;

                while (count--)
                {
                    // Cyclic frequency
                    double w    = *(f++);
                    w           = tan((w > lf ? lf : w) * nf) * kf;

                    complex_transfer_calc(re++, im++, w);
                }
                break;
            }

            case FM_MATCHED:
            {
                double kf   = 1.0 / sParams.fFreq;

                while (count--)
                {
                    // Cyclic frequency
                    double w    = *(f++) * kf;

                    complex_transfer_calc(re++, im++, w);
                }
                break;
            }

            case FM_BYPASS:
            default:
                dsp::fill_one(re, count);
                dsp::fill_zero(im, count);
                return;
        }
    }

    void Filter::freq_chart(float *c, const float *f, size_t count)
    {
        // Calculate frequency chart

        switch (nMode)
        {
            case FM_BILINEAR:
            {
                double nf   = M_PI / double(nSampleRate);
                double kf   = 1.0/tan(sParams.fFreq * nf);
                double lf   = nSampleRate * 0.499;

                while (count--)
                {
                    // Cyclic frequency
                    double w    = *(f++);
                    w           = tan((w > lf ? lf : w) * nf) * kf;

                    complex_transfer_calc(c, &c[1], w);
                    c += 2;
                }
                break;
            }

            case FM_MATCHED:
            {
                double kf   = 1.0 / sParams.fFreq;

                while (count--)
                {
                    // Cyclic frequency
                    double w    = *(f++) * kf;
                    complex_transfer_calc(c, &c[1], w);
                    c += 2;
                }
                break;
            }

            case FM_BYPASS:
            default:
                dsp::pcomplex_fill_ri(c, 1.0f, 0.0f, count);
                return;
        }
    }

    void Filter::process(float *out, const float *in, size_t samples)
    {
        // Check whether we need to rebuild filter
        if (nFlags & (~FF_OWN_BANK))
            rebuild();

        switch (nMode)
        {
            case FM_BILINEAR:
            case FM_MATCHED:
            {
                pBank->process(out, in, samples);
                break;
            }

            default:
            {
                dsp::copy(out, in, samples);
                break;
            }
        }
    }

    void Filter::calc_rlc_filter(size_t type, const filter_params_t *fp)
    {
        cascade_t *c                = NULL;
        nMode                       = FM_BILINEAR;

        switch (type)
        {
            case FLT_BT_AMPLIFIER:
            {
                c           = add_cascade();
                c->t[0]     = fp->fGain;
                c->t[1]     = 0.0f;
                c->t[2]     = 0.0f;

                c->b[0]     = 1.0f;
                c->b[1]     = 0.0f;
                c->b[2]     = 0.0f;

                break;
            }

            case FLT_BT_RLC_LOPASS:
            case FLT_BT_RLC_HIPASS:
            {
                // Add cascade with one pole
                size_t i        = fp->nSlope & 1;
                if (i)
                {
                    c           = add_cascade();
                    c->b[0]     = 1.0;
                    c->b[1]     = 1.0;

                    if (type == FLT_BT_RLC_LOPASS)
                        c->t[0]     = fp->fGain;
                    else
                        c->t[1]     = fp->fGain;
                }

                // Add additional cascades
                for (size_t j=i; j < fp->nSlope; j+=2)
                {
                    c           = add_cascade();
                    c->b[0]     = 1.0;
                    c->b[1]     = 2.0 / (1.0 + fp->fQuality);
                    c->b[2]     = 1.0;

                    if (type == FLT_BT_RLC_LOPASS)
                        c->t[0]     = (j == 0) ? fp->fGain : 1.0;
                    else
                        c->t[2]     = (j == 0) ? fp->fGain : 1.0;
                }

                break;
            }

            case FLT_BT_RLC_LOSHELF:
            case FLT_BT_RLC_HISHELF:
            {
                size_t slope            = fp->nSlope * 2;
                double gain             = sqrt(fp->fGain);
                double fg               = exp(log(gain)/slope);

                for (size_t j=0; j < fp->nSlope; j++)
                {
                    c                       = add_cascade();
                    double *t               = (type == FLT_BT_RLC_LOSHELF) ? c->t : c->b;
                    double *b               = (type == FLT_BT_RLC_LOSHELF) ? c->b : c->t;

                    // Create transfer function
                    t[0]                    = fg;
                    t[1]                    = 2.0 / (1.0 + fp->fQuality);
                    t[2]                    = 1.0 / fg;

                    b[0]                    = 1.0 / fg;
                    b[1]                    = 2.0 / (1.0 + fp->fQuality);
                    b[2]                    = fg;

                    if (j == 0)
                    {
                        c->t[0]                *= gain;
                        c->t[1]                *= gain;
                        c->t[2]                *= gain;
                    }
                }

                break;
            }

            case FLT_BT_RLC_LADDERPASS:
            case FLT_BT_RLC_LADDERREJ:
            {
                size_t slope            = fp->nSlope * 2;
                double gain1            = (type == FLT_BT_RLC_LADDERREJ) ? sqrt(1.0/fp->fGain) : sqrt(fp->fGain);
                double gain2            = (type == FLT_BT_RLC_LADDERREJ) ? sqrt(fp->fGain) : sqrt(1.0/fp->fGain);

                double fg1              = exp(log(gain1)/slope);
                double fg2              = exp(log(gain2)/slope);
                double kf               = fp->fFreq2;

                for (size_t j=0; j < fp->nSlope; j++)
                {
                    // First shelf cascade, lo-shelf for LADDERREJ, hi-shelf for LADDERPASS
                    c                       = add_cascade();
                    double *t               = (type == FLT_BT_RLC_LADDERREJ) ? c->t : c->b;
                    double *b               = (type == FLT_BT_RLC_LADDERREJ) ? c->b : c->t;
                    double fg               = (type == FLT_BT_RLC_LADDERREJ) ? fg2 : fg1;
                    double gain             = (type == FLT_BT_RLC_LADDERREJ) ? gain2 : gain1;

                    // Create transfer function
                    t[0]                    = fg;
                    t[1]                    = 2.0 / (1.0 + fp->fQuality);
                    t[2]                    = 1.0 / fg;

                    b[0]                    = 1.0 / fg;
                    b[1]                    = 2.0 / (1.0 + fp->fQuality);
                    b[2]                    = fg;

                    if (j == 0)
                    {
                        c->t[0]                *= gain;
                        c->t[1]                *= gain;
                        c->t[2]                *= gain;
                    }

                    // Second shelf cascade, hi-shelf always
                    c                       = add_cascade();
                    t                       = c->b;
                    b                       = c->t;

                    // Create transfer function
                    t[0]                    = fg2;
                    t[1]                    = 2.0 * kf / (1.0 + fp->fQuality);
                    t[2]                    = kf*kf / fg2;

                    b[0]                    = 1.0 / fg2;
                    b[1]                    = 2.0 * kf / (1.0 + fp->fQuality);
                    b[2]                    = fg2 * kf * kf;

                    if (j == 0)
                    {
                        c->t[0]                *= gain2;
                        c->t[1]                *= gain2;
                        c->t[2]                *= gain2;
                    }
                }

                break;
            }

            case FLT_BT_RLC_BANDPASS:
            {
                double f2               = 1.0 / fp->fFreq2;
                double k                = (1.0 + f2)/(1.0 + fp->fQuality);

                for (size_t j=0; j < fp->nSlope; j++)
                {
                    c                       = add_cascade();
                    c->t[1]                 = (j == 0) ? exp(fp->nSlope * log(k)) * fp->fGain : 1.0;
                    c->b[0]                 = f2;
                    c->b[1]                 = k;
                    c->b[2]                 = 1.0;
                }

                break;
            }

            case FLT_BT_RLC_BELL:
            {
                double fg               = exp(log(fp->fGain)/fp->nSlope);
                double angle            = atan(fg);
                double k                = 2.0 * (1.0/fg + fg) / (1.0 + (2.0 * fp->fQuality) / fp->nSlope);
                double kt               = k * sin(angle);
                double kb               = k * cos(angle);

                for (size_t j=0; j < fp->nSlope; j++)
                {
                    c                       = add_cascade();

                    // Create transfer function
                    c->t[0]                 = 1.0;
                    c->t[1]                 = kt;
                    c->t[2]                 = 1.0;

                    c->b[0]                 = 1.0;
                    c->b[1]                 = kb;
                    c->b[2]                 = 1.0;
                }

                break;
            }

            // Resonance filter
            case FLT_BT_RLC_RESONANCE:
            {
                double angle            = atan(exp(log(fp->fGain) / fp->nSlope));
                double k                = 2.0 / (1.0 + fp->fQuality);
                double kt               = k * sin(angle);
                double kb               = k * cos(angle);

                for (size_t j=0; j < fp->nSlope; j++)
                {
                    c                       = add_cascade();

                    // Create transfer function
                    c->t[0]                 = 1.0;
                    c->t[1]                 = kt;
                    c->t[2]                 = 1.0;

                    c->b[0]                 = 1.0;
                    c->b[1]                 = kb;
                    c->b[2]                 = 1.0;
                }

                break;
            }

            case FLT_BT_RLC_NOTCH:
            {
                c                       = add_cascade();

                // Create transfer function
                c->t[0]                 = fp->fGain;
                c->t[1]                 = 0;
                c->t[2]                 = fp->fGain;

                // Bottom polynom
                c->b[0]                 = 1.0;
                c->b[1]                 = 2.0 / (1.0 + fp->fQuality);
                c->b[2]                 = 1.0;

                break;
            }

            case FLT_BT_RLC_ENVELOPE:
            {
                size_t slope    = fp->nSlope;
                size_t cj       = 0;
                if (slope & 1)
                {
                    float k                 = 1.0f;

                    for (size_t i=0; i<3; ++i)
                    {
                        c               = add_cascade();
                        c->t[0]         = 1.0f;
                        c->t[1]         = (1.0f + 0.25f)*k;
                        c->t[2]         = 0.25f * k*k;

                        c->b[0]         = 1.0f;
                        c->b[1]         = (0.5f + 0.125f)*k;
                        c->b[2]         = 0.5f * 0.125f * k*k;

                        k              *= 0.0625f;
                        if (!(cj++))
                        {
                            c->t[0]        *= fp->fGain;
                            c->t[1]        *= fp->fGain;
                            c->t[2]        *= fp->fGain;
                        }
                    }
                }
                slope >>= 1;

                for (size_t j=0; j < slope; j++)
                {
                    c                       = add_cascade();
                    c->t[0]                 = (cj == 0) ? fp->fGain : 1.0f;
                    c->t[1]                 = (cj == 0) ? fp->fGain : 1.0f;
                    c->b[0]                 = 1.0f;
                    c->b[1]                 = 0.0005f;
                    cj ++;
                }
                break;
            }

            default:
                nMode           = FM_BYPASS;
                break;
        }
    }

    void Filter::calc_bwc_filter(size_t type, const filter_params_t *fp)
    {
        cascade_t *c                = NULL;

        switch (type)
        {
            case FLT_BT_BWC_LOPASS:
            case FLT_BT_BWC_HIPASS:
            {
                double k    = 1.0f / (1.0f + fp->fQuality);
                size_t i    = fp->nSlope & 1;
                if (i)
                {
                    c           = add_cascade();
                    c->b[0]     = 1.0;
                    c->b[1]     = 1.0;

                    if (type == FLT_BT_BWC_LOPASS)
                        c->t[0]     = fp->fGain;
                    else
                        c->t[1]     = fp->fGain;
                }

                for (size_t j=i; j < fp->nSlope; j += 2)
                {
                    double theta    = ((j - i + 1)*M_PI_2)/fp->nSlope;
                    double tsin     = sin(theta);
                    double tcos     = sqrt(1.0 - tsin*tsin);
                    float kf        = tsin*tsin + k*k * tcos*tcos;

                    c               = add_cascade();

                    // Tranfer function
                    if (type == FLT_BT_BWC_HIPASS)
                    {
                        c->t[2]         = (j == 0) ? fp->fGain : 1.0;

                        c->b[0]         = 1.0 / kf;
                        c->b[1]         = 2.0 * k * tcos / kf;
                        c->b[2]         = 1.0;
                    }
                    else
                    {
                        c->t[0]         = (j == 0) ? fp->fGain : 1.0;

                        c->b[0]         = 1.0;
                        c->b[1]         = 2.0 * k * tcos / kf;
                        c->b[2]         = 1.0 / kf;
                    }
                }

                break;
            }

            case FLT_BT_BWC_HISHELF:
            case FLT_BT_BWC_LOSHELF:
            {
                double gain             = sqrt(fp->fGain);
                double fg               = exp(log(gain)/(2.0*fp->nSlope));
                double k                = 1.0f / (1.0 + fp->fQuality * (1.0 - exp(2.0 - gain - 1.0/gain)));

                for (size_t j=0; j < fp->nSlope; ++j)
                {
                    double theta        = ((2*j + 1)*M_PI_2)/(2*fp->nSlope);
                    double tsin         = sin(theta);
                    double tcos         = sqrt(1.0 - tsin*tsin);
                    double kf           = tsin*tsin + k*k * tcos*tcos;

                    c                   = add_cascade();
                    double *t           = (type == FLT_BT_BWC_HISHELF) ? c->t : c->b;
                    double *b           = (type == FLT_BT_BWC_HISHELF) ? c->b : c->t;

                    // Transfer function
                    t[0]                = kf / fg;
                    t[1]                = 2.0 * k * tcos;
                    t[2]                = fg;

                    b[0]                = fg;
                    b[1]                = 2.0 * k * tcos;
                    b[2]                = kf / fg;

                    if (j == 0)
                    {
                        c->t[0]            *= gain;
                        c->t[1]            *= gain;
                        c->t[2]            *= gain;
                    }
                }

                break;
            }

            case FLT_BT_BWC_LADDERPASS:
            case FLT_BT_BWC_LADDERREJ:
            {
                size_t slope            = fp->nSlope * 2;
                double gain1            = (type == FLT_BT_BWC_LADDERPASS) ? sqrt(fp->fGain) : sqrt(1.0/fp->fGain);
                double gain2            = (type == FLT_BT_BWC_LADDERPASS) ? sqrt(1.0/fp->fGain) : sqrt(fp->fGain);

                double fg1              = exp(log(gain1)/(2.0*fp->nSlope));
                double fg2              = exp(log(gain2)/(2.0*fp->nSlope));
                double k1               = 1.0f / (1.0 + fp->fQuality * (1.0 - exp(2.0 - gain1 - 1.0/gain1)));
                double k2               = 1.0f / (1.0 + fp->fQuality * (1.0 - exp(2.0 - gain2 - 1.0/gain2)));
                double xf               = fp->fFreq2;

                for (size_t j=0; j < fp->nSlope; ++j)
                {
                    double theta        = ((2*j + 1)*M_PI_2)/double(slope);
                    double tsin         = sin(theta);
                    double tcos         = sqrt(1.0 - tsin*tsin);

                    // First shelf cascade, lo-shelf for LADDERREJ, hi-shelf for LADDERPASS
                    double k            = (type == FLT_BT_BWC_LADDERPASS) ? k1 : k2;
                    double fg           = (type == FLT_BT_BWC_LADDERPASS) ? fg1 : fg2;
                    double gain         = (type == FLT_BT_BWC_LADDERPASS) ? gain1 : gain2;
                    double kf           = tsin*tsin + k*k * tcos*tcos;
                    c                   = add_cascade();
                    double *t           = (type == FLT_BT_BWC_LADDERPASS) ? c->t : c->b;
                    double *b           = (type == FLT_BT_BWC_LADDERPASS) ? c->b : c->t;

                    // Transfer function
                    t[0]                = kf / fg;
                    t[1]                = 2.0 * k * tcos;
                    t[2]                = fg;

                    b[0]                = fg;
                    b[1]                = 2.0 * k * tcos;
                    b[2]                = kf / fg;

                    if (j == 0)
                    {
                        c->t[0]            *= gain;
                        c->t[1]            *= gain;
                        c->t[2]            *= gain;
                    }

                    // Second shelf cascade, always hi-shelf
                    kf                  = tsin*tsin + k1*k1 * tcos*tcos;
                    c                   = add_cascade();
                    t                   = c->b;
                    b                   = c->t;

                    // Transfer function
                    t[0]                = kf / fg1;
                    t[1]                = 2.0 * k1 * xf * tcos;
                    t[2]                = fg1 * xf * xf;

                    b[0]                = fg1;
                    b[1]                = 2.0 * k1 * xf * tcos;
                    b[2]                = kf * xf * xf / fg1;

                    if (j == 0)
                    {
                        c->t[0]            *= gain2;
                        c->t[1]            *= gain2;
                        c->t[2]            *= gain2;
                    }
                }

                break;
            }

            case FLT_BT_BWC_BELL:
            {
                double fg               = exp(log(fp->fGain)/double(2*fp->nSlope));
                float k                 = 1.0f / (1.0 + fp->fQuality);

                for (size_t j=0; j < fp->nSlope; ++j)
                {
                    double theta        = ((2*j + 1)*M_PI_2)/(2*fp->nSlope);
                    double tsin         = sin(theta);
                    double tcos         = sqrt(1.0 - tsin*tsin);
                    double kf           = tsin*tsin + k*k * tcos*tcos;

                    if (fp->fGain >= 1.0)
                    {
                        // First cascade
                        c                   = add_cascade();

                        c->t[0]             = 1.0;
                        c->t[1]             = 2.0 * k * tcos * fg / kf;
                        c->t[2]             = 1.0 * fg * fg / kf;

                        c->b[0]             = 1.0;
                        c->b[1]             = 2.0 * k * tcos / kf;
                        c->b[2]             = 1.0 / kf;

                        // Second cascade
                        c                   = add_cascade();

                        c->t[0]             = 1.0;
                        c->t[1]             = 2.0 * k * tcos / fg;
                        c->t[2]             = kf / (fg * fg);

                        c->b[0]             = 1.0;
                        c->b[1]             = 2.0 * k * tcos;
                        c->b[2]             = kf;
                    }
                    else
                    {
                        // First cascade
                        c                   = add_cascade();

                        c->t[0]             = 1.0;
                        c->t[1]             = 2.0 * k * tcos / kf;
                        c->t[2]             = 1.0 / kf;

                        c->b[0]             = 1.0;
                        c->b[1]             = 2.0 * k * tcos / (fg * kf);
                        c->b[2]             = 1.0 / (fg * fg * kf);

                        // Second cascade
                        c                   = add_cascade();

                        c->t[0]             = 1.0;
                        c->t[1]             = 2.0 * k * tcos;
                        c->t[2]             = kf;

                        c->b[0]             = 1.0;
                        c->b[1]             = 2.0 * k * tcos * fg;
                        c->b[2]             = kf * fg * fg;
                    }
                }

                break;
            }

            case FLT_BT_BWC_BANDPASS:
            {
                double f2               = fp->fFreq2;
                double k                = 1.0f / (1.0f + fp->fQuality);

                for (size_t j=0; j < fp->nSlope; ++j)
                {
                    double theta        = ((2*j + 1)*M_PI_2)/(2*fp->nSlope);
                    double tsin         = sin(theta);
                    double tcos         = sqrt(1.0 - tsin*tsin);
                    float kf            = tsin*tsin + k*k * tcos*tcos;

                    // Hi-pass cascade
                    c                   = add_cascade();

                    c->t[2]             = (j == 0) ? fp->fGain : 1.0;

                    c->b[0]             = 1.0 / kf;
                    c->b[1]             = 2.0 * k * tcos / kf;
                    c->b[2]             = 1.0;

                    // Lo-pass cascade
                    c                   = add_cascade();

                    c->t[0]             = 1.0;

                    c->b[0]             = 1.0;
                    c->b[1]             = 2.0 * k * tcos * f2 / kf;
                    c->b[2]             = f2 * f2 / kf;
                }

                break;
            }

            default:
                nMode           = FM_BYPASS;
                break;
        }
    }

    void Filter::calc_lrx_filter(size_t type, const filter_params_t *fp)
    {
        // LRX filter is just twice repeated BWC filter
        // Calculate the same chain twice
        switch (type)
        {
            case FLT_BT_LRX_LOPASS:
                type = FLT_BT_BWC_LOPASS;
                break;
            case FLT_BT_LRX_HIPASS:
                type = FLT_BT_BWC_HIPASS;
                break;
            case FLT_BT_LRX_LOSHELF:
                type = FLT_BT_BWC_LOSHELF;
                break;
            case FLT_BT_LRX_HISHELF:
                type = FLT_BT_BWC_HISHELF;
                break;
            case FLT_BT_LRX_BELL:
                type = FLT_BT_BWC_BELL;
                break;
            case FLT_BT_LRX_BANDPASS:
                type = FLT_BT_BWC_BANDPASS;
                break;
            case FLT_BT_LRX_LADDERPASS:
                type = FLT_BT_BWC_LADDERPASS;
                break;
            case FLT_BT_LRX_LADDERREJ:
                type = FLT_BT_BWC_LADDERREJ;
                break;
            default:
                nMode           = FM_BYPASS;
                return;
        }

        // Have to do some hacks with chain parameters: reduce slope and gain
        filter_params_t bfp = *fp;
        bfp.nSlope          = sParams.nSlope*2;
        bfp.fGain           = sqrtf(bfp.fGain);

        // Calculate two similar chains
        calc_bwc_filter(type, &bfp);
        calc_bwc_filter(type, &bfp);
    }

    /*
        Original filter chain:

                   t[0] + t[1] * p + t[2] * p^2
          H(p) =  ------------------------------
                   b[0] + b[1] * p + b[2] * p^2

        Bilinear transform:

          x    = z^-1

          kf   = 1 / tan(pi * frequency / sample_rate) - frequency shift factor

                       1 - x
          p    = kf * -------   - analog -> digital bilinear transform expression
                       1 + x

        Applied bilinear transform:

                   (t[0] + t[1]*kf + t[2]*kf^2) + 2*(t[0] - t[2]*kf^2)*x + (t[0] - t[1]*kf + t[2]*kf^2)*x^2
          H[x] =  -----------------------------------------------------------------------------------------
                   (b[0] + b[1]*kf + b[2]*kf^2) + 2*(b[0] - b[2]*kf^2)*x + (b[0] - b[1]*kf + b[2]*kf^2)*x^2

        Finally:

          T    =   { t[0], t[1]*kf, t[2]*kf*kf }
          B    =   { b[0], b[1]*kf, b[2]*kf*kf }

                   (T[0] + T[1] + T[2]) + 2*(T[0] - T[2])*z^-1 + (T[0] - T[1] + T[2])*z^-2
          H[z] =  -------------------------------------------------------------------------
                   (B[0] + B[1] + B[2]) + 2*(B[0] - B[2])*z^-1 + (B[0] - B[1] + B[2])*z^-2
     */

    void Filter::bilinear_transform()
    {
        double kf       = 1.0/tan(sParams.fFreq * M_PI / double(nSampleRate));
        double kf2      = kf * kf;
        double T[4], B[4], N;
        size_t chains   = 0;

        for (size_t i=0; i<nItems; ++i)
        {
            cascade_t *c    = &vItems[i];
            double *t       = c->t;
            double *b       = c->b;

            // Calculate top coefficients
            T[0]            = t[0];
            T[1]            = t[1]*kf;
            T[2]            = t[2]*kf2;

            // Calculate bottom coefficients
            B[0]            = b[0];
            B[1]            = b[1]*kf;
            B[2]            = b[2]*kf2;

            // Calculate the convolution
            N               = 1.0 / (B[0] + B[1] + B[2]);

            // Initialize filter parameters
            if ((++chains) > FILTER_CHAINS_MAX)
                break;
            biquad_x1_t *f  = pBank->add_chain();
            if (f == NULL)
                break;

            f->a[0]         = (T[0] + T[1] + T[2]) * N;
            f->a[1]         = f->a[0];
            f->a[2]         = 2.0 * (T[0] - T[2]) * N;
            f->a[3]         = (T[0] - T[1] + T[2]) * N;

            f->b[0]         = 2.0 * (B[2] - B[0]) * N; // Sign negated
            f->b[1]         = (B[1] - B[2] - B[0]) * N; // Sign negated
            f->b[2]         = 0.0f;
            f->b[3]         = 0.0f;
        }
    }

    /*
        Original filter chain:

                   t[0] + t[1] * p + t[2] * p^2     k1 * (p + a[0]) * (p + a[1])
          H(p) =  ------------------------------ = -----------------------------
                   b[0] + b[1] * p + b[2] * p^2     k2 * (p + b[0]) * (p + b[1])

          a[0], a[1], b[0], b[1] may not exist, so there are series of solutions

        Matched Z-transform:

          T    = discretization period

          x    = z^-1

          p + a = 1 - x*exp(-a * T)

          kf   = 1 / f, f = filter frequency

        After the Matched Z-transform the Frequency Chart of the filter has to be normalized!

    */
    void Filter::matched_transform()
    {
        double T[4], B[4], A[2], I[2];
        double f        = sParams.fFreq;
        double TD       = 2.0*M_PI / nSampleRate;
        size_t chains   = 0;

        // Iterate each cascade
        for (size_t i=0; i<nItems; ++i)
        {
            cascade_t *c        = &vItems[i];

            // Process each polynom (top, bottom) individually
            for (size_t i=0; i<2; ++i)
            {
                double *p    = (i) ? c->b : c->t;
                double *P    = (i) ? B : T;

                if (p[2] == 0.0) // Test polynom for second-order
                {
                    P[2]    = 0.0;
                    if (p[1] == 0.0) // Test polynom for first order
                    {
                        // Zero-order polynom
                        P[0]    = p[0];
                        P[1]    = 0.0;
                    }
                    else
                    {
                        // First-order polynom:
                        //   p(s) = p[0] + p[1]*(s/f)
                        //
                        // Transformed polynom:
                        //   P[z] = p[1]/f - p[1]/f * exp(-f*p[0]*T/p[1]) * z^-1
                        double k    = p[1]/f;
                        double R    = -p[0]/k;
                        P[0]        = k;
                        P[1]        = -k * exp(R*TD);
                    }
                }
                else
                {
                    // Second-order polynom:
                    //   p(s) = p[0] + p[1]*(s/f) + p[2]*(s/f)^2 = p[2]/f^2 * (p[0]*f^2/p[2] + p[1]*f/p[2]*s + s^2)
                    //
                    // Calculate the roots of the second-order polynom equation a*x^2 + b*x + c = 0
                    double k    = p[2];
                    double a    = 1.0/(f*f);
                    double b    = p[1]/(f*p[2]);
                    double c    = p[0]/p[2];
                    double D    = b*b - 4.0*a*c;

                    if (D >= 0)
                    {
                        // Has real roots R0 and R1
                        // Transformed form is:
                        //   P[z] = k*(1 - (exp(R0*T) + exp(R1*T))*z^-1 + exp((R0+R1)*T)*z^-2)
                        D           = sqrt(D);
                        double R0   = (-b - D)/(2.0*a);
                        double R1   = (-b + D)/(2.0*a);
                        P[0]        = k;
                        P[1]        = -k * (exp(R0*TD) + exp(R1*TD));
                        P[2]        = k * exp((R0+R1)*TD);
                    }
                    else
                    {
                        // Has complex roots R+j*K and R-j*K
                        // Transformed form is:
                        //   P[z] = k*(1 - 2*exp(R*T)*cos(K*T)*z^-1 + exp(2*R*T)*z^-2)
                        D           = sqrt(-D);
                        double R    = -b / (2.0*a);
                        double K    = D / (2.0*a);
                        P[0]        = k;
                        P[1]        = -2.0 * k * exp(R*TD) * cos(K*TD);
                        P[2]        = k * exp(2.0*R*TD);
                    }
                }

                // We have to calculate the norming factor of the digital filter
                // To do this, we should get the amplitude of the discrete transfer function
                // at the control frequency and the amplitude of the continuous transfer function
                // at the same frequency.
                // As control frequency we take the f/10 value
                // For the discrete transfer function it will be PI*0.2*f / SR
                // For the normalized continuous transfer function it will be always 0.1

                // Calculate the discrete transfer function part at specified frequency
                double w    = M_PI * 0.2 * sParams.fFreq / nSampleRate;
                double re   = P[0]*cos(2*w) + P[1]*cos(w) + P[2];
                double im   = P[0]*sin(2*w) + P[1]*sin(w);
                A[i]        = sqrt(re*re + im*im);

                // Calculate the continuous transfer function part at 1 Hz
                w           = 0.1;
                re          = p[0] - p[2]*w*w;
                im          = p[1]*w;
                I[i]        = sqrt(re*re + im*im);
            }

            // Now calculate the convolution for the new polynom:
            /*
                       T[0] + T[1]*z^-1 + T[2]*z^-2
              H(z) =  ------------------------------
                       B[0] + B[1]*z^-1 + B[2]*z^-2

             */
            double AN       = (A[1]*I[0]) / (A[0]*I[1]); // Normalizing factor for the amplitude to match the analog filter
            double N        = 1.0 / B[0];

            // Initialize filter parameters
            if ((++chains) > FILTER_CHAINS_MAX)
                break;
            biquad_x1_t *f  = pBank->add_chain();
            if (f == NULL)
                break;

            f->a[0]         = T[0] * N * AN;
            f->a[1]         = f->a[0];
            f->a[2]         = T[1] * N * AN;
            f->a[3]         = T[2] * N * AN;

            f->b[0]         = -B[1] * N; // Sign negated
            f->b[1]         = -B[2] * N; // Sign negated
            f->b[2]         = 0.0f;
            f->b[3]         = 0.0f;
        }
    }

    bool Filter::impulse_response(float *out, size_t length)
    {
        if (!(nFlags & FF_OWN_BANK))
            return false;

        if (nFlags & (~FF_OWN_BANK))
            rebuild();

        pBank->impulse_response(out, length);
        return true;
    }

}


