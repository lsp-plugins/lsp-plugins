/*
 * filter.cpp
 *
 *  Created on: 28 июня 2016 г.
 *      Author: sadko
 */

#include <math.h>
#include <core/dsp.h>
#include <core/Filter.h>

namespace lsp
{
    Filter::Filter()
    {
        nSampleRate         = 0;
        nMode               = FM_BYPASS;
        nLatency            = 0;

        sDirect.vItems      = NULL;
        sDirect.vData       = NULL;
    }

    Filter::~Filter()
    {
        destroy();
    }

    inline void Filter::poly_copy(filter_poly_t *dst, const filter_poly_t *src)
    {
        dst->nItems     = src->nItems;
        for (size_t i=0; i<src->nItems; ++i)
            dst->vItems[i]  = src->vItems[i];
    }

    inline void Filter::poly_create(filter_poly_t *dst, size_t count)
    {
        dst->nItems     = count;
        for (size_t i=0; i<count; ++i)
            dst->vItems[i]  = 0.0;
    }

    inline void Filter::poly_mul(filter_poly_t *dst, const double *src)
    {
        size_t items    = (src[2] != 0.0) ? 3 :
                          (src[1] != 0.0) ? 2 : 1;

        filter_poly_t   tmp;
        poly_copy(&tmp, dst);
        poly_create(dst, tmp.nItems + items - 1);

        for (size_t i=0; i < items; ++i)
        {
            float k = src[i];
            for (size_t j=0; j < tmp.nItems; ++j)
                dst->vItems[i+j] += k * tmp.vItems[j];
        }
    }

    bool Filter::init()
    {
        filter_params_t fp;

        fp.nType            = FLT_NONE;
        fp.fFreq            = 1000.0f;
        fp.fFreq2           = 1000.0f;
        fp.fGain            = 1.0f;
        fp.fQuality         = 0;
        fp.nSlope           = 1;

        if (sDirect.vData == NULL)
        {
            size_t cascade_size = ALIGN_SIZE(sizeof(cascade_t) * FILTER_CHAINS_MAX, DEFAULT_ALIGN);

            size_t allocate     = DEFAULT_ALIGN + cascade_size;
            sDirect.vData       = new uint8_t[allocate];
            if (sDirect.vData == NULL)
                return false;

            uint8_t *ptr        = ALIGN_PTR(sDirect.vData, DEFAULT_ALIGN);
            sDirect.vItems      = reinterpret_cast<cascade_t *>(ptr);
            ptr                += cascade_size;

            for (size_t i=0; i<FILTER_CHAINS_MAX; ++i)
                dsp::vec4_zero(sDirect.vItems[i].delay);
        }

        update(48000, &fp);
        return true;
    }

    void Filter::destroy()
    {
        if (sDirect.vData != NULL)
        {
            delete  [] sDirect.vData;
            sDirect.vItems  = NULL;
            sDirect.vData   = NULL;
        }
    }

    void Filter::update(size_t sr, const filter_params_t *params)
    {
        // Init direct filter chain
        filter_params_t *fp     = &sDirect.sParams;
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
            fp->nSlope              = 16;
        if (fp->fFreq < SPEC_FREQ_MIN)
            fp->fFreq               = SPEC_FREQ_MIN;
        else if (fp->fFreq > SPEC_FREQ_MAX)
            fp->fFreq               = SPEC_FREQ_MAX;
        if (fp->fFreq >= (0.49f * nSampleRate))
            fp->fFreq               = 0.49f * nSampleRate;

        // Build direct filter
        build_filter(&sDirect);

        // Clear chain buffers if type of filter changed
        if ((type != fp->nType) || (slope != fp->nSlope))
        {
            for (size_t i=0; i<FILTER_CHAINS_MAX; ++i)
                dsp::vec4_zero(sDirect.vItems[i].delay);
        }
    }

    void Filter::get_params(filter_params_t *params)
    {
        if (params != NULL)
            *params =    sDirect.sParams;
    }

