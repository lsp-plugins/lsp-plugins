/*
 * SyncChirpProcessor.cpp
 *
 *  Created on: 12 Jul 2017
 *      Author: crocoduck
 */

#include <dsp/dsp.h>
#include <dsp/endian.h>
#include <core/debug.h>
#include <core/util/SyncChirpProcessor.h>
#include <math.h>
#include <core/files/LSPCFile.h>
#include <core/files/lspc/LSPCAudioWriter.h>
#include <core/files/lspc/LSPCAudioReader.h>

#define MIN_AMPLITUDE               1.0e-6f         // Chirp Minimal Amplitude
#define DFL_TAIL                    1.0f;           // Default tail acquisition time [s]
#define MAX_TAIL                    5.0f            // Max tail acquisition time [s]
#define DFL_DURATION                5.0f            // Default chirp duration [s] (pre-optimisation)
#define LIM_DURATION                50.0f           // Max test chirp length [s] (pre-optimisation)
#define LIM_OPT_ADDTIME             10.0f           // Additional time to which chirp duration can be increased by optimization [s]
#define LIM_LAG                     (1 << 7)        // Limits higher order responses lags to store
#define ASYM_THRS                   10              // Threshold for the Asymptotic check
#define DFL_INITIAL_FREQ            1.0             // Default initial frequency [Hz]       | Make DFL_INITIAL_FREQ < DFL_FINAL_FREQ
#define DFL_FINAL_FREQ              8000.0          // Default final frequency [Hz]         |
#define MAX_PART_SIZE           	32768           // Max Input - Output partition size for convolution
#define FADEIN_FRACTION             0.3f            // Upper bound for fade in duration as fraction of chirp duration
#define FADEOUT_FRACTION            0.3f            // Upper bound for fade out duration as fraction of chirp duration
#define OVER_BUF_LIMIT_SIZE         (12 * 1024)     // Multiple of 3, 4 and 8
#define ENVELOPE_BUF_LIMIT_SIZE     65536           // Maximum size for post processing envelope follower
#define BG_NOISE_LIMIT             -10.0            // Threshold level to consider postprocessing data reliable (relative to RT low regression line fitting limit)
#define MAX_WINDOW_RANK             16               // Maximum window rank for higher order responses windowing

namespace lsp
{
    SyncChirpProcessor::SyncChirpProcessor()
    {
        nSampleRate                     = -1;

        sChirpParams.enMethod           = SCP_SYNTH_SIMPLE;
        sChirpParams.initialFrequency   = DFL_INITIAL_FREQ;
        sChirpParams.finalFrequency     = DFL_FINAL_FREQ;
        sChirpParams.fDuration          = 0.0f;
        sChirpParams.fAlpha             = 1.0f;

        sChirpParams.fDurationCoarse    = 0.0f;
        sChirpParams.nDuration          = 0;
        sChirpParams.nTimeLags          = 0;
        sChirpParams.nOrder             = 0;
        sChirpParams.beta               = 0.0;
        sChirpParams.gamma              = 0.0;
        sChirpParams.delta              = 0.0;
        sChirpParams.fConvScale         = 0.0f;

        sChirpParams.bAsymptotic        = false;

        sChirpParams.bRecalculate       = true;

        sChirpParams.bReconfigure       = true;

        sFader.enMethod                 = SCP_FADE_NONE;
        sFader.fFadeIn                  = 0.0f;
        sFader.fFadeOut                 = 0.0f;
        sFader.nFadeIn                  = 0;
        sFader.nFadeOut                 = 0;
        sFader.nFadeIn_Over             = 0;
        sFader.nFadeOut_Over            = 0;

        sConvParams.nChannels			= 0;
        sConvParams.nPartitionSize		= 0;
        sConvParams.nConvRank			= 0;
        sConvParams.nImage				= 0;
        sConvParams.nAllocationSize		= 0;
        sConvParams.vPartitions 		= NULL;
        sConvParams.vPaddedLengths		= NULL;
        sConvParams.vInversePrepends    = NULL;
        sConvParams.vConvLengths        = NULL;
        sConvParams.vAlignOffsets       = NULL;
        sConvParams.pData				= NULL;
        sConvParams.vInPart 			= NULL;
        sConvParams.vInvPart 			= NULL;
        sConvParams.vInImage 			= NULL;
        sConvParams.vInvImage 			= NULL;
        sConvParams.vTemp 				= NULL;
        sConvParams.pTempData 			= NULL;
        sConvParams.bReallocateTemp 	= true;

        sCRPostProc.noiseLevel          = 0.0;
        sCRPostProc.noiseValue          = 0.0;
        sCRPostProc.fIrLimit            = 0.0f;
        sCRPostProc.nIrLimit            = 0;
        sCRPostProc.noiseLevelNorm      = 0.0;
        sCRPostProc.noiseValueNorm      = 0.0;
        sCRPostProc.bLowNoise           = false;
        sCRPostProc.nRT                 = 0;
        sCRPostProc.fRT                 = 0.0f;
        sCRPostProc.fCorrelation        = 0.0f;
        sCRPostProc.nHamOrder           = 0;
        sCRPostProc.nHwinSize           = 0;
        sCRPostProc.nWinRank            = 0;
        sCRPostProc.mCoeffsReDet        = 0.0f;
        sCRPostProc.mCoeffsImDet        = 0.0f;
        sCRPostProc.mCoeffsRe           = NULL;
        sCRPostProc.mCoeffsIm           = NULL;
        sCRPostProc.mHigherRe           = NULL;
        sCRPostProc.mHigherIm           = NULL;
        sCRPostProc.mKernelsRe          = NULL;
        sCRPostProc.mKernelsIm          = NULL;
        sCRPostProc.vTemprow1Re         = NULL;
        sCRPostProc.vTemprow1Im         = NULL;
        sCRPostProc.vTemprow2Re         = NULL;
        sCRPostProc.vTemprow2Im         = NULL;
        sCRPostProc.pData               = NULL;

        pChirp                          = NULL;
        pInverseFilter                  = NULL;
        pConvResult                     = NULL;

        enOverMode                      = OM_LANCZOS_8X2;
        nOversampling                   = 0;

        vOverBuffer1                    = NULL;
        vOverBuffer2                    = NULL;
        vEnvelopeBuffer                 = NULL;
        pData                           = NULL;

        bSync                           = true;
    }

    SyncChirpProcessor::~SyncChirpProcessor()
    {
    }

    bool SyncChirpProcessor::init()
    {
        pChirp          = new Sample();
        pInverseFilter  = new Sample();
        pConvResult     = new AudioFile();

        // 1X Buffer for oversampled chirp, 1X Buffer for oversampled inverse filter, 1X Buffer for postprocessor envelope follower
        size_t samples  = 2 * OVER_BUF_LIMIT_SIZE + ENVELOPE_BUF_LIMIT_SIZE;

        float *ptr              = alloc_aligned<float>(pData, samples);
        if (ptr == NULL)
            return false;

        lsp_guard_assert(float *save = ptr);
        vOverBuffer1    = ptr;
        ptr            += OVER_BUF_LIMIT_SIZE;
        vOverBuffer2    = ptr;
        ptr            += OVER_BUF_LIMIT_SIZE;
        vEnvelopeBuffer = ptr;
        ptr            += ENVELOPE_BUF_LIMIT_SIZE;

        lsp_assert(ptr <= &save[samples]);

        return sOver1.init() && sOver2.init();
    }

    void SyncChirpProcessor::destroy()
    {
    	destroyConvolutionParameters();

    	destroyConvolutionTempArrays();

    	destroyIdentificationMatrices();

        if (pChirp != NULL)
        {
            delete pChirp;
            pChirp = NULL;
        }

        if (pInverseFilter != NULL)
        {
            delete pInverseFilter;
            pInverseFilter = NULL;
        }

        if (pConvResult != NULL)
        {
            delete pConvResult;
            pConvResult = NULL;
        }

        free_aligned(pData);
        pData          	= NULL;
        vOverBuffer1   	= NULL;
        vOverBuffer2  	= NULL;
        vEnvelopeBuffer	= NULL;

        sOver1.destroy();
        sOver2.destroy();
    }

    status_t SyncChirpProcessor::allocateConvolutionResult(size_t sampleRate, size_t nchannels, size_t count)
    {
        bool bConvResultReAllocate  = false;

        if (pConvResult == NULL)
            bConvResultReAllocate   = true;
        else if ((pConvResult->samples() != count) || (pConvResult->channels() != nchannels))
            bConvResultReAllocate   = true;

        if (bConvResultReAllocate)
        {
            if (pConvResult != NULL)
            {
                delete pConvResult;
                pConvResult         = NULL;
            }

            AudioFile *s            = new AudioFile();
            if (s == NULL)
                return STATUS_NO_MEM;

            status_t status         = s->create_samples(nchannels, nSampleRate, count);
            if (status != STATUS_OK)
            {
                s->destroy();
                delete s;
                return status;
            }

            pConvResult             = s;
        }

        return STATUS_OK;
    }

    status_t SyncChirpProcessor::allocateIdentificationMatrices(size_t order, size_t windowSize)
    {
        /** Allocating all matrices in the same memory block for fast access.
         *  Row Major order, so chunks of data from convolution result can be
         *  directly copied into matrices rows, and we can process matrices
         *  row by row.
         */

    	destroyIdentificationMatrices();

        if ((order == 0) || (windowSize == 0))
            return STATUS_BAD_ARGUMENTS;

        // 2X order by order matrices (Coefficients, Real Part and Imaginary Part)
        // 2X order by windowSize matrices (Higher order responses, Real Part and Imaginary Part)
        // 2X order by windowSize matrices (Kernel responses, Real Part and Imaginary Part)
        // 4X windowSize long temporary vectors
        size_t samples          = 2 * (order * order) + 4 * (order * windowSize) + 4 * windowSize;

        float *ptr              = alloc_aligned<float>(sCRPostProc.pData, samples);
        if (ptr == NULL)
            return STATUS_NO_MEM;

        lsp_guard_assert(float *save = ptr);
        sCRPostProc.mCoeffsRe   = ptr;
        ptr                    += order * order;
        sCRPostProc.mCoeffsIm   = ptr;
        ptr                    += order * order;
        sCRPostProc.mHigherRe   = ptr;
        ptr                    += order * windowSize;
        sCRPostProc.mHigherIm   = ptr;
        ptr                    += order * windowSize;
        sCRPostProc.mKernelsRe  = ptr;
        ptr                    += order * windowSize;
        sCRPostProc.mKernelsIm  = ptr;
        ptr                    += order * windowSize;
        sCRPostProc.vTemprow1Re = ptr;
        ptr                    += windowSize;
        sCRPostProc.vTemprow1Im = ptr;
        ptr                    += windowSize;
        sCRPostProc.vTemprow2Re = ptr;
        ptr                    += windowSize;
        sCRPostProc.vTemprow2Im = ptr;
        ptr                    += windowSize;

        lsp_assert(ptr <= &save[samples]);

        dsp::fill_zero(sCRPostProc.mCoeffsRe, samples);

        sCRPostProc.nHamOrder   = order;
        sCRPostProc.nHwinSize   = windowSize;

        return STATUS_OK;
    }

