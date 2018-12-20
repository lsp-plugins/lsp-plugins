/*
 * DynamicFilters.cpp
 *
 *  Created on: 1 февр. 2018 г.
 *      Author: sadko
 */

#include <core/filters/DynamicFilters.h>
#include <math.h>

#define BLD_BUF_SIZE    8
#define BUF_SIZE        0x400       /* 1024 samples at one time */

namespace lsp
{
    // Normal analog filter that does not affect any changes to the signal
    const f_cascade_t DynamicFilters::sNormal =
    {
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f, 0.0f }
    };

    DynamicFilters::DynamicFilters()
    {
        vFilters        = NULL;
        vMemory         = NULL;
        vCascades       = NULL;
        vBiquads.ptr    = NULL;
        nFilters        = 0;
        nSampleRate     = 0;
        pData           = NULL;
        bClearMem       = false;
    }

    DynamicFilters::~DynamicFilters()
    {
        destroy();
    }

    status_t DynamicFilters::init(size_t filters)
    {
        // Determine how many bytes to allocate
        size_t b_per_filter_t       = ALIGN_SIZE(sizeof(filter_t) * filters, ALIGN64);
        size_t b_per_memory         = FILTER_CHAINS_MAX * 2 * filters * sizeof(float);
        size_t b_per_cascades       = ALIGN_SIZE(8 * (BUF_SIZE + 8) * sizeof(f_cascade_t), ALIGN64);
        size_t b_per_biquad         = sizeof(biquad_x8_t) * (BUF_SIZE + 8);

        size_t to_alloc             = b_per_filter_t + b_per_memory + b_per_cascades + b_per_biquad;

        // Allocate memory
        uint8_t *ptr                = alloc_aligned<uint8_t>(pData, to_alloc, ALIGN64);
        if (ptr == NULL)
            return STATUS_NO_MEM;

        // Map memory
        vFilters        = reinterpret_cast<filter_t *>(ptr);
        ptr            += b_per_filter_t;
        vMemory         = reinterpret_cast<float *>(ptr);
        ptr            += b_per_memory;
        vCascades       = reinterpret_cast<f_cascade_t *>(ptr);
        ptr            += b_per_cascades;
        vBiquads.ptr    = ptr;
        nFilters        = filters;

        // Initialize all filters with default values
        for (size_t i=0; i<filters; ++i)
        {
            filter_t *f         = &vFilters[i];
            filter_params_t *fp = &f->sParams;

            fp->nType       = FLT_NONE;
            fp->fFreq       = 0.0f;
            fp->fFreq2      = 0.0f;
            fp->fGain       = 0.0f;
            fp->nSlope      = 0;
            fp->fQuality    = 0.0f;
            f->bActive      = false;
        }

        // Cleanup filter memory
        dsp::fill_zero(vMemory, FILTER_CHAINS_MAX * 2 * filters);

        return STATUS_OK;
    }

    void DynamicFilters::set_sample_rate(size_t sr)
    {
        nSampleRate         = sr;
    }

    void DynamicFilters::destroy()
    {
        if (pData != NULL)
            free_aligned(pData);

        vFilters        = NULL;
        vCascades       = NULL;
        vMemory         = NULL;
        vBiquads.ptr    = NULL;
        nFilters        = 0;
        nSampleRate     = 0;
    }

    bool DynamicFilters::set_params(size_t id, const filter_params_t *params)
    {
        if (id >= nFilters)
            return false;
        filter_params_t *fp = &vFilters[id].sParams;
        if (fp->nType != params->nType)
            bClearMem           = true;

        *fp     = *params;

        // Swap frequencies if f2 < f for band-filters
        switch (fp->nType)
        {
            case FLT_BT_RLC_LADDERPASS:
            case FLT_MT_RLC_LADDERPASS:
            case FLT_BT_RLC_LADDERREJ:
            case FLT_MT_RLC_LADDERREJ:
            case FLT_BT_RLC_BANDPASS:
            case FLT_MT_RLC_BANDPASS:
            case FLT_BT_BWC_LADDERPASS:
            case FLT_MT_BWC_LADDERPASS:
            case FLT_BT_BWC_LADDERREJ:
            case FLT_MT_BWC_LADDERREJ:
            case FLT_BT_BWC_BANDPASS:
            case FLT_MT_BWC_BANDPASS:
            case FLT_BT_LRX_LADDERPASS:
            case FLT_MT_LRX_LADDERPASS:
            case FLT_BT_LRX_LADDERREJ:
            case FLT_MT_LRX_LADDERREJ:
            case FLT_BT_LRX_BANDPASS:
            case FLT_MT_LRX_BANDPASS:
                if (fp->fFreq2 < fp->fFreq)
                {
                    float f     = fp->fFreq;
                    fp->fFreq   = fp->fFreq2;
                    fp->fFreq2  = f;
                }
                break;
            default:
                break;
        }

        // Tranform second frequency
        if (fp->nType & 1) // Bilinear transform
        {
            double nf   = M_PI / double(nSampleRate);
            fp->fFreq2  = tan(fp->fFreq * nf) / tan(fp->fFreq2 * nf);
        }
        else // Matched transform
        {
            fp->fFreq2  = fp->fFreq / fp->fFreq2;    // Normalize frequency
        }

        return true;
    }

    bool DynamicFilters::get_params(size_t id, filter_params_t *params)
    {
        if (id >= nFilters)
            return false;
        *params = vFilters[id].sParams;
        return true;
    }

    size_t DynamicFilters::quantify(size_t c, size_t nc)
    {
        // Check if there are some cascades left undone
        ssize_t n = nc - c;
        if (n <= 0)
            return 0;

        if (n >= 4)
            return (n >= 8) ? 8 : 4;

        return (n >= 2) ? 2 : 1;
    }

    void DynamicFilters::process(size_t id, float *out, const float *in, const float *gain, size_t samples)
    {
        // Bypass inactive or non-existing filter
        filter_t *f     = (id < nFilters) ? &vFilters[id] : NULL;
        if ((f == NULL)  || (!f->bActive) || (f->sParams.nType == FLT_NONE) || (f->sParams.nSlope == 0) || (nSampleRate == 0))
        {
            dsp::copy(out, in, samples);
            return;
        }

        // Cleanup filter memory
        if (bClearMem)
        {
            dsp::fill_zero(vMemory, FILTER_CHAINS_MAX * 2 * nFilters);
            bClearMem = false;
        }

        // Frequency coefficient for bilinear transform
        float kf =
                (f->sParams.nType <= FLT_MT_AMPLIFIER) ? 0.95f :
                (f->sParams.nType & 1) ?
                1.0/tan(f->sParams.fFreq * M_PI / double(nSampleRate)) : // bilinear transform coefficient
                2.0*M_PI / nSampleRate; // Matched transfomr coefficient

        // Filter memory
        while (samples > 0)
        {
            // Initialize counter
            size_t to_process       = (samples > BUF_SIZE) ? BUF_SIZE : samples;
            float *fmem             = &vMemory[id * FILTER_CHAINS_MAX * 2];
            const float *src        = in;
            size_t cj               = 0;

            // Process all cascades
            while (true)
            {
                // Generate cascades
                size_t nj               = build_filter_bank(vCascades, &f->sParams, cj, gain, to_process);
                if (nj <= 0)
                    break;

                if (nj == 8)
                {
                    f_cascade_t *h  = vCascades, *t = &vCascades[to_process << 3];
                    h[1] = sNormal; h[2] = sNormal; h[3] = sNormal; h[4] = sNormal; h[5] = sNormal; h[6] = sNormal; h[7] = sNormal; // row 0
                    h[10] = sNormal; h[11] = sNormal; h[12] = sNormal; h[13] = sNormal; h[14] = sNormal; h[15] = sNormal; // row 1
                    h[19] = sNormal; h[20] = sNormal; h[21] = sNormal; h[22] = sNormal; h[23] = sNormal; // row 2
                    h[28] = sNormal; h[29] = sNormal; h[30] = sNormal; h[31] = sNormal; // row 3
                    h[37] = sNormal; h[38] = sNormal; h[39] = sNormal; // row 4
                    h[46] = sNormal; h[47] = sNormal; // row 5
                    h[55] = sNormal; // row 6

                    t[0] = sNormal; // row -7
                    t[8] = sNormal; t[9] = sNormal; // row -6
                    t[16] = sNormal; t[17] = sNormal; t[18] = sNormal; // row -5
                    t[24] = sNormal; t[25] = sNormal; t[26] = sNormal; t[27] = sNormal; // row -4
                    t[32] = sNormal; t[33] = sNormal; t[34] = sNormal; t[35] = sNormal; t[36] = sNormal; // row -3
                    t[40] = sNormal; t[41] = sNormal; t[42] = sNormal; t[43] = sNormal; t[44] = sNormal; t[45] = sNormal; // row -2
                    t[48] = sNormal; t[49] = sNormal; t[50] = sNormal; t[51] = sNormal; t[52] = sNormal; t[53] = sNormal; t[54] = sNormal; // row -1

                    if (f->sParams.nType & 1)
                        dsp::bilinear_transform_x8(vBiquads.x8, vCascades, kf, to_process + 7);
                    else
                        dsp::matched_transform_x8(vBiquads.x8, vCascades, f->sParams.fFreq, kf, to_process + 7);
                    dsp::dyn_biquad_process_x8(out, src, fmem, to_process, vBiquads.x8);
                }
                else if (nj == 4)
                {
                    f_cascade_t *h  = vCascades, *t = &vCascades[to_process << 2];
                    h[1] = sNormal; h[2] = sNormal; h[3] = sNormal; // row 0
                    h[6] = sNormal; h[7] = sNormal; // row 1
                    h[11] = sNormal; // row 2

                    t[0] = sNormal; // row -3
                    t[4] = sNormal; t[5] = sNormal; // row -2
                    t[8] = sNormal; t[9] = sNormal; t[10] = sNormal; // row -1

                    if (f->sParams.nType & 1)
                        dsp::bilinear_transform_x4(vBiquads.x4, vCascades, kf, to_process + 3);
                    else
                        dsp::matched_transform_x4(vBiquads.x4, vCascades, f->sParams.fFreq, kf, to_process + 3);
                    dsp::dyn_biquad_process_x4(out, src, fmem, to_process, vBiquads.x4);
                }
                else if (nj == 2)
                {
                    vCascades[1]                = sNormal;
                    vCascades[to_process << 1]  = sNormal;
                    if (f->sParams.nType & 1)
                        dsp::bilinear_transform_x2(vBiquads.x2, vCascades, kf, to_process + 1);
                    else
                        dsp::matched_transform_x2(vBiquads.x2, vCascades, f->sParams.fFreq, kf, to_process + 1);
                    dsp::dyn_biquad_process_x2(out, src, fmem, to_process, vBiquads.x2);
                }
                else if (nj == 1)
                {
                    if (f->sParams.nType & 1)
                        dsp::bilinear_transform_x1(vBiquads.x1, vCascades, kf, to_process);
                    else
                        dsp::matched_transform_x1(vBiquads.x1, vCascades, f->sParams.fFreq, kf, to_process);
                    dsp::dyn_biquad_process_x1(out, src, fmem, to_process, vBiquads.x1);
                }

                // Update counters and pointers
                cj                     += nj;
                fmem                   += nj*2;
                src                     = out;
            }

            // Update samples and pointers
            samples                -= to_process;
            gain                   += to_process;
            out                    += to_process;
            in                     += to_process;
        }
    }

    size_t DynamicFilters::precalc_lrx_ladder_filter_bank(f_cascade_t *dst, const filter_params_t *fp, size_t cj, const float *sfg, size_t samples)
    {
        size_t slope            = fp->nSlope * 4;
        size_t nc               = quantify(cj, slope); // Number of cascades to generate
        if (nc <= 0)
            return nc;

        // Initialize buffer
        f_cascade_t buf[BLD_BUF_SIZE];
        dsp::fill_zero(buf[0].t, (sizeof(f_cascade_t)*BLD_BUF_SIZE)/sizeof(float));
        size_t bptr = 0;

        // Pre-calculate some values
        f_cascade_t *c      = dst;
        for (size_t i=0; i<samples; ++i)
        {
            f_cascade_t *xc = &buf[bptr];

            xc->t[0]        = sqrtf(sfg[i]); // gain
            xc->t[1]        = 1.0f / xc->t[0]; // 1/gain
            xc->t[2]        = dsp::irootf(xc->t[0], slope); // fg
            xc->t[3]        = 1.0f / xc->t[2];  // 1 / fg
            xc->b[0]        = 1.0f / (1.0f + fp->fQuality * (1.0f - expf(2.0f - xc->t[0] - xc->t[1]))); // k

            // Copy values
            for (size_t j=0; j<nc; ++j)
                *(c++)          = buf[(bptr + j) & (BLD_BUF_SIZE-1)];

            bptr        = (bptr + BLD_BUF_SIZE-1) & (BLD_BUF_SIZE-1);
        }
        for (size_t i=nc; i>0; --i) // Complete tail
        {
            // Copy values
            for (size_t j=0; j<nc; ++j)
                *(c++)          = buf[(bptr + j) & (BLD_BUF_SIZE-1)];
            bptr        = (bptr + BLD_BUF_SIZE-1) & (BLD_BUF_SIZE-1);
        }

        return nc;
    }

    typedef struct precalc_t
    {
        float theta;
        float tcos;
        float tcos2;
        float tsin2;
        float xtcos;
        float xtcos_xf;
    } precalc_t;

    void DynamicFilters::calc_lrx_ladder_filter_bank(f_cascade_t *dst, const filter_params_t *fp, size_t cj, size_t samples, size_t ftype, size_t nc)
    {
        size_t slope            = fp->nSlope * 4;

#if 1
        f_cascade_t *c          = dst;


        precalc_t pc[BLD_BUF_SIZE];
        precalc_t *ppc;

        float gain, k, kf;
        float xf            = fp->fFreq2;
        float xf2           = xf * xf;

        for (size_t j=0; j<nc; ++j)
        {
            ppc             = &pc[j];
            ppc->theta      = ((((cj+j)&(~3)) + 2)*M_PI_2) / float(slope);
            ppc->tcos       = cosf(ppc->theta);
            ppc->tcos2      = ppc->tcos * ppc->tcos;
            ppc->tsin2      = 1.0f - ppc->tcos2;
            ppc->xtcos      = 2.0f * ppc->tcos;
            ppc->xtcos_xf   = 2.0f * ppc->tcos * xf;
        }

        size_t loops = samples + nc - 1;

        if (ftype == FLT_BT_LRX_LADDERPASS)
        {
            for (size_t i=0; i<loops; ++i)
            {
                ppc = pc;

                for (size_t j=0; j<nc; ++j)
                {
                    k                   = c->b[0];
                    kf                  = ppc->tsin2 + k*k * ppc->tcos2;

                    if ((cj + j) & 1)
                    {
                        gain                = c->t[1];

                        // Transfer function
                        c->b[0]             = kf * c->t[3];
                        c->b[1]             = k * ppc->xtcos_xf;
                        c->b[2]             = c->t[2] * xf2;

                        c->t[0]             = c->t[2];
                        c->t[1]             = c->b[1];
                        c->t[2]             = c->b[0] * xf2;
                    }
                    else
                    {
                        gain                = c->t[0];

                        // Transfer function
                        c->t[0]             = kf * c->t[3];
                        c->t[1]             = k * ppc->xtcos;

                        c->b[0]             = c->t[2];
                        c->b[1]             = c->t[1];
                        c->b[2]             = c->t[0];
                    }

                    if (!((cj+j)&(~1)))
                    {
                        c->t[0]            *= gain;
                        c->t[1]            *= gain;
                        c->t[2]            *= gain;
                    }

                    // Move to next cascade
                    c       ++;
                    ppc     ++;
                } // for j
            } // for i
        }
#else

        f_cascade_t *c;

        // Create cascades
        for (size_t j=0; j<nc; ++j)
        {
            // Intialize pointers
            c                   = &dst[(nc+1)*j];       // target cascade pointer

            // Pre-calculate filter parameters
            float theta         = (((cj&(~3)) + 2)*M_PI_2) / float(slope);
            float tcos          = cosf(theta);
            float tcos2         = tcos * tcos;      // cos^2
            float tsin2         = 1.0f - tcos2;     // sin^2
            float xf            = fp->fFreq2;

            float gain, k, kf;

            if (cj & 1) // Second shelf cascade, always hi-shelf
            {
                float xtcos         = 2.0f * tcos * xf;
                float xf2           = xf * xf;

                // Generate cascades
                if (ftype == FLT_BT_LRX_LADDERPASS)
                {
                    for (size_t i=0; i<samples; ++i)
                    {
                        gain                = c->t[1];
                        k                   = c->b[0];
                        kf                  = tsin2 + k*k * tcos2;

                        // Transfer function
                        c->b[0]             = kf * c->t[3];
                        c->b[1]             = k * xtcos;
                        c->b[2]             = c->t[2] * xf2;

                        c->t[0]             = c->t[2];
                        c->t[1]             = c->b[1];
                        c->t[2]             = c->b[0] * xf2;

                        if (!(cj&(~1)))
                        {
                            c->t[0]            *= gain;
                            c->t[1]            *= gain;
                            c->t[2]            *= gain;
                        }

                        // Move to next cascade
                        c                  += nc;
                    }
                }
                else
                {
                    for (size_t i=0; i<samples; ++i)
                    {
                        gain                = c->t[0];
                        k                   = c->b[0];
                        kf                  = tsin2 + k*k * tcos2;

                        // Transfer function
                        c->b[0]             = kf * c->t[2];
                        c->b[1]             = k * xtcos;
                        c->b[2]             = c->t[3] * xf2;

                        c->t[0]             = c->t[3]; // fg;
                        c->t[1]             = c->b[1];
                        c->t[2]             = c->b[0] * xf2;

                        if (!(cj&(~1)))
                        {
                            c->t[0]            *= gain;
                            c->t[1]            *= gain;
                            c->t[2]            *= gain;
                        }

                        // Move to next cascade
                        c                  += nc;
                    }
                }
            }
            else // First shelf cascade, lo-shelf for LADDERREJ, hi-shelf for LADDERPASS
            {
                float xtcos         = 2.0f * tcos;

                // Generate cascades
                if (ftype == FLT_BT_LRX_LADDERPASS)
                {
                    for (size_t i=0; i<samples; ++i)
                    {
                        gain                = c->t[0];
                        k                   = c->b[0];
                        kf                  = tsin2 + k*k * tcos2;

                        // Transfer function
                        c->t[0]             = kf * c->t[3];
                        c->t[1]             = k * xtcos;

                        c->b[0]             = c->t[2];
                        c->b[1]             = c->t[1];
                        c->b[2]             = c->t[0];

                        if (!(cj&(~1)))
                        {
                            c->t[0]            *= gain;
                            c->t[1]            *= gain;
                            c->t[2]            *= gain;
                        }

                        // Move to next cascade
                        c                  += nc;
                    }
                }
                else
                {
                    // Generate cascades
                    for (size_t i=0; i<samples; ++i)
                    {
                        gain                = c->t[0];
                        k                   = c->b[0];
                        kf                  = tsin2 + k*k * tcos2;

                        // Transfer function
                        c->b[0]             = kf * c->t[3];
                        c->b[1]             = k * xtcos;
                        c->b[2]             = c->t[2];

                        c->t[0]             = c->b[2];
                        c->t[1]             = c->b[1];
                        c->t[2]             = c->b[0];

                        if (!(cj&(~1)))
                        {
                            c->t[0]            *= gain;
                            c->t[1]            *= gain;
                            c->t[2]            *= gain;
                        }

                        // Move to next cascade
                        c                  += nc;
                    }
                }
            }

            // Increment cascade number
            cj++;
        }
#endif
    }

    size_t DynamicFilters::build_lrx_ladder_filter_bank(f_cascade_t *dst, const filter_params_t *fp, size_t cj, const float *sfg, size_t samples, size_t ftype)
    {
        size_t nc = precalc_lrx_ladder_filter_bank(dst, fp, cj, sfg, samples);
        if (nc <= 0)
            return nc;

        calc_lrx_ladder_filter_bank(dst, fp, cj, samples, ftype, nc);
        return nc;


//        size_t slope            = fp->nSlope * 4;
//        size_t nc               = quantify(cj, slope); // Number of cascades to generate
//        if (nc <= 0)
//            return nc;
//
//        // Initialize buffer
//        f_cascade_t buf[BLD_BUF_SIZE];
//        dsp::fill_zero(buf[0].t, (sizeof(f_cascade_t)*BLD_BUF_SIZE)/sizeof(float));
//        size_t bptr = 0;
//
//        // Pre-calculate some values
//        f_cascade_t *c      = dst;
//        for (size_t i=0; i<samples; ++i)
//        {
//            f_cascade_t *xc = &buf[bptr];
//
//            xc->t[0]        = sqrtf(sfg[i]); // gain
//            xc->t[1]        = 1.0f / xc->t[0]; // 1/gain
//            xc->t[2]        = dsp::irootf(xc->t[0], slope); // fg
//            xc->t[3]        = 1.0f / xc->t[2];  // 1 / fg
//            xc->b[0]        = 1.0f / (1.0f + fp->fQuality * (1.0f - expf(2.0f - xc->t[0] - xc->t[1]))); // k
//
//            // Copy values
//            for (size_t j=0; j<nc; ++j)
//                *(c++)          = buf[(bptr + j) & (BLD_BUF_SIZE-1)];
//
//            bptr        = (bptr + BLD_BUF_SIZE-1) & (BLD_BUF_SIZE-1);
//        }
//        for (size_t i=nc; i>0; --i) // Complete tail
//        {
//            // Copy values
//            for (size_t j=0; j<nc; ++j)
//                *(c++)          = buf[(bptr + j) & (BLD_BUF_SIZE-1)];
//            bptr        = (bptr + BLD_BUF_SIZE-1) & (BLD_BUF_SIZE-1);
//        }
//
//        // Create cascades
//        for (size_t j=0; j<nc; ++j)
//        {
//            // Intialize pointers
//            c                   = &dst[(nc+1)*j];       // target cascade pointer
//
//            // Pre-calculate filter parameters
//            float theta         = (((cj&(~3)) + 2)*M_PI_2) / float(slope);
//            float tcos          = cosf(theta);
//            float tcos2         = tcos * tcos;      // cos^2
//            float tsin2         = 1.0f - tcos2;     // sin^2
//            float xf            = fp->fFreq2;
//
//            float gain, k, kf;
//
//            if (cj & 1) // Second shelf cascade, always hi-shelf
//            {
//                float xtcos         = 2.0f * tcos * xf;
//                float xf2           = xf * xf;
//
//                // Generate cascades
//                if (ftype == FLT_BT_LRX_LADDERPASS)
//                {
//                    for (size_t i=0; i<samples; ++i)
//                    {
//                        gain                = c->t[1];
//                        k                   = c->b[0];
//                        kf                  = tsin2 + k*k * tcos2;
//
//                        // Transfer function
//                        c->b[0]             = kf * c->t[3];
//                        c->b[1]             = k * xtcos;
//                        c->b[2]             = c->t[2] * xf2;
//
//                        c->t[0]             = c->t[2];
//                        c->t[1]             = c->b[1];
//                        c->t[2]             = c->b[0] * xf2;
//
//                        if (!(cj&(~1)))
//                        {
//                            c->t[0]            *= gain;
//                            c->t[1]            *= gain;
//                            c->t[2]            *= gain;
//                        }
//
//                        // Move to next cascade
//                        c                  += nc;
//                    }
//                }
//                else
//                {
//                    for (size_t i=0; i<samples; ++i)
//                    {
//                        gain                = c->t[0];
//                        k                   = c->b[0];
//                        kf                  = tsin2 + k*k * tcos2;
//
//                        // Transfer function
//                        c->b[0]             = kf * c->t[2];
//                        c->b[1]             = k * xtcos;
//                        c->b[2]             = c->t[3] * xf2;
//
//                        c->t[0]             = c->t[3]; // fg;
//                        c->t[1]             = c->b[1];
//                        c->t[2]             = c->b[0] * xf2;
//
//                        if (!(cj&(~1)))
//                        {
//                            c->t[0]            *= gain;
//                            c->t[1]            *= gain;
//                            c->t[2]            *= gain;
//                        }
//
//                        // Move to next cascade
//                        c                  += nc;
//                    }
//                }
//            }
//            else // First shelf cascade, lo-shelf for LADDERREJ, hi-shelf for LADDERPASS
//            {
//                float xtcos         = 2.0f * tcos;
//
//                // Generate cascades
//                if (ftype == FLT_BT_LRX_LADDERPASS)
//                {
//                    for (size_t i=0; i<samples; ++i)
//                    {
//                        gain                = c->t[0];
//                        k                   = c->b[0];
//                        kf                  = tsin2 + k*k * tcos2;
//
//                        // Transfer function
//                        c->t[0]             = kf * c->t[3];
//                        c->t[1]             = k * xtcos;
//
//                        c->b[0]             = c->t[2];
//                        c->b[1]             = c->t[1];
//                        c->b[2]             = c->t[0];
//
//                        if (!(cj&(~1)))
//                        {
//                            c->t[0]            *= gain;
//                            c->t[1]            *= gain;
//                            c->t[2]            *= gain;
//                        }
//
//                        // Move to next cascade
//                        c                  += nc;
//                    }
//                }
//                else
//                {
//                    // Generate cascades
//                    for (size_t i=0; i<samples; ++i)
//                    {
//                        gain                = c->t[0];
//                        k                   = c->b[0];
//                        kf                  = tsin2 + k*k * tcos2;
//
//                        // Transfer function
//                        c->b[0]             = kf * c->t[3];
//                        c->b[1]             = k * xtcos;
//                        c->b[2]             = c->t[2];
//
//                        c->t[0]             = c->b[2];
//                        c->t[1]             = c->b[1];
//                        c->t[2]             = c->b[0];
//
//                        if (!(cj&(~1)))
//                        {
//                            c->t[0]            *= gain;
//                            c->t[1]            *= gain;
//                            c->t[2]            *= gain;
//                        }
//
//                        // Move to next cascade
//                        c                  += nc;
//                    }
//                }
//            }
//
//            // Increment cascade number
//            cj++;
//        }
//
//        return nc;
    }

    size_t DynamicFilters::build_lrx_shelf_filter_bank(f_cascade_t *dst, const filter_params_t *fp, size_t cj, const float *sfg, size_t samples, size_t ftype)
    {
        size_t nc               = quantify(cj, fp->nSlope*2); // Number of cascades to generate (doubled for LRX filters)
        if (nc <= 0)
            return nc;

        // Initialize buffer
        f_cascade_t buf[BLD_BUF_SIZE];
        dsp::fill_zero(buf[0].t, (sizeof(f_cascade_t)*BLD_BUF_SIZE)/sizeof(float));
        size_t bptr = 0;

        // Pre-calculate some values
        f_cascade_t *c      = dst;
        for (size_t i=0; i<samples; ++i)
        {
            f_cascade_t *xc = &buf[bptr];

            xc->b[3]        = sqrtf(sfg[i]);
            xc->t[0]        = sqrtf(xc->b[3]); // gain
            xc->t[1]        = 1.0f / xc->t[0]; // 1/gain
            xc->t[2]        = dsp::irootf(sqrtf(xc->t[0]), fp->nSlope); // fg
            xc->t[3]        = 1.0f / xc->t[2];  // 1 / fg
            xc->b[0]        = 1.0f / (1.0f + fp->fQuality * (1.0f - expf(2.0f - xc->t[0] - xc->t[1]))); // k

            // Copy values
            for (size_t j=0; j<nc; ++j)
                *(c++)          = buf[(bptr + j) & (BLD_BUF_SIZE-1)];

            bptr        = (bptr + BLD_BUF_SIZE-1) & (BLD_BUF_SIZE-1);
        }
        for (size_t i=nc; i>0; --i) // Complete tail
        {
            // Copy values
            for (size_t j=0; j<nc; ++j)
                *(c++)          = buf[(bptr + j) & (BLD_BUF_SIZE-1)];
            bptr        = (bptr + BLD_BUF_SIZE-1) & (BLD_BUF_SIZE-1);
        }

        // Create cascades
        for (size_t j=0; j<nc; ++j)
        {
            // Intialize pointers
            f_cascade_t *c          = &dst[(nc+1)*j];       // target cascade pointer

            // Pre-calculate filter parameters
            float theta         = (((cj & (~1)) + 1)*M_PI_2)/(2*fp->nSlope); // The number of cascades is just doubled
            float tcos          = cosf(theta);
            float tcos2         = tcos * tcos;      // cos^2
            float tsin2         = 1.0f - tcos2;     // sin^2

            float xtcos         = 2.0f * tcos;

            float k, kf;

            if (ftype == FLT_BT_LRX_HISHELF)
            {
                // Generate cascades
                for (size_t i=0; i<samples; ++i)
                {
                    k                   = c->b[0];
                    kf                  = tsin2 + k*k * tcos2;

                    // Transfer function
                    c->t[0]             = kf * c->t[3];
                    c->t[1]             = k * xtcos;

                    c->b[0]             = c->t[2];
                    c->b[1]             = c->t[1];
                    c->b[2]             = c->t[0];

                    // Move to next cascade
                    c                  += nc;
                }
            }
            else
            {
                // Generate cascades
                for (size_t i=0; i<samples; ++i)
                {
                    k                   = c->b[0];
                    kf                  = tsin2 + k*k * tcos2;

                    // Transfer function
                    c->b[0]             = kf * c->t[3];
                    c->b[1]             = k * xtcos;
                    c->b[2]             = c->t[2];

                    c->t[0]             = c->b[2];
                    c->t[1]             = c->b[1];
                    c->t[2]             = c->b[0];

                    // Move to next cascade
                    c                  += nc;
                }
            }

            // Patch volume
            if (cj == 0)
            {
                f_cascade_t *c          = dst;       // target cascade pointer
                for (size_t i=0; i<samples; ++i)
                {
                    c->t[0]            *= c->b[3];
                    c->t[1]            *= c->b[3];
                    c->t[2]            *= c->b[3];
                    c                  += nc;
                }
            }

            // Increment cascade number
            cj++;
        }

        return nc;
    }

    size_t DynamicFilters::build_filter_bank(f_cascade_t *dst, const filter_params_t *fp, size_t cj, const float *sfg, size_t samples)
    {
        size_t nc;
        size_t ftype = (fp->nType & 1) ? fp->nType : fp->nType-1 ; // xxx_MT -> xxx_BT

        switch (ftype)
        {
            //----------------------------------- MISC SPECIAL FILTERS ------------------------------------
            case FLT_BT_AMPLIFIER:
            {
                if (cj >= 1)
                    return 0;
                nc          = 1;

                // Generate cascades
                f_cascade_t *c          = dst;       // target cascade pointer

                for (size_t i=0; i<samples; ++i)
                {
                    c->t[0]     = sfg[i];
                    c->t[1]     = 0.0f;
                    c->t[2]     = 0.0f;

                    c->b[0]     = 1.0f;
                    c->b[1]     = 0.0f;
                    c->b[2]     = 0.0f;

                    // Move to next cascade
                    c           ++;
                }

                break;
            }

            //----------------------------------- RLC FILTERS ------------------------------------
            case FLT_BT_RLC_LOPASS:
            case FLT_BT_RLC_HIPASS:
            {
                nc                      = quantify(cj, (fp->nSlope >> 1) + (fp->nSlope & 1)); // Number of cascades to generate
                if (nc <= 0)
                    return nc;
                size_t j                = 0;

                // Add cascade with one pole
                if ((cj == 0) && (fp->nSlope & 1))
                {
                    f_cascade_t *c          = dst;       // target cascade pointer

                    // Generate cascades
                    for (size_t i=0; i<samples; ++i)
                    {
                        c->b[0]     = 1.0f;
                        c->b[1]     = 1.0f;
                        c->b[2]     = 0.0f;

                        c->t[0]     = (ftype == FLT_BT_RLC_LOPASS) ? sfg[i] : 0.0f;
                        c->t[1]     = (ftype != FLT_BT_RLC_LOPASS) ? sfg[i] : 0.0f;
                        c->t[2]     = 0.0f;

                        // Move to next cascade
                        c                  += nc;
                    }

                    // Increment cascade number
                    cj++;
                    j++;
                }

                // Add additional cascades
                for (; j<nc; j++)
                {
                    f_cascade_t *c          = &dst[(nc+1)*j];       // target cascade pointer

                    // Generate cascades
                    for (size_t i=0; i<samples; ++i)
                    {
                        // Transfer function
                        c->b[0]             = 1.0;
                        c->b[1]             = 2.0 / (1.0 + fp->fQuality);
                        c->b[2]             = 1.0;

                        c->t[0]             = (ftype == FLT_BT_RLC_LOPASS) ? 1.0f : 0.0f;
                        c->t[1]             = 0.0;
                        c->t[2]             = (ftype != FLT_BT_RLC_LOPASS) ? 1.0f : 0.0f;

                        // Move to next cascade
                        c                  += nc;
                    }

                    // Patch volume
                    if (cj == 0)
                    {
                        f_cascade_t *c          = dst;       // target cascade pointer
                        for (size_t i=0; i<samples; ++i)
                        {
                            float gain         = sfg[i];
                            c->t[0]            *= gain;
                            c->t[1]            *= gain;
                            c->t[2]            *= gain;
                            c                  += nc;
                        }
                    }

                    cj++;
                }

                break;
            }

            case FLT_BT_RLC_LOSHELF:
            case FLT_BT_RLC_HISHELF:
            {
                size_t slope            = fp->nSlope * 2;
                nc                      = quantify(cj, fp->nSlope); // Number of cascades to generate
                if (nc <= 0)
                    return nc;

                for (size_t j=0; j<nc; j++)
                {
                    // Intialize pointers
                    f_cascade_t *c          = &dst[(nc+1)*j];       // target cascade pointer

                    // Generate cascades
                    for (size_t i=0; i<samples; ++i)
                    {
                        double gain         = sqrt(sfg[i]);
                        double fg           = exp(log(gain)/slope);

                        float *t            = (ftype == FLT_BT_RLC_LOSHELF) ? c->t : c->b;
                        float *b            = (ftype == FLT_BT_RLC_LOSHELF) ? c->b : c->t;

                        // Transfer function
                        t[0]                = fg;
                        t[1]                = 2.0 / (1.0 + fp->fQuality);
                        t[2]                = 1.0 / fg;

                        b[0]                = t[2]; // 1.0 / fg
                        b[1]                = t[1]; // 2.0 / (1.0 + fp->fQuality)
                        b[2]                = t[0]; // fg

                        // Move to next cascade
                        c                  += nc;
                    }

                    // Patch volume
                    if (cj == 0)
                    {
                        f_cascade_t *c          = dst;       // target cascade pointer
                        for (size_t i=0; i<samples; ++i)
                        {
                            float gain         = sqrtf(sfg[i]);
                            c->t[0]            *= gain;
                            c->t[1]            *= gain;
                            c->t[2]            *= gain;
                            c                  += nc;
                        }
                    }

                    // Increment cascade number
                    cj++;
                }

                break;
            }

            case FLT_BT_RLC_LADDERPASS:
            case FLT_BT_RLC_LADDERREJ:
            {
                size_t slope            = fp->nSlope * 2;
                nc                      = quantify(cj, slope); // Number of cascades to generate
                if (nc <= 0)
                    return nc;

                double kf               = fp->fFreq2;

                for (size_t j=0; j < nc; j++)
                {
                    // Intialize pointers
                    f_cascade_t *c          = &dst[(nc+1)*j];       // target cascade pointer

                    // Generate cascades
                    if (cj & 1)
                    {
                        for (size_t i=0; i<samples; ++i)
                        {
                            double gain         = (ftype == FLT_BT_RLC_LADDERREJ) ? sqrt(sfg[i]) : sqrt(1.0/sfg[i]);
                            double fg           = exp(log(gain)/slope);

                            // Second shelf cascade, hi-shelf always
                            float *t            = c->b;
                            float *b            = c->t;

                            // Create transfer function
                            t[0]                = fg;
                            t[1]                = 2.0 * kf / (1.0 + fp->fQuality);
                            t[2]                = kf*kf / fg;

                            b[0]                = 1.0 / fg;
                            b[1]                = 2.0 * kf / (1.0 + fp->fQuality);
                            b[2]                = fg * kf * kf;

                            if ((cj>>1) == 0)
                            {
                                c->t[0]            *= gain;
                                c->t[1]            *= gain;
                                c->t[2]            *= gain;
                            }

                            // Move to next cascade
                            c                  += nc;
                        }
                    }
                    else
                    {
                        // Even shelf cascade, lo-shelf for LADDERREJ, hi-shelf for LADDERPASS
                        for (size_t i=0; i<samples; ++i)
                        {
                            double gain1        = (ftype == FLT_BT_RLC_LADDERREJ) ? sqrt(1.0/sfg[i]) : sqrt(sfg[i]);
                            double gain2        = (ftype == FLT_BT_RLC_LADDERREJ) ? sqrt(sfg[i]) : sqrt(1.0/sfg[i]);
                            double fg           = (ftype == FLT_BT_RLC_LADDERREJ) ? exp(log(gain2)/slope) : exp(log(gain1)/slope);
                            double gain         = (ftype == FLT_BT_RLC_LADDERREJ) ? gain2 : gain1;

                            float *t            = (ftype == FLT_BT_RLC_LADDERREJ) ? c->t : c->b;
                            float *b            = (ftype == FLT_BT_RLC_LADDERREJ) ? c->b : c->t;

                            // Create transfer function
                            t[0]                = fg;
                            t[1]                = 2.0 / (1.0 + fp->fQuality);
                            t[2]                = 1.0 / fg;

                            b[0]                = t[2]; // 1.0 / fg
                            b[1]                = t[1]; // 2.0 / (1.0 + fp->fQuality)
                            b[2]                = t[0]; // fg

                            if ((cj>>1) == 0)
                            {
                                c->t[0]            *= gain;
                                c->t[1]            *= gain;
                                c->t[2]            *= gain;
                            }

                            // Move to next cascade
                            c                  += nc;
                        }
                    }

                    // Increment cascade number
                    cj++;
                }

                break;
            }

            case FLT_BT_RLC_BANDPASS:
            {
                nc                      = quantify(cj, fp->nSlope); // Number of cascades to generate
                if (nc <= 0)
                    return nc;

                double f2               = 1.0 / fp->fFreq2;
                double k                = (1.0 + f2)/(1.0 + fp->fQuality);

                // Intialize pointers
                for (size_t j=0; j < nc; j++)
                {
                    f_cascade_t *c          = &dst[(nc+1)*j];       // target cascade pointer

                    for (size_t i=0; i<samples; ++i)
                    {
                        c->t[0]             = 0.0;
                        c->t[1]             = (cj == 0) ? exp(fp->nSlope * log(k)) * sfg[i] : 1.0;
                        c->t[2]             = 0.0;

                        c->b[0]             = f2;
                        c->b[1]             = k;
                        c->b[2]             = 1.0;

                        // Move to next cascade
                        c                  += nc;
                    }

                    // Increment cascade number
                    cj++;
                }

                break;
            }

            case FLT_BT_RLC_BELL:
            {
                nc                      = quantify(cj, fp->nSlope); // Number of cascades to generate
                if (nc <= 0)
                    return nc;

                for (size_t j=0; j < nc; j++)
                {
                    f_cascade_t *c      = &dst[(nc+1)*j];       // target cascade pointer

                    // Generate cascades
                    for (size_t i=0; i<samples; ++i)
                    {
                        double fg           = exp(log(sfg[i])/fp->nSlope);
                        double angle        = atan(fg);
                        double tsin         = sin(angle);
                        double tcos         = sqrt(1.0 - tsin*tsin);
                        double k            = 2.0 * (1.0/fg + fg) / (1.0 + (2.0 * fp->fQuality) / fp->nSlope);

                        // Create transfer function
                        c->t[0]             = 1.0;
                        c->t[1]             = k * tsin;
                        c->t[2]             = 1.0;

                        c->b[0]             = 1.0;
                        c->b[1]             = k * tcos;
                        c->b[2]             = 1.0;

                        // Move to next cascade
                        c                  += nc;
                    }

                    // Increment cascade number
                    cj++;
                }

                break;
            }

            // Resonance filter
            case FLT_BT_RLC_RESONANCE:
            {
                nc                      = quantify(cj, fp->nSlope); // Number of cascades to generate
                if (nc <= 0)
                    return nc;

                for (size_t j=0; j < nc; j++)
                {
                    f_cascade_t *c      = &dst[(nc+1)*j];       // target cascade pointer

                    // Pre-calculate filter parameters
                    double k            = 2.0 / (1.0 + fp->fQuality);

                    // Generate cascades
                    for (size_t i=0; i<samples; ++i)
                    {
                        double angle        = atan(exp(log(sfg[i]) / fp->nSlope));
                        double tsin         = sin(angle);
                        double tcos         = sqrt(1.0 - tsin*tsin);

                        // Create transfer function
                        c->t[0]             = 1.0;
                        c->t[1]             = k * tsin;
                        c->t[2]             = 1.0;

                        c->b[0]             = 1.0;
                        c->b[1]             = k * tcos;
                        c->b[2]             = 1.0;

                        // Move to next cascade
                        c                  += nc;
                    }

                    // Increment cascade number
                    cj++;
                }

                break;
            }

            case FLT_BT_RLC_NOTCH:
            {
                // Need ONLY ONE cascade for notch
                if (cj > 0)
                    return 0;

                nc                  = 1;
                f_cascade_t *c      = dst;       // target cascade pointer
                float k             = 2.0 / (1.0 + fp->fQuality);

                // Generate cascades
                for (size_t i=0; i<samples; ++i)
                {
                    // Create transfer function
                    c->t[0]             = sfg[i];
                    c->t[1]             = 0.0;
                    c->t[2]             = sfg[i];

                    c->b[0]             = 1.0;
                    c->b[1]             = k;
                    c->b[2]             = 1.0;

                    // Move to next cascade
                    c++;
                }

                break;
            }

            case FLT_BT_RLC_ENVELOPE:
            {
                size_t slope            = fp->nSlope;
                size_t max_nc           = (slope & 1) * 3 + (slope >> 1);
                nc                      = quantify(cj, max_nc); // Number of cascades to generate
                if (nc <= 0)
                    return nc;

                // TODO: test this
                size_t j=0;
                while ((slope & 1) && (cj < 3) && (j < nc))
                {
                    f_cascade_t *c      = &dst[(nc+1)*j];       // target cascade pointer
                    float k             = 1.0f / (1 << (cj * 4)); // 1, 1/16, 1/256, 1/4096

                    // Generate cascades
                    for (size_t i=0; i<samples; ++i)
                    {
                        c->t[0]         = 1.0f;
                        c->t[1]         = (1.0f + 0.25f)*k;
                        c->t[2]         = 0.25f * k*k;

                        c->b[0]         = 1.0f;
                        c->b[1]         = (0.5f + 0.125f)*k;
                        c->b[2]         = 0.5f * 0.125f * k*k;

                        if (!cj)
                        {
                            c->t[0]        *= fp->fGain;
                            c->t[1]        *= fp->fGain;
                            c->t[2]        *= fp->fGain;
                        }

                        // Move to next cascade
                        c                  += nc;
                    }

                    // Increment cascade number
                    cj++;
                    j++;
                }

                while (j < nc)
                {
                    f_cascade_t *c      = &dst[(nc+1)*j];       // target cascade pointer

                    // Generate cascades
                    for (size_t i=0; i<samples; ++i)
                    {
                        // Create transfer function
                        c->t[0]             = (j == 0) ? fp->fGain : 1.0f;
                        c->t[1]             = (j == 0) ? fp->fGain : 1.0f;

                        c->b[0]             = 1.0f;
                        c->b[1]             = 0.00005f;

                        // Move to next cascade
                        c                  += nc;
                    }

                    // Increment cascade number
                    cj++;
                    j++;
                }
                break;
            }

            //----------------------------------- BWC FILTERS ------------------------------------
            case FLT_BT_BWC_LOPASS:
            case FLT_BT_BWC_HIPASS:
            {
                nc                      = quantify(cj, (fp->nSlope >> 1) + (fp->nSlope & 1)); // Number of cascades to generate
                if (nc <= 0)
                    return nc;
                size_t j                = 0;

                // Generate first cascade
                if ((cj == 0) && (fp->nSlope & 1))
                {
                    f_cascade_t *c          = dst;       // target cascade pointer

                    // Generate cascades
                    for (size_t i=0; i<samples; ++i)
                    {
                        c->b[0]     = 1.0;
                        c->b[1]     = 1.0;
                        c->b[2]     = 0.0;

                        c->t[0]     = (ftype == FLT_BT_BWC_LOPASS) ? sfg[i] : 0.0;
                        c->t[1]     = (ftype == FLT_BT_BWC_LOPASS) ? 0.0 : sfg[i];
                        c->t[2]     = 0.0;

                        // Move to next cascade
                        c                  += nc;
                    }

                    // Increment cascade number
                    cj++;
                    j++;
                }

                double k    = 1.0f / (1.0f + fp->fQuality);

                // Add additional cascades
                for (; j<nc; j++)
                {
                    f_cascade_t *c      = &dst[(nc+1)*j];       // target cascade pointer

                    // Pre-calculate filter parameters
                    double theta    = ((2*(cj - (fp->nSlope & 1)) + 1)*M_PI_2)/fp->nSlope;
                    double tsin     = sin(theta);
                    double tcos     = sqrt(1.0 - tsin*tsin);
                    float kf1       = 1.0 / (tsin*tsin + k*k * tcos*tcos);

                    // Generate cascades
                    for (size_t i=0; i<samples; ++i)
                    {
                        // Tranfer function
                        if (ftype == FLT_BT_BWC_HIPASS)
                        {
                            c->t[0]         = 0.0;
                            c->t[1]         = 0.0;
                            c->t[2]         = (cj == 0) ? sfg[i] : 1.0;

                            c->b[0]         = kf1;
                            c->b[1]         = 2.0 * k * tcos * kf1;
                            c->b[2]         = 1.0;
                        }
                        else
                        {
                            c->t[0]         = (cj == 0) ? sfg[i] : 1.0;
                            c->t[1]         = 0.0;
                            c->t[2]         = 0.0;

                            c->b[0]         = 1.0;
                            c->b[1]         = 2.0 * k * tcos * kf1;
                            c->b[2]         = kf1;
                        }

                        // Move to next cascade
                        c                  += nc;
                    }

                    // Increment cascade number
                    cj++;
                }

                break;
            }

            case FLT_BT_BWC_HISHELF:
            case FLT_BT_BWC_LOSHELF:
            {
                nc                      = quantify(cj, fp->nSlope); // Number of cascades to generate
                if (nc <= 0)
                    return nc;

                // Create cascades
                for (size_t j=0; j<nc; ++j)
                {
                    // Intialize pointers
                    f_cascade_t *c          = &dst[(nc+1)*j];       // target cascade pointer

                    // Pre-calculate filter parameters
                    double theta        = ((2*cj + 1)*M_PI_2)/(2*fp->nSlope);
                    double tsin         = sin(theta);
                    double tcos         = sqrt(1.0 - tsin*tsin);

                    // Generate cascades
                    for (size_t i=0; i<samples; ++i)
                    {
                        double gain         = sqrt(sfg[i]);
                        double fg           = exp(log(gain)/(2.0*fp->nSlope));
                        double k            = 1.0 / (1.0 + fp->fQuality * (1.0 - exp(2.0 - gain - 1.0/gain)));
                        double kf           = tsin*tsin + k*k * tcos*tcos;

                        float *t            = (ftype == FLT_BT_BWC_HISHELF) ? c->t : c->b;
                        float *b            = (ftype == FLT_BT_BWC_HISHELF) ? c->b : c->t;

                        // Transfer function
                        t[0]                = kf / fg;
                        t[1]                = 2.0 * k * tcos;
                        t[2]                = fg;

                        b[0]                = t[2]; // fg
                        b[1]                = t[1]; // 2.0 * k * tcos;
                        b[2]                = t[0]; // kf / fg

                        // Move to next cascade
                        c                  += nc;
                    }

                    // Patch volume
                    if (cj == 0)
                    {
                        f_cascade_t *c          = dst;       // target cascade pointer
                        for (size_t i=0; i<samples; ++i)
                        {
                            float gain          = sqrtf(sfg[i]);
                            c->t[0]            *= gain;
                            c->t[1]            *= gain;
                            c->t[2]            *= gain;
                            c                  += nc;
                        }
                    }

                    // Increment cascade number
                    cj++;
                }

                break;
            }

            case FLT_BT_BWC_LADDERPASS:
            case FLT_BT_BWC_LADDERREJ:
            {
                // NOTE: this code was not tested because originally LRX filters are used
                size_t slope            = fp->nSlope * 2;
                nc                      = quantify(cj, slope); // Number of cascades to generate
                if (nc <= 0)
                    return nc;

                // Create cascades
                for (size_t j=0; j<nc; ++j)
                {
                    // Intialize pointers
                    f_cascade_t *c      = &dst[(nc+1)*j];       // target cascade pointer

                    // Pre-calculate filter parameters
                    float rc_slope      = 1.0f / slope;
                    float theta         = (((cj&(~1)) + 1)*M_PI_2) * rc_slope;
                    float tcos          = cosf(theta);
                    float tcos2         = tcos * tcos;      // cos^2
                    float tsin2         = 1.0f - tcos2;     // sin^2

                    float gain, k, kf, fg;
                    float *t, *b;

                    if (cj & 1) // Second shelf cascade, always hi-shelf
                    {
                        float xf            = fp->fFreq2;
                        float xf2           = xf * xf;
                        float xtcos         = 2.0f * tcos * xf;

                        // Generate cascades
                        for (size_t i=0; i<samples; ++i)
                        {
                            gain                = (ftype == FLT_BT_BWC_LADDERPASS) ? sqrtf(sfg[i]) : sqrtf(1.0/sfg[i]);
                            fg                  = expf(logf(gain) * rc_slope);
                            k                   = 1.0f / (1.0 + fp->fQuality * (1.0 - expf(2.0 - gain - 1.0/gain)));
                            kf                  = tsin2 + k*k * tcos2;

                            t                   = c->b;
                            b                   = c->t;

                            // Transfer function
                            t[0]                = kf / fg;
                            t[1]                = k * xtcos;
                            t[2]                = fg * xf2;

                            b[0]                = fg;
                            b[1]                = t[1];
                            b[2]                = t[0] * xf2;

                            if (!(cj&(~1)))
                            {
                                float gain2         = 1.0f / gain;
                                c->t[0]            *= gain2;
                                c->t[1]            *= gain2;
                                c->t[2]            *= gain2;
                            }

                            // Move to next cascade
                            c                  += nc;
                        }
                    }
                    else // First shelf cascade, lo-shelf for LADDERREJ, hi-shelf for LADDERPASS
                    {
                        float xtcos         = 2.0f * tcos;

                        // Generate cascades
                        for (size_t i=0; i<samples; ++i)
                        {
                            gain            = sqrtf(sfg[i]);
                            k               = 1.0f / (1.0 + fp->fQuality * (1.0 - expf(2.0 - gain - 1.0/gain)));
                            fg              = expf(logf(gain) * rc_slope);
                            kf              = tsin2 + k*k * tcos2;

                            if (ftype == FLT_BT_BWC_LADDERPASS)
                            {
                                t               = c->t;
                                b               = c->b;
                            }
                            else
                            {
                                t               = c->b;
                                b               = c->t;
                            }

                            // Transfer function
                            t[0]                = kf / fg;
                            t[1]                = k * xtcos;
                            t[2]                = fg;

                            b[0]                = t[2]; // fg
                            b[1]                = t[1]; // 2.0 * k * tcos
                            b[2]                = t[0]; // kf / fg;

                            if (!(cj&(~1)))
                            {
                                c->t[0]            *= gain;
                                c->t[1]            *= gain;
                                c->t[2]            *= gain;
                            }

                            // Move to next cascade
                            c                  += nc;
                        }
                    }

                    // Increment cascade number
                    cj++;
                }

                break;
            }

            case FLT_BT_BWC_BELL:
            {
                size_t slope            = fp->nSlope * 2;
                nc                      = quantify(cj, slope); // Number of cascades to generate
                if (nc <= 0)
                    return nc;

                // Create cascades
                for (size_t j=0; j<nc; ++j)
                {
                    // Intialize pointers
                    f_cascade_t *c          = &dst[(nc+1)*j];       // target cascade pointer

                    // Pre-calculate filter parameters
                    double theta        = (((cj&(~1)) + 1)*M_PI_2)/(slope);
                    double tsin         = sin(theta);
                    double tcos         = sqrt(1.0 - tsin*tsin);
                    float k             = 1.0f / (1.0 + fp->fQuality);
                    double kf           = tsin*tsin + k*k * tcos*tcos;

                    if (cj & 1) // Second cascade
                    {
                        // Generate cascades
                        for (size_t i=0; i<samples; ++i)
                        {
                            double fg           = exp(log(sfg[i])/slope);

                            // Transfer function
                            if (sfg[i] >= 1.0)
                            {
                                c->t[0]             = 1.0;
                                c->t[1]             = 2.0 * k * tcos / fg;
                                c->t[2]             = kf / (fg * fg);

                                c->b[0]             = 1.0;
                                c->b[1]             = 2.0 * k * tcos;
                                c->b[2]             = kf;
                            }
                            else
                            {
                                c->t[0]             = 1.0;
                                c->t[1]             = 2.0 * k * tcos;
                                c->t[2]             = kf;

                                c->b[0]             = 1.0;
                                c->b[1]             = 2.0 * k * tcos * fg;
                                c->b[2]             = kf * fg * fg;
                            }

                            // Move to next cascade
                            c                  += nc;
                        }
                    }
                    else // First cascade
                    {
                        // Generate cascades
                        for (size_t i=0; i<samples; ++i)
                        {
                            double fg           = exp(log(sfg[i])/slope);

                            // Transfer function
                            if (sfg[i] >= 1.0)
                            {
                                c->t[0]             = 1.0;
                                c->t[1]             = 2.0 * k * tcos * fg / kf;
                                c->t[2]             = 1.0 * fg * fg / kf;

                                c->b[0]             = 1.0;
                                c->b[1]             = 2.0 * k * tcos / kf;
                                c->b[2]             = 1.0 / kf;
                            }
                            else
                            {
                                c->t[0]             = 1.0;
                                c->t[1]             = 2.0 * k * tcos / kf;
                                c->t[2]             = 1.0 / kf;

                                c->b[0]             = 1.0;
                                c->b[1]             = 2.0 * k * tcos / (fg * kf);
                                c->b[2]             = 1.0 / (fg * fg * kf);

                            }

                            // Move to next cascade
                            c                  += nc;
                        }
                    }

                    // Increment cascade number
                    cj++;
                }

                break;
            }

            case FLT_BT_BWC_BANDPASS:
            {
                size_t slope            = fp->nSlope * 2;
                nc                      = quantify(cj, slope); // Number of cascades to generate
                if (nc <= 0)
                    return nc;

                double f2               = fp->fFreq2;
                double k                = 1.0f / (1.0f + fp->fQuality);

                // Create cascades
                for (size_t j=0; j<nc; ++j)
                {
                    // Intialize pointers
                    f_cascade_t *c          = &dst[(nc+1)*j];       // target cascade pointer

                    // Pre-calculate filter parameters
                    double theta        = (((cj&(~1)) + 1)*M_PI_2)/slope;
                    double tsin         = sin(theta);
                    double tcos         = sqrt(1.0 - tsin*tsin);
                    double kf1          = 1.0/(tsin*tsin + k*k * tcos*tcos);

                    if (cj & 1) // Hi-pass cascade
                    {
                        for (size_t i=0; i<samples; ++i)
                        {
                            // Transfer function
                            c->t[0]             = 1.0;
                            c->t[1]             = 0.0;
                            c->t[2]             = 0.0;

                            c->b[0]             = 1.0;
                            c->b[1]             = 2.0 * k * tcos * f2 * kf1;
                            c->b[2]             = f2 * f2 * kf1;

                            // Move to next cascade
                            c                  += nc;
                        }
                    }
                    else // Lo-pass cascade
                    {
                        for (size_t i=0; i<samples; ++i)
                        {
                            // Transfer function
                            c->t[0]             = 0.0;
                            c->t[1]             = 0.0;
                            c->t[2]             = (cj == 0) ? sfg[i] : 1.0;

                            c->b[0]             = kf1;
                            c->b[1]             = 2.0 * k * tcos * kf1;
                            c->b[2]             = 1.0;

                            // Move to next cascade
                            c                  += nc;
                        }
                    }

                    // Increment cascade number
                    cj++;
                }

                break;
            }

            //----------------------------------- LRX FILTERS ------------------------------------
            case FLT_BT_LRX_LOPASS:
            case FLT_BT_LRX_HIPASS:
            {
                nc                      = quantify(cj, fp->nSlope*2); // Number of cascades to generate
                if (nc <= 0)
                    return nc;

                double k    = 1.0f / (1.0f + fp->fQuality);

                // Add additional cascades
                for (size_t j=0; j<nc; j++)
                {
                    f_cascade_t *c      = &dst[(nc+1)*j];       // target cascade pointer

                    // Pre-calculate filter parameters
                    double theta    = (((cj & (~1)) + 1)*M_PI_2)/(fp->nSlope*2); // The number of cascades is just doubled
                    double tsin     = sin(theta);
                    double tcos     = sqrt(1.0 - tsin*tsin);
                    float kf1       = 1.0 / (tsin*tsin + k*k * tcos*tcos);

                    // Generate cascades
                    for (size_t i=0; i<samples; ++i)
                    {
                        // Tranfer function
                        if (ftype == FLT_BT_LRX_HIPASS)
                        {
                            c->t[0]         = 0.0;
                            c->t[1]         = 0.0;
                            c->t[2]         = (cj == 0) ? sfg[i] : 1.0;

                            c->b[0]         = kf1;
                            c->b[1]         = 2.0 * k * tcos * kf1;
                            c->b[2]         = 1.0;
                        }
                        else
                        {
                            c->t[0]         = (cj == 0) ? sfg[i] : 1.0;
                            c->t[1]         = 0.0;
                            c->t[2]         = 0.0;

                            c->b[0]         = 1.0;
                            c->b[1]         = 2.0 * k * tcos * kf1;
                            c->b[2]         = kf1;
                        }

                        // Move to next cascade
                        c                  += nc;
                    }

                    // Increment cascade number
                    cj++;
                }

                break;
            }

            case FLT_BT_LRX_HISHELF:
            case FLT_BT_LRX_LOSHELF:
                return build_lrx_shelf_filter_bank(dst, fp, cj, sfg, samples, ftype);

            case FLT_BT_LRX_LADDERPASS:
            case FLT_BT_LRX_LADDERREJ:
                return build_lrx_ladder_filter_bank(dst, fp, cj, sfg, samples, ftype);

            case FLT_BT_LRX_BELL:
            {
                size_t slope            = fp->nSlope * 4;
                nc                      = quantify(cj, slope); // Number of cascades to generate
                if (nc <= 0)
                    return nc;

                // Create cascades
                for (size_t j=0; j<nc; ++j)
                {
                    // Intialize pointers
                    f_cascade_t *c          = &dst[(nc+1)*j];       // target cascade pointer

                    // Pre-calculate filter parameters
                    double theta        = (((cj&(~3)) + 2)*M_PI_2)/(slope);
                    double tsin         = sin(theta);
                    double tcos         = sqrt(1.0 - tsin*tsin);
                    float k             = 1.0f / (1.0 + fp->fQuality);
                    double kf           = tsin*tsin + k*k * tcos*tcos;

                    if (cj & 1) // Second cascade
                    {
                        // Generate cascades
                        for (size_t i=0; i<samples; ++i)
                        {
                            double fg           = exp(log(sfg[i])/slope);

                            // Transfer function
                            if (sfg[i] >= 1.0)
                            {
                                c->t[0]             = 1.0;
                                c->t[1]             = 2.0 * k * tcos / fg;
                                c->t[2]             = kf / (fg * fg);

                                c->b[0]             = 1.0;
                                c->b[1]             = 2.0 * k * tcos;
                                c->b[2]             = kf;
                            }
                            else
                            {
                                c->t[0]             = 1.0;
                                c->t[1]             = 2.0 * k * tcos;
                                c->t[2]             = kf;

                                c->b[0]             = 1.0;
                                c->b[1]             = 2.0 * k * tcos * fg;
                                c->b[2]             = kf * fg * fg;
                            }

                            // Move to next cascade
                            c                  += nc;
                        }
                    }
                    else // First cascade
                    {
                        // Generate cascades
                        for (size_t i=0; i<samples; ++i)
                        {
                            double fg           = exp(log(sfg[i])/slope);

                            // Transfer function
                            if (sfg[i] >= 1.0)
                            {
                                c->t[0]             = 1.0;
                                c->t[1]             = 2.0 * k * tcos * fg / kf;
                                c->t[2]             = 1.0 * fg * fg / kf;

                                c->b[0]             = 1.0;
                                c->b[1]             = 2.0 * k * tcos / kf;
                                c->b[2]             = 1.0 / kf;
                            }
                            else
                            {
                                c->t[0]             = 1.0;
                                c->t[1]             = 2.0 * k * tcos / kf;
                                c->t[2]             = 1.0 / kf;

                                c->b[0]             = 1.0;
                                c->b[1]             = 2.0 * k * tcos / (fg * kf);
                                c->b[2]             = 1.0 / (fg * fg * kf);

                            }

                            // Move to next cascade
                            c                  += nc;
                        }
                    }

                    // Increment cascade number
                    cj++;
                }

                break;
            }

            case FLT_BT_LRX_BANDPASS:
            {
                size_t slope            = fp->nSlope * 4;
                nc                      = quantify(cj, slope); // Number of cascades to generate
                if (nc <= 0)
                    return nc;

                double f2               = fp->fFreq2;
                double k                = 1.0f / (1.0f + fp->fQuality);

                // Create cascades
                for (size_t j=0; j<nc; ++j)
                {
                    // Intialize pointers
                    f_cascade_t *c          = &dst[(nc+1)*j];       // target cascade pointer

                    // Pre-calculate filter parameters
                    double theta        = (((cj&(~3)) + 2)*M_PI_2)/slope;
                    double tsin         = sin(theta);
                    double tcos         = sqrt(1.0 - tsin*tsin);
                    double kf1          = 1.0/(tsin*tsin + k*k * tcos*tcos);

                    if (cj & 1) // Hi-pass cascade
                    {
                        for (size_t i=0; i<samples; ++i)
                        {
                            // Transfer function
                            c->t[0]             = 1.0;
                            c->t[1]             = 0.0;
                            c->t[2]             = 0.0;

                            c->b[0]             = 1.0;
                            c->b[1]             = 2.0 * k * tcos * f2 * kf1;
                            c->b[2]             = f2 * f2 * kf1;

                            // Move to next cascade
                            c                  += nc;
                        }
                    }
                    else // Lo-pass cascade
                    {
                        for (size_t i=0; i<samples; ++i)
                        {
                            // Transfer function
                            c->t[0]             = 0.0;
                            c->t[1]             = 0.0;
                            c->t[2]             = (cj == 0) ? sfg[i] : 1.0;

                            c->b[0]             = kf1;
                            c->b[1]             = 2.0 * k * tcos * kf1;
                            c->b[2]             = 1.0;

                            // Move to next cascade
                            c                  += nc;
                        }
                    }

                    // Increment cascade number
                    cj++;
                }

                break;
            }

            default:
                nc = 0;
                break;
        }

        return nc;
    }

    void DynamicFilters::complex_transfer_calc(float *re, float *im, double f, size_t nc)
    {
        double f2       = f * f; // f ^ 2

        f_cascade_t *c  = vCascades;

        for (size_t i=0; i<nc; ++i)
        {
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
            b_re            = (*re)*w_re - (*im)*w_im;
            b_im            = (*re)*w_im + (*im)*w_re;

            // Commit result
            *re             = b_re;
            *im             = b_im;

            // Move cascade pointer diagonally in a filter bank
            c              += (nc + 1);
        }
    }

    bool DynamicFilters::freq_chart(size_t id, float *re, float *im, const float *f, float gain, size_t count)
    {
        if (id >= nFilters)
            return false;
        filter_params_t *fp = &vFilters[id].sParams;

        // Initialize values
        dsp::fill_one(re, count);
        dsp::fill_zero(im, count);
        if (fp->nType == FLT_NONE)
            return true;

        // Process the filter
        size_t cj   = 0;

        if (fp->nType & 1) // Bilinear
        {
            double nf   = M_PI / double(nSampleRate);
            double kf   = 1.0/tan(fp->fFreq * nf);
            double lf   = nSampleRate * 0.499;

            // Process all cascades
            while (true)
            {
                // Generate cascades
                size_t nj               = build_filter_bank(vCascades, fp, cj, &gain, 1);
                if (nj <= 0)
                    break;

                for (size_t i=0; i<count; ++i)
                {
                    // Cyclic frequency
                    double w    = *(f++);
                    w           = tan((w > lf ? lf : w) * nf) * kf;

                    complex_transfer_calc(&re[i], &im[i], w, nj);
                }

                // Update counters and pointers
                cj                     += nj;
            }
        }
        else
        {
            double kf   = 1.0 / fp->fFreq;

            // Process all cascades
            while (true)
            {
                // Generate cascades
                size_t nj               = build_filter_bank(vCascades, fp, cj, &gain, 1);
                if (nj <= 0)
                    break;

                for (size_t i=0; i<count; ++i)
                {
                    // Cyclic frequency
                    double w    = *(f++) * kf;
                    complex_transfer_calc(&re[i], &im[i], w, nj);
                }

                // Update counters and pointers
                cj                     += nj;
            }
        }

        return true;
    }

    bool DynamicFilters::freq_chart(size_t id, float *dst, const float *f, float gain, size_t count)
    {
        if (id >= nFilters)
            return false;

        filter_params_t *fp = &vFilters[id].sParams;

        // Initialize values
        switch (fp->nType)
        {
            case FLT_NONE:
                dsp::pcomplex_fill_ri(dst, 1.0f, 0.0f, count);
                return true;

            case FLT_BT_AMPLIFIER:
            case FLT_MT_AMPLIFIER:
                dsp::pcomplex_fill_ri(dst, gain, 0.0f, count);
                return true;

            default:
                dsp::pcomplex_fill_ri(dst, 1.0f, 0.0f, count);
                break;
        }

        // Process the filter
        size_t cj   = 0;
        count     <<= 1;

        if (fp->nType & 1) // Bilinear
        {
            double nf   = M_PI / double(nSampleRate);
            double kf   = 1.0/tan(fp->fFreq * nf);
            double lf   = nSampleRate * 0.499;

            // Process all cascades
            while (true)
            {
                // Generate cascades
                size_t nj               = build_filter_bank(vCascades, fp, cj, &gain, 1);
                if (nj <= 0)
                    break;

                for (size_t i=0; i<count; i+=2)
                {
                    // Cyclic frequency
                    double w    = *(f++);
                    w           = tan((w > lf ? lf : w) * nf) * kf;

                    complex_transfer_calc(&dst[i], &dst[i+1], w, nj);
                }

                // Update counters and pointers
                cj                     += nj;
            }
        }
        else
        {
            double kf   = 1.0 / fp->fFreq;

            // Process all cascades
            while (true)
            {
                // Generate cascades
                size_t nj               = build_filter_bank(vCascades, fp, cj, &gain, 1);
                if (nj <= 0)
                    break;

                for (size_t i=0; i<count; i+=2)
                {
                    // Cyclic frequency
                    double w    = *(f++) * kf;
                    complex_transfer_calc(&dst[i], &dst[i+1], w, nj);
                }

                // Update counters and pointers
                cj                     += nj;
            }
        }

        return true;
    }
} /* namespace lsp */