    Filter::cascade_t *Filter::add_cascade(filter_chain_t *chain)
    {
        // Get cascade
        cascade_t *c = (chain->nItems >= FILTER_CHAINS_MAX) ?
            &chain->vItems[FILTER_CHAINS_MAX-1] :
            &chain->vItems[chain->nItems];

        // Increment number of chains
        if (chain->nItems < FILTER_CHAINS_MAX)
            chain->nItems++;

        // Initialize cascade
        for (size_t i=0; i<4; ++i)
        {
            c->t[i] = 0.0;
            c->b[i] = 0.0;
        }

        // Return cascade
        return c;
    }

    void Filter::build_transfer_function(filter_chain_t *chain)
    {
        // Estimate the size of top and bottom polynoms
        poly_create(&chain->sTop, 1);
        poly_create(&chain->sBottom, 1);
        chain->sTop.vItems[0]       = 1.0;
        chain->sBottom.vItems[0]    = 1.0;

        for (size_t i=0; i<chain->nItems; ++i)
        {
            poly_mul(&chain->sTop, chain->vItems[i].t);
            poly_mul(&chain->sBottom, chain->vItems[i].b);
        }
    }

    void Filter::build_filter(filter_chain_t *chain)
    {
        // Reset number of cascades
        chain->nItems           = 0;
        chain->sTop.nItems      = 0;
        chain->sBottom.nItems   = 0;

        // Calculate filter
        switch (chain->sParams.nType)
        {
            case FLT_BT_RLC_LOPASS:
            case FLT_BT_RLC_HIPASS:
            case FLT_BT_RLC_LOSHELF:
            case FLT_BT_RLC_HISHELF:
            case FLT_BT_RLC_BELL:
            case FLT_BT_RLC_RESONANCE:
            case FLT_BT_RLC_NOTCH:
            case FLT_BT_RLC_BANDPASS:
            {
                // Calculate filter parameters
                calc_rlc_filter(chain->sParams.nType, &chain->sParams, chain);
                build_transfer_function(chain);
                bilinear_transform(chain);
                break;
            }

            case FLT_MT_RLC_LOPASS:
            case FLT_MT_RLC_HIPASS:
            case FLT_MT_RLC_LOSHELF:
            case FLT_MT_RLC_HISHELF:
            case FLT_MT_RLC_BELL:
            case FLT_MT_RLC_RESONANCE:
            case FLT_MT_RLC_NOTCH:
            case FLT_MT_RLC_BANDPASS:
            {
                // Calculate filter parameters
                calc_rlc_filter(chain->sParams.nType - 1, &chain->sParams, chain);
                build_transfer_function(chain);
                matched_transform(chain);
                break;
            }

            case FLT_BT_BWC_LOPASS:
            case FLT_BT_BWC_HIPASS:
            case FLT_BT_BWC_LOSHELF:
            case FLT_BT_BWC_HISHELF:
            case FLT_BT_BWC_BELL:
            case FLT_BT_BWC_BANDPASS:
            {
                // Calculate filter parameters
                calc_bwc_filter(chain->sParams.nType, &chain->sParams, chain);
                build_transfer_function(chain);
                bilinear_transform(chain);
                break;
            }

            case FLT_MT_BWC_LOPASS:
            case FLT_MT_BWC_HIPASS:
            case FLT_MT_BWC_LOSHELF:
            case FLT_MT_BWC_HISHELF:
            case FLT_MT_BWC_BELL:
            case FLT_MT_BWC_BANDPASS:
            {
                // Calculate filter parameters
                calc_bwc_filter(chain->sParams.nType - 1, &chain->sParams, chain);
                build_transfer_function(chain);
                matched_transform(chain);
                break;
            }

            case FLT_BT_LRX_LOPASS:
            case FLT_BT_LRX_HIPASS:
            case FLT_BT_LRX_LOSHELF:
            case FLT_BT_LRX_HISHELF:
            case FLT_BT_LRX_BELL:
            case FLT_BT_LRX_BANDPASS:
            {
                // Calculate filter parameters
                calc_lrx_filter(chain->sParams.nType, chain);
                build_transfer_function(chain);
                bilinear_transform(chain);
                break;
            }

            case FLT_MT_LRX_LOPASS:
            case FLT_MT_LRX_HIPASS:
            case FLT_MT_LRX_LOSHELF:
            case FLT_MT_LRX_HISHELF:
            case FLT_MT_LRX_BELL:
            case FLT_MT_LRX_BANDPASS:
            {
                // Calculate filter parameters
                calc_lrx_filter(chain->sParams.nType - 1, chain);
                build_transfer_function(chain);
                matched_transform(chain);
                break;
            }

            case FLT_NONE:
            default:
                nMode           = FM_BYPASS;
                break;
        }
    }

