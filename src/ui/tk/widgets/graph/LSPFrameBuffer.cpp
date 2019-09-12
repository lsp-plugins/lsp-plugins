/*
 * LSPFrame.cpp
 *
 *  Created on: 12 нояб. 2018 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <core/sugar.h>
#include <dsp/dsp.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPFrameBuffer::metadata = { "LSPFrameBuffer", &LSPGraphItem::metadata };
        
        LSPFrameBuffer::LSPFrameBuffer(LSPDisplay *dpy): LSPGraphItem(dpy)
        {
            nChanges    = 0;
            nRows       = 0;
            nCols       = 0;
            nCurrRow    = 0;
            vData       = NULL;
            vTempRGBA   = NULL;
            pData       = NULL;

            fTransparency    = 1.0f;
            nAngle      = 0;
            fHPos       = -1.0f;
            fVPos       = 1.0f;
            fWidth      = 1.0f;
            fHeight     = 1.0f;
            bClear      = true;
            nPalette    = 0;
            pCalcColor  = &LSPFrameBuffer::calc_rainbow_color;

            pClass      = &metadata;

            sBgColor.set_rgba(0.0f, 0.0f, 0.0f, 1.0f); // Full transparency
            sColor.set_rgba(1.0f, 0.0f, 0.0f, 0.0f);

            // Store actual color value
            sColRGBA.r  = sColor.red();
            sColRGBA.g  = sColor.green();
            sColRGBA.b  = sColor.blue();
            sColRGBA.a  = sColor.alpha();

            sBgRGBA.r   = sBgColor.red();
            sBgRGBA.g   = sBgColor.green();
            sBgRGBA.b   = sBgColor.blue();
            sBgRGBA.a   = sBgColor.alpha();
        }
        
        LSPFrameBuffer::~LSPFrameBuffer()
        {
            drop_data();
        }
    
        void LSPFrameBuffer::drop_data()
        {
            if (vData != NULL)
            {
                free_aligned(pData);
                vData = NULL;
                pData = NULL;
            }
            vTempRGBA   = NULL;
        }

        void LSPFrameBuffer::allocate_buffer()
        {
            size_t amount = nRows * nCols;
            if (amount <= 0)
                return;

            amount     += nCols * 4; // RGBA x number of columns for temporary buffer
            vData       = alloc_aligned<float>(pData, amount, ALIGN64);
            vTempRGBA   = &vData[nRows * nCols];
        }

        float *LSPFrameBuffer::get_buffer()
        {
            if (vData == NULL)
                allocate_buffer();
            return vData;
        }

        float *LSPFrameBuffer::get_rgba_buffer()
        {
            if (vTempRGBA == NULL)
                allocate_buffer();
            return vTempRGBA;
        }

        status_t LSPFrameBuffer::init()
        {
            status_t result = LSPGraphItem::init();
            if (result != STATUS_OK)
                return result;

            return STATUS_OK;
        }

        void LSPFrameBuffer::destroy()
        {
            drop_data();
        }

        void LSPFrameBuffer::check_color_changed()
        {
            // Trigger bClean flag if color changed
            if (!bClear)
            {
                bClear =
                    (sColor.red() != sColRGBA.r) ||
                    (sColor.green() != sColRGBA.g) ||
                    (sColor.blue() != sColRGBA.b) ||
                    (sColor.alpha() != sColRGBA.a);

                if (!bClear)
                    bClear =
                        (sBgColor.red() != sBgRGBA.r) ||
                        (sBgColor.green() != sBgRGBA.g) ||
                        (sBgColor.blue() != sBgRGBA.b) ||
                        (sBgColor.alpha() != sBgRGBA.a);
            }

            // Store actual color value
            sColRGBA.r  = sColor.red();
            sColRGBA.g  = sColor.green();
            sColRGBA.b  = sColor.blue();
            sColRGBA.a  = sColor.alpha();

            sBgRGBA.r   = sBgColor.red();
            sBgRGBA.g   = sBgColor.green();
            sBgRGBA.b   = sBgColor.blue();
            sBgRGBA.a   = sBgColor.alpha();
        }

        status_t LSPFrameBuffer::append_data(uint32_t row_id, const float *data)
        {
            float *buf = get_buffer();
            if (buf == NULL)
                return STATUS_NO_MEM;

            if (nCurrRow != row_id)
            {
                lsp_trace("Row out of sync: curr=%d, requested=%d", int(nCurrRow), int(row_id));
                bClear      = true;
            }

            nCurrRow        = row_id + 1; // Estimate next row number
            size_t dst_row  = row_id % nRows;
            dsp::limit2(&buf[dst_row * nCols], data, 0.0f, 1.0f, nCols);
//            dsp::limit_saturate2(&buf[dst_row * nCols], data, nCols);
            query_draw();

            nChanges++;

            return STATUS_OK;
        }

        void LSPFrameBuffer::set_rows(size_t rows)
        {
            if (nRows == rows)
                return;
            nRows = rows;
            drop_data();
            query_draw();
        }

        void LSPFrameBuffer::set_cols(size_t cols)
        {
            if (nCols == cols)
                return;
            nCols = cols;
            drop_data();
            query_draw();
        }

        void LSPFrameBuffer::set_size(size_t rows, size_t cols)
        {
            if ((nRows == rows) && (nCols == cols))
                return;
            nRows = rows;
            nCols = cols;
            drop_data();
            query_draw();
        }

        void LSPFrameBuffer::set_angle(size_t angle)
        {
            if (nAngle == angle)
                return;
            nAngle = angle;
            bClear = true;
            query_draw();
        }

        void LSPFrameBuffer::set_hpos(float value)
        {
            if (fHPos == value)
                return;
            fHPos = value;
            query_draw();
        }

        void LSPFrameBuffer::set_vpos(float value)
        {
            if (fVPos == value)
                return;
            fVPos = value;
            query_draw();
        }

        void LSPFrameBuffer::set_width(float value)
        {
            if (fWidth == value)
                return;
            fWidth = value;
            query_draw();
        }

        void LSPFrameBuffer::set_height(float value)
        {
            if (fHeight == value)
                return;
            fHeight = value;
            query_draw();
        }

        void LSPFrameBuffer::set_transparency(float value)
        {
            if (fTransparency != value)
                fTransparency = value;
            query_draw();
        }

        void LSPFrameBuffer::set_palette(size_t value)
        {
            if (nPalette == value)
                return;

            switch (value % 5)
            {
                case 0: pCalcColor  = &LSPFrameBuffer::calc_rainbow_color; break;
                case 1: pCalcColor  = &LSPFrameBuffer::calc_fog_color; break;
                case 2: pCalcColor  = &LSPFrameBuffer::calc_color; break;
                case 3: pCalcColor  = &LSPFrameBuffer::calc_lightness; break;
                case 4: pCalcColor  = &LSPFrameBuffer::calc_lightness2; break;
                default:
                    pCalcColor  = &LSPFrameBuffer::calc_rainbow_color; break;
                    break;
            }

            nPalette = value;
            bClear = true;
            query_draw();
        }

        void LSPFrameBuffer::calc_rainbow_color(float *rgba, const float *v, size_t n)
        {
            dsp::hsla_hue_eff_t eff;
            eff.h       = sColor.hue();
            eff.s       = sColor.saturation();
            eff.l       = sColor.lightness();
            eff.a       = sColor.alpha();
            eff.thresh  = 1.0f / 3.0f;

            dsp::eff_hsla_hue(rgba, v, &eff, n);
            dsp::hsla_to_rgba(rgba, rgba, n);
        }

        void LSPFrameBuffer::calc_fog_color(float *rgba, const float *v, size_t n)
        {
            dsp::hsla_alpha_eff_t eff;
            eff.h       = sColor.hue();
            eff.s       = sColor.saturation();
            eff.l       = sColor.lightness();
            eff.a       = sColor.alpha();

            dsp::eff_hsla_alpha(rgba, v, &eff, n);
            dsp::hsla_to_rgba(rgba, rgba, n);
        }

        void LSPFrameBuffer::calc_color(float *rgba, const float *v, size_t n)
        {
            dsp::hsla_sat_eff_t eff;
            eff.h       = sColor.hue();
            eff.s       = sColor.saturation();
            eff.l       = sColor.lightness();
            eff.a       = sColor.alpha();
            eff.thresh  = 0.25f;

            dsp::eff_hsla_sat(rgba, v, &eff, n);
            dsp::hsla_to_rgba(rgba, rgba, n);
        }

        void LSPFrameBuffer::calc_lightness(float *rgba, const float *v, size_t n)
        {
            dsp::hsla_light_eff_t eff;
            eff.h       = sColor.hue();
            eff.s       = sColor.saturation();
            eff.l       = 1.0f;
            eff.a       = sColor.alpha();
            eff.thresh  = 0.25f;

            dsp::eff_hsla_light(rgba, v, &eff, n);
            dsp::hsla_to_rgba(rgba, rgba, n);
        }

        void LSPFrameBuffer::calc_lightness2(float *rgba, const float *v, size_t n)
        {
            dsp::hsla_light_eff_t eff;
            eff.h       = sColor.hue();
            eff.s       = sColor.saturation();
            eff.l       = 0.5f;
            eff.a       = sColor.alpha();
            eff.thresh  = 0.25f;

            dsp::eff_hsla_light(rgba, v, &eff, n);
            dsp::hsla_to_rgba(rgba, rgba, n);
        }

        void LSPFrameBuffer::render(ISurface *s, bool force)
        {
            // Check size
            if ((nRows <= 0) || (nCols <= 0))
                return;

            // Get data buffer
            float *buf = get_buffer();
            float *rgba = get_rgba_buffer();
            if ((buf == NULL) || (rgba == NULL))
                return;

            // Get drawing surface
            ISurface *pp = get_surface(s, nCols, nRows);
            if (pp == NULL)
                return;

            // Check whether the color has changed and we need to clear buffer
            check_color_changed();

            // Deploy new changes
            if ((nChanges > 0) || (bClear))
            {
                // Get target buffer for rendering
                uint8_t *xp = reinterpret_cast<uint8_t *>(pp->start_direct());
                if (xp == NULL)
                    return;

                // Do not draw more than can
                if ((nChanges >= nRows) || (bClear))
                    nChanges    = nRows;

                // Shift buffer
                size_t stride = pp->stride();
                ::memmove(&xp[stride * nChanges], xp, (nRows - nChanges) * stride);

                // Draw dots
                Color c(1.0f, 0.0f, 0.0f);
                size_t row = (nCurrRow + nRows - 1) % nRows;

                for (size_t i=0; i<nChanges; ++i, xp += stride)
                {
                    const float *p = &vData[row * nCols];
                    (this->*pCalcColor)(rgba, p, nCols);
                    dsp::rgba_to_bgra32(xp, rgba, nCols);
                    row = (row + nRows - 1) % nRows;
                }

                pp->end_direct();

                nChanges    = 0;
                bClear      = false;
            }

            // Draw surface on the target
            float x, y, sx, sy;
            float ra = -0.5f * nAngle * M_PI;

            x = 0.5f * (fHPos + 1.0f) * s->width();
            y = 0.5f * (1.0f - fVPos) * s->height();

            switch (nAngle & 0x03)
            {
                case 0:
                    sx = fWidth * s->width() / nCols;
                    sy = fHeight * s->height() / nRows;

                    if (sx < 0.0f)
                        x       -= sx * nCols;
                    if (sy < 0.0f)
                        y       -= sy * nRows;
                    break;

                case 1:
                    sx = fWidth * s->width() / nRows;
                    sy = fHeight * s->height() / nCols;

                    if (sx < 0.0f)
                        x       -= sx * nRows;
                    if (sy > 0.0f)
                        y       += sy * nCols;
                    break;

                case 2:
                    sx = fWidth * s->width() / nCols;
                    sy = fHeight * s->height() / nRows;

                    if (sx > 0.0f)
                        x       += sx * nCols;
                    if (sy > 0.0f)
                        y       += sy * nRows;
                    break;

                case 3:
                    sx = fWidth * s->width() / nRows;
                    sy = fHeight * s->height() / nCols;

                    if (sx > 0.0f)
                        x       += sx * nRows;
                    if (sy < 0.0f)
                        y       -= sy * nCols;
                    break;

                default:
                    break;
            }

            s->draw_rotate_alpha(pp, x, y, sx, sy, ra, fTransparency);
        }
    } /* namespace tk */
} /* namespace lsp */
