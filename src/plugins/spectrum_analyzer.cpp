/*
 * corellator.cpp
 *
 *  Created on: 28 сент. 2015 г.
 *      Author: sadko
 */

#include <core/dsp.h>

#include <plugins/spectrum_analyzer.h>
#include <core/debug.h>
#include <core/windows.h>
#include <core/envelope.h>
#include <core/colors.h>
#include <core/Color.h>

#include <string.h>

#define BUFFER_SIZE         (0x10000 / sizeof(float))

namespace lsp
{
    //-------------------------------------------------------------------------
    spectrum_analyzer_base::spectrum_analyzer_base(const plugin_metadata_t &metadata): plugin_t(metadata)
    {
        pChannels       = NULL;
        pIDisplay       = NULL;
    }

    spectrum_analyzer_base::~spectrum_analyzer_base()
    {
        pChannels       = NULL;
        pIDisplay       = NULL;
    }

    spectrum_analyzer_base::sa_core_t *spectrum_analyzer_base::create_channels(const plugin_metadata_t *m)
    {
        // Determine number of cores
        size_t channels     = 0;
        for (const port_t *p=m->ports; p->id != NULL; ++p)
            if ((p->role == R_AUDIO) && (IS_IN_PORT(p)))
                channels++;

        lsp_trace("this=%p, channels = %d", this, int(channels));

        // Calculate header size
        size_t hdr_size         = (sizeof(sa_core_t) + sizeof(sa_channel_t) * channels + 0x1f) & (~size_t(0x1f));
        size_t max_fft_items    = (1 << spectrum_analyzer_base_metadata::RANK_MAX);
        size_t max_fft_buf_size = sizeof(float) * max_fft_items;
        size_t freq_buf_size    = sizeof(float) * spectrum_analyzer_base_metadata::MESH_POINTS;
        size_t ind_buf_size     = sizeof(uint32_t) * spectrum_analyzer_base_metadata::MESH_POINTS;
        size_t buffers          = channels * 2 + 6; // FFT buffers: (sig_re + fft_amp) * channels + sig_re + sig_im + fft_re + fft_im + window + envelope
        size_t alloc            = hdr_size + freq_buf_size + ind_buf_size + buffers * max_fft_buf_size;

        lsp_trace("header_size = %d", int(hdr_size));
        lsp_trace("max_fft_buf_size = %d", int(max_fft_buf_size));
        lsp_trace("freq_buf_size = %d", int(freq_buf_size));
        lsp_trace("ind_buf_size = %d", int(ind_buf_size));
        lsp_trace("buffers = %d", int(buffers));
        lsp_trace("alloc = %d", int(alloc));

        // Allocate data
        uint8_t *ptr        = new uint8_t[alloc];
        if (ptr == NULL)
            return NULL;

        // Initialize core
        sa_core_t *core     = reinterpret_cast<sa_core_t *>(ptr);
        ptr                += hdr_size;

        core->nChannels     = channels;
        core->bBypass       = false;
        core->nRank         = spectrum_analyzer_base_metadata::RANK_DFL;
        core->nChannel      = 0;
        core->fSelector     = spectrum_analyzer_base_metadata::SELECTOR_DFL;
        core->fMinFreq      = spectrum_analyzer_base_metadata::FREQ_MIN;
        core->fMaxFreq      = spectrum_analyzer_base_metadata::FREQ_MAX;
        core->nWindow       = spectrum_analyzer_base_metadata::WND_DFL;
        core->nEnvelope     = spectrum_analyzer_base_metadata::ENV_DFL;
        core->fReactivity   = spectrum_analyzer_base_metadata::REACT_TIME_DFL;
        core->fTau          = 1.0f;
        core->fPreamp       = spectrum_analyzer_base_metadata::PREAMP_DFL;

        // Initialize pointers and cleanup buffers
        core->vFrequences   = reinterpret_cast<float *>(ptr);
        ptr                += freq_buf_size;
        dsp::fill_zero(core->vFrequences, spectrum_analyzer_base_metadata::MESH_POINTS);
        lsp_trace("vFrequences = %p", core->vFrequences);

        core->vIndexes      = reinterpret_cast<uint32_t *>(ptr);
        ptr                += ind_buf_size;
        memset(core->vIndexes, 0, ind_buf_size);
        lsp_trace("vIndexes = %p", core->vIndexes);

        core->vSigRe        = reinterpret_cast<float *>(ptr);
        ptr                += max_fft_buf_size;
        dsp::fill_zero(core->vSigRe, max_fft_items);
        lsp_trace("vSigRe = %p", core->vSigRe);

        core->vSigIm        = reinterpret_cast<float *>(ptr);
        ptr                += max_fft_buf_size;
        dsp::fill_zero(core->vSigIm, max_fft_items);
        lsp_trace("vSigIm = %p", core->vSigIm);

        core->vFftRe        = reinterpret_cast<float *>(ptr);
        ptr                += max_fft_buf_size;
        dsp::fill_zero(core->vFftRe, max_fft_items);
        lsp_trace("vFftRe = %p", core->vFftRe);

        core->vFftIm        = reinterpret_cast<float *>(ptr);
        ptr                += max_fft_buf_size;
        dsp::fill_zero(core->vFftIm, max_fft_items);
        lsp_trace("vFftIm = %p", core->vFftIm);

        core->vWindow       = reinterpret_cast<float *>(ptr);
        ptr                += max_fft_buf_size;
        init_window(core);
        lsp_trace("vWindow = %p", core->vWindow);

        core->vEnvelope     = reinterpret_cast<float *>(ptr);
        ptr                += max_fft_buf_size;
        envelope::reverse_noise(core->vEnvelope, 1 << core->nRank, envelope::envelope_t(core->nEnvelope));
        lsp_trace("vEnvelope = %p", core->vEnvelope);

        // Initialize channels
        for (size_t i=0; i<channels; ++i)
        {
            sa_channel_t *c     = &core->vChannels[i];

            // Initialize fields
            c->vSigRe           = reinterpret_cast<float *>(ptr);
            ptr                += max_fft_buf_size;
            c->vFftAmp          = reinterpret_cast<float *>(ptr);
            ptr                += max_fft_buf_size;
            c->bOn              = false;
            c->bFreeze          = false;
            c->bSend            = false;
            c->fGain            = 1.0f;
            c->fHue             = 0.0f;
            c->nSamples         = 0;

            // Port references
            c->pIn              = NULL;
            c->pOut             = NULL;
            c->pOn              = NULL;
            c->pFreeze          = NULL;
            c->pHue             = NULL;
            c->pShift           = NULL;
            c->pSpec            = NULL;

            // Clear buffers
            dsp::fill_zero(c->vSigRe, max_fft_items);
            dsp::fill_zero(c->vFftAmp, max_fft_items);
        }

        return core;
    }

