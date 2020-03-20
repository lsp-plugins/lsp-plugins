/*
 * LSPAudioFile.cpp
 *
 *  Created on: 23 окт. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <ui/tk/helpers/draw.h>
#include <ui/tk/helpers/mime.h>
#include <core/files/url.h>
#include <dsp/dsp.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPAudioFile::metadata = { "LSPAudioFile", &LSPWidget::metadata };

        LSPAudioFile::channel_t::channel_t(LSPWidget *widget):
            sColor(widget),
            sFadeColor(widget),
            sLineColor(widget)
        {
            nSamples        = 0;
            nCapacity       = 0;
            vSamples        = NULL;

            nFadeIn         = 0.0f;
            nFadeOut        = 0.0f;
        }

        LSPAudioFile::AudioFileSink::AudioFileSink(LSPAudioFile *af): LSPUrlSink("file://")
        {
            pWidget     = af;
        }

        LSPAudioFile::AudioFileSink::~AudioFileSink()
        {
            pWidget     = NULL;
        }

        void LSPAudioFile::AudioFileSink::unbind()
        {
            pWidget     = NULL;
        }

        status_t LSPAudioFile::AudioFileSink::commit_url(const LSPString *url)
        {
            LSPString decoded;
            status_t res = (url->starts_with_ascii("file://")) ?
                    url_decode(&decoded, url, 7) :
                    url_decode(&decoded, url);

            if (res != STATUS_OK)
                return res;

            lsp_trace("Set file path to %s", decoded.get_native());
            pWidget->sFileName.swap(&decoded);
            pWidget->slots()->execute(LSPSLOT_SUBMIT, pWidget, NULL);

            return STATUS_OK;
        }

        LSPAudioFile::LSPAudioFile(LSPDisplay *dpy):
            LSPWidget(dpy),
            sHint(this),
            sFont(dpy, this),
            sHintFont(dpy, this),
            sConstraints(this),
            sDialog(dpy),
            sColor(this),
            sAxisColor(this)
        {
            pClass          = &metadata;
            pGlass          = NULL;
            pGraph          = NULL;
            nBtnWidth       = 0;
            nBtnHeight      = 0;
            nBMask          = 0;
            nBorder         = 4;
            nRadius         = 10;
            nStatus         = 0;
            pPopup          = NULL;

            nDecimSize      = 0;
            vDecimX         = NULL;
            vDecimY         = NULL;

            pSink           = NULL;
        }
        
        LSPAudioFile::~LSPAudioFile()
        {
            destroy_data();
        }

        status_t LSPAudioFile::init()
        {
            status_t result = LSPWidget::init();
            if (result != STATUS_OK)
                return result;

            pSink       = new AudioFileSink(this);
            if (pSink == NULL)
                return STATUS_NO_MEM;
            pSink->acquire();

            sHint.bind();

            sFont.init();
            sFont.set_size(10);
            sFont.set_bold(true);

            sHintFont.init();
            sHintFont.set_size(16);
            sHintFont.set_bold(true);

            init_color(C_GLASS, &sColor);
            init_color(C_GRAPH_LINE, &sAxisColor);
            init_color(C_GRAPH_TEXT, sFont.color());
            init_color(C_STATUS_OK, sHintFont.color());

            // Initialize dialog
            LSP_STATUS_ASSERT(sDialog.init());

            sDialog.title()->set("titles.load_audio_file");
            LSPFileFilter *f = sDialog.filter();
            {
                LSPFileFilterItem ffi;
                ffi.pattern()->set("*.wav");
                ffi.title()->set("files.audio.wave");
                ffi.set_extension(".wav");
                f->add(&ffi);

                ffi.pattern()->set("*");
                ffi.title()->set("files.all");
                ffi.set_extension("");
                f->add(&ffi);
            }
            f->set_default(0);

            sDialog.action_title()->set("actions.load");
            sDialog.bind_action(slot_on_dialog_submit, self());
            sDialog.slots()->bind(LSPSLOT_HIDE, slot_on_dialog_close, self());

            // Initialize slots
            ui_handler_id_t id = 0;
            id = sSlots.add(LSPSLOT_SUBMIT, slot_on_submit, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_CLOSE, slot_on_close, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_ACTIVATE, slot_on_close, self());
            if (id < 0) return -id;

            return result;
        }

        status_t LSPAudioFile::slot_on_submit(LSPWidget *sender, void *ptr, void *data)
        {
            LSPAudioFile *_this = widget_ptrcast<LSPAudioFile>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPAudioFile::slot_on_close(LSPWidget *sender, void *ptr, void *data)
        {
            LSPAudioFile *_this = widget_ptrcast<LSPAudioFile>(ptr);
            return (_this != NULL) ? _this->on_close() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPAudioFile::slot_on_activate(LSPWidget *sender, void *ptr, void *data)
        {
            LSPAudioFile *_this = widget_ptrcast<LSPAudioFile>(ptr);
            return (_this != NULL) ? _this->on_activate() : STATUS_BAD_ARGUMENTS;
        }

        void LSPAudioFile::destroy()
        {
            destroy_data();
            LSPWidget::destroy();
        }

        status_t LSPAudioFile::set_file_name(const char *text)
        {
            if (text == NULL)
                sFileName.truncate();
            else if (!sFileName.set_native(text))
                return STATUS_NO_MEM;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPAudioFile::set_file_name(const LSPString *text)
        {
            if (!sFileName.set(text))
                return STATUS_NO_MEM;
            query_draw();
            return STATUS_OK;
        }

        LSPAudioFile::channel_t *LSPAudioFile::create_channel(color_t color)
        {
            channel_t *ch = new channel_t(this);
            if (ch == NULL)
                return NULL;

            init_color(color, &ch->sColor);
            init_color(C_YELLOW, &ch->sFadeColor);
            init_color(C_YELLOW, &ch->sLineColor);
            ch->sFadeColor.alpha(0.5f);

            return ch;
        }

        void LSPAudioFile::destroy_channel(channel_t *channel)
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

        void LSPAudioFile::destroy_data()
        {
            // Destroy sink
            if (pSink != NULL)
            {
                pSink->unbind();
                pSink->release();
                pSink   = NULL;
            }

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

            // Destroy dialog
            sDialog.destroy();

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

        status_t LSPAudioFile::set_channels(size_t n)
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

        status_t LSPAudioFile::add_channel()
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

        status_t LSPAudioFile::add_channels(size_t n)
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

        status_t LSPAudioFile::remove_channel(size_t i)
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

        status_t LSPAudioFile::swap_channels(size_t a, size_t b)
        {
            if (!vChannels.swap(a, b))
                return STATUS_BAD_ARGUMENTS;

            query_draw();
            return STATUS_OK;
        }

        status_t LSPAudioFile::set_channel_fade_in(size_t i, float value)
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

        status_t LSPAudioFile::set_channel_fade_out(size_t i, float value)
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

        status_t LSPAudioFile::set_channel_data(size_t i, size_t samples, const float *data)
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

        status_t LSPAudioFile::clear_channel_data(size_t i)
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

        status_t LSPAudioFile::clear_all_channel_data()
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

        void LSPAudioFile::render_channel(ISurface *s, channel_t *c, ssize_t y, ssize_t w, ssize_t h)//, const Color &fill, const Color &wire)
        {
            if ((c->vSamples == NULL) || (c->nSamples <= 0) || (w <= 0))
                return;

            // Prepare palette
            Color color(c->sColor);
            Color line_col(c->sLineColor);
            Color fade_col(c->sFadeColor);
            color.scale_lightness(brightness());
            line_col.scale_lightness(brightness());
            fade_col.scale_lightness(brightness());

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
            s->draw_poly(vDecimX, vDecimY, w+2, 1.0f, color, line_col);

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
                s->draw_poly(&vDecimY[0], &vDecimY[3], 3, 1.0f, fill, fade_col);
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
                s->draw_poly(&vDecimY[0], &vDecimY[3], 3, 1.0f, fill, fade_col);
            }
        }

        ISurface *LSPAudioFile::render_graph(ISurface *s, ssize_t w, ssize_t h)
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

            // Prepare palette
            Color color(sColor);
            Color axis_color(sAxisColor);
            color.scale_lightness(brightness());
            axis_color.scale_lightness(brightness());

            // Clear canvas
            pGraph->clear(color);
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
                    pGraph->line(0.0f, yc, w, yc, 1.0f, axis_color);
                }
            }

            // Draw file name
            if ((nStatus & AF_SHOW_FNAME) && (sFileName.length() > 0))
            {
                ssize_t index1 = sFileName.rindex_of('/') + 1;
                ssize_t index2 = sFileName.rindex_of('\\') + 1;
                if (index1 < index2)
                    index1  = index2;
                if ((index1 < 0) || (index1 >= ssize_t(sFileName.length())))
                    index1  = 0;

                font_parameters_t fp;
                text_parameters_t tp;

                sFont.get_parameters(pGraph, &fp);
                sFont.get_text_parameters(pGraph, &tp, &sFileName, index1);

                Color cl(color, 0.25f);
                size_t bw = 4;
                pGraph->set_antialiasing(true);
                pGraph->fill_round_rect(0, h - bw - fp.Height, tp.Width + bw * 2, fp.Height + bw, bw, SURFMASK_ALL_CORNER, cl);
                pGraph->set_antialiasing(false);
                sFont.draw(pGraph, bw - tp.XBearing, h - bw*0.5f - fp.Descent, &sFileName, index1);
            }

            if (nStatus & AF_SHOW_HINT)
            {
                LSPString hint;
                sHint.format(&hint);

                if (!hint.is_empty())
                {
                    font_parameters_t fp;
                    text_parameters_t tp;

                    pGraph->set_antialiasing(false);
                    sHintFont.get_parameters(pGraph, &fp);
                    sHintFont.get_text_parameters(pGraph, &tp, &hint);

                    sHintFont.draw(pGraph, (w - tp.Width) * 0.5f, (h - fp.Height) * 0.5f + fp.Ascent, &hint);
                }
            }

            pGraph->set_antialiasing(aa);

            return pGraph;
        }

        void LSPAudioFile::draw(ISurface *s)
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

            // Prepare palette
            Color bg_color(sBgColor);
            Color color(sColor);
            color.scale_lightness(brightness());

            // Draw background
            s->fill_frame(
                    0, 0, sSize.nWidth, sSize.nHeight,
                    bl + xbw, bt + xbw, bw - xbw*2, bh - xbw*2,
                    bg_color);

            s->fill_round_rect(bl, bt, bw, bh, nRadius, SURFMASK_ALL_CORNER, color);

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
            ISurface *cv = create_border_glass(s, &pGlass, bw, bh, nBorder + ((nStatus & AF_PRESSED) ? 1 : 0), nRadius, SURFMASK_ALL_CORNER, color);
            if (cv != NULL)
                s->draw(cv, bl, bt);
        }

        void LSPAudioFile::drop_glass()
        {
            if (pGlass != NULL)
            {
                pGlass->destroy();
                delete pGlass;
                pGlass = NULL;
            }
        }

        bool LSPAudioFile::hide()
        {
            bool result = LSPWidget::hide();
            if (pGlass != NULL)
            {
                pGlass->destroy();
                delete pGlass;
                pGlass = NULL;
            }

            if (pGraph != NULL)
            {
                pGraph->destroy();
                delete pGraph;
                pGraph = NULL;
            }

            return result;
        }

        status_t LSPAudioFile::set_radius(size_t radius)
        {
            if (nRadius == radius)
                return STATUS_OK;
            nRadius = radius;
            query_resize();
            return STATUS_OK;
        }

        status_t LSPAudioFile::set_border(size_t border)
        {
            if (nBorder == border)
                return STATUS_OK;
            nBorder = border;
            query_resize();
            return STATUS_OK;
        }

        status_t LSPAudioFile::set_path(const LSPString *path)
        {
            if (!sPath.set(path))
                return STATUS_NO_MEM;
            return (sDialog.visible()) ? sDialog.set_path(&sPath) : STATUS_OK;
        }

        status_t LSPAudioFile::set_path(const char *path)
        {
            if (!sPath.set_native(path))
                return STATUS_NO_MEM;
            return (sDialog.visible()) ? sDialog.set_path(&sPath) : STATUS_OK;
        }

        void LSPAudioFile::set_show_data(bool value)
        {
            size_t flags = nStatus;
            nStatus = (value) ? nStatus | AF_SHOW_DATA : nStatus & (~AF_SHOW_DATA);
            if (nStatus == flags)
                return;
            query_draw();
        }

        void LSPAudioFile::set_show_hint(bool value)
        {
            size_t flags = nStatus;
            nStatus = (value) ? nStatus | AF_SHOW_HINT : nStatus & (~AF_SHOW_HINT);
            if (nStatus == flags)
                return;
            query_draw();
        }

        void LSPAudioFile::set_show_file_name(bool value)
        {
            size_t flags = nStatus;
            nStatus = (value) ? nStatus | AF_SHOW_FNAME : nStatus & (~AF_SHOW_FNAME);
            if (nStatus == flags)
                return;
            query_draw();
        }

        void LSPAudioFile::size_request(size_request_t *r)
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

        bool LSPAudioFile::check_mouse_over(ssize_t x, ssize_t y)
        {
            x -= sSize.nLeft;
            y -= sSize.nTop;

            if ((x < ssize_t(sPadding.left())) || (x > ssize_t(sSize.nWidth -sPadding.right())))
                return false;
            if ((y < ssize_t(sPadding.top())) || (y > ssize_t(sSize.nHeight - size_t(sPadding.bottom()))))
                return false;

            // Check special case: corners
            if (x < ssize_t(nRadius))
            {
                if (y < ssize_t(nRadius))
                {
                    float dx = nRadius - x, dy = nRadius - y;
                    return (dx*dx + dy*dy) <= nRadius * nRadius;
                }
                else if (y > ssize_t(sSize.nHeight - nRadius))
                {
                    float dx = nRadius - x, dy = y - sSize.nHeight + nRadius;
                    return (dx*dx + dy*dy) <= nRadius * nRadius;
                }
            }
            else if (x > ssize_t(sSize.nWidth + nRadius))
            {
                if (y < ssize_t(nRadius))
                {
                    float dx = x - sSize.nWidth + nRadius, dy = nRadius - y;
                    return (dx*dx + dy*dy) <= nRadius * nRadius;
                }
                else if (y > ssize_t(sSize.nHeight - nRadius))
                {
                    float dx = x - sSize.nWidth + nRadius, dy = y - sSize.nHeight + nRadius;
                    return (dx*dx + dy*dy) <= nRadius * nRadius;
                }
            }

            return true;
        }

        status_t LSPAudioFile::on_mouse_down(const ws_event_t *e)
        {
            nBMask         |= (1 << e->nCode);
            size_t flags    = nStatus;
            nStatus         = (nBMask == (1 << MCB_LEFT)) && (check_mouse_over(e->nLeft, e->nTop)) ? nStatus | AF_PRESSED : nStatus & (~AF_PRESSED);
            if (flags != nStatus)
            {
                drop_glass();
                query_draw();
            }
            return STATUS_OK;
        }

        status_t LSPAudioFile::slot_on_dialog_submit(LSPWidget *sender, void *ptr, void *data)
        {
            // Cast widget
            LSPAudioFile *_this = widget_ptrcast<LSPAudioFile>(ptr);
            if (_this == NULL)
                return STATUS_BAD_STATE;

            // Get selected file
            status_t result = _this->sDialog.get_selected_file(&_this->sFileName);
            if (result != STATUS_OK)
                return result;

            // OK, file name was submitted
            _this->query_draw();
            return _this->sSlots.execute(LSPSLOT_SUBMIT, _this, data);
        }

        status_t LSPAudioFile::slot_on_dialog_close(LSPWidget *sender, void *ptr, void *data)
        {
            // Cast widget
            LSPAudioFile *_this = widget_ptrcast<LSPAudioFile>(ptr);
            if (_this == NULL)
                return STATUS_BAD_STATE;

            // Remember the last path used
            _this->sDialog.get_path(&_this->sPath);
            return _this->sSlots.execute(LSPSLOT_CLOSE, _this, data);
        }

        status_t LSPAudioFile::on_mouse_up(const ws_event_t *e)
        {
            bool pressed    = (nBMask == (1 << MCB_LEFT)) && (check_mouse_over(e->nLeft, e->nTop));

            size_t flags    = nStatus;
            nBMask         &= ~(1 << e->nCode);
            if (nBMask == 0)
                nStatus        &= ~AF_PRESSED;

            if (flags != nStatus)
            {
                drop_glass();
                query_draw();
            }

            if (nBMask == 0)
            {
                if ((pressed) && (e->nCode == MCB_LEFT))
                {
                    status_t result = sSlots.execute(LSPSLOT_ACTIVATE, NULL);
                    if (result == STATUS_OK)
                    {
                        sDialog.set_path(&sPath);
                        sDialog.show(this);
                    }
                }
                else if (e->nCode == MCB_RIGHT)
                {
                    if (pPopup != NULL)
                        pPopup->show(this, e);
                }
            }

            return STATUS_OK;
        }

        status_t LSPAudioFile::on_mouse_move(const ws_event_t *e)
        {
//            bool pressed    = bPressed;
//            bPressed        = (nBMask == (1 << MCB_LEFT)) && (check_mouse_over(e->nLeft, e->nTop));

            size_t flags    = nStatus;
            nStatus         = (nBMask == (1 << MCB_LEFT)) && (check_mouse_over(e->nLeft, e->nTop)) ? nStatus | AF_PRESSED : nStatus & (~AF_PRESSED);
            if (flags != nStatus)
            {
                drop_glass();
                query_draw();
            }

            return STATUS_OK;
        }

        status_t LSPAudioFile::on_mouse_dbl_click(const ws_event_t *e)
        {
            if (e->nCode != MCB_RIGHT)
                return STATUS_OK;

            sFileName.truncate();
            lsp_trace("mouse double click");
//            nFileStatus = STATUS_UNSPECIFIED;
            return sSlots.execute(LSPSLOT_SUBMIT, NULL);
        }

        status_t LSPAudioFile::on_submit()
        {
            return STATUS_OK;
        }

        status_t LSPAudioFile::on_close()
        {
            return STATUS_OK;
        }

        status_t LSPAudioFile::on_activate()
        {
            return STATUS_OK;
        }

        status_t LSPAudioFile::on_drag_request(const ws_event_t *e, const char * const *ctype)
        {
            ssize_t idx = pSink->select_mime_type(ctype);
            if (idx >= 0)
                pDisplay->accept_drag(pSink, DRAG_COPY, true, &sSize);
            else
                pDisplay->reject_drag();
            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */
