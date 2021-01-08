/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 14 авг. 2016 г.
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

#include <core/types.h>
#include <dsp/dsp.h>
#include <core/util/Analyzer.h>

#include <math.h>

namespace lsp
{
    
    Analyzer::Analyzer()
    {
        construct();
    }

    Analyzer::~Analyzer()
    {
        destroy();
    }

    void Analyzer::construct()
    {
        nChannels       = 0;
        nMaxRank        = 0;
        nRank           = 0;
        nSampleRate     = 0;
        nMaxSampleRate  = 0;
        nBufSize        = 0;
        nCounter        = 0;
        nPeriod         = 0;
        nStep           = 0;
        fReactivity     = 0.0f;
        fTau            = 1.0f;
        fRate           = 1.0f;
        fMinRate        = 1.0f;
        fShift          = 1.0f;
        nReconfigure    = 0;
        nEnvelope       = envelope::PINK_NOISE;
        nWindow         = windows::HANN;
        bActive         = true;

        vChannels       = NULL;
        vData           = NULL;
        vSigRe          = NULL;
        vFftReIm        = NULL;
        vWindow         = NULL;
        vEnvelope       = NULL;
    }

    void Analyzer::destroy()
    {
        if (vChannels != NULL)
        {
            delete [] vChannels;
            vChannels   = NULL;
        }

        free_aligned(vData);
    }

    bool Analyzer::init(size_t channels, size_t max_rank, size_t max_sr, float min_rate)
    {
        destroy();

        size_t fft_size         = 1 << max_rank;
        nBufSize                = ALIGN_SIZE(fft_size + size_t(float(max_sr * 2) / min_rate) + DEFAULT_ALIGN, DEFAULT_ALIGN);
        size_t allocate         = 5 * fft_size +                // vSigRe, vFftReIm (re + im), vWindow, vEnvelope
                                  channels * nBufSize +         // c->vBuffer
                                  channels * fft_size +         // c->vAmp
                                  channels * fft_size;          // c->vData

        // Allocate data
        float *abuf         = alloc_aligned<float>(vData, allocate);
        if (abuf == NULL)
            return false;

        // Allocate channels
        channel_t *clist    = new channel_t[channels];
        if (clist == NULL)
        {
            delete [] abuf;
            return false;
        }

        nChannels           = channels;
        nMaxRank            = max_rank;
        nRank               = max_rank;
        nMaxSampleRate      = max_sr;
        fMinRate            = min_rate;

        // Clear buffers
        dsp::fill_zero(abuf, allocate);

        // Initialize buffers
        vSigRe              = abuf;
        abuf               += fft_size;
        vFftReIm            = abuf;
        abuf               += fft_size * 2;
        vWindow             = abuf;
        abuf               += fft_size;
        vEnvelope           = abuf;
        abuf               += fft_size;

        // Initialize channels
        vChannels           = clist;
        for (size_t i=0; i<channels; ++i)
        {
            channel_t *c        = &vChannels[i];

            // FFT buffers
            c->vBuffer          = abuf;
            abuf               += nBufSize;
            c->vAmp             = abuf;
            abuf               += fft_size;
            c->vData            = abuf;
            abuf               += fft_size;

            // Counters
            c->nHead            = 0;
            c->nDelay           = 0;
            c->bFreeze          = false;
            c->bActive          = true;
        }

        // Set reconfiguration flags
        nReconfigure        = R_ALL;

        return true;
    }

    void Analyzer::set_sample_rate(size_t sr)
    {
        sr              = lsp_min(sr, nMaxSampleRate);
        if (nSampleRate == sr)
            return;

        nSampleRate     = sr;
        nReconfigure   |= R_ALL;
    }

    void Analyzer::set_rate(float rate)
    {
        rate        = lsp_max(fMinRate, rate);
        if (fRate == rate)
            return;

        fRate           = rate;
        nReconfigure   |= R_COUNTERS;
    }

    void Analyzer::set_window(size_t window)
    {
        if (nWindow == window)
            return;

        nWindow         = window;
        nReconfigure   |= R_WINDOW;
    }

    void Analyzer::set_envelope(size_t envelope)
    {
        if (nEnvelope == envelope)
            return;

        nEnvelope       = envelope;
        nReconfigure   |= R_ENVELOPE;
    }

    void Analyzer::set_shift(float shift)
    {
        if (fShift == shift)
            return;

        fShift          = shift;
        nReconfigure   |= R_ENVELOPE;
    }

    void Analyzer::set_reactivity(float reactivity)
    {
        if (fReactivity == reactivity)
            return;

        fReactivity     = reactivity;
        nReconfigure   |= R_TAU;
    }

    bool Analyzer::set_rank(size_t rank)
    {
        if ((rank < 2) || (rank > nMaxRank))
            return false;
        else if (nRank == rank)
            return true;
        nRank           = rank;
        nReconfigure   |= R_ALL;
        return true;
    }