    void spectrum_analyzer_base::destroy_channels(sa_core_t *channels)
    {
        if (channels == NULL)
            return;
        delete [] reinterpret_cast<uint8_t *>(channels);
    }

    void spectrum_analyzer_base::init(IWrapper *wrapper)
    {
        // Pass wrapper
        plugin_t::init(wrapper);

        // Allocate channels
        pChannels       = create_channels(pMetadata);
        if (pChannels == NULL)
            return;

        // Seek for first input port
        size_t port_id = 0;
        while (port_id < vPorts.size())
        {
            IPort *vp = vPorts[port_id];
            if (vp == NULL)
                continue;
            const port_t *p = vp->metadata();
            if (p == NULL)
                continue;
            if ((p->id != NULL) && (p->role == R_AUDIO) && (IS_IN_PORT(p)))
                break;
            port_id++;
        }

        lsp_trace("port_id = %d", int(port_id));

        // Now we are available to map the ports for channels
        for (size_t i=0; i<pChannels->nChannels; ++i)
        {
            lsp_trace("binding channel %d", int(i));

            IPort *vp = vPorts[port_id];
            if (vp == NULL)
                break;
            const port_t *p = vp->metadata();
            if (p == NULL)
                break;
            if ((p->id == NULL) || (p->role != R_AUDIO) || (!IS_IN_PORT(p)))
                break;

            sa_channel_t *c     = &pChannels->vChannels[i];
            c->pIn              = vPorts[port_id++];
            c->pOut             = vPorts[port_id++];
            c->pOn              = vPorts[port_id++];
            c->pSolo            = vPorts[port_id++];
            c->pFreeze          = vPorts[port_id++];
            c->pHue             = vPorts[port_id++];
            c->pShift           = vPorts[port_id++];
            c->pSpec            = vPorts[port_id++];

            // Sync metadata
            const port_t *meta  = c->pOn->metadata();
            if (meta != NULL)
                c->bOn              = meta->start >= 0.5f;
            meta                = c->pSolo->metadata();
            if (meta != NULL)
                c->bSolo        = meta->start >= 0.5f;
            meta                = c->pFreeze->metadata();
            if (meta != NULL)
                c->bFreeze          = meta->start >= 0.5f;
            meta                = c->pShift->metadata();
            if (meta != NULL)
                c->fGain            = meta->start;

            lsp_trace("channel %d successful bound", int(i));
        }

        // Initialize basic ports
        pChannels->pBypass      = vPorts[port_id++];
        pChannels->pTolerance   = vPorts[port_id++];
        pChannels->pWindow      = vPorts[port_id++];
        pChannels->pEnvelope    = vPorts[port_id++];
        pChannels->pPreamp      = vPorts[port_id++];
        pChannels->pReactivity  = vPorts[port_id++];
        pChannels->pChannel     = vPorts[port_id++];
        pChannels->pSelector    = vPorts[port_id++];
        pChannels->pFrequency   = vPorts[port_id++];
        pChannels->pLevel       = vPorts[port_id++];

        // Initialize values
        pChannels->fMinFreq     = pChannels->pFrequency->metadata()->min;
        pChannels->fMaxFreq     = pChannels->pFrequency->metadata()->max;

        lsp_trace("this=%p, basic ports successful bound", this);
    }

