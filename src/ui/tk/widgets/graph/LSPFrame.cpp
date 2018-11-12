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
        const w_class_t LSPFrame::metadata = { "LSPFrame", &LSPGraphItem::metadata };
        
        LSPFrame::LSPFrame(LSPDisplay *dpy): LSPGraphItem(dpy)
        {
            nChanges    = 0;
            nRows       = 0;
            nCols       = 0;
            nCurrRow    = 0;
            vData       = NULL;
            pData       = NULL;

            fOpacity    = 1.0f;
            nAngle      = 0;
            nHPos       = 0;
            nVPos       = 0;
            nWidth      = 32;
            nHeight     = 32;
            bClear      = true;

            pClass      = &metadata;

            sBgColor.set_rgba(0.0f, 0.0f, 0.0f, 1.0f); // Full transparency
        }
        
        LSPFrame::~LSPFrame()
        {
            drop_data();
        }
    
        void LSPFrame::drop_data()
        {
            if (vData != NULL)
            {
                free_aligned(pData);
                vData = NULL;
                pData = NULL;
            }
        }

        float *LSPFrame::get_buffer()
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

        status_t LSPFrame::init()
        {
            status_t result = LSPGraphItem::init();
            if (result != STATUS_OK)
                return result;

            return STATUS_OK;
        }

        void LSPFrame::destroy()
        {
            drop_data();
        }

        status_t LSPFrame::append_data(const float *data)
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

        void LSPFrame::set_rows(size_t rows)
        {
            if (nRows == rows)
                return;
            nRows = rows;
            drop_data();
            query_draw();
        }

        void LSPFrame::set_cols(size_t cols)
        {
            if (nCols == cols)
                return;
            nCols = cols;
            drop_data();
            query_draw();
        }

        void LSPFrame::set_angle(size_t angle)
        {
            if (nAngle == angle)
                return;
            nAngle = angle;
            query_draw();
        }

        void LSPFrame::set_hpos(ssize_t value)
        {
            if (nHPos == value)
                return;
            nHPos = value;
            query_draw();
        }

        void LSPFrame::set_vpos(ssize_t value)
        {
            if (nVPos == value)
                return;
            nVPos = value;
            query_draw();
        }

        void LSPFrame::set_width(ssize_t value)
        {
            if (nWidth == value)
                return;
            nWidth = value;
            query_draw();
        }

        void LSPFrame::set_height(ssize_t value)
        {
            if (nHeight == value)
                return;
            nHeight = value;
            query_draw();
        }

        void LSPFrame::set_opacity(float value)
        {
            if (fOpacity != value)
                fOpacity = value;
            query_draw();
        }

        void LSPFrame::render(ISurface *s, bool force)
        {
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
                if ((bClear) || (nChanges >= nRows))
                    pp->clear(sBgColor);
                else
                {
                    pp->create_copy();
                    pp->clear(sBgColor);
                }
            }
        }
    } /* namespace tk */
} /* namespace lsp */