    double Filter::complex_transfer_calc(double *p_re, double *p_im, double x, const filter_poly_t *fp)
    {
        // Initialize loop variables
        size_t count        = fp->nItems;
        const double *poly  = fp->vItems;

        double k            = (x != 0.0) ? exp(log(x) * (count >> 1)) : 1.0;
        double r_re         = 0.0, r_im = 0.0;
        double t_re         = 1.0 / k, t_im  = 1.0/k;
        double dx           = x * x; // dx == x^2

        // Calculate 4 elements of the polynom per each iteration
        while (count >= 4)
        {
            r_re   += t_re * *(poly++);
            t_re   *= dx;
            r_im   += t_im * *(poly++);
            t_im   *= dx;
            r_re   -= t_re * *(poly++);
            t_re   *= dx;
            r_im   -= t_im * *(poly++);
            t_im   *= dx;

            count  -= 4;
        }

        // complete the iteration
        while (count > 0)
        {
            r_re   += t_re * *(poly++);
            t_re   *= dx;
            if (!(--count))
                break;
            r_im   += t_im * *(poly++);
            t_im   *= dx;
            if (!(--count))
                break;
            r_re   -= t_re * *(poly++);
            t_re   *= dx;
            if (!(--count))
                break;
            r_im   -= t_im * *(poly++);
            t_im   *= dx;
            if (!(--count))
                break;
        }

        // Store the result
        *p_re       = r_re;
        *p_im       = r_im * x;
        return k;
    }

    void Filter::freq_chart(filter_chain_t *chain, float *re, float *im, const float *f, size_t count)
    {
        // Calculate frequency chart
        double t_re, t_im, b_re, b_im;

        switch (nMode)
        {
            case FM_BILINEAR:
            {
                double nf   = M_PI / double(nSampleRate);
                double kf   = 1.0/tan(chain->sParams.fFreq * nf);
                double lf   = nSampleRate * 0.499;

                while (count--)
                {
                    // Cyclic frequency
                    double w    = *(f++);
                    w           = tan((w > lf ? lf : w) * nf) * kf;

                    // Calculate top part of the transfer function Z[j*w]
                    double k    = complex_transfer_calc(&t_re, &t_im, w, &chain->sTop);

                    // Calculate bottom part of the transfer function P[j*w]
                    k          /= complex_transfer_calc(&b_re, &b_im, w, &chain->sBottom);

                    // Calculate the transfer function H[j*w] = Z[j*w]/P[j*w] in complex domain
                    w           =   k / (b_re * b_re + b_im * b_im);
                    *(re++)     =   (t_re * b_re + t_im * b_im) * w;
                    *(im++)     =   (t_im * b_re - t_re * b_im) * w;
                }
                break;
            }

            case FM_MATCHED:
            {
                double kf   = 1.0 / chain->sParams.fFreq;

                while (count--)
                {
                    // Cyclic frequency
                    double w    = *(f++) * kf;

                    // Calculate top part of the transfer function Z[j*w]
                    double k    = complex_transfer_calc(&t_re, &t_im, w, &chain->sTop);

                    // Calculate bottom part of the transfer function P[j*w]
                    k          /= complex_transfer_calc(&b_re, &b_im, w, &chain->sBottom);

                    // Calculate the transfer function H[j*w] = Z[j*w]/P[j*w] in complex domain
                    w           =   k / (b_re * b_re + b_im * b_im);
                    *(re++)     =   (t_re * b_re + t_im * b_im) * w;
                    *(im++)     =   (t_im * b_re - t_re * b_im) * w;
                }
                break;
            }

            default:
            case FM_BYPASS:
            {
                dsp::fill_one(re, count);
                dsp::fill_zero(im, count);
                return;
            }
        }
    }