    bool Analyzer::freeze_channel(size_t channel, bool freeze)
    {
        if (channel >= nChannels)
            return false;
        vChannels[channel].bFreeze      = freeze;
        return true;
    }

    bool Analyzer::enable_channel(size_t channel, bool enable)
    {
        if (channel >= nChannels)
            return false;
        if (vChannels[channel].bActive == enable)
            return false;

        vChannels[channel].bActive      = enable;
        nReconfigure   |= R_COUNTERS;
        return true;
    }

    void Analyzer::reconfigure()
    {
        if (!nReconfigure)
            return;

        size_t fft_size     = 1 << nRank;
        size_t fft_period   = float(nSampleRate) / fRate;
        nStep               = fft_period / (nChannels + 1);
        nPeriod             = nStep * (nChannels + 1);

        // Update envelope
        if (nReconfigure & R_ENVELOPE)
        {
            envelope::reverse_noise(vEnvelope, fft_size, envelope::envelope_t(nEnvelope));
            dsp::mul_k2(vEnvelope, fShift / fft_size, fft_size);
        }
        // Clear analysis
        if (nReconfigure & R_ANALYSIS)
        {
            for (size_t i=0; i<nChannels; ++i)
            {
                dsp::fill_zero(vChannels[i].vAmp, fft_size);
                dsp::fill_zero(vChannels[i].vData, fft_size);
            }
        }
        // Update window
        if (nReconfigure & R_WINDOW)
            windows::window(vWindow, fft_size, windows::window_t(nWindow));
        // Update reactivity
        if (nReconfigure & R_TAU)
            fTau    = 1.0f - expf(logf(1.0f - M_SQRT1_2) / seconds_to_samples(float(nSampleRate) / float(nPeriod), fReactivity));
        // Update counters
        if (nReconfigure & R_COUNTERS)
        {
            for (size_t i=0; i<nChannels; ++i)
                vChannels[i].nDelay     = nPeriod - (i+1)*nStep;
        }

        // Clear reconfiguration flag and update strobe signal
        nReconfigure    = 0;
    }

    void Analyzer::process(const float * const *in, size_t samples)
    {
        if (vChannels == NULL)
            return;

        // Auto-apply reconfiguration
        reconfigure();

        // Do main processing
        channel_t *c;
        ssize_t fft_size    = 1 << nRank;
        ssize_t fft_csize   = (fft_size >> 1) + 1;

        for (size_t offset = 0; offset < samples; ++offset)
        {
            // Determine the actual position
            size_t channel  = nCounter / nStep;
            size_t off      = nCounter % nStep;

            // Need to do FFT transform/sync?
            if (off == 0)
            {
                if (channel < nChannels)
                {
                    // Regular channel, need to perform FFT if
                    c   = &vChannels[channel];

                    // Perform FFT only for active channels
                    if (!c->bFreeze)
                    {
                        if ((bActive) && (c->bActive))
                        {
                            // Get the time mark to start from
                            ssize_t doff    = c->nHead - (fft_size + c->nDelay);

                            lsp_trace("channel=%d, offset=%d, samples=%d", int(channel), int(doff), int(samples));

                            if (doff < 0)
                                doff           += nBufSize;

                            // Prepare the real buffer
                            ssize_t count   = nBufSize - doff;
                            if (count < fft_size)
                            {
                                dsp::mul3(vSigRe, &c->vBuffer[doff], vWindow, count);
                                dsp::mul3(&vSigRe[count], c->vBuffer, &vWindow[count], fft_size - count);
                            }
                            else
                                dsp::mul3(vSigRe, &c->vBuffer[doff], vWindow, fft_size);

                            // Do Real->complex conversion and FFT
                            dsp::pcomplex_r2c(vFftReIm, vSigRe, fft_size);
                            dsp::packed_direct_fft(vFftReIm, vFftReIm, nRank);
                            // Get complex argument
                            dsp::pcomplex_mod(vFftReIm, vFftReIm, fft_csize);
                            // Mix with the previous value
                            dsp::mix2(c->vAmp, vFftReIm, 1.0 - fTau, fTau, fft_csize);
                        }
                        else
                            dsp::fill_zero(c->vAmp, fft_size);
                    } // c->bFreeze
                }
                else
                {
                    // Strobe trigger, copy buffers
                    for (size_t i=0; i<nChannels; ++i)
                    {
                        c = &vChannels[i];
                        dsp::copy(c->vData, c->vAmp, fft_size);
                    }
                }
            } // off == 0

            // How many samples to process?
            size_t to_process   = lsp_min(samples - offset, nStep - off);

            // Commit data to delay buffers for each channel
            for (size_t i=0; i<nChannels; ++i)
            {
                c                   = &vChannels[i];
                const float *src    = (in != NULL) ? in[i] : NULL;
                size_t ncopy        = nBufSize - c->nHead;

                if (ncopy < to_process)
                {
                    if (src != NULL)
                    {
                        dsp::copy(&c->vBuffer[c->nHead], &src[offset], ncopy);
                        dsp::copy(c->vBuffer, &src[offset + ncopy], to_process - ncopy);
                    }
                    else
                    {
                        dsp::fill_zero(&c->vBuffer[c->nHead], ncopy);
                        dsp::fill_zero(c->vBuffer, to_process - ncopy);
                    }
                    c->nHead            = to_process - ncopy;
                }
                else
                {
                    if (src != NULL)
                        dsp::copy(&c->vBuffer[c->nHead], &src[offset], to_process);
                    else
                        dsp::fill_zero(&c->vBuffer[c->nHead], to_process);
                    c->nHead           += to_process;
                }
            }

            // Update position
            offset     += to_process;
            nCounter    = nCounter + to_process;
            if (nCounter >= nPeriod)
                nCounter   -= nPeriod;
        }
    }