    void SyncChirpProcessor::destroyIdentificationMatrices()
    {
        free_aligned(sCRPostProc.pData);
        sCRPostProc.pData   = NULL;
        sCRPostProc.nHamOrder   = 0;
        sCRPostProc.nHwinSize   = 0;
        sCRPostProc.mCoeffsRe   = NULL;
        sCRPostProc.mCoeffsIm   = NULL;
        sCRPostProc.mHigherRe   = NULL;
        sCRPostProc.mHigherIm   = NULL;
        sCRPostProc.mKernelsRe  = NULL;
        sCRPostProc.mKernelsIm  = NULL;
        sCRPostProc.vTemprow1Re = NULL;
        sCRPostProc.vTemprow1Im = NULL;
        sCRPostProc.vTemprow2Re = NULL;
        sCRPostProc.vTemprow2Im = NULL;
    }

    inline size_t SyncChirpProcessor::sub2ind_Coeffs(size_t r, size_t c)
    {
        return r * sCRPostProc.nHamOrder + c;
    }

    inline size_t SyncChirpProcessor::sub2ind_Data(size_t r, size_t c)
    {
        return r * sCRPostProc.nHwinSize + c;
    }

    void SyncChirpProcessor::fillCoefficientsMatrices()
    {
        if (
            (sCRPostProc.mCoeffsRe == NULL) ||
            (sCRPostProc.mCoeffsIm == NULL) ||
            (sCRPostProc.nHamOrder == 0)
           )
            return;

        dsp::fill_zero(sCRPostProc.mCoeffsRe, sCRPostProc.nHamOrder * sCRPostProc.nHamOrder);
        dsp::fill_zero(sCRPostProc.mCoeffsIm, sCRPostProc.nHamOrder * sCRPostProc.nHamOrder);

        // For triangular matrices, like this one, the determinant is the product of the diagonal elements.
        double determinantRe    = 1.0;
        double determinantIm    = 0.0;

        // Stores double precision matrix values for determinant calculation
        double valueRe          = 0.0;
        double valueIm          = 0.0;

        // Row major order: better to start loops from columns
        for (size_t c = 0; c < sCRPostProc.nHamOrder; ++c)
        {
            // This factor multiplies all the values in the column.
            double columnFactor = pow(sChirpParams.fAlpha, c); // Since we have the convolution result already scaled by 1 / fAlpha, the exponent is c instead of c + 1

            for (size_t r = 0; r <= c; ++r)  // This is a lower triangular matrix
            {
                size_t k = sub2ind_Coeffs(r, c);
                size_t n = r + 1;
                size_t m = c + 1;

                // Only the values for even m + n can (and have) to be calculated.
                if ((m + n) % 2 == 0)
                {
                    // This real factor is present for all elements. If m + n is even,
                    // then m - n is also even and (m - n) >> 1 is its exact
                    // half.
                    double reCoeff  = columnFactor * exp2(1.0 - m) * nchoosek(m, (m - n) >> 1);

                    // To complete the element calculation, we need to multiply
                    // reCoeff by (-1)^exponent, with exponent = 2.0 * m - 0.5 * (n - 1.0).
                    // If exponent is integer, then (-1)^exponent is 1 if
                    // exponent is even and -1 if exponent is odd. In all these
                    // cases the final result is real. exponent is integer if
                    // and only if n - 1 is even.
                    if ((n - 1) % 2 == 0)
                    {
                        ssize_t exponent = 2 * m;

                        if (n != 1)
                            exponent -= ssize_t((n - 1) >> 1);

                        if (exponent % 2 == 0)
                        {
                            sCRPostProc.mCoeffsRe[k]    = float(reCoeff);
                            valueRe                     = reCoeff;
                            valueIm                     = 0.0;
                        }
                        else
                        {
                            sCRPostProc.mCoeffsRe[k]    = float(-reCoeff);
                            valueRe                     = -reCoeff;
                            valueIm                     = 0.0;
                        }
                    }
                    else
                    {
                        // If exponent is not integer (-1)^exponent is complex.
                        // It is calculated here by Euler's formula, as -1 = exp(i * pi).
                        // So, we need to multiply pi by exponent and go back
                        // to real part and imaginary part with Euler's formula.
                        // This is done below, but the argument for cos and sin
                        // defining the real and imaginary parts is further wrapped
                        // within [0 and 2pi].
                        double exponent = 2.0 * m - 0.5 * (n - 1.0);
                        double angle    = M_PI * (exponent - 2.0 * floor(0.5 * exponent)); // Wrapped within [0, 2 * M_PI] for maximal accuracy

                        valueRe                     = reCoeff * cos(angle);
                        valueIm                     = reCoeff * sin(angle);

                        sCRPostProc.mCoeffsRe[k]    = float(valueRe);
                        sCRPostProc.mCoeffsIm[k]    = float(valueIm);
                    }
                }

                if (r == c)
                {
                    double determinantRe_temp   = determinantRe;
                    double determinantIm_temp   = determinantIm;
                    determinantRe   = (determinantRe_temp * valueRe) - (determinantIm_temp * valueIm);
                    determinantIm   = (determinantRe_temp * valueIm) + (determinantIm_temp * valueRe);
                }
            }
        }

        sCRPostProc.mCoeffsReDet = determinantRe;
        sCRPostProc.mCoeffsImDet = determinantIm;
    }

    void SyncChirpProcessor::solve()
    {
        if (
            (sCRPostProc.mCoeffsRe      == NULL)    ||
            (sCRPostProc.mCoeffsIm      == NULL)    ||
            (sCRPostProc.nHamOrder      == 0)       ||
            (sCRPostProc.mHigherRe      == NULL)    ||
            (sCRPostProc.mHigherIm      == NULL)    ||
            (sCRPostProc.mKernelsRe     == NULL)    ||
            (sCRPostProc.mKernelsIm     == NULL)    ||
            (sCRPostProc.nHwinSize      == 0)       ||
            (sCRPostProc.vTemprow1Re    == NULL)    ||
            (sCRPostProc.vTemprow1Im    == NULL)    ||
            (sCRPostProc.vTemprow2Re    == NULL)    ||
            (sCRPostProc.vTemprow2Im    == NULL)
           )
            return;

        // Fill with zeros all the kernels matrices and temp vectors
        dsp::fill_zero(sCRPostProc.mKernelsRe, sCRPostProc.nHamOrder * sCRPostProc.nHwinSize);
        dsp::fill_zero(sCRPostProc.mKernelsIm, sCRPostProc.nHamOrder * sCRPostProc.nHwinSize);

        dsp::fill_zero(sCRPostProc.vTemprow1Re, sCRPostProc.nHwinSize);
        dsp::fill_zero(sCRPostProc.vTemprow1Im, sCRPostProc.nHwinSize);

        dsp::fill_zero(sCRPostProc.vTemprow2Re, sCRPostProc.nHwinSize);
        dsp::fill_zero(sCRPostProc.vTemprow2Im, sCRPostProc.nHwinSize);

        // We aim to solve the linear systems Coeffs * Kernels = Higher for the unknown matrix Kernels (a linear system per column of Kernels and Higher)
        // Coeffs is upper triangular. So we use backward substitution.
        // Kernels = mathematical notation for sCRPostProc.mKernelsRe + im * sCRPostProc.mKernelsIm,
        // and Coeffs and Higher similarly.

        // In backward substituion, we calculate the rows of Kernels starting from the bottom, as that has the simplest solution and we have all the data
        // for it. Then, the other rows are calculated one by one by using all the rows previously calculated.

        // To be noted: the equation above is valid only for positive frequenc. The negative ones need to be calculated by conjugating the Coeff elements,
        // so that we preserve Hermitian symmetry.

        size_t nyquist  = sCRPostProc.nHwinSize / 2;
        size_t negStart = (sCRPostProc.nHwinSize - nyquist + 1) % sCRPostProc.nHwinSize; // Negative samples start here.

        // This in the index witch which we step through the rows, bottom to top.
        ssize_t r = sCRPostProc.nHamOrder - 1;

        while (r >= 0)
        {
            // The formula for the Kernels row elements of the selected r row is:
            // Kernels[r, :] = (Higher[r, :] - sum from c = r + 1 to HamOrder Coeffs[r, c] .*  Kernels[c, :]) ./ Coeffs[r, r]
            // Where * is complex element-wise multiplication, / is complex element-wise division and : means "all elements'.
            // As a reminder, Coeffs is a HamOrder by HamOrder matrix;
            //                Kernels is a HamOrder by WinSize matrix;
            //                Higher is a HamOrder by WinSize matrix.

            // We start by copying Higher[r, :] over to Kernels[r, :]
            size_t rowSelect    = sub2ind_Data(r, 0);
            dsp::copy(&sCRPostProc.mKernelsRe[rowSelect], &sCRPostProc.mHigherRe[rowSelect], sCRPostProc.nHwinSize);
            dsp::copy(&sCRPostProc.mKernelsIm[rowSelect], &sCRPostProc.mHigherIm[rowSelect], sCRPostProc.nHwinSize);

            // We will then accumulate the sum into Temprow1
            dsp::fill_zero(sCRPostProc.vTemprow1Re, sCRPostProc.nHwinSize);
            dsp::fill_zero(sCRPostProc.vTemprow1Im, sCRPostProc.nHwinSize);

            for (size_t c = r + 1; c < sCRPostProc.nHamOrder; ++c)
            {
                size_t coeffIdx     = sub2ind_Coeffs(r, c);
                size_t kRowSelect   = sub2ind_Data(c, 0);

                // We fill Temprow2 with Coeffs[r, c], so that we can do element-wise complex multiplication of Coeffs[r, c]
                // and Kernels[c, :]. We do the multiplication in place into Temprow2.
                // Make Hermitian Vector.
                dsp::fill(sCRPostProc.vTemprow2Re, sCRPostProc.mCoeffsRe[coeffIdx], sCRPostProc.nHwinSize);
                dsp::fill(sCRPostProc.vTemprow2Im, sCRPostProc.mCoeffsIm[coeffIdx], negStart - 1);
                dsp::fill(&sCRPostProc.vTemprow2Im[negStart], -sCRPostProc.mCoeffsIm[coeffIdx], sCRPostProc.nHwinSize - negStart);

                dsp::complex_mul3(
                        sCRPostProc.vTemprow2Re, sCRPostProc.vTemprow2Im,
                        sCRPostProc.vTemprow2Re, sCRPostProc.vTemprow2Im,
                        &sCRPostProc.mKernelsRe[kRowSelect], &sCRPostProc.mKernelsIm[kRowSelect],
                        sCRPostProc.nHwinSize
                        );

                dsp::add2(sCRPostProc.vTemprow1Re, sCRPostProc.vTemprow2Re, sCRPostProc.nHwinSize);
                dsp::add2(sCRPostProc.vTemprow1Im, sCRPostProc.vTemprow2Im, sCRPostProc.nHwinSize);
            }

            // Now we can subtract in place the accumulated sum from Kernels[r, :] which, being initialized to
            // Higher[r, :], yields to the numerator of the expression.
            dsp::sub2(&sCRPostProc.mKernelsRe[rowSelect], sCRPostProc.vTemprow1Re, sCRPostProc.nHwinSize);
            dsp::sub2(&sCRPostProc.mKernelsIm[rowSelect], sCRPostProc.vTemprow1Im, sCRPostProc.nHwinSize);

            // We just need to element-wise divide this numerator by Coeffs[r, r], which is the same as
            // element-wise multiplying with the complex inverse of Coeffs[r, r]
            float coeffRe       = 0.0f;
            float coeffIm       = 0.0f;
            size_t coeffIdx     = sub2ind_Coeffs(r, r);
            dsp::complex_rcp2(&coeffRe, &coeffIm, &sCRPostProc.mCoeffsRe[coeffIdx], &sCRPostProc.mCoeffsIm[coeffIdx], 1);

            // Make Hermitian vector
            dsp::fill(sCRPostProc.vTemprow2Re, coeffRe, sCRPostProc.nHwinSize);
            dsp::fill(sCRPostProc.vTemprow2Im, coeffIm, negStart - 1);
            dsp::fill(&sCRPostProc.vTemprow2Im[negStart], -coeffIm, sCRPostProc.nHwinSize - negStart);

            dsp::complex_mul3(
                    &sCRPostProc.mKernelsRe[rowSelect], &sCRPostProc.mKernelsIm[rowSelect],
                    &sCRPostProc.mKernelsRe[rowSelect], &sCRPostProc.mKernelsIm[rowSelect],
                    sCRPostProc.vTemprow2Re, sCRPostProc.vTemprow2Im,
                    sCRPostProc.nHwinSize
                    );

            --r;
        }
    }

