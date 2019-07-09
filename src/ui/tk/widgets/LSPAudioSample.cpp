/*
 * LSPAudioSample.cpp
 *
 *  Created on: 9 июл. 2019 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <ui/tk/helpers/draw.h>

namespace lsp
{
    namespace tk
    {
        
        const w_class_t LSPAudioSample::metadata = { "LSPAudioSample", &LSPWidget::metadata };

        LSPAudioSample::channel_t::channel_t(LSPWidget *parent):
            sColor(parent),
            sFadeColor(parent),
            sLineColor(parent)
        {
            nSamples        = 0;
            nCapacity       = 0;
            vSamples        = NULL;
            nFadeIn         = 0;
            nFadeOut        = 0;
        }

        LSPAudioSample::LSPAudioSample(LSPDisplay *dpy):
            LSPWidget(dpy),
            sBgColor(this),
            sColor(this),
            sAxisColor(this),
            sHintFont(this),
            sConstraints(this)
        {
            pClass          = &metadata;

            pGlass          = NULL;
            pGraph          = NULL;
            nBorder         = 4;
            nRadius         = 10;
            nStatus         = 0;

            nDecimSize      = 0;
            vDecimX         = NULL;
            vDecimY         = NULL;
        }
        
        LSPAudioSample::~LSPAudioSample()
        {
            destroy_data();
        }

        void LSPAudioSample::drop_glass()
        {
            if (pGlass != NULL)
            {
                pGlass->destroy();
                delete pGlass;
                pGlass = NULL;
            }
        }

        void LSPAudioSample::destroy_data()
        {
            // Destroy surfaces
            drop_glass();

            if (pGraph != NULL)
            {
                pGraph->destroy();
                delete pGraph;
                pGraph = NULL;
            }

            if (vDecimX != NULL)
                lsp_free(vDecimX);
            vDecimX = NULL;
            vDecimY = NULL;
            nDecimSize = 0;

            // Destroy all channel data
            size_t n = vChannels.size();
            for (size_t i=0; i<n; ++i)
            {
                channel_t *c = vChannels.at(i);
                if (c == NULL)
                    continue;
                destroy_channel(c);
            }
            vChannels.flush();
        }

        status_t LSPAudioSample::set_channels(size_t n)
        {
            size_t nc = vChannels.size();
            if (n < nc) // There are more channels present than requested
            {
                // Remove channels
                while ((nc--) > n)
                {
                    channel_t *c = vChannels.at(n);
                    if (!vChannels.remove(n))
                        return STATUS_NO_MEM;
                    if (c == NULL)
                        continue;
                    destroy_channel(c);
                }

                query_resize();
            }
            else if (n > nc) // There are more channels requested than present
            {
                while ((nc++) < n)
                {
                    channel_t *c = create_channel((nc & 1) ? C_LEFT_CHANNEL : C_RIGHT_CHANNEL);
                    if (c == NULL)
                        return STATUS_NO_MEM;
                    if (!vChannels.add(c))
                    {
                        destroy_channel(c);
                        return STATUS_NO_MEM;
                    }
                }

                query_resize();
            }

            return STATUS_OK;
        }

        status_t LSPAudioSample::set_hint(const char *text)
        {
            if (!sHint.set_native(text))
                return STATUS_NO_MEM;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPAudioSample::set_hint(const LSPString *text)
        {
            if (!sHint.set(text))
                return STATUS_NO_MEM;
            query_draw();
            return STATUS_OK;
        }

        LSPAudioSample::channel_t *LSPAudioSample::create_channel(color_t color)
        {
            channel_t *ch = new channel_t(this);
            if (ch == NULL)
                return NULL;

            ch->nSamples    = 0;
            ch->nCapacity   = 0;
            ch->vSamples    = NULL;

            ch->nFadeIn     = 0;
            ch->nFadeOut    = 0;
            init_color(color, &ch->sColor);
            init_color(C_YELLOW, &ch->sFadeColor);
            init_color(C_YELLOW, &ch->sLineColor);
            ch->sFadeColor.alpha(0.5f);

            return ch;
        }

        void LSPAudioSample::destroy_channel(channel_t *channel)
        {
            if (channel == NULL)
                return;
            if (channel->vSamples != NULL)
            {
                lsp_free(channel->vSamples);
                channel->vSamples   = 0;
            }
            delete channel;
        }

        status_t LSPAudioSample::add_channel()
        {
            size_t nc = vChannels.size();
            channel_t *c = create_channel(((++nc) & 1) ? C_LEFT_CHANNEL : C_RIGHT_CHANNEL);
            if (c == NULL)
                return STATUS_NO_MEM;
            if (!vChannels.add(c))
            {
                destroy_channel(c);
                return STATUS_NO_MEM;
            }

            query_resize();

            return STATUS_OK;
        }

        status_t LSPAudioSample::add_channels(size_t n)
        {
            size_t nc = vChannels.size();
            n += nc;

            query_resize();

            while ((nc++) < n)
            {
                channel_t *c = create_channel((nc & 1) ? C_LEFT_CHANNEL : C_RIGHT_CHANNEL);
                if (c == NULL)
                    return STATUS_NO_MEM;
                if (!vChannels.add(c))
                {
                    destroy_channel(c);
                    return STATUS_NO_MEM;
                }
            }

            return STATUS_OK;
        }

        status_t LSPAudioSample::remove_channel(size_t i)
        {
            channel_t *c = vChannels.get(i);
            if (c == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!vChannels.remove(i))
                return STATUS_NO_MEM;

            destroy_channel(c);
            query_resize();
            return STATUS_OK;
        }

        status_t LSPAudioSample::swap_channels(size_t a, size_t b)
        {
            if (!vChannels.swap(a, b))
                return STATUS_BAD_ARGUMENTS;

            query_draw();
            return STATUS_OK;
        }

        status_t LSPAudioSample::set_channel_fade_in(size_t i, float value)
        {
            channel_t *c = vChannels.get(i);
            if (c == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (c->nFadeIn == value)
                return STATUS_OK;
            c->nFadeIn      = value;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPAudioSample::set_channel_fade_out(size_t i, float value)
        {
            channel_t *c = vChannels.get(i);
            if (c == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (c->nFadeOut == value)
                return STATUS_OK;
            c->nFadeOut     = value;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPAudioSample::set_channel_data(size_t i, size_t samples, const float *data)
        {
            channel_t *c = vChannels.get(i);
            if (c == NULL)
                return STATUS_BAD_ARGUMENTS;

            size_t allocate = ALIGN_SIZE(samples, 16);
            if (c->nCapacity < allocate)
            {
                float *ptr = lsp_trealloc(float, c->vSamples, allocate);
                if (ptr == NULL)
                    return STATUS_NO_MEM;
                c->vSamples     = ptr;
                c->nCapacity    = allocate;
            }

            dsp::copy(c->vSamples, data, samples);
            c->nSamples     = samples;
            query_draw();

            return STATUS_OK;
        }

        status_t LSPAudioSample::clear_channel_data(size_t i)
        {
            channel_t *c = vChannels.get(i);
            if (c == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (c->nSamples <= 0)
                return STATUS_OK;

            c->nSamples     = 0;
            c->nCapacity    = 0;
            if (c->vSamples != NULL)
            {
                lsp_free(c->vSamples);
                c->vSamples     = NULL;
            }

            query_draw();
            return STATUS_OK;
        }

        status_t LSPAudioSample::clear_all_channel_data()
        {
            size_t n = vChannels.size();
            if (n <= 0)
                return STATUS_OK;

            for (size_t i=0; i<n; ++i)
            {
                channel_t *c = vChannels.at(i);
                if (c == NULL)
                    continue;

                c->nSamples     = 0;
                c->nCapacity    = 0;
                if (c->vSamples != NULL)
                {
                    lsp_free(c->vSamples);
                    c->vSamples     = NULL;
                }
            }

            query_draw();
            return STATUS_OK;
        }


        status_t LSPAudioSample::init()
        {
            status_t result = LSPWidget::init();
            if (result != STATUS_OK)
                return result;

            sHintFont.init();
            sHintFont.set_size(16);
            sHintFont.set_bold(true);

            init_color(C_BACKGROUND, &sBgColor);
            init_color(C_GLASS, &sColor);
            init_color(C_GRAPH_LINE, &sAxisColor);
            init_color(C_STATUS_OK, sHintFont.color());

            return STATUS_OK;
        }

        void LSPAudioSample::destroy()
        {
            destroy_data();
            LSPWidget::destroy();
        }

        void LSPAudioSample::render_channel(ISurface *s, channel_t *c, ssize_t y, ssize_t w, ssize_t h)
        {
            if ((c->vSamples == NULL) || (c->nSamples <= 0) || (w <= 0))
                return;

            // Start and end points
            vDecimY[0]      = 0.0f;
            vDecimY[w+1]    = 0.0f;
            float *dst      = &vDecimY[1];
            const float *src= c->vSamples;
            size_t width    = w;
            float k         = float(c->nSamples) / float(width);

            // Perform decimation
            if (c->nSamples == width) // 1:1 copy
                dsp::copy(dst, src, width);
            else if (c->nSamples < width) // Extension
            {
                for (size_t i=0; i<width; ++i)
                    *(dst++) = src[size_t(i*k)];
            }
            else // Decimation
            {
                size_t x1 = 0;

                for (size_t i=0; i<width;)
                {
                    // Calculate the second coordinate
                    size_t x2 = (++i) * k;
                    if (x2 >= c->nSamples)
                        x2 = c->nSamples-1;

                    // Find the maximum value between x1 and x2
                    *dst = src[x1];
                    while ((++x1) < x2)
                        if ((*dst) < src[x2])
                            *dst = src[x2];
                    ++dst;
                    x1 = x2; // remember the new value of x1
                }
            }

            // Apply transformations
            for (size_t i=0; i<size_t(w+2); ++i)
                vDecimY[i] = y + vDecimY[i] * h;

            // Draw
            s->draw_poly(vDecimX, vDecimY, w+2, 1.0f, c->sColor, c->sLineColor);

            // What's with fade-in
            if (c->nFadeIn > 0)
            {
                Color fill(c->sFadeColor);
                fill.alpha(1.0f - (1.0f - fill.alpha()) * 0.5f);
                vDecimY[0] = 0.0f;
                vDecimY[1] = c->nFadeIn * k;
                vDecimY[2] = 0.0f;
                vDecimY[3] = y;
                vDecimY[4] = y + h;
                vDecimY[5] = y + h;
                s->draw_poly(&vDecimY[0], &vDecimY[3], 3, 1.0f, fill, c->sFadeColor);
            }
            if (c->nFadeOut > 0)
            {
                Color fill(c->sFadeColor);
                fill.alpha(1.0f - (1.0f - fill.alpha()) * 0.5f);
                vDecimY[0] = w;
                vDecimY[1] = w - c->nFadeOut * k;
                vDecimY[2] = w;
                vDecimY[3] = y;
                vDecimY[4] = y + h;
                vDecimY[5] = y + h;
                s->draw_poly(&vDecimY[0], &vDecimY[3], 3, 1.0f, fill, c->sFadeColor);
            }
        }

        ISurface *LSPAudioSample::render_graph(ISurface *s, ssize_t w, ssize_t h)
        {
            size_t channels = vChannels.size();

            if (channels <= 0)
            {
                if (pGraph != NULL)
                {
                    pGraph->destroy();
                    delete pGraph;
                    pGraph = NULL;
                }
            }
            // Check surface
            if (pGraph != NULL)
            {
                if ((w != ssize_t(pGraph->width())) || (h != ssize_t(pGraph->height())))
                {
                    pGraph->destroy();
                    delete pGraph;
                    pGraph    = NULL;
                }
            }

            // Create new surface if needed
            if (pGraph == NULL)
            {
                if (s == NULL)
                    return NULL;
                pGraph        = s->create(w, h);
                if (pGraph == NULL)
                    return NULL;
            }

            // Clear canvas
            pGraph->clear(sColor);
            float aa = pGraph->get_antialiasing();

            // Init decimation buffer
            if (nStatus & AF_SHOW_DATA)
            {
                size_t  sz_decim    = ALIGN_SIZE(w+2, 16); // 2 additional points at start and end
                if (nDecimSize < sz_decim)
                {
                    // Try to allocate memory
                    float *ptr  = lsp_trealloc(float, vDecimX, sz_decim * 2);
                    if (ptr == NULL)
                        return pGraph;

                    // Store new pointers
                    vDecimX     = ptr;
                    vDecimY     = &ptr[sz_decim];
                    nDecimSize  = sz_decim;
                }

                // Initialize decimation buffer
                vDecimX[0]      = -1.0f;
                for (ssize_t i=0; i<=w; ++i)
                    vDecimX[i+1]    = float(i);

                // Calculate number of pairs
                size_t pairs    = (channels + 1) >> 1;;
                float  delta    = float(h)/float(pairs);

                for (size_t i=0, ci=0; i<pairs; ++i)
                {
                    ssize_t ys      = i * delta, ye = (i + 1) * delta;
                    ssize_t yc      = (ye + ys) >> 1;

                    pGraph->set_antialiasing(true);
                    channel_t *c    = vChannels.at(ci);
                    if (c != NULL)
                        render_channel(pGraph, c, yc, w, ys - yc); //, fill, c->sColor);

                    if ((++ci) >= channels)
                        ci--;
                    c    = vChannels.at(ci);
                    if (c != NULL)
                        render_channel(pGraph, c, yc, w, ye - yc); //, fill, c->sColor);
                    ++ci;

                    pGraph->set_antialiasing(false);
                    pGraph->line(0.0f, yc, w, yc, 1.0f, sAxisColor);
                }
            }

            if (nStatus & AF_SHOW_HINT)
            {
                font_parameters_t fp;
                text_parameters_t tp;

                pGraph->set_antialiasing(false);
                sHintFont.get_parameters(pGraph, &fp);
                sHintFont.get_text_parameters(pGraph, &tp, &sHint);

                sHintFont.draw(pGraph, (w - tp.Width) * 0.5f, (h - fp.Height) * 0.5f + fp.Ascent, &sHint);
            }

            pGraph->set_antialiasing(aa);

            return pGraph;
        }

        void LSPAudioSample::draw(ISurface *s)
        {
            // Determine left and top coordinates
            ssize_t bs  = nBorder + nRadius * M_SQRT2 * 0.5f;
            ssize_t bl  = sPadding.left();
            ssize_t bt  = sPadding.top();
            ssize_t bw  = sSize.nWidth  - sPadding.horizontal();
            ssize_t bh  = sSize.nHeight - sPadding.vertical();
            ssize_t rl  = bl + bs;
            ssize_t rt  = bt + bs;
            ssize_t gw  = bw - bs*2;
            ssize_t gh  = bh - bs*2;

            ssize_t xbw = nBorder;

            // Draw background
            s->fill_frame(
                    0, 0, sSize.nWidth, sSize.nHeight,
                    bl + xbw, bt + xbw, bw - xbw*2, bh - xbw*2,
                    sBgColor);

            s->fill_round_rect(bl, bt, bw, bh, nRadius, SURFMASK_ALL_CORNER, sColor);

            // Draw main contents
            if ((gw > 0) && (gh > 0))
            {
                ISurface *cv    = render_graph(s, gw, gh);
                if (cv != NULL)
                {
                    if (nStatus & AF_PRESSED)
                        s->draw(cv, rl + 1, rt + 1, float(gw - 2.0f) / gw, float(gh - 2.0f) / gh);
                    else
                        s->draw(cv, rl, rt);
                }
            }

            // Draw the glass and the border
            ISurface *cv = create_border_glass(s, &pGlass, bw, bh, nBorder + ((nStatus & AF_PRESSED) ? 1 : 0), nRadius, SURFMASK_ALL_CORNER, sColor);
            if (cv != NULL)
                s->draw(cv, bl, bt);
        }

        void LSPAudioSample::size_request(size_request_t *r)
        {
            size_t nc = vChannels.size();
            nc = (nc + 1) & (~1); // Round up to 2

            ssize_t bs      = nBorder + nRadius * M_SQRT2 * 0.5f;
            r->nMinWidth    = 16;
            r->nMinHeight   = nc * 16;
            if (r->nMinHeight < 16)
                r->nMinHeight = 16;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;

            // Add external size constraints
            sConstraints.apply(r);

            // Add padding and size
            r->nMinWidth   += (bs << 1) + sPadding.horizontal();
            r->nMinHeight  += (bs << 1) + sPadding.vertical();
            if (r->nMaxWidth >= 0)
                r->nMaxWidth   += (bs << 1) + sPadding.horizontal();
            if (r->nMaxHeight >= 0)
                r->nMaxHeight  += (bs << 1) + sPadding.vertical();
        }

    
    } /* namespace tk */
} /* namespace lsp */
