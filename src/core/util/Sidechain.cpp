/*
 * Sidechain.cpp
 *
 *  Created on: 14 сент. 2016 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <core/util/Sidechain.h>

#define REFRESH_RATE        0x1000
#define MIN_GAP_ITEMS       0x200

namespace lsp
{
    Sidechain::Sidechain()
    {
        nReactivity         = 0;
        fReactivity         = 0.0f;
        fTau                = 0.0f;
        fRmsValue           = 0.0f;
        nSource             = SCS_MIDDLE;
        nMode               = SCM_RMS;
        nSampleRate         = 0;
        nRefresh            = 0;
        nChannels           = 0;
        fMaxReactivity      = 0.0f;
        fGain               = 1.0f;
        bUpdate             = true;
        bMidSide            = false;
        pPreEq              = NULL;
    }

    Sidechain::~Sidechain()
    {
        destroy();
    }

    void Sidechain::destroy()
    {
        sBuffer.destroy();
    }

    bool Sidechain::init(size_t channels, float max_reactivity)
    {
        if ((channels != 1) && (channels != 2))
            return false;

        nReactivity         = 0;
        fReactivity         = 0.0f;
        fTau                = 0.0f;
        fRmsValue           = 0.0f;
        nSource             = SCS_MIDDLE;
        nMode               = SCM_RMS;
        nSampleRate         = 0;
        nRefresh            = 0;
        nChannels           = channels;
        fMaxReactivity      = max_reactivity;
        fGain               = 1.0f;
        bUpdate             = true;

        return true;
    }

    void Sidechain::set_sample_rate(size_t sr)
    {
        nSampleRate         = sr;
        bUpdate             = true;
        size_t gap          = millis_to_samples(sr, fMaxReactivity);
        size_t buf_size     = (gap < MIN_GAP_ITEMS) ? MIN_GAP_ITEMS : gap;
        sBuffer.init(buf_size * 4, gap);
    }

    void Sidechain::update_settings()
    {
        nReactivity         = millis_to_samples(nSampleRate, fReactivity);
        fTau                = 1.0f - expf(logf(1.0f - M_SQRT1_2) / (nReactivity)); // Tau is based on seconds
        nRefresh            = REFRESH_RATE; // Force the function to be refreshed
    }

    void Sidechain::refresh_processing()
    {
        switch (nMode)
        {
            case SCM_PEAK:
                fRmsValue       = 0.0f;
                break;

            case SCM_UNIFORM:
                fRmsValue       = dsp::h_abs_sum(sBuffer.tail(nReactivity), nReactivity);
                break;

            case SCM_RMS:
                fRmsValue       = dsp::h_sqr_sum(sBuffer.tail(nReactivity), nReactivity);
                break;

            default:
                break;
        }
    }

    bool Sidechain::preprocess(float *out, const float **in, size_t samples)
    {
        if (nChannels == 2)
        {
            if (bMidSide)
            {
                switch (nSource)
                {
                    case SCS_LEFT:
                        dsp::ms_to_left(out, in[0], in[1], samples);
                        if (pPreEq != NULL)
                            pPreEq->process(out, out, samples);
                        dsp::abs1(out, samples);
                        break;
                    case SCS_RIGHT:
                        dsp::ms_to_right(out, in[0], in[1], samples);
                        if (pPreEq != NULL)
                            pPreEq->process(out, out, samples);
                        dsp::abs1(out, samples);
                        break;
                    case SCS_MIDDLE:
                        if (pPreEq != NULL)
                        {
                            pPreEq->process(out, in[0], samples);
                            dsp::abs1(out, samples);
                        }
                        else
                            dsp::abs2(out, in[0], samples);
                        break;
                    case SCS_SIDE:
                        if (pPreEq != NULL)
                        {
                            pPreEq->process(out, in[1], samples);
                            dsp::abs1(out, samples);
                        }
                        else
                            dsp::abs2(out, in[1], samples);
                        break;
                    default:
                        break;
                }
            }
            else
            {
                switch (nSource)
                {
                    case SCS_LEFT:
                        if (pPreEq != NULL)
                        {
                            pPreEq->process(out, in[0], samples);
                            dsp::abs1(out, samples);
                        }
                        else
                            dsp::abs2(out, in[0], samples);
                        break;
                    case SCS_RIGHT:
                        if (pPreEq != NULL)
                        {
                            pPreEq->process(out, in[1], samples);
                            dsp::abs1(out, samples);
                        }
                        else
                            dsp::abs2(out, in[1], samples);
                        break;
                    case SCS_MIDDLE:
                        dsp::lr_to_mid(out, in[0], in[1], samples);
                        if (pPreEq != NULL)
                            pPreEq->process(out, out, samples);
                        dsp::abs1(out, samples);
                        break;
                    case SCS_SIDE:
                        dsp::lr_to_side(out, in[0], in[1], samples);
                        if (pPreEq != NULL)
                            pPreEq->process(out, out, samples);
                        dsp::abs1(out, samples);
                        break;
                    default:
                        break;
                }
            }
        }
        else if (nChannels == 1)
        {
            if (pPreEq != NULL)
            {
                pPreEq->process(out, in[0], samples);
                dsp::abs1(out, samples);
            }
            else
                dsp::abs2(out, in[0], samples);
        }
        else
        {
            dsp::fill_zero(out, samples);
            if (pPreEq != NULL)
            {
                pPreEq->process(out, out, samples);
                dsp::abs1(out, samples);
            }
            return false;
        }

        return true;
    }

    bool Sidechain::preprocess(float *out, const float *in)
    {
        float s;

        if (nChannels == 2)
        {
            if (bMidSide)
            {
                switch (nSource)
                {
                    case SCS_LEFT:
                        s = in[0] + in[1];
                        if (pPreEq != NULL)
                            pPreEq->process(&s, &s, 1);
                        break;
                    case SCS_RIGHT:
                        s = in[0] - in[1];
                        if (pPreEq != NULL)
                            pPreEq->process(&s, &s, 1);
                        break;
                    case SCS_MIDDLE:
                        s = in[0];
                        if (pPreEq != NULL)
                            pPreEq->process(&s, &s, 1);
                        break;
                    case SCS_SIDE:
                        s = in[1];
                        if (pPreEq != NULL)
                            pPreEq->process(&s, &s, 1);
                        break;
                    default:
                        s = in[0];
                        break;
                }
            }
            else
            {
                switch (nSource)
                {
                    case SCS_LEFT:
                        s = in[0];
                        break;
                    case SCS_RIGHT:
                        s = in[1];
                        break;
                    case SCS_MIDDLE:
                        s = (in[0] + in[1])*0.5f;
                        if (pPreEq != NULL)
                            pPreEq->process(&s, &s, 1);
                        break;
                    case SCS_SIDE:
                        s = (in[0] - in[1])*0.5f;
                        if (pPreEq != NULL)
                            pPreEq->process(&s, &s, 1);
                        break;
                    default:
                        s = (in[0] + in[1])*0.5f;
                        break;
                }
            }
        }
        else if (nChannels == 1)
        {
            s       = in[0];
            if (pPreEq != NULL)
                pPreEq->process(&s, &s, 1);
        }
        else
        {
            s       = 0.0f;
            if (pPreEq != NULL)
                pPreEq->process(&s, &s, 1);
            *out    = s;
            return false;
        }

        *out    = (s < 0.0f) ? -s : s;
        return true;
    }

    void Sidechain::process(float *out, const float **in, size_t samples)
    {
        // Check if need update settings
        if (bUpdate)
        {
            update_settings();
            bUpdate     = false;
        }

        // Determine what source to use
        if (!preprocess(out, in, samples))
            return;

        // Adjust pre-amplification
        if (fGain != 1.0f)
            dsp::mul_k2(out, fGain, samples);

        // Update refresh counter
        nRefresh       += samples;
        if (nRefresh >= REFRESH_RATE)
        {
            refresh_processing();
            nRefresh   %= REFRESH_RATE;
        }

        // Calculate sidechain function
        switch (nMode)
        {
            // Peak processing
            case SCM_PEAK:
            {
                while (samples > 0)
                {
                    size_t n    = sBuffer.append(out, samples);
                    sBuffer.shift(n);
                    out        += n;
                    samples    -= n;
                }
                break;
            }

            // Lo-pass filter processing
            case SCM_LPF:
            {
                while (samples > 0)
                {
                    size_t n    = sBuffer.append(out, samples);
                    sBuffer.shift(n);
                    samples    -= n;

                    while (n--)
                    {
                        fRmsValue      += fTau * ((*out) - fRmsValue);
                        *(out++)        = (fRmsValue < 0.0f) ? 0.0f : fRmsValue;
                    }
                }
                break;
            }

            // Uniform processing
            case SCM_UNIFORM:
            {
                if (nReactivity <= 0)
                    break;
                float interval  = nReactivity;

                while (samples > 0)
                {
                    size_t n    = sBuffer.append(out, samples);
                    float *p    = sBuffer.tail(nReactivity + n);
                    samples    -= n;

                    for (size_t i=0; i<n; ++i)
                    {
                        fRmsValue      += *(out) - *(p++);
                        *(out++)        = (fRmsValue < 0.0f) ? 0.0f : fRmsValue / interval;
                    }

                    // Remove old sample
                    sBuffer.shift(n);
                }
                break;
            }

            // RMS processing
            case SCM_RMS:
            {
                if (nReactivity <= 0)
                    break;
                float interval  = nReactivity;

                while (samples > 0)
                {
                    size_t n        = sBuffer.append(out, samples);
                    float *p        = sBuffer.tail(nReactivity + n);
                    samples        -= n;

                    for (size_t i=0; i<n; ++i)
                    {
                        float sample    = *out;
                        float last      = *(p++);
                        fRmsValue      += sample*sample - last*last;
                        *(out++)        = (fRmsValue < 0.0f) ? 0.0f : sqrtf(fRmsValue / interval);
                    }
                    sBuffer.shift(n);
                }
                break;
            }

            default:
                break;
        }
    }

    float Sidechain::process(const float *in)
    {
        // Check if need update settings
        if (bUpdate)
        {
            update_settings();
            bUpdate     = false;
        }

        float out   = 0.0f;
        if (!preprocess(&out, in))
            return out;

        // Adjust pre-amplification
        out *= fGain;

        // Update refresh counter
        nRefresh       ++;
        if (nRefresh >= REFRESH_RATE)
        {
            refresh_processing();
            nRefresh   %= REFRESH_RATE;
        }

        // Calculate sidechain function
        switch (nMode)
        {
            // Peak processing
            case SCM_PEAK:
            {
                sBuffer.append(out);
                sBuffer.shift();
                break;
            }

            // Lo-pass filter processing
            case SCM_LPF:
            {
                sBuffer.append(out);
                sBuffer.shift();
                fRmsValue      += fTau * (out - fRmsValue);
                out             = (fRmsValue < 0.0f) ? 0.0f : fRmsValue;
                break;
            }

            // Uniform processing
            case SCM_UNIFORM:
            {
                if (nReactivity <= 0)
                    break;
                sBuffer.append(out);
                fRmsValue      += out - sBuffer.last(nReactivity + 1);
                out             = (fRmsValue < 0.0f) ? 0.0f : fRmsValue / float(nReactivity);
                sBuffer.shift();
                break;
            }

            // RMS processing
            case SCM_RMS:
            {
                if (nReactivity <= 0)
                    break;
                sBuffer.append(out);
                float last      = sBuffer.last(nReactivity + 1);
                fRmsValue      += out*out - last*last;
                out             = (fRmsValue < 0.0f) ? 0.0f : sqrtf(fRmsValue / float(nReactivity));
                sBuffer.shift();
                break;
            }

            default:
                break;
        }

        return out;
    }

    void Sidechain::dump(IStateDumper *v) const
    {
        v->write_object("sBuffer", &sBuffer);
        v->write("nReactivity", nReactivity);
        v->write("fReactivity", fReactivity);
        v->write("fTau", fTau);
        v->write("fRmsValue", fRmsValue);
        v->write("nSource", nSource);
        v->write("nMode", nMode);
        v->write("nSampleRate", nSampleRate);
        v->write("nRefresh", nRefresh);
        v->write("nChannels", nChannels);
        v->write("fMaxReactivity", fMaxReactivity);
        v->write("fGain", fGain);
        v->write("bUpdate", bUpdate);
        v->write("bMidSide", bMidSide);
        v->write("pPreEq", pPreEq);
    }

} /* namespace lsp */
