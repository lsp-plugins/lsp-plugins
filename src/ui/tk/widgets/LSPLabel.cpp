/*
 * LSPLabel.cpp
 *
 *  Created on: 6 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPLabel::metadata = { "LSPLabel", &LSPWidget::metadata };

        LSPLabel::LSPLabel(LSPDisplay *dpy):
            LSPWidget(dpy),
            sFont(dpy, this),
            sText(this)
        {
            fVAlign     = 0.5f;
            fHAlign     = 0.5f;
            nBorder     = 0;
            pClass      = &metadata;

            sFont.set_size(12.0f);
        }

        LSPLabel::~LSPLabel()
        {
        }

        status_t LSPLabel::init()
        {
            status_t result = LSPWidget::init();
            if (result != STATUS_OK)
                return result;

            sFont.init();
            init_color(C_LABEL_TEXT, sFont.color());

            sText.bind();

            return STATUS_OK;
        }

        void LSPLabel::destroy()
        {
        }

        void LSPLabel::set_valign(float align)
        {
            fVAlign     = align;
            query_draw();
        }

        void LSPLabel::set_halign(float align)
        {
            fHAlign     = align;
            query_draw();
        }

        void LSPLabel::set_align(float halign, float valign)
        {
            fVAlign     = valign;
            fHAlign     = halign;
            query_draw();
        }

        void LSPLabel::set_border(float border)
        {
            nBorder     = border;
            query_resize();
        }

        void LSPLabel::draw(ISurface *s)
        {
            // Initialize palette
            Color bg_color(sBgColor);
            Color font_color(sFont.raw_color());

            font_color.scale_lightness(brightness());

            // Draw background
            s->clear(bg_color);

            // Get text parameters
            font_parameters_t fp;
            text_parameters_t tp;
            sFont.get_parameters(s, &fp);

            // Estimate number of lines
            LSPString text;
            sText.format(&text);

            ssize_t n_lines = 1 + text.count('\n');
            ssize_t dy      = sSize.nHeight - fp.Height*n_lines - (nBorder << 1);
            ssize_t y       = nBorder - fp.Descent + dy * fVAlign;

            // Estimate text size
            ssize_t last = 0, curr = 0, tail = 0, len = text.length();

            while (curr < len)
            {
                // Get next line indexes
                curr    = text.index_of(last, '\n');
                if (curr < 0)
                {
                    curr        = len;
                    tail        = len;
                }
                else
                {
                    tail        = curr;
                    if ((tail > last) && (text.at(tail-1) == '\r'))
                        --tail;
                }

                // Calculate text location
                sFont.get_text_parameters(s, &tp, &text, last, tail);
                ssize_t dx  = sSize.nWidth - tp.Width - (nBorder << 1);
                ssize_t x   = nBorder + dx * fHAlign - tp.XBearing;
                y          += fp.Height;

                sFont.draw(s, x, y, font_color, &text, last, tail);
                last    = curr + 1;

//                s->line(x - 3, y - 3, x + 3, y + 3, 1, red);
//                s->line(x + 3, y - 3, x - 3, y + 3, 1, red);
//
//                s->line(x - 3 + tp.XAdvance, y - 3, x + 3 + tp.XAdvance, y + 3, 1, yellow);
//                s->line(x + 3 + tp.XAdvance, y - 3, x - 3 + tp.XAdvance, y + 3, 1, yellow);
//                s->line(x - 3 + tp.XAdvance, y - 3, x + 3 + tp.XAdvance, y + 3, 1, yellow);
//                s->line(x + 3 + tp.XBearing, y - 3, x - 3 + tp.XBearing, y + 3, 1, yellow);
            }
        }

        void LSPLabel::size_request(size_request_t *r)
        {
            r->nMinWidth    = 0;
            r->nMinHeight   = 0;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;

            LSPString text;
            sText.format(&text);

            if (pDisplay == NULL)
                return;
            ISurface *s = pDisplay->create_surface(1, 1);
            if (s == NULL)
                return;

            text_parameters_t tp;
            font_parameters_t fp;
            sFont.get_parameters(s, &fp);
            sFont.get_multiline_text_parameters(s, &tp, &text);
            if (tp.Height < fp.Height)
                tp.Height   = fp.Height;

            r->nMinWidth    = tp.Width + (nBorder + 1) * 2;
            r->nMinHeight   = tp.Height + (nBorder + 1) * 2;
            if (fill())
            {
                r->nMaxWidth    = -1;
                r->nMaxHeight   = -1;
            }
            else
            {
                r->nMaxWidth    = r->nMinWidth;
                r->nMaxHeight   = r->nMinHeight;
            }

            s->destroy();
            delete s;
        }

        void LSPLabel::query_safe_resize()
        {
            size_request_t sr;
            size_request(&sr);
            if (sr.nMinWidth < 0)
                sr.nMinWidth    = sSize.nWidth;
            if (sr.nMinHeight < 0)
                sr.nMinHeight   = sSize.nHeight;

            if ((sr.nMinWidth > sSize.nWidth) || (sr.nMinHeight > sSize.nHeight))
                query_resize();
            else if ((((sSize.nWidth - sr.nMinWidth) > 4) || ((sSize.nHeight - sr.nMinHeight) > 4)) && (!fill()))
                query_resize();
            else
                query_draw();
        }

    } /* namespace tk */
} /* namespace lsp */