    void Filter::process(float *out, const float *in, size_t samples)
    {
        switch (nMode)
        {
            case FM_BILINEAR:
            case FM_MATCHED:
            {
                const float *src = in;
                for (size_t i=0; i<sDirect.nItems; ++i)
                {
                    dsp::biquad_process_multi(out, src, samples, sDirect.vItems[i].delay, sDirect.vItems[i].ir);
                    src = out;   // actual data for the next chain is in output buffer now
                }
                break;
            }

            default:
            {
                dsp::copy(out, in, samples);
                break;
            }
        }
    }

    void Filter::calc_rlc_filter(size_t type, const filter_params_t *fp, filter_chain_t *chain)
    {
        cascade_t *c                = NULL;
        nMode                       = FM_BILINEAR;

        switch (type)
        {
            case FLT_BT_RLC_LOPASS:
            case FLT_BT_RLC_HIPASS:
            {
                // Add cascade with one pole
                size_t i        = fp->nSlope & 1;
                if (i)
                {
                    c           = add_cascade(chain);
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
                    c           = add_cascade(chain);
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
                    c                       = add_cascade(chain);
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

            case FLT_BT_RLC_BANDPASS:
            {
                double f2               = fp->fFreq2 / fp->fFreq;
                double k                = (1.0 + f2)/(1.0 + fp->fQuality);

                for (size_t j=0; j < fp->nSlope; j++)
                {
                    c                       = add_cascade(chain);
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
                    c                       = add_cascade(chain);

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
                    c                       = add_cascade(chain);

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
                c                       = add_cascade(chain);

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

            default:
                nMode           = FM_BYPASS;
                break;
        }
    }

    void Filter::calc_bwc_filter(size_t type, const filter_params_t *fp, filter_chain_t *chain)
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
                    c           = add_cascade(chain);
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
                    double tcos     = sqrtf(1.0 - tsin*tsin);
                    float kf        = tsin*tsin + k*k * tcos*tcos;

                    c               = add_cascade(chain);

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
                    double tcos         = sqrtf(1.0 - tsin*tsin);
                    double kf           = tsin*tsin + k*k * tcos*tcos;

                    c                   = add_cascade(chain);
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

            case FLT_BT_BWC_BELL:
            {
                double fg               = exp(log(fp->fGain)/double(2*fp->nSlope));
                float k                 = 1.0f / (1.0 + fp->fQuality);

                for (size_t j=0; j < fp->nSlope; ++j)
                {
                    double theta        = ((2*j + 1)*M_PI_2)/(2*fp->nSlope);
                    double tsin         = sin(theta);
                    double tcos         = sqrtf(1.0 - tsin*tsin);
                    double kf           = tsin*tsin + k*k * tcos*tcos;

                    if (fp->fGain >= 1.0)
                    {
                        // First cascade
                        c                   = add_cascade(chain);

                        c->t[0]             = 1.0;
                        c->t[1]             = 2.0 * k * tcos * fg / kf;
                        c->t[2]             = 1.0 * fg * fg / kf;

                        c->b[0]             = 1.0;
                        c->b[1]             = 2.0 * k * tcos / kf;
                        c->b[2]             = 1.0 / kf;

                        // Second cascade
                        c                   = add_cascade(chain);

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
                        c                   = add_cascade(chain);

                        c->t[0]             = 1.0;
                        c->t[1]             = 2.0 * k * tcos / kf;
                        c->t[2]             = 1.0 / kf;

                        c->b[0]             = 1.0;
                        c->b[1]             = 2.0 * k * tcos / (fg * kf);
                        c->b[2]             = 1.0 / (fg * fg * kf);

                        // Second cascade
                        c                   = add_cascade(chain);

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
                double f2               = fp->fFreq / fp->fFreq2;
                double k                = 1.0f / (1.0f + fp->fQuality);

                for (size_t j=0; j < fp->nSlope; ++j)
                {
                    double theta        = ((2*j + 1)*M_PI_2)/(2*fp->nSlope);
                    double tsin         = sin(theta);
                    double tcos         = sqrtf(1.0 - tsin*tsin);
                    float kf            = tsin*tsin + k*k * tcos*tcos;

                    // Hi-pass cascade
                    c                   = add_cascade(chain);

                    c->t[2]             = (j == 0) ? fp->fGain : 1.0;

                    c->b[0]             = 1.0 / kf;
                    c->b[1]             = 2.0 * k * tcos / kf;
                    c->b[2]             = 1.0;

                    // Lo-pass cascade
                    c                   = add_cascade(chain);

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

    void Filter::calc_lrx_filter(size_t type, filter_chain_t *chain)
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
            default:
                nMode           = FM_BYPASS;
                return;
        }

        // Have to do some hacks with chain parameters: reduce slope and gain
        filter_params_t fp  = chain->sParams;
        fp.nSlope           = chain->sParams.nSlope*2;
//        fp.fQuality         = expf(logf(chain->sParams.fQuality) / float(fp.nSlope));
        fp.fGain            = sqrtf(fp.fGain);

        // Calculate two similar chains
        calc_bwc_filter(type, &fp, chain);
        calc_bwc_filter(type, &fp, chain);
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

    void Filter::bilinear_transform(filter_chain_t *chain)
    {
        double kf   = 1.0/tan(chain->sParams.fFreq * M_PI / double(nSampleRate));
        double kf2  = kf * kf;
        double T[4], B[4], N;

        for (size_t i=0; i<chain->nItems; ++i)
        {
            cascade_t *c    = &chain->vItems[i];
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
            c->ir[0]        = 2.0 * (B[2] - B[0]) * N; // Sign negated
            c->ir[1]        = 2.0 * (T[0] - T[2]) * N;
            c->ir[2]        = (B[1] - B[2] - B[0]) * N; // Sign negated
            c->ir[3]        = (T[0] - T[1] + T[2]) * N;
            c->ir[4]        = (T[0] + T[1] + T[2]) * N;
            c->ir[5]        = 0.0;
            c->ir[6]        = 0.0;
            c->ir[7]        = 0.0;
        }

        nMode                       = FM_BILINEAR;
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
    void Filter::matched_transform(filter_chain_t *chain)
    {
        double T[4], B[4], A[2], I[2];
        double f    = chain->sParams.fFreq;
        double TD   = 2.0*M_PI / nSampleRate;

        // Iterate each cascade
        for (size_t i=0; i<chain->nItems; ++i)
        {
            cascade_t *c        = &chain->vItems[i];

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
                double w    = M_PI * 0.2 * chain->sParams.fFreq / nSampleRate;
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
            c->ir[0]        = -B[1] * N; // Sign negated
            c->ir[1]        = T[1] * N * AN;
            c->ir[2]        = -B[2] * N; // Sign negated
            c->ir[3]        = T[2] * N * AN;
            c->ir[4]        = T[0] * N * AN;
            c->ir[5]        = 0.0;
            c->ir[6]        = 0.0;
            c->ir[7]        = 0.0;
        }

        nMode                       = FM_MATCHED;
    }

    void Filter::impulse_response(float *out, size_t length)
    {
        float delays[FILTER_CHAINS_MAX * 4];
        float *p = delays;

        // Backup and clean the delay chains for FIR filters
        for (size_t i=0; i<FILTER_CHAINS_MAX; ++i)
            for (size_t j=0; j<4; ++j)
            {
                *(p++)                      = sDirect.vItems[i].delay[j];
                sDirect.vItems[i].delay[j]  = 0.0f;
            }

        // Prepare the single impulse response
        dsp::fill_zero(out, length);
        out[0]  = 1.0f;

        // Call for processing
        process(out, out, length);

        // Restore delay chains for FIR filters
        p = delays;
        for (size_t i=0; i<FILTER_CHAINS_MAX; ++i)
            for (size_t j=0; j<4; ++j)
                sDirect.vItems[i].delay[j]  = *(p++);
    }

}