    void SyncChirpProcessor::force_kernels_DC_block()
    {
        if (
            (sCRPostProc.mKernelsRe     == NULL)    ||
            (sCRPostProc.mKernelsIm     == NULL)    ||
            (sCRPostProc.nHwinSize      == 0)
           )
            return;

        size_t nyquist = sCRPostProc.nHwinSize / 2;

        // Sample below which the responses are below the chirp starting frequency.
        // We impose the responses to be 0 here (there is no data here).
        size_t dcSamples = 2.0 * nyquist * sChirpParams.initialFrequency / nSampleRate;
        dcSamples = (dcSamples > 0) ? dcSamples : 1;

        // The low frequency is at the very left of each frequency response.
        for (size_t m = 1; m <= sCRPostProc.nHamOrder; ++m)
        {
            size_t rowSelect = sub2ind_Data(m - 1, 0);
            dsp::fill_zero(&sCRPostProc.mKernelsRe[rowSelect], dcSamples);
            dsp::fill_zero(&sCRPostProc.mKernelsIm[rowSelect], dcSamples);
        }
    }

    void SyncChirpProcessor::windowHigherOrderResponses(size_t channel, bool doInnerSmoothing, size_t nFadeIn, size_t nFadeOut, windows::window_t windowType) //size_t offset)
    {
        if (
            (sCRPostProc.nHamOrder      == 0)       ||
            (sCRPostProc.mHigherRe      == NULL)    ||
            (sCRPostProc.mHigherIm      == NULL)    ||
            (sCRPostProc.nHwinSize      == 0)       ||
            (sCRPostProc.vTemprow1Re    == NULL)    ||
            (sCRPostProc.vTemprow1Im    == NULL)    ||
            (sCRPostProc.vTemprow2Re    == NULL)    ||
            (sCRPostProc.vTemprow2Im    == NULL)
           )
            return;

        if (pConvResult == NULL)
            return;

        if (channel >= sConvParams.nChannels)
        	return;

        size_t dataLength = pConvResult->samples();
        if (dataLength == 0)
            return;

        float *vResult          = pConvResult->channel(channel);
        if (vResult == NULL)
        	return;

        // We locate the center of the convolution result, that acts as a
        // reference point (origin of time). Beware that for linear phase
        // systems we will see a linear impulse response -centered- around this.
        size_t timeOrigin       = (dataLength / 2) - 1;
        size_t maxCount         = dataLength - timeOrigin;

        // We will fill the matrix of higher order responses with the higher
        // order frequency responses. So, we first fill everything with zero,
        // just in case there was some rubbish.
        // Then, we move along the negative time zone of the convolution
        // result, and we find the centres of the higher order impulse responses.
        // To window them, we get as ahead as we can from their center, and we
        // copy all the way to farthest possible from the centre. The reason
        // is that the responses are log spaced.
        // Then, we copy the samples into the row of the mHigher matrix, taking
        // care that the centre is aligned to the middle of the row.
        // Finally, we Fourier transform and, since the copyheads were floats,
        // but we actually used close integer indexes, we compensate in the
        // frequency domain by doing noninteger sample shift.
        dsp::fill_zero(sCRPostProc.mHigherRe, sCRPostProc.nHamOrder * sCRPostProc.nHwinSize);
        dsp::fill_zero(sCRPostProc.mHigherIm, sCRPostProc.nHamOrder * sCRPostProc.nHwinSize);

        dsp::fill_zero(sCRPostProc.vTemprow1Re, sCRPostProc.nHwinSize);
        dsp::fill_zero(sCRPostProc.vTemprow1Im, sCRPostProc.nHwinSize);
        dsp::fill_zero(sCRPostProc.vTemprow2Re, sCRPostProc.nHwinSize);
        dsp::fill_zero(sCRPostProc.vTemprow2Im, sCRPostProc.nHwinSize);;

        // Nyquist sample of the higher order frequency responses
        size_t nyquist              = sCRPostProc.nHwinSize / 2;

        double gap2prev             = maxCount;
        double halfWindowWidth      = 0.5 * sCRPostProc.nHwinSize;

        for (size_t m = 1; m <= sCRPostProc.nHamOrder; ++m)
        {
            double higherOrigin     = timeOrigin - seconds_to_samples(nSampleRate, sChirpParams.gamma * log(m));
            double gap2next         = seconds_to_samples(nSampleRate, sChirpParams.gamma * log(1.0 + 1.0 / m));
            if (m > 1)
                gap2prev            = seconds_to_samples(nSampleRate, sChirpParams.gamma * log(m / (m - 1.0)));

            double maxAhead         = 0.5 * gap2next;
            double maxBehind        = 0.5 * gap2prev;

            double headGap          = (maxAhead > halfWindowWidth) ? halfWindowWidth : maxAhead;
            double tailGap          = (maxBehind > halfWindowWidth) ? halfWindowWidth : maxBehind;

            double dCopyHead        = higherOrigin - headGap;

            if (dCopyHead < 0)
                break;

            size_t nCopyHead    = dCopyHead;

            size_t copyCount    = headGap + tailGap;

            double dWindowHead  = halfWindowWidth - headGap;
            size_t nWindowHead  = dWindowHead;

            dsp::fill_zero(sCRPostProc.vTemprow1Re, sCRPostProc.nHwinSize);
            dsp::fill_zero(sCRPostProc.vTemprow1Im, sCRPostProc.nHwinSize);
            dsp::copy(&sCRPostProc.vTemprow1Re[nWindowHead], &vResult[nCopyHead], copyCount);

            // Applying the smoothing fade-in and fade out to the data.
            if (doInnerSmoothing)
            {
                size_t fadeInLength     = (nFadeIn < headGap) ? nFadeIn: headGap;
                size_t fadeOutLength    = (nFadeOut < tailGap) ? nFadeOut : tailGap;

                float *fadeHead         = &sCRPostProc.vTemprow1Re[nWindowHead];

                for (size_t n = 0; n < fadeInLength; ++n)
                {
                    fadeHead[n] *= 0.5f * (sin(M_PI * (double(n) / fadeInLength - 0.5)) + 1.0f);
                }

                fadeHead                = &sCRPostProc.vTemprow1Re[nWindowHead + copyCount - fadeOutLength - 1];

                for (size_t n = 1; n <= fadeOutLength; ++n)
                {
                    fadeHead[n] *= 0.5f * (sin(-M_PI * (double(n) / fadeOutLength - 0.5)) + 1.0f);
                }
            }

            // Applying overall smoothing window
            windows::window(sCRPostProc.vTemprow2Re, sCRPostProc.nHwinSize, windowType);

            dsp::mul2(sCRPostProc.vTemprow1Re, sCRPostProc.vTemprow2Re, sCRPostProc.nHwinSize);

            dsp::direct_fft(
                    sCRPostProc.vTemprow2Re, sCRPostProc.vTemprow2Im,
                    sCRPostProc.vTemprow1Re, sCRPostProc.vTemprow1Im,
                    sCRPostProc.nWinRank
                    );

            double shift        = nCopyHead - dCopyHead + dWindowHead - nWindowHead;

            size_t rowSelect    = sub2ind_Data(m - 1, 0);

            for (size_t k = 0; k <= nyquist; ++k)
            {
                size_t p            = (sCRPostProc.nHwinSize - k) % sCRPostProc.nHwinSize;

                double delayFactor  = shift * double(k) / sCRPostProc.nHwinSize;
                double angle        = 2.0 * M_PI * (delayFactor - floor(delayFactor)); // Wrapped within [0, 2 * M_PI]

                sCRPostProc.vTemprow1Re[k] = cos(angle);
                sCRPostProc.vTemprow1Im[k] = -sin(angle);

                if ((k != 0) && k != nyquist)
                {
                    sCRPostProc.vTemprow1Re[p] = sCRPostProc.vTemprow1Re[k];
                    sCRPostProc.vTemprow1Im[p] = -sCRPostProc.vTemprow1Im[k];
                }

            }

            dsp::complex_mul3(
                    &sCRPostProc.mHigherRe[rowSelect], &sCRPostProc.mHigherIm[rowSelect],
                    sCRPostProc.vTemprow2Re, sCRPostProc.vTemprow2Im,
                    sCRPostProc.vTemprow1Re, sCRPostProc.vTemprow1Im,
                    sCRPostProc.nHwinSize
                    );
        }
    }

