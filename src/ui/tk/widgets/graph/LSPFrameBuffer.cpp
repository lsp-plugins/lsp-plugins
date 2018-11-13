/*
 * LSPFrame.cpp
 *
 *  Created on: 12 нояб. 2018 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <core/sugar.h>
#include <dsp/dsp.h>

static const float DIV_2_3 = 2.0f / 3.0f;

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPFrameBuffer::metadata = { "LSPFrame", &LSPGraphItem::metadata };
        
        LSPFrameBuffer::LSPFrameBuffer(LSPDisplay *dpy): LSPGraphItem(dpy)
        {
            nChanges    = 0;
            nRows       = 0;
            nCols       = 0;
            nCurrRow    = 0;
            vData       = NULL;
            pData       = NULL;

            fOpacity    = 1.0f;
            nAngle      = 0;
            fHPos       = -1.0f;
            fVPos       = -1.0f;
            fWidth      = 1.0f;
            fHeight     = 1.0f;
            bClear      = true;

            pClass      = &metadata;

            sBgColor.set_rgba(0.0f, 0.0f, 0.0f, 1.0f); // Full transparency
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
        }

        float *LSPFrameBuffer::get_buffer()
        {
            if (vData != NULL)
                return vData;

            size_t amount = nRows * nCols;
            if (amount <= 0)
                return NULL;

            vData = alloc_aligned<float>(pData, amount, ALIGN64);
            if (vData == NULL)
                return NULL;

            dsp::fill_zero(vData, amount);
            nCurrRow    = 0;
            nChanges    = 0;
            bClear      = true;

            return vData;
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

        status_t LSPFrameBuffer::append_data(const float *data)
        {
            float *buf = get_buffer();
            if (buf == NULL)
                return STATUS_NO_MEM;

            if (++nCurrRow > nRows)
                nCurrRow = 0;

            dsp::copy(&buf[nCurrRow * nCols], data, nCols);
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

        void LSPFrameBuffer::set_opacity(float value)
        {
            if (fOpacity != value)
                fOpacity = value;
            query_draw();
        }

        void LSPFrameBuffer::calc_color(Color &c, float value)
        {
            value = (value >= 0.0f) ? 1.0f - value : 1.0f + value;
            if (value < DIV_2_3)
                c.set_hsla(value, 1.0f, 1.0f, 0.0f);
            else
                c.set_hsla(DIV_2_3, 1.0f, 1.0f, (value - DIV_2_3) * 3.0f);
        }

        void LSPFrameBuffer::render(ISurface *s, bool force)
        {
            // Check size
            if ((nRows <= 0) || (nCols <= 0))
                return;

            // Get data buffer
            float *buf = get_buffer();
            if (buf == NULL)
                return;

            // Get drawing surface
            ISurface *pp = get_surface(s, nRows, nCols);
            if (pp == NULL)
                return;

            // Deploy new changes
            if ((nChanges > 0) || (bClear))
            {
                // Shift surface
                if ((bClear) || (nChanges >= nRows))
                {
                    pp->clear(sBgColor);
                    nChanges = nRows;
                }
                else
                {
                    ISurface *c = pp->create_copy();
                    if (c != NULL)
                    {
                        pp->clear(sBgColor);

                        switch (nAngle & 0x03)
                        {
                            case 0: pp->draw(c, 0, nChanges); break;
                            case 1: pp->draw(c, nChanges, 0); break;
                            case 2: pp->draw(c, 0, -ssize_t(nChanges)); break;
                            case 3: pp->draw(c, -ssize_t(nChanges), 0); break;
                            default: break;
                        }
                        c->destroy();
                        delete c;
                    }
                }

                // Draw dots
                Color c;
                size_t row = nCurrRow;
                switch (nAngle & 0x03)
                {
                    case 0:
                        for (ssize_t y=0; nChanges > 0; ++y, --nChanges)
                        {
                            const float *p = &vData[row * nCols];
                            for (size_t x=0; x<nCols; ++x)
                            {
                                calc_color(c, *(p++));
                                pp->square_dot(x, y, 1.0f, c);
                            }
                            row = (row + nRows - 1) % nRows;
                        }
                        break;
                    case 1:
                        for (ssize_t x=0; nChanges > 0; ++x, --nChanges)
                        {
                            const float *p = &vData[row * nCols];
                            for (size_t y=0; y<nCols; ++y)
                            {
                                calc_color(c, *(p++));
                                pp->square_dot(x, y, 1.0f, c);
                            }
                            row = (row + nRows - 1) % nRows;
                        }
                        break;
                    case 2:
                        for (ssize_t y=nRows-1; nChanges > 0; --y, --nChanges)
                        {
                            const float *p = &vData[row * nCols];
                            for (size_t x=0; x<nCols; ++x)
                            {
                                calc_color(c, *(p++));
                                pp->square_dot(x, y, 1.0f, c);
                            }
                            row = (row + nRows - 1) % nRows;
                        }
                        break;
                    case 3:
                        for (ssize_t x=nRows-1; nChanges > 0; --x, --nChanges)
                        {
                            const float *p = &vData[row * nCols];
                            for (size_t y=0; y<nCols; ++y)
                            {
                                calc_color(c, *(p++));
                                pp->square_dot(x, y, 1.0f, c);
                            }
                            row = (row + nRows - 1) % nRows;
                        }
                        break;
                }

                // Reset number of changes and clear flag
                nChanges    = 0;
                bClear      = false;
            }

            // Draw surface on the target
            float x = (fHPos + 1.0f) * s->width();
            float y = (1.0f - fVPos) * s->height();

            if (nAngle & 1)
            {
                float w = ((s->width() << 1) * fWidth) / nRows;
                float h = ((s->height() << 1) * fHeight) / nCols;
                s->draw(pSurface, x, y, w, h);
            }
            else
            {
                float w = ((s->width() << 1) * fWidth) / nCols;
                float h = ((s->height() << 1) * fHeight) / nRows;
                s->draw(pSurface, x, y, w, h);
            }
        }
    } /* namespace tk */
} /* namespace lsp */