    void spectrum_analyzer_base::destroy()
    {
        if (pChannels != NULL)
        {
            destroy_channels(pChannels);
            pChannels       = NULL;
        }
        if (pIDisplay != NULL)
        {
            pIDisplay->detroy();
            pIDisplay       = NULL;
        }
    }

    void spectrum_analyzer_base::update_settings()
    {
        if (pChannels == NULL)
            return;
        lsp_trace("this=%p", this);

        bool update_window      = false;
        bool update_env         = false;
        bool update_buf         = false;
        size_t rank             = pChannels->pTolerance->getValue() + spectrum_analyzer_base_metadata::RANK_MIN;
        if (rank != pChannels->nRank)
        {
            pChannels->nRank        = rank;
            update_window           = true;
            update_env              = true;
            update_buf              = true;
            update_frequences();
        }

        size_t window           = pChannels->pWindow->getValue();
        size_t fft_size         = 1 << pChannels->nRank;
        if (window != pChannels->nWindow)
        {
            pChannels->nWindow      = window;
            update_window           = true;
        }

        size_t env              = pChannels->pEnvelope->getValue();
        if (env != pChannels->nEnvelope)
        {
            pChannels->nEnvelope    = env;
            update_env              = true;
        }

        if (update_window)
            init_window(pChannels);
        if (update_env)
            envelope::reverse_noise(pChannels->vEnvelope, fft_size >> 1, envelope::envelope_t(pChannels->nEnvelope));

        pChannels->bBypass      = pChannels->pBypass->getValue();
        pChannels->nChannel     = pChannels->pChannel->getValue();
        pChannels->fSelector    = pChannels->pSelector->getValue() * 0.01;
        pChannels->fPreamp      = pChannels->pPreamp->getValue();

        set_reactivity(pChannels->pReactivity->getValue());

        lsp_trace("bypass       = %s",     (pChannels->bBypass) ? "true" : "false");
        lsp_trace("rank         = %d",     int(pChannels->nRank));
        lsp_trace("channel      = %d",     int(pChannels->nChannel));
        lsp_trace("selector     = %.3f",   pChannels->fSelector);
        lsp_trace("preamp       = %.3f",   pChannels->fPreamp);
        lsp_trace("reactivity   = %.3f",   pChannels->fReactivity);
        lsp_trace("tau          = %.5f",   pChannels->fTau);

        size_t step                 = pChannels->nMaxSamples / pChannels->nChannels;

        // Check that there are soloing channels
        size_t has_solo         = 0;
        for (size_t i=0; i<pChannels->nChannels; ++i)
        {
            sa_channel_t *c     = &pChannels->vChannels[i];
            if (c->pSolo->getValue() >= 0.5f)
                has_solo++;
        }

        // Process channel parameters
        for (size_t i=0; i<pChannels->nChannels; ++i)
        {
            sa_channel_t *c     = &pChannels->vChannels[i];
            c->bOn              = c->pOn->getValue() >= 0.5f;
            c->bSolo            = c->pSolo->getValue() >= 0.5f;
            c->bFreeze          = c->pFreeze->getValue() >= 0.5f;
            c->bSend            = c->bOn && ((has_solo == 0) || ((has_solo > 0) && (c->bSolo)));
            c->fGain            = c->pShift->getValue();
            c->fHue             = c->pHue->getValue();
            if (update_buf)
            {
                dsp::fill_zero(c->vSigRe, 1 << pChannels->nRank);
                dsp::fill_zero(c->vFftAmp, 1 << pChannels->nRank);
                c->nSamples         = step * i;
            }

            lsp_trace("c[%d].on          = %s",     int(i), (c->bOn) ? "true" : "false");
            lsp_trace("c[%d].freeze      = %s",     int(i), (c->bFreeze) ? "true" : "false");
            lsp_trace("c[%d].gain        = %.3f",   int(i), c->fGain);
            lsp_trace("c[%d].samples     = %d",     int(i), int(c->nSamples));
        }
    }