    status_t SyncChirpProcessor::fill_with_kernel_taps(float *dst)
    {
        if (
                (sCRPostProc.mKernelsRe     == NULL)    ||
                (sCRPostProc.mKernelsIm     == NULL)
                )
            return STATUS_NO_DATA;

        if (dst == NULL)
            return STATUS_BAD_ARGUMENTS;

        for (size_t o = 0; o < sCRPostProc.nHamOrder; ++o)
        {
            size_t rowSelect = sub2ind_Data(o, 0);

            dsp::reverse_fft(
                    &dst[rowSelect], sCRPostProc.vTemprow1Im,
                    &sCRPostProc.mKernelsRe[rowSelect], &sCRPostProc.mKernelsIm[rowSelect],
                    sCRPostProc.nWinRank
                  );

        }

        return STATUS_OK;
    }

    status_t SyncChirpProcessor::get_kernel_fir(float *dst, size_t order)
    {
        if (
                (sCRPostProc.mKernelsRe     == NULL)    ||
                (sCRPostProc.mKernelsIm     == NULL)
                )
            return STATUS_NO_DATA;

        if (
                (dst == NULL) ||
                (order < 1) ||
                (order > sCRPostProc.nHamOrder)
                )
            return STATUS_BAD_ARGUMENTS;

        size_t rowSelect = sub2ind_Data(order - 1, 0);

        dsp::reverse_fft(
                dst, sCRPostProc.vTemprow1Im,
                &sCRPostProc.mKernelsRe[rowSelect], &sCRPostProc.mKernelsIm[rowSelect],
                sCRPostProc.nWinRank
              );

        return STATUS_OK;
    }

    inline double SyncChirpProcessor::calculate_chirp_sample(size_t sampleRate, size_t chirpIdx)
    {
        double phase        = sChirpParams.delta * (exp(chirpIdx / (sampleRate * sChirpParams.gamma)) - 1.0);
        phase               = phase - 2.0 * M_PI * floor(phase * M_1_PI * 0.5); // Wrap within [0, 2 * M_PI] for maximal accuracy
        return              sin(phase);
    }

    inline double SyncChirpProcessor::calculate_inverse_filter_sample(size_t sampleRate, double chirpValue, size_t chirpIdx)
    {
        return (chirpValue * exp(chirpIdx / (sampleRate * sChirpParams.gamma)) * 2.0 * sChirpParams.beta * M_1_PI / sChirpParams.gamma);
    }

    float SyncChirpProcessor::calculate_fading_window_sample(size_t windowIdx)
    {
        switch (sFader.enMethod)
        {
            case SCP_FADE_NONE:
                return 1.0f;
            case SCP_FADE_RAISED_COSINES:
            {
                size_t fadeInLength     = 0;
                size_t fadeOutLength    = 0;
                size_t chirpDuration    = 0;

                switch (sChirpParams.enMethod)
                {
                    case SCP_SYNTH_SIMPLE:
                        fadeInLength    = sFader.nFadeIn;
                        fadeOutLength   = sFader.nFadeOut;
                        chirpDuration   = sChirpParams.nDuration;
                        break;

                    case SCP_SYNTH_CHIRPBANDLIMITED:
                    case SCP_SYNTH_BANDLIMITED:
                        fadeInLength    = sFader.nFadeIn_Over;
                        fadeOutLength   = sFader.nFadeOut_Over;
                        chirpDuration   = nOversampling * sChirpParams.nDuration;
                        break;

                    default:
                        fadeInLength    = 0;
                        fadeOutLength   = 0;
                        chirpDuration   = sChirpParams.nDuration;
                        break;
                }

                if (windowIdx < fadeInLength)
                    return (0.5f * (1.0f - cosf(M_PI * windowIdx / fadeInLength)));
                else if ((windowIdx > (chirpDuration - fadeOutLength)) && (windowIdx < chirpDuration))
                    return (0.5f * (1.0f - cosf(M_PI * (chirpDuration - windowIdx) / fadeOutLength)));
                else if (windowIdx >= chirpDuration)
                    return 0.0f;
                else
                    return 1.0f;
            }
            default:
                return 1.0f;
        }
    }

    status_t SyncChirpProcessor::profile_background_noise(size_t channel, size_t head, size_t count)
    {
        if (pConvResult == NULL)
            return STATUS_NO_DATA;

		if (channel >= sConvParams.nChannels)
			return STATUS_BAD_ARGUMENTS;

        size_t dataLength = pConvResult->samples();
        if (dataLength == 0)
            return STATUS_NO_DATA;

        if (head >= (dataLength - count))
            return STATUS_BAD_ARGUMENTS;

        float *vResult = pConvResult->channel(channel);
		if (vResult == NULL)
			return STATUS_BAD_ARGUMENTS;

        double noisePeak        = dsp::abs_max(&vResult[head], count);

        sCRPostProc.noiseLevel  = ceil(20.0 * log10(noisePeak));
        sCRPostProc.noiseValue  = exp(M_LN10 * 0.05 * sCRPostProc.noiseLevel);

        return STATUS_OK;
    }

    status_t SyncChirpProcessor::calibrate_backwards_integration_limit(size_t channel, size_t head, size_t windowSize, double tolerance)
    {
        if (pConvResult == NULL)
            return STATUS_NO_DATA;

		if (channel >= sConvParams.nChannels)
			return STATUS_BAD_ARGUMENTS;

        size_t dataLength = pConvResult->samples();
        if (dataLength == 0)
            return STATUS_NO_DATA;

        if (head >= dataLength)
            return STATUS_BAD_ARGUMENTS;

        float *vResult = pConvResult->channel(channel);
        if (vResult == NULL)
        	return STATUS_BAD_ARGUMENTS;

        size_t samples          = dataLength - head;
        float *vData            = &vResult[head];

        windowSize              = (windowSize < ENVELOPE_BUF_LIMIT_SIZE) ? windowSize : ENVELOPE_BUF_LIMIT_SIZE;

        // Let's start searching for the point at which the envelope fades into
        // noise from the right of the peak: it is not gonna be earlier than
        // there.
        size_t peakIdx          = dsp::abs_max_index(vData, samples);

        // We will now follow the envelope of the signal with our window, and
        // track when it fades below the noise floor.

        size_t integrationLimit = samples;

        bool doSearch           = 20.0 * log10(fabs(vData[peakIdx])) > (sCRPostProc.noiseLevel + tolerance);

        while (doSearch)
        {
            dsp::fill_zero(vEnvelopeBuffer, windowSize);
            size_t windowHead       = 0;
            size_t windowPeakIdx    = 0;

            for (size_t n = peakIdx; n < samples; ++n)
            {
                ++windowHead;
                windowHead          = windowHead % windowSize;

                vEnvelopeBuffer[windowHead] = fabs(vData[n]);

                if (windowPeakIdx == windowHead)
                    windowPeakIdx   = dsp::max_index(vEnvelopeBuffer, windowSize);
                else if (vEnvelopeBuffer[windowHead] > vEnvelopeBuffer[windowPeakIdx])
                    windowPeakIdx   = windowHead;

                double peak         = vEnvelopeBuffer[windowPeakIdx];

                if (peak <= sCRPostProc.noiseValue)
                {
                    integrationLimit    = n;
                    peakIdx             = dsp::abs_max_index(&vData[n], samples - n) + n; // Need to offset properly this index so that it indexes into vData.
                    doSearch            = 20.0 * log10(fabs(vData[peakIdx])) > (sCRPostProc.noiseLevel + tolerance);
                    break;
                }
            }
        }

        // This integration limit will be relative to head.
        sCRPostProc.nIrLimit    = integrationLimit;
        sCRPostProc.fIrLimit    = samples_to_seconds(nSampleRate, sCRPostProc.nIrLimit);

        return STATUS_OK;
    }

