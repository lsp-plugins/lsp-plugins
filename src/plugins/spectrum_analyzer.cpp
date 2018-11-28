/*
 * corellator.cpp
 *
 *  Created on: 28 сент. 2015 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>

#include <plugins/spectrum_analyzer.h>
#include <core/debug.h>
#include <core/windows.h>
#include <core/envelope.h>
#include <core/colors.h>
#include <core/util/Color.h>

#include <string.h>

#define BUFFER_SIZE         (0x10000 / sizeof(float))

namespace lsp
{
    //-------------------------------------------------------------------------
    spectrum_analyzer_base::spectrum_analyzer_base(const plugin_metadata_t &metadata): plugin_t(metadata)
    {
        nChannels       = 0;
        vChannels       = NULL;
        pData           = NULL;
        vFrequences     = NULL;
        vIndexes        = NULL;

        bBypass         = false;
        nChannel        = 0;
        fSelector       = 0;
        fMinFreq        = 0;
        fMaxFreq        = 0;
        fReactivity     = 0.0f;
        fTau            = 0.0f;
        fPreamp         = 0.0f;
        fZoom           = 0.0f;

        pBypass         = NULL;
        pMode           = NULL;
        pTolerance      = NULL;
        pWindow         = NULL;
        pEnvelope       = NULL;
        pPreamp         = NULL;
        pZoom           = NULL;
        pReactivity     = NULL;
        pChannel        = NULL;
        pSelector       = NULL;
        pFrequency      = NULL;
        pLevel          = NULL;

        pFreeze         = NULL;
        pSpp            = NULL;

        for (size_t i=0; i<2; ++i)
        {
            vSpc[i].nPortId     = -1;

            vSpc[i].pPortId     = NULL;
            vSpc[i].pFBuffer    = NULL;
        }

        pIDisplay       = NULL;
    }

    spectrum_analyzer_base::~spectrum_analyzer_base()
    {
        vChannels       = NULL;
        pIDisplay       = NULL;
    }

    bool spectrum_analyzer_base::create_channels(size_t channels)
    {
        lsp_trace("this=%p, channels = %d", this, int(channels));

        // Calculate header size
        size_t hdr_size         = ALIGN_SIZE(sizeof(sa_channel_t) * channels, 64);
        size_t freq_buf_size    = sizeof(float) * spectrum_analyzer_base_metadata::MESH_POINTS;
        size_t ind_buf_size     = sizeof(uint32_t) * spectrum_analyzer_base_metadata::MESH_POINTS;
        size_t alloc            = hdr_size + freq_buf_size + ind_buf_size;

        lsp_trace("header_size = %d", int(hdr_size));
        lsp_trace("freq_buf_size = %d", int(freq_buf_size));
        lsp_trace("ind_buf_size = %d", int(ind_buf_size));
        lsp_trace("alloc = %d", int(alloc));

        // Allocate data
        uint8_t *ptr        = alloc_aligned<uint8_t>(pData, alloc, 64);
        if (ptr == NULL)
            return false;
        lsp_guard_assert( uint8_t *guard = ptr );

        // Initialize core
        nChannels       = channels;
        nChannel        = 0;
        fSelector       = spectrum_analyzer_base_metadata::SELECTOR_DFL;
        fMinFreq        = spectrum_analyzer_base_metadata::FREQ_MIN;
        fMaxFreq        = spectrum_analyzer_base_metadata::FREQ_MAX;
        fReactivity     = spectrum_analyzer_base_metadata::REACT_TIME_DFL;
        fTau            = 1.0f;
        fPreamp         = spectrum_analyzer_base_metadata::PREAMP_DFL;

        // Initialize pointers and cleanup buffers
        vChannels       = reinterpret_cast<sa_channel_t *>(ptr);
        ptr            += hdr_size;
        lsp_trace("vChannels = %p", vChannels);

        vFrequences   = reinterpret_cast<float *>(ptr);
        ptr          += freq_buf_size;
        dsp::fill_zero(vFrequences, spectrum_analyzer_base_metadata::MESH_POINTS);
        lsp_trace("vFrequences = %p", vFrequences);

        vIndexes      = reinterpret_cast<uint32_t *>(ptr);
        ptr          += ind_buf_size;
        memset(vIndexes, 0, ind_buf_size);
        lsp_trace("vIndexes = %p", vIndexes);

        // Initialize channels
        for (size_t i=0; i<channels; ++i)
        {
            sa_channel_t *c     = &vChannels[i];

            // Initialize fields
            c->bOn              = false;
            c->bSolo            = false;
            c->bSend            = false;
            c->fGain            = 1.0f;
            c->fHue             = 0.0f;

            // Port references
            c->pIn              = NULL;
            c->pOut             = NULL;
            c->pOn              = NULL;
            c->pFreeze          = NULL;
            c->pHue             = NULL;
            c->pShift           = NULL;
            c->pSpec            = NULL;
        }

        lsp_assert(ptr <= &guard[alloc]);

        return true;
    }

    void spectrum_analyzer_base::init(IWrapper *wrapper)
    {
        // Pass wrapper
        plugin_t::init(wrapper);

        // Determine number of channels
        size_t channels     = 0;
        if (pMetadata == NULL)
            return;
        for (const port_t *p=pMetadata->ports; p->id != NULL; ++p)
            if ((p->role == R_AUDIO) && (IS_IN_PORT(p)))
                channels++;

        sAnalyzer.init(channels, spectrum_analyzer_base_metadata::RANK_MAX);
        sAnalyzer.set_rate(spectrum_analyzer_base_metadata::REFRESH_RATE);

        // Allocate channels
        if (!create_channels(channels))
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
        for (size_t i=0; i<nChannels; ++i)
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

            sa_channel_t *c     = &vChannels[i];
            c->pIn              = vPorts[port_id++];
            c->pOut             = vPorts[port_id++];
            c->pOn              = vPorts[port_id++];
            c->pSolo            = vPorts[port_id++];
            c->pFreeze          = vPorts[port_id++];
            c->pHue             = vPorts[port_id++];
            c->pShift           = vPorts[port_id++];
            c->pSpec            = vPorts[port_id++];

            // Sync metadata
            const port_t *meta  = c->pSolo->metadata();
            if (meta != NULL)
                c->bSolo        = meta->start >= 0.5f;
            meta                = c->pShift->metadata();
            if (meta != NULL)
                c->fGain            = meta->start;

            lsp_trace("channel %d successful bound", int(i));
        }

        // Initialize basic ports
        pBypass         = vPorts[port_id++];
        pMode           = vPorts[port_id++];
        pTolerance      = vPorts[port_id++];
        pWindow         = vPorts[port_id++];
        pEnvelope       = vPorts[port_id++];
        pPreamp         = vPorts[port_id++];
        pZoom           = vPorts[port_id++];
        pReactivity     = vPorts[port_id++];
        pChannel        = vPorts[port_id++];
        pSelector       = vPorts[port_id++];
        pFrequency      = vPorts[port_id++];
        pLevel          = vPorts[port_id++];

        // Bind spectralizer ports
        if (nChannels > 2)
            vSpc[0].pPortId     = vPorts[port_id++];
        vSpc[0].pFBuffer    = vPorts[port_id++];

        if (nChannels >= 2)
        {
            if (nChannels > 2)
                vSpc[1].pPortId     = vPorts[port_id++];
            vSpc[1].pFBuffer    = vPorts[port_id++];
        }

        // Initialize values
        fMinFreq        = pFrequency->metadata()->min;
        fMaxFreq        = pFrequency->metadata()->max;

        lsp_trace("this=%p, basic ports successful bound", this);
    }

    void spectrum_analyzer_base::destroy()
    {
        sAnalyzer.destroy();

        if (pData != NULL)
        {
            free_aligned(pData);
            pData           = NULL;
        }
        vFrequences     = NULL;
        vIndexes        = NULL;

        if (pIDisplay != NULL)
        {
            pIDisplay->detroy();
            pIDisplay       = NULL;
        }
    }

    spectrum_analyzer_base::mode_t spectrum_analyzer_base::decode_mode(size_t mode)
    {
        if (nChannels == 1)
        {
            switch (mode)
            {
                case 0: return SA_ANALYZER;
                case 1: return SA_MASTERING;
                case 2: return SA_SPECTRALIZER;
                default:
                    return SA_ANALYZER;
            }
        }
        else if (nChannels == 2)
        {
            switch (mode)
            {
                case 0: return SA_ANALYZER_STEREO;
                case 1: return SA_MASTERING_STEREO;
                case 2: return SA_SPECTRALIZER;
                case 3: return SA_SPECTRALIZER_STEREO;
                default:
                    return SA_ANALYZER;
            }
        }
        else
        {
            switch (mode)
            {
                case 0: return SA_ANALYZER;
                case 1: return SA_ANALYZER_STEREO;
                case 2: return SA_MASTERING;
                case 3: return SA_MASTERING_STEREO;
                case 4: return SA_SPECTRALIZER;
                case 5: return SA_SPECTRALIZER_STEREO;
                default:
                    return SA_ANALYZER;
            }
        }
    }

    void spectrum_analyzer_base::update_multiple_settings()
    {
        // Check that there are soloing channels
        size_t has_solo         = 0;
        for (size_t i=0; i<nChannels; ++i)
        {
            sa_channel_t *c     = &vChannels[i];
            if (c->pSolo->getValue() >= 0.5f)
                has_solo++;
        }

        // Process channel parameters
        bool freeze_all     = pFreeze->getValue() >= 0.5f;

        for (size_t i=0; i<nChannels; ++i)
        {
            sa_channel_t *c     = &vChannels[i];

            c->bOn              = c->pOn->getValue() >= 0.5f;
            c->bFreeze          = (freeze_all) || (c->pFreeze->getValue() >= 0.5f);
            c->bSolo            = c->pSolo->getValue() >= 0.5f;
            c->bSend            = (c->bOn) && ((has_solo == 0) || ((has_solo > 0) && (c->bSolo)));
            c->fGain            = c->pShift->getValue();
            c->fHue             = c->pHue->getValue();
        }
    }

    void spectrum_analyzer_base::update_x2_settings(ssize_t ch1, ssize_t ch2)
    {
        bool freeze_all     = pFreeze->getValue() >= 0.5f;
        ssize_t nc          = nChannels;

        if (ch1 >= nc)
            ch1 -= nc;
        if (ch2 >= nc)
            ch2 -= nc;

        for (ssize_t i=0; i<nc; ++i)
        {
            sa_channel_t *c     = &vChannels[i];

            c->bOn              = (i == ch1) || (i == ch2);
            c->bFreeze          = (freeze_all) || (c->pFreeze->getValue() >= 0.5f);
            c->bSolo            = false;
            c->bSend            = c->bOn;
            c->fGain            = c->pShift->getValue();
            c->fHue             = c->pHue->getValue();
        }
    }

    void spectrum_analyzer_base::update_spectralizer_x2_settings(ssize_t ch1, ssize_t ch2)
    {
        bool freeze_all     = pFreeze->getValue() >= 0.5f;
        ssize_t nc          = nChannels;

        if (ch1 >= nc)
            ch1 -= nc;
        if (ch2 >= nc)
            ch2 -= nc;

        for (ssize_t i=0; i<nc; ++i)
        {
            sa_channel_t *c     = &vChannels[i];

            c->bOn              = (i == ch1) || (i == ch2);
            c->bFreeze          = (freeze_all) || (c->pFreeze->getValue() >= 0.5f);
            c->bSolo            = false;
            c->bSend            = false; // We do not need to send mesh data because utilizing framebuffer ports
            c->fGain            = c->pShift->getValue();
            c->fHue             = c->pHue->getValue();
        }
    }

    void spectrum_analyzer_base::update_settings()
    {
        // Update global settings
        bBypass                 = pBypass->getValue();
        nChannel                = pChannel->getValue();
        fSelector               = pSelector->getValue() * 0.01;
        fPreamp                 = pPreamp->getValue();
        fZoom                   = pZoom->getValue();
        size_t rank             = pTolerance->getValue() + spectrum_analyzer_base_metadata::RANK_MIN;

        lsp_trace("rank         = %d",     int(rank));
        lsp_trace("channel      = %d",     int(nChannel));
        lsp_trace("selector     = %.3f",   fSelector);
        lsp_trace("preamp       = %.3f",   fPreamp);
        lsp_trace("reactivity   = %.3f",   fReactivity);
        lsp_trace("tau          = %.5f",   fTau);

        // Update channel state depending on the mode
        mode_t mode = decode_mode(pMode->getValue());

        switch (mode)
        {
            case SA_ANALYZER:
            case SA_MASTERING:
                update_multiple_settings();
                break;

            case SA_ANALYZER_STEREO:
            case SA_MASTERING_STEREO:
                if (nChannels > 2)
                    update_x2_settings(vSpc[0].pPortId->getValue(), vSpc[1].pPortId->getValue());
                else if (nChannels == 2)
                    update_x2_settings(0, 1);
                else
                    update_x2_settings(0, -1); // This should not happen, but... let's do a special scenario
                break;

            case SA_SPECTRALIZER:
            case SA_SPECTRALIZER_STEREO:
            default:
                break;
        }

        // Update analysis parameters
        bool sync_freqs         = rank != sAnalyzer.get_rank();
        if (sync_freqs)
            sAnalyzer.set_rank(rank);

        sAnalyzer.set_reactivity(pReactivity->getValue());
        sAnalyzer.set_window(pWindow->getValue());
        sAnalyzer.set_envelope(pEnvelope->getValue());

        for (size_t i=0; i<nChannels; ++i)
        {
            sa_channel_t *c     = &vChannels[i];
            sAnalyzer.enable_channel(i, c->bOn);
            sAnalyzer.freeze_channel(i, c->bFreeze);
        }

        // Reconfigure analyzer if required
        if (sAnalyzer.needs_reconfiguration())
            sAnalyzer.reconfigure();

        if (sync_freqs)
            sAnalyzer.get_frequencies(vFrequences, vIndexes,
                    fMinFreq, fMaxFreq, spectrum_analyzer_base_metadata::MESH_POINTS);
    }

    void spectrum_analyzer_base::update_sample_rate(long sr)
    {
        lsp_trace("this=%p, sample_rate = %d", this, int(sr));
        sAnalyzer.set_sample_rate(sr);
        if (sAnalyzer.needs_reconfiguration())
            sAnalyzer.reconfigure();

        sAnalyzer.get_frequencies(vFrequences, vIndexes,
                fMinFreq, fMaxFreq, spectrum_analyzer_base_metadata::MESH_POINTS);
    }

    void spectrum_analyzer_base::process(size_t samples)
    {
        // Always query for drawing
        pWrapper->query_display_draw();

        // Now process the channels
        size_t fft_size     = 1 << sAnalyzer.get_rank();

        for (size_t i=0; i<nChannels; ++i)
        {
            // Get channel pointer
            sa_channel_t *c     = &vChannels[i];

            // Get primitives
            const float *in     = reinterpret_cast<float *>(c->pIn->getBuffer());
            float *out          = reinterpret_cast<float *>(c->pOut->getBuffer());
            mesh_t *mesh        = reinterpret_cast<mesh_t *>(c->pSpec->getBuffer());

            // Determine if there is a request for sync
            bool mesh_request   = (mesh != NULL) && (mesh->isEmpty());
            bool data_request   = (nChannel == i);

            // Bypass signal
            dsp::copy(out, in, samples);
            sAnalyzer.process(i, in, samples);  // Process the data

            if (bBypass)
            {
                if (mesh_request)
                    mesh->data(2, 0);       // Set mesh to empty data
                if (data_request)
                {
                    pFrequency->setValue(0);
                    pLevel->setValue(0);
                }
                continue;
            }

            // Copy data to output channel
            if (data_request)
            {
                size_t idx  = fSelector * ((fft_size - 1) >> 1);
                pFrequency->setValue(float(idx * fSampleRate) / float(fft_size));
                float lvl = sAnalyzer.get_level(i, idx);
                pLevel->setValue(lvl * c->fGain * fPreamp );
            }

            // Copy data to mesh
            if (mesh_request)
            {
                if (c->bSend)
                {
                    // Copy frequency points
                    dsp::copy(mesh->pvData[0], vFrequences, spectrum_analyzer_base_metadata::MESH_POINTS);

                    float *v        = mesh->pvData[1];
                    uint32_t *idx   = vIndexes;

                    sAnalyzer.get_spectrum(i, v, idx, spectrum_analyzer_base_metadata::MESH_POINTS);
                    dsp::scale2(v, c->fGain * fPreamp, spectrum_analyzer_base_metadata::MESH_POINTS);

                    // Mark mesh containing data
                    mesh->data(2, spectrum_analyzer_base_metadata::MESH_POINTS);
                }
                else
                    mesh->data(2, 0);
            }
        }
    }

    bool spectrum_analyzer_base::inline_display(ICanvas *cv, size_t width, size_t height)
    {
        // Check proportions
        if (height > (R_GOLDEN_RATIO * width))
            height  = R_GOLDEN_RATIO * width;

        // Init canvas
        if (!cv->init(width, height))
            return false;
        width   = cv->width();
        height  = cv->height();

        // Clear background
        bool bypass = bBypass;
        cv->set_color_rgb((bypass) ? CV_DISABLED : CV_BACKGROUND);
        cv->paint();

        // Draw axis
        cv->set_line_width(1.0);

        float zx    = 1.0f/SPEC_FREQ_MIN;
        float zy    = fZoom/GAIN_AMP_M_72_DB;
        float dx    = width/(logf(SPEC_FREQ_MAX)-logf(SPEC_FREQ_MIN));
        float dy    = height/(logf(GAIN_AMP_M_72_DB/fZoom)-logf(GAIN_AMP_P_24_DB*fZoom));

        // Draw vertical lines
        cv->set_color_rgb(CV_YELLOW, 0.5f);
        for (float i=100.0f; i<SPEC_FREQ_MAX; i *= 10.0f)
        {
            float ax = dx*(logf(i*zx));
            cv->line(ax, 0, ax, height);
        }

        // Draw horizontal lines
        cv->set_color_rgb(CV_WHITE, 0.5f);
        for (float i=GAIN_AMP_M_60_DB; i<GAIN_AMP_P_24_DB; i *= GAIN_AMP_P_12_DB)
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
            uint32_t *idx   = reinterpret_cast<uint32_t *>(alloca(width * sizeof(uint32_t))); //vIndexes;

            for (size_t j=0; j<width; ++j)
            {
                size_t k        = j*ni;
                idx[j]          = vIndexes[k];
            }

            for (size_t i=0; i<nChannels; ++i)
            {
                // Output only active channel
                sa_channel_t *c = &vChannels[i];
                if (!c->bSend)
                    continue;

                for (size_t j=0; j<width; ++j)
                    b->v[0][j]      = vFrequences[idx[j]];

                sAnalyzer.get_spectrum(i, b->v[1], idx, width);

                dsp::scale2(b->v[1], c->fGain * fPreamp, width);

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


