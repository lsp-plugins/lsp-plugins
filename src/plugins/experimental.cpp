/*
 * experimental.cpp
 *
 *  Created on: 11 янв. 2017 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <plugins/experimental.h>
#include <core/util/Color.h>
#include <core/colors.h>

namespace lsp
{
#ifndef LSP_NO_EXPERIMENTAL
    static const uint32_t colors[] =
    {
        0xff0000,   // Red
        0xffff00,   // Yellow
        0x00ff00,   // Green
        0x00ffff,   // Cyan
        0x0000ff,   // Blue
        0xff00ff    // Magenta
    };

    test_plugin::test_plugin(): plugin_t(metadata)
    {
        for (size_t i=0; i<2; ++i)
        {
            pIn[i]          = NULL;
            pOut[i]         = NULL;
        }
        pMesh           = NULL;
        pFB             = NULL;
        pGain           = NULL;
        fGain           = 1.0f;
        nPhase          = 0;
        nColorID        = 0;
        nStatus         = STATUS_UNSPECIFIED;

        pFileName       = NULL;
        pHeadCut        = NULL;
        pTailCut        = NULL;
        pFadeIn         = NULL;
        pFadeOut        = NULL;
        pStatus         = NULL;
        pLength         = NULL;
        pData           = NULL;

        pOutFile        = NULL;
        pOutCmd         = NULL;
        pOutStatus      = NULL;
        pOutProgress    = NULL;

        nProgCurr       = 0;
        nProgLast       = 0;
        bFileSet        = false;

        nOscPhase       = 0;
        nOscLeft        = 0;
        nRows           = 0;

        vOsc[0].A0      = 0.25f;
        vOsc[0].X0      = 64;
        vOsc[0].W0      = 2.0f;
        vOsc[0].P0      = 0.0f;
        vOsc[0].R0      = 0.01f;

        vOsc[1].A0      = 0.25f;
        vOsc[1].X0      = 128;
        vOsc[1].W0      = 6.5f;
        vOsc[1].P0      = 1.0f;
        vOsc[1].R0      = 0.1f;

        vOsc[2].A0      = 0.15f;
        vOsc[2].X0      = 192;
        vOsc[2].W0      = 1.33f;
        vOsc[2].P0      = 0.5f;
        vOsc[2].R0      = 0.05f;
    }

    test_plugin::~test_plugin()
    {
    }

    void test_plugin::update_settings()
    {
        fGain           = pGain->getValue();

        // Process file
        path_t *path = pFileName->getBuffer<path_t>();
        if ((path != NULL) && (path->pending()))
        {
            const char *fname = path->get_path();
            lsp_trace("Submitted file name: %s", fname);

            size_t len = strlen(fname);
            if (len < 4)
                nStatus     = (len > 0) ? STATUS_BAD_ARGUMENTS : STATUS_UNSPECIFIED;
            else
            {
                fname += len - 4;
                if (fname[0] != '.')
                    nStatus     = STATUS_BAD_ARGUMENTS;
                else if ((fname[1] != 'w') && (fname[1] != 'W'))
                    nStatus     = STATUS_BAD_ARGUMENTS;
                else if ((fname[2] != 'a') && (fname[2] != 'A'))
                    nStatus     = STATUS_BAD_ARGUMENTS;
                else if ((fname[3] != 'v') && (fname[3] != 'V'))
                    nStatus     = STATUS_BAD_ARGUMENTS;
                else
                    nStatus     = STATUS_OK;
            }

            path->accept();
        }
    }

    void test_plugin::init(IWrapper *wrapper)
    {
        plugin_t::init(wrapper);

        // Remember pointers to ports
        size_t port_id = 0;
        for (size_t i=0; i<2; ++i)
            pIn[i]          = vPorts[port_id++];
        for (size_t i=0; i<2; ++i)
            pOut[i]         = vPorts[port_id++];
        pGain           = vPorts[port_id++];
        pMesh           = vPorts[port_id++];
        pFB             = vPorts[port_id++];
        port_id        += 4; // skip modes

        pFileName       = vPorts[port_id++];
        pHeadCut        = vPorts[port_id++];
        pTailCut        = vPorts[port_id++];
        pFadeIn         = vPorts[port_id++];
        pFadeOut        = vPorts[port_id++];
        pStatus         = vPorts[port_id++];
        pLength         = vPorts[port_id++];
        pData           = vPorts[port_id++];

        pOutFile        = vPorts[port_id++];
        pOutCmd         = vPorts[port_id++];
        pOutStatus      = vPorts[port_id++];
        pOutProgress    = vPorts[port_id++];
    }

    void test_plugin::process(size_t samples)
    {
        for (size_t i=0; i<2; ++i)
        {
            // Get data buffers
            float *in = pIn[i]->getBuffer<float>();
            if (in == NULL)
                continue;
            float *out = pOut[i]->getBuffer<float>();
            if (out == NULL)
                continue;

            // dsp::scale - optimized version for
            //  *(out++) = *(in++) * fGain    x   samples times
            dsp::mul_k3(out, in, fGain, samples);
        }

        size_t out_status = pOutStatus->getValue();
        // We're 'loading' file?
        if (out_status == STATUS_LOADING)
        {
            nProgCurr   += samples;
            if (nProgCurr >= nProgLast)
            {
                nProgCurr = nProgLast;
                pOutStatus->setValue(STATUS_OK);

                path_t *path = pOutFile->getBuffer<path_t>();
                if (path->accepted())
                    path->commit();
            }
            pOutProgress->setValue(float(nProgCurr * 100.0f) / float(nProgLast));
//            lsp_trace("set progress %f%%", pOutProgress->getValue());
            bFileSet = false;
        }
        else
        {
            // We've got pending file primitive?
            path_t *path = pOutFile->getBuffer<path_t>();
            if ((path != NULL) && (path->pending()))
            {
                bFileSet = true;
                path->accept();
            }

            // We can launch file saving routine?
            if ((bFileSet) && (pOutCmd->getValue() > 0.5f))
            {
                pOutStatus->setValue(STATUS_LOADING);
                nProgLast       = fSampleRate * 10;
                nProgCurr       = 0;

                pOutProgress->setValue(0);
            }
        }


        // Fill mesh with some stuff
        nPhase      = ((nPhase + samples) & 0x7ffff);

        mesh_t *mesh = pMesh->getBuffer<mesh_t>();
        if ((mesh != NULL) && (mesh->isEmpty()))
        {
            float phase = (nPhase * M_PI * 2.0f) / float(0x80000);
            for (size_t i=0; i<320; ++i)
            {
                mesh->pvData[0][i] = 10.0f * expf(logf(2400.0f) * i / 320.0f);
                mesh->pvData[1][i] = 10 * (1.0f + cosf(phase + (2.0f * M_PI * i) / 160.0f));
            }
            mesh->data(2, 320);
        }

        // Fill framebuffers with some stuff
        size_t ns = nOscLeft + samples;
        while (ns >= FRM_BUFFER_SIZE)
        {
            ns             -= FRM_BUFFER_SIZE;

            float time      = float(nOscPhase) / 0x80000;

            dsp::fill(vBuffer, 0.5f, FRM_BUFFER_SIZE);
            for (size_t i=0; i<3; ++i)
                oscillate(vBuffer, &vOsc[i], time, FRM_BUFFER_SIZE);

            if (pFB == NULL)
            {
                lsp_trace("Framebuffer port is NULL");
                continue;
            }

            frame_buffer_t *fb = pFB->getBuffer<frame_buffer_t>();
            if ((fb != NULL) && (fb->rows() > nRows))
            {
                fb->write_row(vBuffer);
//                nRows += 2;
            }

            nOscPhase       = (nOscPhase + FRM_BUFFER_SIZE); // & 0x7ffff;
        }
        nOscLeft            = ns;

        // Process file
        path_t *path = pFileName->getBuffer<path_t>();
        if ((path != NULL) && (path->accepted()))
        {
            // Commit path state
            pLength->setValue(samples_to_millis(fSampleRate, 256));

            mesh = pData->getBuffer<mesh_t>();
            if (mesh != NULL)
            {
                if (nStatus == STATUS_OK)
                {
                    size_t N = pData->metadata()->start;

                    float *dst = mesh->pvData[0];
                    for (size_t i=0; i<N; ++i)
                        dst[i] = fabs(sinf(0.025f * M_PI * i));

                    dst = mesh->pvData[1];
                    for (size_t i=0; i<N; ++i)
                        dst[i] = fabs(cosf(0.025f * M_PI * i) / (1.0f + 0.05f * i));

                    mesh->data(2, N);
                }
                else
                    mesh->data(0, 0);
            }

            path->commit();
        }

        pStatus->setValue(nStatus);

        // Query inline display for redraw
        pWrapper->query_display_draw();
    }

    void test_plugin::oscillate(float *dst, const osc_t *osc, float t, ssize_t n)
    {
        float P = 2.0f * M_PI * osc->W0 * t + osc->P0;

        for (ssize_t x=0; x < n; ++x)
        {
            float dx = -0.05f * fabs(osc->X0 - x);
            dst[x] += osc->A0 * cosf(P + dx) * expf(osc->R0 * dx);
        }
    }

    bool test_plugin::inline_display(ICanvas *cv, size_t width, size_t height)
    {
        // Init canvas
        if (!cv->init(width, height))
            return false;
        width   = cv->width();
        height  = cv->height();

        size_t base         = nColorID / 20;
        float mix           = (nColorID % 20) * 0.05f;
        if ((++nColorID) >= 120)
            nColorID = 0;

        uint32_t b_color    = colors[base];
        uint32_t m_color    = colors[(base+1)%6];

        float b             = (b_color & 0xff) * (1.0f - mix) + (m_color & 0xff) * mix;
        float g             = ((b_color >> 8) & 0xff) * (1.0f - mix) + ((m_color >> 8) & 0xff) * mix;
        float r             = ((b_color >> 16) & 0xff) * (1.0f - mix) + ((m_color >> 16) & 0xff) * mix;

        if (b > 0xff)
            b = 0xff;
        if (g > 0xff)
            g = 0xff;
        if (r > 0xff)
            r = 0xff;

        b_color             = uint32_t(b) | (uint32_t(g) << 8) | (uint32_t(r) << 16);

        cv->set_color_rgb(CV_BACKGROUND);
        cv->paint();

        cv->set_line_width(2.0);
        cv->set_color(b_color);
        cv->circle(width >> 1, height >>1, width >> 2);


        return true;
    }

    filter_analyzer::filter_analyzer(): plugin_t(metadata)
    {
        pIn     = NULL;
        pOut    = NULL;
        pGraph  = NULL;
    }

    filter_analyzer::~filter_analyzer()
    {
        for (size_t i=0; i<2; ++i)
            vFilters[i].sFilter.destroy();
    }

    void filter_analyzer::init(IWrapper *wrapper)
    {
        plugin_t::init(wrapper);

        // Initialize filters
        for (size_t i=0; i<2; ++i)
        {
            pfilter_t *pf   = &vFilters[i];
            pf->sFP.nType   = FLT_NONE;
            pf->sFP.fFreq   = FREQ_DFL;
            pf->sFP.fFreq2  = FREQ_DFL * 10.0f;
            pf->sFP.fGain   = GAIN_DFL;
            pf->sFP.nSlope  = 1;
            pf->sFP.fQuality= QUALITY_DFL;

            pf->sFilter.init(NULL);
            pf->nOp         = 0;
        }

        // Bind ports
        size_t port_id = 0;
        pIn             = vPorts[port_id++];
        pOut            = vPorts[port_id++];
        ++port_id;  // skip 'area'
        pGraph          = vPorts[port_id++];

        for (size_t i=0; i<2; ++i)
        {
            pfilter_t *pf   = &vFilters[i];

            pf->pType       = vPorts[port_id++];
            pf->pSlope      = vPorts[port_id++];
            pf->pOp         = vPorts[port_id++];
            pf->pFreqLo     = vPorts[port_id++];
            pf->pFreqHi     = vPorts[port_id++];
            pf->pGain       = vPorts[port_id++];
            pf->pQuality    = vPorts[port_id++];
        }
    }

    void filter_analyzer::process(size_t samples)
    {
        // Bypass signal
        dsp::copy(pOut->getBuffer<float>(), pIn->getBuffer<float>(), samples);

        // Render the output
        mesh_t *m = pGraph->getBuffer<mesh_t>();
        if ((m != NULL) && (m->isEmpty()))
        {
            // Generate list of frequencies, cleanup other buffers
            float zp[2], tzp[2];
            float *f = m->pvData[0];
            float fstep     = logf(FREQ_MAX/FREQ_MIN) / (MESH_POINTS - 1);
            for (size_t i=0; i<MESH_POINTS; ++i)
                f[i]    = FREQ_MIN * expf(i * fstep);
            dsp::fill_zero(m->pvData[1], MESH_POINTS);
            dsp::fill_zero(m->pvData[2], MESH_POINTS);

            // Compute transfer function of the system
            size_t nf   = 0;
            dsp::fill_zero(vChart, MESH_POINTS*2);
            for (size_t i=0; i<2; ++i)
            {
                pfilter_t *pf   = &vFilters[i];
                if (pf->nOp == 0)
                    continue;
                ++nf;
                pf->sFilter.freq_chart(vTmpBuf, m->pvData[0], MESH_POINTS);
                pf->sFilter.freq_chart(tzp, m->pvData[0], 1);
                if (pf->nOp == 1) // add
                {
                    dsp::add2(vChart, vTmpBuf, MESH_POINTS * 2);
                    dsp::add2(tzp, zp, 2);
                }
                else if (pf->nOp == 2) // sub
                {
                    dsp::sub2(vChart, vTmpBuf, MESH_POINTS * 2);
                    dsp::sub2(tzp, zp, 2);
                }
            }

            // No active filters?
            if (nf <= 0)
                dsp::pcomplex_fill_ri(vChart, 1.0f, 0.0f, MESH_POINTS);

            // Compute transfer function (amplitude and phase)
            dsp::pcomplex_modarg(m->pvData[1], m->pvData[2], vChart, MESH_POINTS);
            dsp::pcomplex_modarg(&zp[0], &zp[1], zp, 1);
            dsp::mul_k2(m->pvData[2], 180.0f / M_PI, MESH_POINTS);

            // Patch the phase
            f = m->pvData[2];
            if (f[0] > 1e-3)
                dsp::sub_k2(f, 360.0f - f[0], MESH_POINTS);
            for (size_t i=1; i<MESH_POINTS; ++i)
            {
                float a = f[i-1], b=f[i];
                if (fabs(a-b) < 90.0f) // Detect jump over PI*2
                    continue;

                // Patch the curve
                if (b > a)
                    dsp::sub_k2(&f[i], 360.0f, MESH_POINTS-i);
                else
                    dsp::add_k2(&f[i], 360.0f, MESH_POINTS-i);
            }

            // Notify
            m->data(3, MESH_POINTS);
        }
    }

    void filter_analyzer::update_settings()
    {
        // Update filters
        for (size_t i=0; i<2; ++i)
        {
            pfilter_t *pf   = &vFilters[i];

            pf->sFP.nType   = pf->pType->getValue();
            pf->sFP.fFreq   = pf->pFreqLo->getValue();
            pf->sFP.fFreq2  = pf->pFreqHi->getValue();
            pf->sFP.fGain   = pf->pGain->getValue();
            pf->sFP.nSlope  = pf->pSlope->getValue() + 1;
            pf->sFP.fQuality= pf->pQuality->getValue();

            pf->sFilter.update(fSampleRate, &pf->sFP);
            pf->sFilter.rebuild();

            pf->nOp         = pf->pOp->getValue();
        }
    }

    void filter_analyzer::set_sample_rate(long sr)
    {
        plugin_t::set_sample_rate(sr);

        // Update filter parameters
        for (size_t i=0; i<2; ++i)
            vFilters[i].sFilter.update(sr, &vFilters[i].sFP);
    }
#endif
}