    status_t SyncChirpProcessor::calculate_reverberation_time(size_t channel, size_t head, double decayThreshold, double highRegLevel, double lowRegLevel, size_t limit)
    {
        if (pConvResult == NULL)
            return STATUS_NO_DATA;

		if (channel >= sConvParams.nChannels)
			return STATUS_BAD_ARGUMENTS;

        size_t dataLength = pConvResult->samples();
        if (dataLength == 0)
            return STATUS_NO_DATA;

        if ((highRegLevel > 0) || (lowRegLevel > 0) || (highRegLevel <= lowRegLevel) || (head >= dataLength))
            return STATUS_BAD_ARGUMENTS;

        // Extract estimate of RT from positive time impulse response.
        // While scanning through the IR:
        // Find the decay energy curve values
        // Find the regression line between the Regression thresholds.
        // Find the time at which the regression line reaches the low decay threshold.

        // Backwards integration of the squared impulse response supplies the average
        // energy decay curve. This curve is maximal at 0 time and decays as time
        // gets larger. Here we use it normalised: so the 0 time value is 1.

        float *vResult          = pConvResult->channel(channel);
        if (vResult == NULL)
        	return STATUS_BAD_ARGUMENTS;

        size_t samples          = dataLength - head;
        size_t count            = (samples > limit) ? limit : samples;
        float *vData            = &vResult[head];

        // Initialise the squared impulse response summer and statistics at
        // iteration 0.

        double lookAheadSum     = 0.0;

        for (size_t n = 0; n < count; ++n)
        {
            double value        = vData[n];
            lookAheadSum       += (value * value);
        }

        double energyDecayNorm  = lookAheadSum;                         // This is the norm to normalise the decay values
        double convolutionNorm  = sqrt(nSampleRate / lookAheadSum);     // This is the norm to normalise the impulse response to its total energy
        double energyDecayValue = 1.0;                                  // First value is then 1: lookAheadSum / energyDecayNorm
        double energyDecayLevel = 0.0;                                  // Correspondent decay level [dB]

        double energyDecayMean  = 0;                                    // Decay mean
        double energyDecaySqErr = 0;                                    // Decay Squared Error (from the mean)
        double samplesMean      = 0;                                    // Samples Mean
        double samplesSqErr     = 0;                                    // Samples Squared Error (from the mean)
        double comoment         = 0;                                    // Co-moment for co-variance calculation
        size_t counter          = 1;                                    // Counts processed samples

        // Subsequent iterations by simply updating the values (online algorithms)
        for (size_t m = 1; m < count; ++m)
        {
            double value        = vData[m - 1];
            lookAheadSum       -= (value * value);

            energyDecayValue    = lookAheadSum / energyDecayNorm;
            energyDecayLevel    = 10.0 * log10(energyDecayValue);

            // Finish calculation?
            if (energyDecayLevel < lowRegLevel)
                break;

            // Do linear regression?
            if (energyDecayLevel <= highRegLevel)
            {
                double samplesMean_previous         = samplesMean;
                double energyDecayMean_previous     = energyDecayMean;

                energyDecayMean                    += (energyDecayLevel - energyDecayMean) / counter;
                energyDecaySqErr                   += (energyDecayLevel - energyDecayMean) * (energyDecayLevel - energyDecayMean_previous);
                samplesMean                        += (m - samplesMean) / counter;
                samplesSqErr                       += (m - samplesMean) * (m - samplesMean_previous);
                comoment                           += (m - samplesMean) * (energyDecayLevel - energyDecayMean_previous);

                ++counter;
            }
        }

        // Regression line parameters (all variance normalisation factors get erased):
        double slope            = comoment / samplesSqErr;
        double intercept        = energyDecayMean - slope * samplesMean;
        double correlation      = sqrt(samplesSqErr * energyDecaySqErr);

        // RT extrapolation:
        sCRPostProc.nRT             = ((decayThreshold - intercept) / slope); // - middle + head; // to make relative to middle
        sCRPostProc.fRT             = samples_to_seconds(nSampleRate, sCRPostProc.nRT);
        sCRPostProc.fCorrelation    = (correlation == 0.0) ? correlation : comoment / correlation; // Avoid NANs

        // Other data:
        sCRPostProc.noiseValueNorm  = convolutionNorm * sCRPostProc.noiseValue;
        sCRPostProc.noiseLevelNorm  = 20.0 * log10(sCRPostProc.noiseValueNorm);

        sCRPostProc.bLowNoise       = (sCRPostProc.noiseLevelNorm < (lowRegLevel + BG_NOISE_LIMIT));

        return STATUS_OK;
    }

    status_t SyncChirpProcessor::calculate_reverberation_time(size_t channel, size_t head, scp_rtcalc_t rtCalc, size_t limit)
    {
        switch (rtCalc)
        {
            case SCP_RT_EDT_0:
                return calculate_reverberation_time(channel, head, -60.0,  0.0, -10.0, limit);
            case SCP_RT_EDT_1:
                return calculate_reverberation_time(channel, head, -60.0, -1.0, -10.0, limit);
            case SCP_RT_T_10:
                return calculate_reverberation_time(channel, head, -60.0, -5.0, -15.0, limit);
            case SCP_RT_T_20:
                return calculate_reverberation_time(channel, head, -60.0, -5.0, -25.0, limit);
            case SCP_RT_T_30:
                return calculate_reverberation_time(channel, head, -60.0, -5.0, -35.0, limit);
            default:
                return calculate_reverberation_time(channel, head, -60.0, -5.0, -25.0, limit);
        }
    }

    double SyncChirpProcessor::nchoosek(size_t n, size_t k)
    {
        if (n < k)
            return 0;

        double binomialCoefficient = 1.0;

        if ((k == 0) || (k == n))
            return binomialCoefficient;

        // nchoosek(n, k) == nchoosek(n - k, k), but the latter needs less
        // iterations if the below is true.
        if (k > (n >> 1))
            k = n - k;

        // Using doubles as integer division will give the wrong result
        for (double i = 1; i <= k; ++i)
        {
            binomialCoefficient *= (double(n + 1 - i) / i);
        }

        return round(binomialCoefficient);
    }

    status_t SyncChirpProcessor::reconfigure()
    {
        if (bSync)
            update_settings();

        if (!sChirpParams.bReconfigure)
            return STATUS_OK;

        // Chirp

        bool bChirpReAllocate   = false;
        if ((pChirp == NULL) || (!pChirp->valid()))
            bChirpReAllocate    = true;
        else if ((pChirp->length() != sChirpParams.nDuration) || (pChirp->channels() != 1))
            bChirpReAllocate    = true;

        if (bChirpReAllocate)
        {
            delete pChirp;
            pChirp              = NULL;

            Sample *s           = new Sample();
            if (s == NULL)
                return STATUS_NO_MEM;

            if (!s->init(1, sChirpParams.nDuration, sChirpParams.nDuration))
            {
                s->destroy();   // Do not forget to free previously allocated data!
                delete s;
                return STATUS_NO_MEM;
            }

            pChirp              = s;
        }

        // Inverse filter

        bool bInvReAllocate     = false;
        if ((pInverseFilter == NULL) || (!pInverseFilter->valid()))
            bInvReAllocate      = true;
        else if ((pInverseFilter->length() != sChirpParams.nDuration) || (pInverseFilter->channels() != 1))
            bInvReAllocate      = true;

        if (bInvReAllocate)
        {
            delete pInverseFilter;
            pInverseFilter      = NULL;

            Sample *s           = new Sample();
            if (s == NULL)
                return STATUS_NO_MEM;

            if (!s->init(1, sChirpParams.nDuration, sChirpParams.nDuration))
            {
                s->destroy();
                delete s;
                return STATUS_NO_MEM;
            }

            pInverseFilter      = s;
        }

        // Calculate all time series -------------------------------------------

        float *vChirp           = pChirp->getBuffer(0);
        float *vInverseFilter   = pInverseFilter->getBuffer(0);

        switch (sChirpParams.enMethod)
        {
            case SCP_SYNTH_SIMPLE:
            {
                for (size_t n = 0; n < sChirpParams.nDuration; ++n)
                {
                    size_t k            = sChirpParams.nDuration - 1 - n;
                    double value        = calculate_chirp_sample(nSampleRate, n);

                    vChirp[n]           = sChirpParams.fAlpha * float(value) * calculate_fading_window_sample(n);
                    vInverseFilter[k]   = float(calculate_inverse_filter_sample(nSampleRate, value, n));
                }
            }
            break;

            case SCP_SYNTH_CHIRPBANDLIMITED:
            {
                for (size_t n = 0; n < sChirpParams.nDuration; ++n)
                {
                    size_t k            = sChirpParams.nDuration - 1 - n;
                    double value        = calculate_chirp_sample(nSampleRate, n);

                    vInverseFilter[k]   = float(calculate_inverse_filter_sample(nSampleRate, value, n));
                }

                size_t to_synth         = nOversampling * sChirpParams.nDuration;
                size_t n                = 0;

                while (to_synth > 0)
                {
                    size_t to_do = (to_synth > OVER_BUF_LIMIT_SIZE) ? OVER_BUF_LIMIT_SIZE : to_synth;

                    for (size_t m = 0; m < to_do; ++m)
                    {
                        double value    = calculate_chirp_sample(nOversampling * nSampleRate, n);
                        vOverBuffer1[m] = sChirpParams.fAlpha * float(value) * calculate_fading_window_sample(n);
                        ++n;
                    }

                    sOver1.downsample(vChirp, vOverBuffer1, to_do / nOversampling);

                    to_synth           -= to_do;
                    vChirp             += (to_do / nOversampling);
                }
            }
            break;

            case SCP_SYNTH_BANDLIMITED:
            {
                size_t inverseHead  = sChirpParams.nDuration;
                size_t to_synth     = nOversampling * sChirpParams.nDuration;
                size_t n            = 0;

                while (to_synth > 0)
                {
                    size_t to_do    = (to_synth > OVER_BUF_LIMIT_SIZE) ? OVER_BUF_LIMIT_SIZE : to_synth;

                    for (size_t m = 0; m < to_do; ++m)
                    {
                        double value        = calculate_chirp_sample(nOversampling * nSampleRate, n);
                        vOverBuffer1[m]     = sChirpParams.fAlpha * float(value) * calculate_fading_window_sample(n);
                        vOverBuffer2[m]     = float(calculate_inverse_filter_sample(nOversampling * nSampleRate, value, n));
                        ++n;
                    }

                    sOver1.downsample(vChirp, vOverBuffer1, to_do / nOversampling);

                    sOver2.downsample(&vInverseFilter[inverseHead - (to_do / nOversampling)], vOverBuffer2, to_do / nOversampling);
                    dsp::reverse1(&vInverseFilter[inverseHead - (to_do / nOversampling)], to_do / nOversampling);

                    to_synth       -= to_do;
                    vChirp         += (to_do / nOversampling);
                    inverseHead    -= (to_do / nOversampling);
                }
            }
            break;

            default:
                return STATUS_UNSPECIFIED;
        }

        sChirpParams.bReconfigure       = false;

        return STATUS_OK;
    }

    void SyncChirpProcessor::calculateConvolutionPartitionSize(size_t partSizeLimit)
    {
    	partSizeLimit = (partSizeLimit < MAX_PART_SIZE) ? partSizeLimit : MAX_PART_SIZE;
    	partSizeLimit = (partSizeLimit == 0) ? MAX_PART_SIZE : partSizeLimit;

    	// Force the partition size to be a power of 2 and find convolution rank
    	size_t nPartitionSize	= 1;
		size_t nExponent 		= 0;

		while (nPartitionSize < partSizeLimit)
		{
			nPartitionSize <<= 1;
			++nExponent;
		}

		size_t nConvRank 	= nExponent + 1; 		// Rank of each partition convolution
		size_t nImage 		= 1 << (nConvRank + 1); // Size of the convolution image

		sConvParams.bReallocateTemp = false;

		if (nPartitionSize != sConvParams.nPartitionSize)
		{
			sConvParams.bReallocateTemp	= true;
			sConvParams.nPartitionSize	= nPartitionSize;
			sConvParams.nConvRank 		= nConvRank;
			sConvParams.nImage 			= nImage;
		}
    }