    void spectrum_analyzer_base::update_sample_rate(long sr)
    {
        lsp_trace("this=%p, sample_rate = %d", this, int(sr));
        update_frequences();

        pChannels->nMaxSamples      = float(fSampleRate) / float(spectrum_analyzer_base_metadata::REFRESH_RATE);
        size_t step                 = pChannels->nMaxSamples / pChannels->nChannels;
        for (size_t i=0; i<pChannels->nChannels; ++i)
            pChannels->vChannels[i].nSamples    = step * i;
        lsp_trace("nMaxSamples      = %d", int(pChannels->nMaxSamples));
        set_reactivity(pChannels->fReactivity);
    }

    void spectrum_analyzer_base::update_frequences()
    {
        size_t fft_size     = 1 << pChannels->nRank;
        size_t fft_csize    = (fft_size >> 1) + 1;

        float *frq          = pChannels->vFrequences;
        uint32_t *ind       = pChannels->vIndexes;
        float norm          = logf(pChannels->fMaxFreq / pChannels->fMinFreq) / (spectrum_analyzer_base_metadata::MESH_POINTS);
        float scale         = float(fft_size) / float(fSampleRate);

        lsp_trace("min_freq     = %.3f", pChannels->fMinFreq);
        lsp_trace("max_freq     = %.3f", pChannels->fMaxFreq);
        lsp_trace("norm         = %.3f", norm);
        lsp_trace("scale        = %.3f", scale);

        for (size_t i=0; i<spectrum_analyzer_base_metadata::MESH_POINTS; ++i, ++frq, ++ind)
        {
            float f             = pChannels->fMinFreq * expf(i * norm);
            size_t idx          = scale * f;
            if (idx > fft_csize)
                idx                 = fft_csize + 1;

            *frq                = f;
            *ind                = idx;
        }
    }

