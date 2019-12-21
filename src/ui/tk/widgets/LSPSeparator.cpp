/*
 * LSPSeparator.cpp
 *
 *  Created on: 9 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPSeparator::metadata = { "LSPSeparator", &LSPWidget::metadata };

        LSPSeparator::LSPSeparator(LSPDisplay *dpy, bool horizontal): LSPWidget(dpy),
            sColor(this)
        {
            nSize           = -1;
            nBorder         = 2;
            nLineWidth      = 1;
            nPadding        = 0;
            enOrientation   = (horizontal) ? O_HORIZONTAL : O_VERTICAL;
            pClass          = &metadata;
        }
        
        LSPSeparator::~LSPSeparator()
        {
        }

        status_t LSPSeparator::init()
        {
            status_t result = LSPWidget::init();
            if (result != STATUS_OK)
                return result;

            init_color(C_LABEL_TEXT, &sColor);

            return STATUS_OK;
        }

        void LSPSeparator::set_size(ssize_t value)
        {
            nSize       = value;
            query_resize();
        }

        void LSPSeparator::set_border(size_t value)
        {
            nBorder     = value;
            query_resize();
        }

        void LSPSeparator::set_padding(size_t value)
        {
            nPadding    = value;
            query_resize();
        }

        void LSPSeparator::set_line_width(size_t value)
        {
            nLineWidth  = value;
            query_resize();
        }

        void LSPSeparator::set_horizontal(bool value)
        {
            set_orientation((value) ? O_HORIZONTAL : O_VERTICAL);
        }

        void LSPSeparator::set_vertical(bool value)
        {
            set_orientation((value) ? O_VERTICAL : O_HORIZONTAL);
        }

        void LSPSeparator::set_orientation(orientation_t value)
        {
            if (enOrientation == value)
                return;

            enOrientation = value;
            query_resize();
        }

        void LSPSeparator::render(ISurface *s, bool force)
        {
            // Prepare palette
            Color bg_color(sBgColor);
            Color color(sColor);
            color.scale_lightness(brightness());

            // Draw background
            s->fill_rect(sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight, bg_color);

            // Draw separator
            ssize_t width   = sSize.nWidth - nBorder * 2;
            ssize_t height  = sSize.nHeight - nBorder * 2;

            if (enOrientation == O_HORIZONTAL)
            {
                ssize_t length  = (nSize >= 0) ? nSize : width - nPadding * 2;
                if (length < ssize_t(nLineWidth))
                    length      = nLineWidth;
                s->fill_rect(sSize.nLeft + ((sSize.nWidth - length) >> 1), sSize.nTop + ((sSize.nHeight - nLineWidth) >> 1), length, nLineWidth, color);
            }
            else
            {
                ssize_t length  = (nSize >= 0) ? nSize : height - nPadding * 2;
                if (length < ssize_t(nLineWidth))
                    length      = nLineWidth;
                s->fill_rect(sSize.nLeft + ((sSize.nWidth - nLineWidth) >> 1), sSize.nTop + ((sSize.nHeight - length) >> 1), nLineWidth, length, color);
            }
        }

        void LSPSeparator::size_request(size_request_t *r)
        {
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;

            if (enOrientation == O_HORIZONTAL)
            {
                r->nMinWidth    = (nBorder + nPadding)*2 + nLineWidth;
                r->nMinHeight   = nBorder*2 + nLineWidth;
                if (nSize > 0)
                    r->nMinWidth   += nSize;
            }
            else
            {
                r->nMinWidth    = nBorder*2 + nLineWidth;
                r->nMinHeight   = (nBorder + nPadding)*2 + nLineWidth;
                if (nSize > 0)
                    r->nMinHeight  += nSize;
            };
        }
    
    } /* namespace tk */
} /* namespace lsp */