    status_t SyncChirpProcessor::allocateConvolutionParameters(size_t nchannels)
    {
    	bool bReallocate = false;

    	if (sConvParams.nChannels != nchannels)
    		bReallocate = true;

    	if (bReallocate)
    	{
    		destroyConvolutionParameters();

        	// Each pointer in conv_t points to an array that holds one value per channel.
        	size_t samples = 5 * nchannels;

    		size_t *ptr              		= alloc_aligned<size_t>(sConvParams.pData, samples);
			if (ptr == NULL)
				return STATUS_NO_MEM;

			lsp_guard_assert(size_t *save = ptr);
			sConvParams.vPartitions 		= ptr;
			ptr                    		   += nchannels;
			sConvParams.vPaddedLengths 		= ptr;
			ptr                    		   += nchannels;
			sConvParams.vInversePrepends 	= ptr;
			ptr                    		   += nchannels;
			sConvParams.vConvLengths        = ptr;
			ptr                    		   += nchannels;
			sConvParams.vAlignOffsets       = ptr;
			ptr                            += nchannels;

			lsp_assert(ptr <= &save[samples]);

			sConvParams.nChannels 			= nchannels;
    	}

    	return STATUS_OK;
    }

    void SyncChirpProcessor::destroyConvolutionParameters()
    {
    	free_aligned(sConvParams.pData);
    	sConvParams.pData				= NULL;
        sConvParams.vPartitions 		= NULL;
        sConvParams.vPaddedLengths		= NULL;
        sConvParams.vInversePrepends    = NULL;
        sConvParams.vConvLengths        = NULL;
        sConvParams.vAlignOffsets       = NULL;
    }

    void SyncChirpProcessor::calculateConvolutionParameters(Sample **data, size_t *offset)
    {
    	sConvParams.nAllocationSize = 0;

    	for (size_t ch = 0; ch < sConvParams.nChannels; ++ch)
    	{
    		// Set pointers to data and lengths. Use the maximum length to determine
			// pads to reach integer multiple of partition length.
			// In the processing: imaging the input data and inverse filter to be in
			// padded arrays, but taking the data directly from the pointers rather
			// than crating these arrays. The input data are imagined as padded in
			// the tail, the inverse filter as padded at the beginning (to not shift
			// the convolution centre).

			size_t nInputData       		    = data[ch]->length() - offset[ch];
			size_t nInverseFilter   		    = pInverseFilter->length();

			size_t nMaxLength       		    = (nInputData > nInverseFilter) ? nInputData : nInverseFilter;
			sConvParams.vPartitions[ch]   	    = (nMaxLength / sConvParams.nPartitionSize) + 1;
			sConvParams.vPaddedLengths[ch] 	    = sConvParams.vPartitions[ch] * sConvParams.nPartitionSize;
			sConvParams.vInversePrepends[ch]    = sConvParams.vPaddedLengths[ch] - nInverseFilter; // Amount of pad samples prepending the inverse filter
			sConvParams.vConvLengths[ch]        = 2 * sConvParams.vPaddedLengths[ch]; // Size of the convolution of 2 complete padded data sequences

			if (sConvParams.vConvLengths[ch] > sConvParams.nAllocationSize)
				sConvParams.nAllocationSize = sConvParams.vConvLengths[ch];
    	}

    	// The offsets can be calculated only after the allocation size is known
    	size_t middle = (sConvParams.nAllocationSize / 2) - 1;

    	for (size_t ch = 0; ch < sConvParams.nChannels; ++ch)
    	    sConvParams.vAlignOffsets[ch] = middle - (sConvParams.vConvLengths[ch] / 2) + 1;
    }

    status_t SyncChirpProcessor::allocateConvolutionTempArrays()
    {
    	if (!sConvParams.bReallocateTemp)
    		return STATUS_OK;

    	destroyConvolutionTempArrays();

        // Allocate 2X Partition temp buffers + 2X Partition Image temp buffers 1X Temporary buffer
        size_t samples          = 2 * sConvParams.nPartitionSize + 3 * sConvParams.nImage;

		float *ptr        		= alloc_aligned<float>(sConvParams.pTempData, samples);
		if (ptr == NULL)
			return STATUS_NO_MEM;

		lsp_guard_assert(float *save = ptr);
        sConvParams.vInPart     = ptr;
        ptr                    += sConvParams.nPartitionSize;
        sConvParams.vInvPart    = ptr;
        ptr                    += sConvParams.nPartitionSize;
        sConvParams.vInImage    = ptr;
        ptr                    += sConvParams.nImage;
        sConvParams.vInvImage   = ptr;
        ptr                    += sConvParams.nImage;
        sConvParams.vTemp       = ptr;
        ptr                    += sConvParams.nImage;

        lsp_assert(ptr <= &save[samples]);

        dsp::fill_zero(sConvParams.vInPart, samples);

        return STATUS_OK;
    }

    void SyncChirpProcessor::destroyConvolutionTempArrays()
    {
    	free_aligned(sConvParams.pTempData);
    	sConvParams.pTempData 	= NULL;
    	sConvParams.vInPart     = NULL;
    	sConvParams.vInvPart    = NULL;
    	sConvParams.vInImage    = NULL;
    	sConvParams.vInvImage   = NULL;
    	sConvParams.vTemp       = NULL;
    }

    status_t SyncChirpProcessor::do_linear_convolutions(Sample **data, size_t *offset, size_t nchannels, size_t partSizeLimit)
    {
    	if ((data == NULL) || (offset == NULL) || (nchannels == 0))
    		return STATUS_NO_DATA;

    	calculateConvolutionPartitionSize(partSizeLimit);

    	status_t status = allocateConvolutionParameters(nchannels);
		if (status != STATUS_OK)
			return status;

    	calculateConvolutionParameters(data, offset);

    	status = allocateConvolutionResult(nSampleRate, sConvParams.nChannels, sConvParams.nAllocationSize);
		if (status != STATUS_OK)
			return status;

		lsp_assert(sConvParams.nChannels == pConvResult->channels());

		status = allocateConvolutionTempArrays();
		if (status != STATUS_OK)
			return status;

    	for (size_t ch = 0; ch < nchannels; ++ch)
    	{
    		status = do_linear_convolution(data[ch], offset[ch], ch);

    		if (status != STATUS_OK)
    			return status;
    	}

    	return STATUS_OK;
    }

    status_t SyncChirpProcessor::do_linear_convolution(Sample *data, size_t offset, size_t channel)
    {
        if ((pInverseFilter == NULL) || (data == NULL))
            return STATUS_NO_DATA;

        if (channel >= sConvParams.nChannels)
        	return STATUS_BAD_ARGUMENTS;

        dsp::fill_zero(sConvParams.vInPart, 	sConvParams.nPartitionSize);
        dsp::fill_zero(sConvParams.vInvPart, 	sConvParams.nPartitionSize);
        dsp::fill_zero(sConvParams.vInImage, 	sConvParams.nImage);
        dsp::fill_zero(sConvParams.vInvImage, 	sConvParams.nImage);
        dsp::fill_zero(sConvParams.vTemp, 		sConvParams.nImage);

        // Results
		float *vInputData       = data->getBuffer(0, offset);
		size_t nInputData       = data->length() - offset;

		float *vInverseFilter   = pInverseFilter->getBuffer(0);

        float *vResult 			= pConvResult->channel(channel);
        if (vResult == NULL)
        	return STATUS_BAD_ARGUMENTS;

        // Do the convolution.
        bool bNullInputPart     = false; // If one of the operands of the convolution is zeros, set to true and avoid convolution
        bool bNullInversePart   = false;

        for (size_t inp = 0; inp < sConvParams.vPartitions[channel]; ++inp) // Cycle through input partitions
        {
            // Scanning through the input data: current position is inputHead.
            size_t inputHead    = inp * sConvParams.nPartitionSize;

            // Evaluate distance of head from end of data
            ssize_t inSamplesAhead = ssize_t(nInputData) - ssize_t(inputHead);

            if (inSamplesAhead > ssize_t(sConvParams.nPartitionSize)) // The whole current partition is within the input data
            {
                bNullInputPart  = false;
                dsp::fastconv_parse(sConvParams.vInImage, &vInputData[inputHead], sConvParams.nConvRank);
            }
            else if (inSamplesAhead > 0) // The current partition is across the end of input data and the beginning of the pad
            {
                bNullInputPart  = false;
                dsp::copy(sConvParams.vInPart, &vInputData[inputHead], inSamplesAhead);
                dsp::fill_zero(&sConvParams.vInPart[inSamplesAhead], sConvParams.nPartitionSize - inSamplesAhead);
                dsp::fastconv_parse(sConvParams.vInImage, sConvParams.vInPart, sConvParams.nConvRank);
            }
            else // The current partition is completely in the pad
            {
                bNullInputPart  = true;
            }

            // Initialise the position tracker for the inverse filter.
            // This will be bumped as the loop scans through the pad first
            // and the actual inverse filter data later.
            size_t inverseHead  = 0;

            for (size_t invp = 0; invp < sConvParams.vPartitions[channel]; ++invp) // Cycle through inverse filter partitions
            {
                // Scanning through the imaginary array made of prepend pad followed by inverse filter:
                // current position is virtualHead
                size_t virtualHead = invp * sConvParams.nPartitionSize;

                // Evaluate distance of head from end of prepend pad
                ssize_t viSamplesAhead = ssize_t(sConvParams.vInversePrepends[channel]) - ssize_t(virtualHead);

                if (viSamplesAhead > ssize_t(sConvParams.nPartitionSize)) // The whole partition is within the pad
                {
                    bNullInversePart = true;
                }
                else if (viSamplesAhead > 0) // The partition is across the end of the pad and the beginning of data
                {
                    bNullInversePart    = false;
                    size_t toCopy       = sConvParams.nPartitionSize - viSamplesAhead;
                    dsp::fill_zero(sConvParams.vInvPart, viSamplesAhead);
                    dsp::copy(&sConvParams.vInvPart[viSamplesAhead], &vInverseFilter[inverseHead], toCopy);
                    dsp::fastconv_parse(sConvParams.vInvImage, sConvParams.vInvPart, sConvParams.nConvRank);
                    inverseHead += toCopy;
                }
                else // The partition is inside the inverse filter
                {
                    bNullInversePart    = false;
                    dsp::fastconv_parse(sConvParams.vInvImage, &vInverseFilter[inverseHead], sConvParams.nConvRank);
                    inverseHead += sConvParams.nPartitionSize;
                }

                if (bNullInputPart || bNullInversePart)
                    continue; // Result is zero, nothing to do.

                dsp::fastconv_apply(&vResult[sConvParams.nPartitionSize * (inp + invp) + sConvParams.vAlignOffsets[channel]],
                            sConvParams.vTemp,
                            sConvParams.vInImage,
                            sConvParams.vInvImage,
                            sConvParams.nConvRank
                            );
            }
        }

        // Normalising by square sample rate to recover physical units.
        dsp::scale2(vResult, sChirpParams.fConvScale / (nSampleRate * nSampleRate), sConvParams.vConvLengths[channel]);

        return STATUS_OK;
    }