    bool Analyzer::read_frequencies(float *frq, float start, float stop, size_t count, size_t flags)
    {
        if ((vChannels == NULL) || (count == 0))
            return false;
        else if (count == 1)
        {
            *frq            = start;
            return true;
        }

        // Analyze flags
        if (flags == FRQA_SCALE_LOGARITHMIC)
        {
            // Initialize list of frequencies
            float norm          = logf(stop/start) / (--count);

            for (size_t i=0; i<count; ++i)
                frq[i]              = start * expf(i * norm);
        }
        else if (flags == FRQA_SCALE_LINEAR)
        {
            float norm          = (stop - start) / (--count);
            for (size_t i=0; i<count; ++i)
                frq[i]              = start + i * norm;
        }
        else
            return false;

        frq[count]          = stop;
        return true;
    }

    bool Analyzer::get_spectrum(size_t channel, float *out, const uint32_t *idx, size_t count)
    {
        if ((vChannels == NULL) || (channel >= nChannels))
            return false;

        channel_t *c        = &vChannels[channel];
        for (size_t i=0; i<count; ++i)
        {
            size_t j            = idx[i];
            out[i]              = c->vData[j] * vEnvelope[j];
        }

        return true;
    }

    float Analyzer::get_level(size_t channel, const uint32_t idx)
    {
        if ((vChannels == NULL) || (channel >= nChannels))
            return 0.0f;

        return vChannels[channel].vData[idx] * vEnvelope[idx];
    }

    void Analyzer::get_frequencies(float *frq, uint32_t *idx, float start, float stop, size_t count)
    {
        size_t fft_size     = 1 << nRank;
        size_t fft_csize    = (fft_size >> 1) + 1;
        float scale         = float(fft_size) / float(nSampleRate);

        // Initialize list of frequencies
        float norm          = logf(stop/start) / (count - 1);

        for (size_t i=0; i<count; ++i)
        {
            float f         = start * expf(i * norm);
            size_t ix       = scale * f;
            if (ix > fft_csize)
                ix                  = fft_csize;

            frq[i]          = f;
            idx[i]          = ix;
        }
    }

    void Analyzer::dump(IStateDumper *v) const
    {
        v->write("nChannels", nChannels);
        v->write("nMaxRank", nMaxRank);
        v->write("nRank", nRank);
        v->write("nSampleRate", nSampleRate);
        v->write("nMaxSampleRate", nMaxSampleRate);
        v->write("nBufSize", nBufSize);
        v->write("nCounter", nCounter);
        v->write("nPeriod", nPeriod);
        v->write("nStep", nStep);
        v->write("fReactivity", fReactivity);
        v->write("fTau", fTau);
        v->write("fRate", fRate);
        v->write("fMinRate", fMinRate);
        v->write("fShift", fShift);
        v->write("nReconfigure", nReconfigure);
        v->write("nEnvelope", nEnvelope);
        v->write("nWindow", nWindow);
        v->write("bActive", bActive);

        v->begin_array("vChannels", vChannels, nChannels);
        for (size_t i=0; i<nChannels; ++i)
        {
            const channel_t *c = &vChannels[i];
            v->begin_object(c, sizeof(channel_t));
            {
                v->write("vBuffer", c->vBuffer);
                v->write("vAmp", c->vAmp);
                v->write("vData", c->vData);
                v->write("bFreeze", c->bFreeze);
                v->write("bActive", c->bActive);
            }
            v->end_object();
        }
        v->end_array();

        v->write("vData", vData);
        v->write("vSigRe", vSigRe);
        v->write("vFftReIm", vFftReIm);
        v->write("vWindow", vWindow);
        v->write("vEnvelope", vEnvelope);
    }

} /* namespace lsp */
