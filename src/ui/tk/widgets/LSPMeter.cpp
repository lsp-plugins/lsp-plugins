/*
 * LSPMeter.cpp
 *
 *  Created on: 12 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPMeter::metadata = { "LSPMeter", &LSPWidget::metadata };

        LSPMeter::channel_t::channel_t(LSPWidget *widget):
            sColor(widget),
            sYellow(widget),
            sRed(widget),
            sBalance(widget)
        {
            fMin         = 0.0f;
            fMax         = 1.0f;
            fBalance     = 0.5f;
            fPeak        = 0.0f;
            fValue       = 0.0f;
            fRedZone     = 4.0f/6.0f;
            fYellowZone  = 3.0f/6.0f;
            fDarkZone[0] = 0.0f;
            fDarkZone[1] = 0.0f;
            fDarkZone[2] = 0.0f;
            sText        = NULL;
            nFlags       = 0;
            fDark[0]     = 0.0f;
            fDark[1]     = 0.0f;
            fDark[2]     = 0.0f;
        }

        LSPMeter::channel_t::~channel_t()
        {
            if (sText != NULL)
            {
                ::free(sText);
                sText    = NULL;
            }
        }

        LSPMeter::LSPMeter(LSPDisplay *dpy):
            LSPWidget(dpy),
            sIndColor(this),
            sFont(this)
        {
            nAngle      = 0;
            nMWidth     = 20;
            nMHeight    = 192;
            nBorder     = 2;
            bValues     = true;
            nSpacing    = 1;
            vChannels   = NULL;
            nChannels   = 0;
            pClass      = &metadata;
        }


        LSPMeter::~LSPMeter()
        {
            drop_data();
        }

        status_t LSPMeter::init()
        {
            status_t result = LSPWidget::init();
            if (result != STATUS_OK)
                return result;

            init_color(C_GLASS, &sIndColor);

            sFont.init();
            sFont.set_size(9);

            return STATUS_OK;
        }

        void LSPMeter::destroy()
        {
            drop_data();
            LSPWidget::destroy();
        }

        void LSPMeter::drop_data()
        {
            if (vChannels == NULL)
                return;

            for (size_t i=0; i<nChannels; ++i)
            {
                if (vChannels[i] != NULL)
                {
                    delete vChannels[i];
                    vChannels[i] = NULL;
                }
            }
            nChannels = 0;

            delete [] vChannels;
            vChannels = NULL;
        }

        status_t LSPMeter::set_mtr_min(size_t i, float value)
        {
            if (i >= nChannels)
                return STATUS_NOT_FOUND;
            if (vChannels[i]->fMin == value)
                return STATUS_OK;
            vChannels[i]->fMin       = value;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPMeter::set_mtr_max(size_t i, float value)
        {
            if (i >= nChannels)
                return STATUS_NOT_FOUND;
            if (vChannels[i]->fMax == value)
                return STATUS_OK;
            vChannels[i]->fMax       = value;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPMeter::set_mtr_balance(size_t i, float value)
        {
            if (i >= nChannels)
                return STATUS_NOT_FOUND;
            if (vChannels[i]->fBalance == value)
                return STATUS_OK;
            vChannels[i]->fBalance   = value;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPMeter::set_mtr_peak(size_t i, float value)
        {
            if (i >= nChannels)
                return STATUS_NOT_FOUND;
            if (vChannels[i]->fPeak == value)
                return STATUS_OK;
            vChannels[i]->fPeak      = value;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPMeter::set_mtr_value(size_t i, float value)
        {
            if (i >= nChannels)
                return STATUS_NOT_FOUND;
            if (vChannels[i]->fValue == value)
                return STATUS_OK;
            vChannels[i]->fValue     = value;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPMeter::set_mtr_rz_value(size_t i, float value)
        {
            if (i >= nChannels)
                return STATUS_NOT_FOUND;
            if (vChannels[i]->fRedZone == value)
                return STATUS_OK;
            vChannels[i]->fRedZone   = value;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPMeter::set_mtr_yz_value(size_t i, float value)
        {
            if (i >= nChannels)
                return STATUS_NOT_FOUND;
            if (vChannels[i]->fYellowZone == value)
                return STATUS_OK;
            vChannels[i]->fYellowZone= value;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPMeter::set_mtr_dz0_value(size_t i, float value)
        {
            if (i >= nChannels)
                return STATUS_NOT_FOUND;
            if (vChannels[i]->fDarkZone[0] == value)
                return STATUS_OK;
            vChannels[i]->fDarkZone[0]  = value;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPMeter::set_mtr_dz1_value(size_t i, float value)
        {
            if (i >= nChannels)
                return STATUS_NOT_FOUND;
            if (vChannels[i]->fDarkZone[1] == value)
                return STATUS_OK;
            vChannels[i]->fDarkZone[1]  = value;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPMeter::set_mtr_dz2_value(size_t i, float value)
        {
            if (i >= nChannels)
                return STATUS_NOT_FOUND;
            if (vChannels[i]->fDarkZone[2] == value)
                return STATUS_OK;
            vChannels[i]->fDarkZone[2]  = value;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPMeter::set_mtr_dz0_amount(size_t i, float value)
        {
            if (i >= nChannels)
                return STATUS_NOT_FOUND;
            if (vChannels[i]->fDark[0] == value)
                return STATUS_OK;
            vChannels[i]->fDark[0]  = value;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPMeter::set_mtr_dz1_amount(size_t i, float value)
        {
            if (i >= nChannels)
                return STATUS_NOT_FOUND;
            if (vChannels[i]->fDark[1] == value)
                return STATUS_OK;
            vChannels[i]->fDark[1]  = value;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPMeter::set_mtr_dz2_amount(size_t i, float value)
        {
            if (i >= nChannels)
                return STATUS_NOT_FOUND;
            if (vChannels[i]->fDark[2] == value)
                return STATUS_OK;
            vChannels[i]->fDark[2]  = value;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPMeter::set_mtr_text(size_t i, const char *text)
        {
            if (i >= nChannels)
                return STATUS_NOT_FOUND;
            char *s = vChannels[i]->sText;

            if (s == text)
                return STATUS_OK;
            else if (text == NULL)
            {
                free(s);
                vChannels[i]->sText = NULL;
                query_draw();

                return STATUS_OK;
            }
            else if (s != NULL)
            {
                if (!strcmp(s, text))
                    return STATUS_OK;
            }

            // Make a copy
            char *copy = strdup(text);
            if (copy == NULL)
                return STATUS_NO_MEM;

            // Replace old string by new
            if (s != NULL)
                free(s);
            vChannels[i]->sText = copy;

            query_draw();

            return STATUS_OK;
        }

        status_t LSPMeter::set_channels(size_t channels)
        {
            if (nChannels == channels)
                return STATUS_OK;

            if (channels <= 0)
            {
                drop_data();
                query_resize();
                return STATUS_OK;
            }

            // Allocate new list of channels
            channel_t **nc = new channel_t *[channels];
            if (nc == NULL)
                return STATUS_NO_MEM;

            // Copy previous values
            size_t to_copy = (nChannels < channels) ? nChannels : channels;
            for (size_t i=0; i<to_copy; ++i)
                nc[i]   = vChannels[i];

            if (channels > nChannels)
            {
                // List is growing
                for (size_t i=nChannels; i<channels; ++i)
                {
                    channel_t *c = new channel_t(this);
                    if (c == NULL)
                    {
                        for (size_t j=nChannels; j<i; ++i)
                            delete nc[j];
                        delete [] nc;
                        return STATUS_NO_MEM;
                    }

                    init_color(C_GREEN, &c->sColor);
                    init_color(C_YELLOW, &c->sYellow);
                    init_color(C_RED, &c->sRed);
                    init_color(C_YELLOW, &c->sBalance);

                    nc[i]   = c;
                }
            }
            else
            {
                // List is lowering
                for (size_t i=channels; i<nChannels; ++i)
                    delete vChannels[i];
            }

            // Drop previous pointer to channels and replace by new
            delete [] vChannels;
            vChannels       = nc;
            nChannels       = channels;

            query_resize();
            return STATUS_OK;
        }

        void LSPMeter::set_angle(size_t value)
        {
            value      %= 4;
            if (nAngle == value)
                return;
            nAngle      = value;
            query_resize();
        }

        void LSPMeter::set_border(size_t value)
        {
            if (size_t(nBorder) == value)
                return;
            nBorder      = value;
            query_resize();
        }

        void LSPMeter::set_mtr_width(size_t value)
        {
            if (nMWidth == value)
                return;
            nMWidth     = value;
            query_resize();
        }

        void LSPMeter::set_mtr_height(size_t value)
        {
            if (nMHeight == value)
                return;
            nMHeight    = value;
            query_resize();
        }

        status_t LSPMeter::set_flag(size_t i, size_t flag, bool value)
        {
            if (i >= nChannels)
                return STATUS_NOT_FOUND;

            size_t flags = vChannels[i]->nFlags;
            if (value)
                flags  |= flag;
            else
                flags  &= ~flag;
            if (vChannels[i]->nFlags == flags)
                return STATUS_OK;

            vChannels[i]->nFlags = flags;
            query_draw();

            return STATUS_OK;
        }

        void LSPMeter::show_values(bool show)
        {
            if (bValues == show)
                return;
            bValues = show;
            query_resize();
        }

        void LSPMeter::set_spacing(size_t spacing)
        {
            if (nSpacing == spacing)
                return;
            nSpacing = spacing;
            query_resize();
        }

        void LSPMeter::size_request(size_request_t *r)
        {
            size_t width  = nBorder * 2;
            size_t height = nBorder * 2;

            if (bValues)
            {
                // Create temporary surface
                if (pDisplay == NULL)
                    return;
                ISurface *s = pDisplay->create_surface(1, 1);
                if (s == NULL)
                    return;

                font_parameters_t fp;
                text_parameters_t tp;

                sFont.get_parameters(s, &fp);
                sFont.get_text_parameters(s, &tp, "+99.9");

                ssize_t txt_w   = tp.Width;
                ssize_t txt_h   = fp.Height;
                if (nAngle & 1)
                    height     += ((nChannels > 1) ? 2*txt_h : txt_h) + 2;
                else
                    width      += txt_w + 4;

                // Destroy surface
                s->destroy();
                delete s;
            }

            size_t pairs    = (nChannels + 1) >> 1;
            size_t ov_width = (nMWidth * pairs) + (pairs + 1) * nSpacing;

            if (nAngle & 1)
            {
                width      += ov_width;
                height     += nMHeight;
            }
            else
            {
                width      += nMHeight + 2 + 3;
                height     += ov_width;
            }

            r->nMinWidth    = width;
            r->nMinHeight   = height;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;
        }

        void LSPMeter::draw_meter(ISurface *s, channel_t *c, float x, float y, ssize_t dx, ssize_t dy, float wx, float wy, size_t n)
        {
            float dist          = c->fMax - c->fMin;
            dist               += (dist > 0.0f) ? 1e-4 : -1e-4; // Error correction delta
            float delta         = dist / float(n);
            Color cl;

            float vmin          = c->fMin;
            float bright        = brightness();

//            if (c->nFlags & MF_BALANCE)
//                lsp_trace("fmin=%f, fmax=%f, delta=%f, n=%d", c->fMin, c->fMax, delta, int(n));

            for (size_t i=0; i<n; ++i)
            {
                float vmax          = vmin + delta;

//                if (c->nFlags & MF_BALANCE)
//                    lsp_trace("  vmin=%f, vmax=%f, fbalance=%f", vmin, vmax, c->fBalance);

                // Determine what color to use for this segment
                if ((c->nFlags & MF_BALANCE) && (c->fBalance >= vmin) && (c->fBalance < vmax))
                    cl.copy(c->sBalance.color());
                else if ((c->nFlags & MF_RED) && (c->fRedZone <= vmin))
                    cl.copy(c->sRed.color());
                else if ((c->nFlags & MF_YELLOW) && (c->fYellowZone <= vmin))
                    cl.copy(c->sYellow.color());
                else
                    cl.copy(c->sColor.color());

                // Darken color if needed
                if ((c->nFlags & MF_DZONE2) && (c->fDarkZone[2] >= vmax))
                    cl.darken(c->fDark[2]);
                else if ((c->nFlags & MF_DZONE1) && (c->fDarkZone[1] >= vmax))
                    cl.darken(c->fDark[1]);
                else if ((c->nFlags & MF_DZONE0) && (c->fDarkZone[0] >= vmax))
                    cl.darken(c->fDark[0]);

                // Now determine if we need to darken the color
                bool matched = false;

                if (c->nFlags & MF_INACTIVE)
                    matched = false;
                else if (c->nFlags & MF_BALANCE)
                {
                    matched     = (c->fBalance < c->fValue) ?
                        ((vmax > c->fBalance) && (vmin <= c->fValue))
                        : ((vmax > c->fValue) && (vmin <= c->fBalance));

                    if ((!matched) && (c->nFlags & MF_PEAK))
                        matched     = (c->fPeak >= vmin) && (c->fPeak < vmax);

                    matched ^= bool(c->nFlags & MF_REVERSIVE);
                }
                else
                {
                    matched     = (vmin < c->fValue);
                    if ((!matched) && (c->nFlags & MF_PEAK))
                        matched     = (c->fPeak > vmin) && (c->fPeak <= vmax);

                    matched ^= bool(c->nFlags & MF_REVERSIVE);
                }

                if (! matched)
                    cl.blend(sIndColor.color(), 0.05f);
                cl.scale_lightness(bright);

                // Draw the segment
                s->fill_rect(x, y, wx, wy, cl);

                // Update location
                x      += dx;
                y      += dy;
                vmin    = vmax;
            }
        }

        void LSPMeter::out_text(ISurface *s, channel_t *c, float x, float y)
        {
            // Do not show anything for inactive meter
            if (c->nFlags & MF_INACTIVE)
                return;

            Color cl; //(1.0f, 1.0f, 1.0f);

            float value = (c->nFlags & MF_PEAK) ? c->fPeak : c->fValue;
            if ((c->nFlags & MF_RED) && (c->fRedZone <= value))
                cl.copy(c->sRed.color());
            else if ((c->nFlags & MF_YELLOW) && (c->fYellowZone <= value))
                cl.copy(c->sYellow.color());
            else
                cl.copy(c->sColor.color());

            // Darken color if needed
            if ((c->nFlags & MF_DZONE2) && (c->fDarkZone[2] >= value))
                cl.darken(c->fDark[2]);
            else if ((c->nFlags & MF_DZONE1) && (c->fDarkZone[1] >= value))
                cl.darken(c->fDark[1]);
            else if ((c->nFlags & MF_DZONE0) && (c->fDarkZone[0] >= value))
                cl.darken(c->fDark[0]);

            cl.scale_lightness(brightness());

//            s->line(x - 3, y, x + 3, y, 1.0f, white);
//            s->line(x, y-3, x, y+3, 1.0f, white);
            font_parameters_t   fp;
            text_parameters_t   tp;
            sFont.get_parameters(s, &fp);
            sFont.get_text_parameters(s, &tp, c->sText);

            ssize_t fx = x - tp.Width  * 0.5f;
            ssize_t fy = y - fp.Height * 0.5f + fp.Ascent;

            sFont.draw(s, fx, fy, cl, c->sText);
        }

        void LSPMeter::draw(ISurface *s)
        {
            // Prepare palette
            Color bg_color(sBgColor);
            Color ind_color(sIndColor);

            ind_color.scale_lightness(brightness());

            // Variables
            font_parameters_t   fp;
            text_parameters_t   tp;

            size_t w            = sSize.nWidth;  // Real width
            size_t h            = sSize.nHeight; // Real height
            ssize_t cx          = (w >> 1) - nBorder; // X center
            ssize_t cy          = (h >> 1) - nBorder; // Y center
            ssize_t tsx         = 0;
            ssize_t tsy         = 0;

            // Draw background
            s->fill_rect(0, 0, sSize.nWidth, sSize.nHeight, bg_color);
            bool aa             = s->set_antialiasing(true);

            // Estimate text field size
            ssize_t m_width     = nMWidth;
            ssize_t m_height    = (nAngle & 1) ? sSize.nHeight - nBorder*2 : sSize.nWidth - nBorder * 2;

            if (bValues)
            {
                sFont.get_parameters(s, &fp);
                sFont.get_text_parameters(s, &tp, "+99.9");

                tsx         = tp.Width + 2;
                tsy         = (nChannels > 1) ? 2 * (fp.Height + 3) : fp.Height + 2;
                m_height   -= (nAngle & 1) ? tsy : tsx;
            }

            ssize_t s_width     = m_width;
            ssize_t p_width     = m_width >> 1;
            ssize_t fp_width    = (nSpacing + (p_width<<1));

            size_t pairs        = nChannels >> 1;
            ssize_t ov_width    = fp_width * pairs + (s_width + 1) * (nChannels & 1) - 1;
            ssize_t ov_height   = m_height + ((nAngle & 1) ? tsy : tsx);
            size_t segments     = m_height >> 2;
            size_t swidth       = segments * 4;

            // Draw meter
            if (nAngle & 1)
            {
                // Vertical meter
                ssize_t ta_height   = ov_height - swidth - 2;
                ssize_t left        = cx - (ov_width >> 1);
                ssize_t top         = cy - (ov_height >> 1);
                if (left < nBorder)
                    left                = nBorder;
                if (top < nBorder)
                    top                 = nBorder;

                float mtr_ix        = left + 0.5f;
                ssize_t w2          = p_width - 1;

                s->fill_rect(left - nBorder, top - nBorder, ov_width + (nBorder << 1), ov_height + (nBorder << 1), ind_color);
                channel_t **c       = vChannels;

                if (nAngle & 2)
                {
                    // Top to bottom
                    float mtr_iy        = top + ov_height - swidth + 0.5f;
                    ssize_t txt_cx, txt_cy;

                    for (size_t i=0; i < pairs; ++i)
                    {
                        txt_cx              = left + fp_width*i + p_width;
                        txt_cy              = top + (fp.Height * 0.5f) + 2;
                        draw_meter(s, *c, mtr_ix, mtr_iy, 0, 4, w2, 3, segments);
//                        s->line(txt_cx - 3, txt_cy, txt_cx + 3, txt_cy, 1, (*c)->sColor);
//                        s->line(txt_cx, txt_cy - 3, txt_cx, txt_cy + 3, 1, (*c)->sColor);
                        out_text(s, *c, txt_cx, txt_cy);
                        mtr_ix             += p_width;
                        c                  ++;

                        txt_cy             += (fp.Height + 2);
                        draw_meter(s, *c, mtr_ix, mtr_iy, 0, 4, w2, 3, segments);
//                        s->line(txt_cx - 3, txt_cy, txt_cx + 3, txt_cy, 1, (*c)->sColor);
//                        s->line(txt_cx, txt_cy - 3, txt_cx, txt_cy + 3, 1, (*c)->sColor);
                        out_text(s, *c, txt_cx, txt_cy);
                        mtr_ix             += p_width + nSpacing;
                        c                  ++;
                    }
                    if (nChannels & 1)
                    {
                        txt_cx              = left + fp_width*pairs + (s_width >> 1);
                        txt_cy              = top + (ta_height>>1) + 2;
                        draw_meter(s, *c, mtr_ix, mtr_iy, 0, 4, s_width - 1, 3, segments);
//                        s->line(txt_cx - 3, txt_cy, txt_cx + 3, txt_cy, 1, (*c)->sColor);
//                        s->line(txt_cx, txt_cy - 3, txt_cx, txt_cy + 3, 1, (*c)->sColor);
                        out_text(s, *c, txt_cx, txt_cy);
                    }
                }
                else
                {
                    // Bottom to top
                    float mtr_iy        = top + swidth - 3.5f;
                    ssize_t txt_cx,  txt_cy;

                    for (size_t i=0; i < pairs; ++i)
                    {
                        txt_cx              = left + fp_width*i + p_width;
                        txt_cy              = top + ov_height + (fp.Height * 0.5f) - ta_height;
                        draw_meter(s, *c, mtr_ix, mtr_iy, 0, -4, w2, 3, segments);
//                        s->line(txt_cx - 3, txt_cy, txt_cx + 3, txt_cy, 1, (*c)->sColor);
//                        s->line(txt_cx, txt_cy - 3, txt_cx, txt_cy + 3, 1, (*c)->sColor);
                        out_text(s, *c, txt_cx, txt_cy);
                        mtr_ix             += p_width;
                        c                  ++;

                        txt_cy             += (fp.Height + 2);
                        draw_meter(s, *c, mtr_ix, mtr_iy, 0, -4, w2, 3, segments);
//                        s->line(txt_cx - 3, txt_cy, txt_cx + 3, txt_cy, 1, (*c)->sColor);
//                        s->line(txt_cx, txt_cy - 3, txt_cx, txt_cy + 3, 1, (*c)->sColor);
                        out_text(s, *c, txt_cx, txt_cy);
                        mtr_ix             += p_width + nSpacing;
                        c                  ++;
                    }
                    if (nChannels & 1)
                    {
                        txt_cx              = left + fp_width*pairs + (s_width >> 1);
                        txt_cy              = top + ov_height - (ta_height>>1);
                        draw_meter(s, *c, mtr_ix, mtr_iy, 0, -4, s_width - 1, 3, segments);
//                        s->line(txt_cx - 3, txt_cy, txt_cx + 3, txt_cy, 1, (*c)->sColor);
//                        s->line(txt_cx, txt_cy - 3, txt_cx, txt_cy + 3, 1, (*c)->sColor);
                        out_text(s, *c, txt_cx, txt_cy);
                    }
                }
            }
            else
            {
                // Horizontal meter
                ssize_t ta_width    = ov_height - swidth - 2;
                ssize_t left        = cx - (ov_height >> 1);
                ssize_t top         = cy - (ov_width >> 1);
                if (left < nBorder)
                    left                = nBorder;
                if (top < nBorder)
                    top                 = nBorder;

                float mtr_iy        = top + 0.5f;
                ssize_t w2          = p_width - 1;

                // Draw glass
                s->fill_rect(left - nBorder, top - nBorder, ov_height + (nBorder << 1), ov_width + (nBorder << 1), ind_color);
                channel_t **c       = vChannels;

                if (nAngle & 2)
                {
                    // Right to left
                    float mtr_ix        = left + swidth - 3.5f;
                    ssize_t txt_cx      = left + ov_height - (ta_width >> 1);
                    ssize_t txt_cy;

                    for (size_t i=0; i < pairs; ++i)
                    {
                        txt_cy              = top + fp_width*i + (p_width >> 1);
                        draw_meter(s, *c, mtr_ix, mtr_iy, -4, 0, 3, w2, segments);
                        out_text(s, *c, txt_cx, txt_cy);
                        mtr_iy             += p_width;
                        c                  ++;

                        txt_cy             += p_width;
                        draw_meter(s, *c, mtr_ix, mtr_iy, -4, 0, 3, w2, segments);
                        out_text(s, *c, txt_cx, txt_cy);
                        mtr_iy             += p_width + nSpacing;
                        c                  ++;
                    }
                    if (nChannels & 1)
                    {
                        txt_cy              = top + fp_width*pairs + (s_width >> 1);
                        draw_meter(s, *c, mtr_ix, mtr_iy, -4, 0, 3, s_width - 1, segments);
                        out_text(s, *c, txt_cx, txt_cy);
                    }
                }
                else
                {
                    // Left to right
                    float mtr_ix        = left + ov_height - swidth + 0.5f;
                    ssize_t txt_cx      = left + (ta_width >> 1);
                    ssize_t txt_cy      = top;

                    for (size_t i=0; i < pairs; ++i)
                    {
                        txt_cy              = top + fp_width*i + (p_width >> 1);
                        draw_meter(s, *c, mtr_ix, mtr_iy,  4, 0, 3, w2, segments);
                        out_text(s, *c, txt_cx, txt_cy);
                        mtr_iy             += p_width;
                        c                  ++;

                        txt_cy             += p_width;
                        draw_meter(s, *c, mtr_ix, mtr_iy,  4, 0, 3, w2, segments);
                        out_text(s, *c, txt_cx, txt_cy);
                        mtr_iy             += p_width + nSpacing;
                        c                  ++;
                    }
                    if (nChannels & 1)
                    {
                        txt_cy              = top + fp_width*pairs + (s_width >> 1);
                        draw_meter(s, *c, mtr_ix, mtr_iy, 4, 0, 3, s_width - 1, segments);
                        out_text(s, *c, txt_cx, txt_cy);
                    }
                }
            }

            s->set_antialiasing(aa);
        }

    } /* namespace tk */
} /* namespace lsp */