    status_t SyncChirpProcessor::postprocess_linear_convolution(size_t channel, ssize_t offset, scp_rtcalc_t rtCalc, float windowSize, double tolerance)
    {
        if (pConvResult == NULL)
            return STATUS_NO_DATA;

		if (channel >= sConvParams.nChannels)
			return STATUS_BAD_ARGUMENTS;

        size_t dataLength = pConvResult->samples();
        if (dataLength == 0)
            return STATUS_NO_DATA;

        size_t middle = (dataLength / 2) - 1;

        // Convert offset to unsigned, so to make safe operations with unsigned types
        size_t nOffset          = 0;
        bool bPositiveOffset    = true;

        if (offset > 0)
        {
            nOffset             = offset;
        }
        else
        {
            nOffset             = -offset;
            bPositiveOffset     = false;
        }

        nOffset                 = (nOffset > middle) ? middle : nOffset;

        // Define limits for all subroutines.
        // For profiling the background noise, we focus in the first half of
        // the impulse response, the Chirp Duration samples directly to the left
        // of the middle, as we are sure any background noise is in there.
        // We never go to the right, though, as we are sure the linear impulse
        // response is there.
        // We process instead the impulse response starting from middle + offset.

        size_t bgProfileHead    = middle - sChirpParams.nDuration;
        size_t bgProfileCount   = 0;
        size_t irProcessHead    = 0;

        if (bPositiveOffset)
        {
            bgProfileCount      = sChirpParams.nDuration; // All duration samples up to middle
            irProcessHead       = middle + nOffset;
        }
        else
        {
            bgProfileCount      = sChirpParams.nDuration - nOffset; // All duration samples up to middle + offset
            irProcessHead       = middle - nOffset;
        }

        irProcessHead           = (irProcessHead > middle) ? middle : irProcessHead;

        status_t returnValue;

        returnValue             = profile_background_noise(channel, bgProfileHead, bgProfileCount);

        if (returnValue != STATUS_OK)
            return returnValue;

        size_t bufSize          = seconds_to_samples(nSampleRate, windowSize);
        returnValue             = calibrate_backwards_integration_limit(channel, irProcessHead, bufSize, tolerance);

        if (returnValue != STATUS_OK)
            return returnValue;

        return                    calculate_reverberation_time(channel, irProcessHead, rtCalc, sCRPostProc.nIrLimit);
    }

    status_t SyncChirpProcessor::postprocess_nonlinear_convolution(size_t channel, size_t order, bool doInnerSmoothing, size_t nFadeIn, size_t nFadeOut, windows::window_t windowType, size_t nWindowRank)
    {
		if (channel >= sConvParams.nChannels)
			return STATUS_BAD_ARGUMENTS;

        sCRPostProc.nWinRank    = (nWindowRank < MAX_WINDOW_RANK) ? nWindowRank : MAX_WINDOW_RANK;
        size_t nTaps            = 1 << nWindowRank;

        status_t returnValue    = allocateIdentificationMatrices(order, nTaps);

        if (returnValue != STATUS_OK)
            return returnValue;

        fillCoefficientsMatrices();
        windowHigherOrderResponses(channel, doInnerSmoothing, nFadeIn, nFadeOut, windowType);
        solve();
        force_kernels_DC_block();

        return STATUS_OK;
    }

    status_t SyncChirpProcessor::save_linear_convolution(const char *path, size_t head, size_t count)
    {
        if (pConvResult == NULL)
            return STATUS_NO_DATA;

        size_t dataLength = pConvResult->samples();

        if (dataLength == 0)
            return STATUS_NO_DATA;

        if (head >= dataLength)
            return STATUS_BAD_ARGUMENTS;

        size_t maxCount         = dataLength - head;
        count                   = (count > maxCount) ? maxCount : count;

        status_t status         = pConvResult->store_samples(path, head, count);

        if (status != STATUS_OK)
            return status;

        return STATUS_OK;
    }

    status_t SyncChirpProcessor::save_linear_convolution(const char *path, ssize_t offset, size_t count)
    {
        if (pConvResult == NULL)
            return STATUS_NO_DATA;

        size_t dataLength = pConvResult->samples();

        if (dataLength == 0)
            return STATUS_NO_DATA;

        size_t middle   = (dataLength / 2) - 1;

        size_t nOffset  = 0;
        size_t head     = 0;

        if (offset > 0)
        {
            nOffset     = offset;
            head        = middle + nOffset;
            head        = (head < dataLength) ? head : dataLength;
        }
        else
        {
            nOffset     = -offset;
            nOffset     = (nOffset < middle) ? nOffset : middle;
            head        = middle - nOffset;
        }

        return save_linear_convolution(path, head, count);
    }

    status_t SyncChirpProcessor::save_linear_convolution(const char *path, size_t count)
    {
        if (pConvResult == NULL)
            return STATUS_NO_DATA;

        size_t dataLength = pConvResult->samples();

        if (dataLength == 0)
            return STATUS_NO_DATA;

        // Save only the positive time result, which starts after the first half
        size_t head = (dataLength / 2) - 1;

        return save_linear_convolution(path, head, count);
    }

    status_t SyncChirpProcessor::save_to_lspc(const char *path, ssize_t offset)
    {
        if (pConvResult == NULL)
            return STATUS_NO_DATA;

        size_t dataLength = pConvResult->samples();
        if (dataLength == 0)
            return STATUS_NO_DATA;

        // Create chunk file
        LSPCFile fd;
        LSPCAudioWriter aw;
        lspc_audio_parameters_t p;

        status_t res = fd.create(path);
        if (res != STATUS_OK)
        {
            aw.close();
            fd.close();
            return res;
        }

        // Write complete convolution result as audio chunk
        p.channels          = sConvParams.nChannels;
        p.sample_format     = __IF_LEBE(LSPC_SAMPLE_FMT_F32LE, LSPC_SAMPLE_FMT_F32BE);;
        p.sample_rate       = nSampleRate;
        p.codec             = LSPC_CODEC_PCM;
        p.frames            = dataLength;

        res = aw.open(&fd, &p);
        if (res != STATUS_OK)
        {
            aw.close();
            fd.close();
            return res;
        }

        uint32_t audio_chunk_id = aw.unique_id();
        const float **vp = reinterpret_cast<const float **>(alloca(sizeof(float *) * p.channels));
        for (size_t i = 0, n = p.channels; i < n; ++i)
            vp[i] = pConvResult->channel(i);

        res = aw.write_samples(vp, dataLength);
        if (res == STATUS_OK)
            res = aw.close();

        if (res != STATUS_OK)
        {
            aw.close();
            fd.close();
            return res;
        }

        // Write profile data chunk

        // Figuring out the number of frames to skip
        size_t skip = 0;

        size_t middle       = dataLength / 2 - 1;
        // We remove 1 now because, without offset, we skip all samples BEFORE the middle.
        size_t skipNoOffset = middle - 1;
        size_t maxAhead     = dataLength - skipNoOffset;

        if (offset >= 0)
        {
            size_t nOffset  = offset;
            nOffset         = (nOffset > maxAhead)? maxAhead : nOffset;
            skip            = skipNoOffset + nOffset;
        }
        else
        {
            size_t nOffset  = -offset;
            nOffset         = (nOffset > skipNoOffset)? skipNoOffset : nOffset;
            skip            = skipNoOffset - nOffset;
        }

        LSPCChunkWriter *wr = fd.write_chunk(LSPC_CHUNK_PROFILE);

        lspc_chunk_audio_profile_t prof;
        bzero(&prof, sizeof(lspc_chunk_audio_profile_t));

        prof.common.version     = 2;
        prof.common.size        = sizeof(lspc_chunk_audio_profile_t);
        prof.chunk_id           = audio_chunk_id;
        prof.chirp_order        = sChirpParams.nOrder;
        prof.alpha              = sChirpParams.fAlpha;
        prof.beta               = sChirpParams.beta;
        prof.gamma              = sChirpParams.gamma;
        prof.delta              = sChirpParams.delta;
        prof.initial_freq       = sChirpParams.initialFrequency;
        prof.final_freq         = sChirpParams.finalFrequency;
        prof.skip               = skip;

        // Convert header fields CPU -> BE
        prof.chunk_id           = CPU_TO_BE(prof.chunk_id);
        prof.chirp_order        = CPU_TO_BE(prof.chirp_order);
        prof.alpha              = CPU_TO_BE(prof.alpha);
        prof.beta               = CPU_TO_BE(prof.beta);
        prof.gamma              = CPU_TO_BE(prof.gamma);
        prof.delta              = CPU_TO_BE(prof.delta);
        prof.initial_freq       = CPU_TO_BE(prof.initial_freq);
        prof.final_freq         = CPU_TO_BE(prof.final_freq);
        prof.skip               = CPU_TO_BE(prof.skip);

        res = wr->write_header(&prof);
        if (res == STATUS_OK)
            res = wr->flush();
        if (res == STATUS_OK)
            res = wr->close();

        if (res != STATUS_OK)
        {
            wr->close();
            delete wr;
            fd.close();
            return res;
        }

        delete wr;
        res = fd.close();
        return res;
    }