    void spectrum_analyzer_base::process(size_t samples)
    {
        if (pChannels == NULL)
            return;

        // Always query for drawing
        pWrapper->query_display_draw();

//        // Check that there are soloing channels
//        size_t has_solo         = 0;
//        for (size_t i=0; i<pChannels->nChannels; ++i)
//        {
//            if (pChannels->vChannels[i].bSolo)
//                has_solo++;
//        }

        // Now process the channels
        size_t fft_size     = 1 << pChannels->nRank;
        size_t fft_csize    = (fft_size >> 1);

        for (size_t i=0; i<pChannels->nChannels; ++i)
        {
            // Get channel pointer
            sa_channel_t *c     = &pChannels->vChannels[i];

            // Get primitives
            const float *in     = reinterpret_cast<float *>(c->pIn->getBuffer());
            float *out          = reinterpret_cast<float *>(c->pOut->getBuffer());
            mesh_t *mesh        = reinterpret_cast<mesh_t *>(c->pSpec->getBuffer());

            // Determine if there is a request for sync
            bool mesh_request   = (mesh != NULL) && (mesh->isEmpty());
            bool data_request   = (pChannels->nChannel == i);

            // Bypass signal
            dsp::copy(out, in, samples);
            if (pChannels->bBypass)
            {
                if (mesh_request)
                    mesh->data(2, 0);       // Set mesh to empty data
                if (data_request)
                {
                    pChannels->pFrequency->setValue(0);
                    pChannels->pLevel->setValue(0);
                }
                continue;
            }

            // Process signal by channel
            size_t left     = samples;
            while (true)
            {
                // Calculate amount of samples left to process before FFT processing is triggered
                size_t trigger      = pChannels->nMaxSamples - c->nSamples;

                if (trigger > 0)
                {
                    // Calculate number of samples to put to the buffer
                    size_t put          = (left < trigger) ? left : trigger;

                    // Put the samples to the buffer, limit to fft_size
                    if (put >= fft_size)
                    {
                        dsp::copy(c->vSigRe, in, fft_size); // Overwrite all data
                        put     = fft_size;
                    }
                    else
                    {
                        dsp::copy(c->vSigRe, &c->vSigRe[put], fft_size - put); // Remove data from the beginning
                        dsp::copy(&c->vSigRe[fft_size - put], in, put); // append data to the end
                    }

                    // Update counters and pointer
                    in                 += put;
                    left               -= put;
                    trigger            -= put;
                    c->nSamples        += put;
                }

                // Trigger FFT if buffer is fully filled
                if (trigger == 0)
                {
                    // Perform FFT only for active channels
                    if (!c->bFreeze)
                    {
                        if ((!c->bOn) || (pChannels->bBypass))
                        {
                            dsp::fill_zero(c->vFftAmp, fft_size);
                        }
                        else
                        {
                            // Apply window to the temporary buffer
                            dsp::mul3(pChannels->vSigRe, c->vSigRe, pChannels->vWindow, fft_size);
                            // Do FFT
                            dsp::direct_fft(pChannels->vFftRe, pChannels->vFftIm, pChannels->vSigRe, pChannels->vSigIm, pChannels->nRank);
                            // Leave only positive frequencies
                            dsp::combine_fft(pChannels->vFftRe, pChannels->vFftIm, pChannels->vFftRe, pChannels->vFftIm, pChannels->nRank);
                            // Get complex argument
                            dsp::complex_mod(pChannels->vFftRe, pChannels->vFftRe, pChannels->vFftIm, fft_csize);
                            // Mix with the previous value
                            dsp::mix2(c->vFftAmp, pChannels->vFftRe, 1.0 - pChannels->fTau, pChannels->fTau, fft_csize);
                        }
                    }

                    // Update counter
                    c->nSamples        -= pChannels->nMaxSamples;
                }

                // Check that there are no more samples to process
                if (left == 0)
                    break;
            }

            // Copy data to output channel
            if (data_request)
            {
                size_t idx  = pChannels->fSelector * ((fft_size - 1) >> 1);
                pChannels->pFrequency->setValue(float(idx * fSampleRate) / float(fft_size));
                pChannels->pLevel->setValue(c->vFftAmp[idx] * c->fGain * pChannels->fPreamp * pChannels->vEnvelope[idx]);
            }

            // Copy data to mesh
            if (mesh_request)
            {
                // Determine that mesh has to be sent to the UI
//                bool send       = c->bOn;
//                if (has_solo > 0)
//                    send            = send && c->bSolo;

                if (c->bSend)
                {
                    // Copy frequency points
                    dsp::copy(mesh->pvData[0], pChannels->vFrequences, spectrum_analyzer_base_metadata::MESH_POINTS);

                    float *v        = mesh->pvData[1];
                    uint32_t *idx   = pChannels->vIndexes;
                    // Small optimization to prevent additional multiplications
                    if (pChannels->nEnvelope == envelope::WHITE_NOISE)
                    {
                        for (size_t i=0; i<spectrum_analyzer_base_metadata::MESH_POINTS; ++i)
                            *(v++)          = c->vFftAmp[idx[i]];
                    }
                    else
                    {
                        for (size_t i=0; i<spectrum_analyzer_base_metadata::MESH_POINTS; ++i)
                        {
                            size_t j        = idx[i];
                            *(v++)          = c->vFftAmp[j] * pChannels->vEnvelope[j];
                        }
                    }
                    dsp::scale2(mesh->pvData[1], c->fGain * pChannels->fPreamp, spectrum_analyzer_base_metadata::MESH_POINTS);

                    // Mark mesh containing data
                    mesh->data(2, spectrum_analyzer_base_metadata::MESH_POINTS);
                }
                else
                    mesh->data(2, 0);
            }
        }
    }