    status_t SyncChirpProcessor::load_from_lspc(const char *path)
    {
        LSPCFile fd;
        status_t res = fd.open(path);
        if (res != STATUS_OK)
        {
            fd.close();
            return res;
        }

        // Find profile chunk
        uint32_t chunk_id = 0;
        LSPCChunkReader *rd = fd.find_chunk(LSPC_CHUNK_PROFILE, &chunk_id);
        if (rd == NULL)
        {
            fd.close();
            return STATUS_CORRUPTED_FILE;
        }

        lspc_chunk_audio_profile_t prof;
        rd->read_header(&prof, sizeof(lspc_chunk_audio_profile_t));
        res = status_t(rd->last_error());
        if ((res != STATUS_OK) && (res != STATUS_EOF))
        {
            fd.close();
            delete rd;
            return res;
        }

        // Convert header fields BE -> CPU
        prof.chunk_id       = BE_TO_CPU(prof.chunk_id);
        prof.chirp_order    = BE_TO_CPU(prof.chirp_order);
        prof.alpha          = BE_TO_CPU(prof.alpha);
        prof.beta           = BE_TO_CPU(prof.beta);
        prof.gamma          = BE_TO_CPU(prof.gamma);
        prof.delta          = BE_TO_CPU(prof.delta);
        prof.initial_freq   = BE_TO_CPU(prof.initial_freq);
        prof.final_freq     = BE_TO_CPU(prof.final_freq);

        // Checking that data are meaningful.
        if ((prof.alpha < MIN_AMPLITUDE) ||
            (prof.beta <= 0.0) ||
            (prof.gamma <= 0.0) ||
            (prof.delta <= 0.0) ||
            (prof.initial_freq <= 0.0) ||
            (prof.final_freq <= prof.initial_freq) ||
            (prof.chirp_order != size_t(prof.final_freq / prof.initial_freq))
            )
        {
            rd->close();
            fd.close();
            delete rd;
            return STATUS_CORRUPTED_FILE;
        }

        chunk_id = prof.chunk_id;

        // Close chunk reader
        res = rd->close();
        if (res != STATUS_OK)
        {
            fd.close();
            delete rd;
            return res;
        }
        delete rd;

        // Read audio chunk
        LSPCAudioReader ar;
        lspc_audio_parameters_t p;

        res = ar.open(&fd);
        if (res != STATUS_OK)
        {
            fd.close();
            return res;
        }

        res = ar.get_parameters(&p);
        if (res != STATUS_OK)
        {
            fd.close();
            return res;
        }

        status_t status = allocateConvolutionResult(p.sample_rate, p.channels, p.frames);
        if (status != STATUS_OK)
        {
            fd.close();
            return status;
        }

        float **vp = reinterpret_cast<float **>(alloca(sizeof(float *) * p.channels));
        for (size_t i = 0, n = p.channels; i < n; ++i)
            vp[i] = pConvResult->channel(i);

        size_t read = 0;
        while (read < p.frames)
        {
            size_t to_read = p.frames - read;
            if (to_read > p.frames)
                to_read = p.frames;

            ssize_t n_read = ar.read_samples(vp, to_read);

            read += n_read;
            for (size_t i = 0, n = p.channels; i < n; ++i)
                vp[i] += n_read;
        }

//        sConvParams.nChannels           = p.channels;
        sChirpParams.nOrder             = prof.chirp_order;
        sChirpParams.fAlpha             = prof.alpha;
        sChirpParams.beta               = prof.beta;
        sChirpParams.gamma              = prof.gamma;
        sChirpParams.delta              = prof.delta;
        sChirpParams.initialFrequency   = prof.initial_freq;
        sChirpParams.finalFrequency     = prof.final_freq;
        sChirpParams.bRecalculate       = true;
        sChirpParams.bReconfigure       = true;
        bSync                           = true;

        res = ar.close();
        if (res != STATUS_OK)
        {
            fd.close();
            return res;
        }

        res = fd.close();
        return res;
    }

    void SyncChirpProcessor::update_settings()
    {
        if (!bSync)
            return;

        // Chirp parameters:
        sChirpParams.fAlpha                 = (sChirpParams.fAlpha > 0.0f)? sChirpParams.fAlpha : -sChirpParams.fAlpha;
        sChirpParams.fAlpha                 = (sChirpParams.fAlpha < MIN_AMPLITUDE)? MIN_AMPLITUDE : sChirpParams.fAlpha;

        sChirpParams.fConvScale             = 1.0f / sChirpParams.fAlpha;

        if (sChirpParams.bRecalculate)
        {
            // Set up frequency values according to limits:
            if (sChirpParams.finalFrequency <= 0.0)
                sChirpParams.finalFrequency = DFL_FINAL_FREQ;

            sChirpParams.finalFrequency     = (sChirpParams.finalFrequency > (nSampleRate / 2.0)) ? (nSampleRate / 2.0) : sChirpParams.finalFrequency;

            if (sChirpParams.initialFrequency <= 0.0)
                sChirpParams.initialFrequency = DFL_INITIAL_FREQ;

            sChirpParams.initialFrequency   = (sChirpParams.initialFrequency >= sChirpParams.finalFrequency) ? DFL_INITIAL_FREQ : sChirpParams.initialFrequency;

            if (sChirpParams.initialFrequency == sChirpParams.finalFrequency)
            {
                sChirpParams.initialFrequency   = DFL_INITIAL_FREQ;
                sChirpParams.finalFrequency     = DFL_FINAL_FREQ;
            }

            // Optimise initial frequency so that the final frequency is a harmonic of the initial frequency
            sChirpParams.nOrder             = sChirpParams.finalFrequency / sChirpParams.initialFrequency;
            sChirpParams.initialFrequency   = sChirpParams.finalFrequency / sChirpParams.nOrder;
            sChirpParams.beta               = 2.0 * M_PI * sChirpParams.initialFrequency;

            if (sChirpParams.fDuration <= 0.0f)
                sChirpParams.fDuration      = DFL_DURATION;

            sChirpParams.fDurationCoarse    = sChirpParams.fDuration;   // Saving the pre-optimisation value for reference
            sChirpParams.fDuration          = (sChirpParams.fDuration < LIM_DURATION) ? sChirpParams.fDuration : LIM_DURATION;

            double n            = 1.0;
            double minDuration  = log(sChirpParams.nOrder) / sChirpParams.initialFrequency;

            while (sChirpParams.fDuration <= sChirpParams.fDurationCoarse)
            {
                sChirpParams.fDuration      = n * minDuration;
                sChirpParams.gamma          = n / sChirpParams.initialFrequency;

                n += 1.0;
            }

            sChirpParams.nDuration          = seconds_to_samples(nSampleRate, sChirpParams.fDuration);

            // Final signal parameter:
            sChirpParams.delta              = sChirpParams.beta * sChirpParams.gamma;

            // Number of time lags:
            sChirpParams.nTimeLags = (sChirpParams.nOrder < LIM_LAG) ? sChirpParams.nOrder : LIM_LAG;

            // Check if chirp is Asymptotic:
            if (sChirpParams.beta > ASYM_THRS / sChirpParams.gamma)
                sChirpParams.bAsymptotic    = true;

            sChirpParams.bRecalculate       = false;
        }

        float maxFadeIn                     = sChirpParams.fDuration * FADEIN_FRACTION;
        float maxFadeOut                    = sChirpParams.fDuration * FADEOUT_FRACTION;

        if (sFader.fFadeIn < 0.0f)
            sFader.fFadeIn                  = 0.0f;

        if (sFader.fFadeOut < 0.0f)
            sFader.fFadeOut                 = 0.0f;

        if (sFader.fFadeIn > maxFadeIn)
            sFader.fFadeIn                  = maxFadeIn;

        if (sFader.fFadeOut > maxFadeOut)
            sFader.fFadeOut                 = maxFadeOut;

        sFader.nFadeIn                      = seconds_to_samples(nSampleRate, sFader.fFadeIn);
        sFader.nFadeOut                     = seconds_to_samples(nSampleRate, sFader.fFadeOut);

        sOver1.set_sample_rate(nSampleRate);
        sOver1.set_mode(enOverMode);
        if (sOver1.modified())
            sOver1.update_settings();

        sOver2.set_sample_rate(nSampleRate);
        sOver2.set_mode(enOverMode);
        if (sOver2.modified())
            sOver2.update_settings();

        nOversampling                       = sOver1.get_oversampling();

        sFader.nFadeIn_Over                 = seconds_to_samples(nOversampling * nSampleRate, sFader.fFadeIn);
        sFader.nFadeOut_Over                = seconds_to_samples(nOversampling * nSampleRate, sFader.fFadeOut);

        bSync = false;
    }

    void SyncChirpProcessor::get_convolution_result_plottable_samples(size_t channel, float *dst, size_t head, size_t convLimit, size_t plotCount, bool normalize)
    {
        size_t irSamples = pConvResult->samples();
        if (irSamples == 0)
            return;

        float *vResult		= pConvResult->channel(channel);
        float *vData		= &vResult[head];
        size_t maxConvLimit	= irSamples - head;
        convLimit 			= (convLimit < maxConvLimit) ? convLimit : maxConvLimit;

        float decimationStep = convLimit / float(plotCount);

        dsp::fill_zero(dst, plotCount);

        if (decimationStep < 1.0f) // Zero filling upsampling. Simple and visually accurate to what a digital IR is.
        {
            size_t plotDataHead             = 0;

            for (size_t n = 0; n < convLimit; ++n)
            {
                dst[plotDataHead]           = vData[n];
                plotDataHead               += (1.0f / decimationStep);

                if (plotDataHead >= plotCount)
                    break;
            }
        }
        else if (decimationStep == 1.0f) // Nothing to do
        {
            dsp::copy(dst, &vResult[head], plotCount);
        }
        else    // Decimation downsampling
        {
            size_t plotDataHead             = 0;
            size_t plotDataDownLimit        = 0;
            size_t plotDataRange            = decimationStep - 1.0f;

            for (size_t n = 0; n < plotCount; ++n)
            {
                plotDataHead                = dsp::abs_max_index(&vData[plotDataDownLimit], plotDataRange) + plotDataDownLimit;
                dst[n]                      = vData[plotDataHead];
                plotDataDownLimit          += decimationStep;

                if (plotDataDownLimit >= convLimit)
                    break;

                size_t samplesAhead         = convLimit - plotDataDownLimit;
                plotDataRange               = (plotDataRange > samplesAhead) ? samplesAhead : plotDataRange;
            }
        }

        if (normalize)
            dsp::scale2(dst, 1.0f / dsp::abs_max(vResult, irSamples), plotCount);
    }

    void SyncChirpProcessor::get_convolution_result_plottable_samples(size_t channel, float *dst, ssize_t offset, size_t convLimit, size_t plotCount, bool normalize)
    {
        size_t irSamples = pConvResult->samples();
        if (irSamples == 0)
            return;

        size_t middle	= (irSamples / 2) - 1;
        size_t maxAhead	= irSamples - middle - convLimit;

        size_t nOffset	= 0;
        size_t head		= 0;

        if (offset > 0)
        {
            nOffset	= offset;
            nOffset	= (nOffset < maxAhead) ? nOffset : maxAhead;
            head	= middle + nOffset;
        }
        else
        {
            nOffset	= -offset;
            nOffset	= (nOffset > middle) ? middle : nOffset;
            head   	= middle - nOffset;
        }

        get_convolution_result_plottable_samples(channel, dst, head, convLimit, plotCount, normalize);
    }

    void SyncChirpProcessor::get_convolution_result_plottable_samples(size_t channel, float *dst, size_t convLimit, size_t plotCount, bool normalize)
    {
        size_t irSamples = pConvResult->samples();

        if (irSamples == 0)
            return;

        size_t head = (irSamples / 2) - 1;

        get_convolution_result_plottable_samples(channel, dst, head, convLimit, plotCount, normalize);
    }
}