    void spectrum_analyzer_base::set_reactivity(float reactivity)
    {
        lsp_trace("this=%p, reactivity = %f, sample_rate=%d, max_samples=%d",
                this, reactivity, int(fSampleRate), int(pChannels->nMaxSamples));
        pChannels->fReactivity      = reactivity;
        pChannels->fTau             = 1.0f - expf(logf(1.0f - M_SQRT1_2) / seconds_to_samples(fSampleRate / pChannels->nMaxSamples, reactivity));
    }

    void spectrum_analyzer_base::init_window(sa_core_t *core)
    {
        size_t fft_size     = 1 << core->nRank;
        size_t fft_minsize  = 1 << spectrum_analyzer_base_metadata::RANK_MIN;

        // Create window
        windows::window(core->vWindow, fft_size, windows::window_t(core->nWindow));

        // Calculate normalization multiplier
        float norm          = (1.0f / float(fft_minsize));

        // Normalize window
        dsp::scale2(core->vWindow, norm, fft_size);
    }

    bool spectrum_analyzer_base::inline_display(ICanvas *cv, size_t width, size_t height)
    {
        if (pChannels == NULL)
            return false;

        // Check proportions
        if (height > (R_GOLDEN_RATIO * width))
            height  = R_GOLDEN_RATIO * width;

        // Init canvas
        if (!cv->init(width, height))
            return false;
        width   = cv->width();
        height  = cv->height();

        // Clear background
        bool bypass = pChannels->bBypass;
        cv->set_color_rgb((bypass) ? CV_DISABLED : CV_BACKGROUND);
        cv->paint();

        // Draw axis
        cv->set_line_width(1.0);

        float zx    = 1.0f/SPEC_FREQ_MIN;
        float zy    = 1.0f/GAIN_AMP_M_72_DB;
        float dx    = width/(logf(SPEC_FREQ_MAX)-logf(SPEC_FREQ_MIN));
        float dy    = -height/(logf(GAIN_AMP_P_24_DB)-logf(GAIN_AMP_M_72_DB));

        // Draw vertical lines
        cv->set_color_rgb(CV_YELLOW, 0.5f);
        for (float i=100.0f; i<SPEC_FREQ_MAX; i *= 10.0f)
        {
            float ax = dx*(logf(i*zx));
            cv->line(ax, 0, ax, height);
        }

        // Draw horizontal lines
        cv->set_color_rgb(CV_WHITE, 0.5f);
        for (float i=GAIN_AMP_M_48_DB; i<GAIN_AMP_P_24_DB; i *= GAIN_AMP_P_24_DB)
        {
            float ay = height + dy*(logf(i*zy));
            cv->line(0, ay, width, ay);
        }

        // Allocate buffer: f, a(f), x, y
        pIDisplay           = float_buffer_t::reuse(pIDisplay, 4, width);
        float_buffer_t *b   = pIDisplay;
        if (b == NULL)
            return false;

        if (!bypass)
        {
            Color col(CV_MESH);
            cv->set_line_width(2);

            float ni        = float(spectrum_analyzer_base_metadata::MESH_POINTS) / width; // Normalizing index
            uint32_t *idx   = pChannels->vIndexes;
            for (size_t i=0; i<pChannels->nChannels; ++i)
            {
                // Output only active channel
                sa_channel_t *c = &pChannels->vChannels[i];
                if (!c->bSend)
                    continue;

                if (pChannels->nEnvelope == envelope::WHITE_NOISE)
                {
                    for (size_t j=0; j<width; ++j)
                    {
                        size_t k        = j*ni;
                        b->v[0][j]      = pChannels->vFrequences[k];
                        b->v[1][j]      = c->vFftAmp[idx[k]];
                    }
                }
                else
                {
                    for (size_t j=0; j<width; ++j)
                    {
                        size_t k        = j*ni;
                        size_t l        = idx[k];
                        b->v[0][j]      = pChannels->vFrequences[k];
                        b->v[1][j]      =  c->vFftAmp[l] * pChannels->vEnvelope[l];
                    }
                }

                dsp::scale2(b->v[1], c->fGain * pChannels->fPreamp, width);

                dsp::fill(b->v[2], 0.0f, width);
                dsp::fill(b->v[3], height, width);
                dsp::axis_apply_log(b->v[2], b->v[3], b->v[0], zx, dx, 0.0f, width);
                dsp::axis_apply_log(b->v[2], b->v[3], b->v[1], zy, 0.0f, dy, width);

                // Draw mesh
                col.hue(c->fHue);
                cv->set_color(col);
                cv->draw_lines(b->v[2], b->v[3], width);
            }
        }

        return true;
    }

    //-------------------------------------------------------------------------
    spectrum_analyzer_x1::spectrum_analyzer_x1(): spectrum_analyzer_base(metadata)
    {
    }

    spectrum_analyzer_x2::spectrum_analyzer_x2(): spectrum_analyzer_base(metadata)
    {
    }

    spectrum_analyzer_x4::spectrum_analyzer_x4(): spectrum_analyzer_base(metadata)
    {
    }

    spectrum_analyzer_x8::spectrum_analyzer_x8(): spectrum_analyzer_base(metadata)
    {
    }

    spectrum_analyzer_x12::spectrum_analyzer_x12(): spectrum_analyzer_base(metadata)
    {
    }

    spectrum_analyzer_x16::spectrum_analyzer_x16(): spectrum_analyzer_base(metadata)
    {
    }
} /* namespace lsp */


