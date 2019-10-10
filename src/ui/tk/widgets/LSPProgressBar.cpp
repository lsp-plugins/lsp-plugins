/*
 * LSPProgressBar.cpp
 *
 *  Created on: 2 июл. 2019 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPProgressBar::metadata = { "LSPProgressBar", &LSPWidget::metadata };

        LSPProgressBar::LSPProgressBar(LSPDisplay *dpy):
            LSPWidget(dpy),
            sFont(this),
            sColor(this),
            sSelColor(this)
        {
            fMin        = 0.0f;
            fMax        = 100.0f;
            fValue      = 50.0f;
            nMinWidth   = -1;
            nMinHeight  = -1;

            pClass      = &metadata;
        }
        
        LSPProgressBar::~LSPProgressBar()
        {
        }

        status_t LSPProgressBar::init()
        {
            status_t result = LSPWidget::init();
            if (result != STATUS_OK)
                return result;

            this->set_vfill(false);

            if (pDisplay != NULL)
            {
                // Get theme
                LSPTheme *theme = pDisplay->theme();
                if (theme != NULL)
                    sFont.init(theme->font());
            }

            init_color(C_BACKGROUND, sFont.color());
            init_color(C_LABEL_TEXT, &sColor);
            init_color(C_KNOB_SCALE, &sSelColor);

            return STATUS_OK;
        }
    
        status_t LSPProgressBar::set_text(const char *text)
        {
            if (!sText.set_utf8(text))
                return STATUS_NO_MEM;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPProgressBar::set_text(const LSPString *text)
        {
            if (!sText.set(text))
                return STATUS_NO_MEM;
            query_draw();
            return STATUS_OK;
        }

        bool LSPProgressBar::set_min_value(const float v)
        {
            if (fMin == v)
                return false;
            fMin    = v;
            query_draw();
            return true;
        }

        bool LSPProgressBar::set_max_value(const float v)
        {
            if (fMax == v)
                return false;
            fMax    = v;
            query_draw();
            return true;
        }

        bool LSPProgressBar::set_value(const float v)
        {
            if (fValue == v)
                return false;
            fValue  = v;
            query_draw();
            return true;
        }

        void LSPProgressBar::set_min_width(ssize_t value)
        {
            if (nMinWidth == value)
                return;
            nMinWidth = value;
            query_resize();
        }

        void LSPProgressBar::set_min_height(ssize_t value)
        {
            if (nMinHeight == value)
                return;
            nMinHeight = value;
            query_resize();
        }

        void LSPProgressBar::draw(ISurface *s)
        {
            size_t w    = sSize.nWidth;
            size_t h    = sSize.nHeight;
            size_t dw   = sSize.nWidth - 4;

            // Prepare palette
            Color bg_color(sBgColor);
            Color color(sColor);
            Color sel_color(sSelColor);
            Color font(sFont.raw_color());

            color.scale_lightness(brightness());
            sel_color.scale_lightness(brightness());
            font.scale_lightness(brightness());

            // Draw the entire control
            s->clear(bg_color);
            bool aa = s->set_antialiasing(true);
            s->fill_round_rect(0.5f, 0.5f, w-1, h-1, 4.0f, SURFMASK_ALL_CORNER, color);
            s->set_antialiasing(aa);

            font_parameters_t fp;
            text_parameters_t tp;
            if (!sText.is_empty())
            {
                sFont.get_parameters(s, &fp);
                sFont.get_text_parameters(s, &tp, &sText);
                sFont.draw(s, (w - tp.Width) * 0.5f, (h - fp.Height) * 0.5f + fp.Ascent, font, &sText);
            }

            // Need to draw the value over?
            float k = fabs(fValue / (fMax - fMin));
            if (k < 0.0f)
                k       = 0.0f;
            else if (k > 1.0f)
                k       = 1.0f;

            size_t pixels = dw * k;
            if (pixels > 0)
            {
                ISurface *xs = pDisplay->create_surface(pixels + 2, h);
                if (xs == NULL)
                    return;

                xs->clear(bg_color);
                aa = xs->set_antialiasing(true);
                xs->fill_round_rect(0.5f, 0.5f, w-1, h-1, 4.0f, SURFMASK_ALL_CORNER, sel_color);
                xs->set_antialiasing(aa);

                if (!sText.is_empty())
                    sFont.draw(xs, (w - tp.Width) * 0.5f, (h - fp.Height) * 0.5f + fp.Ascent, color, &sText);
                s->draw(xs, 0, 0);

                xs->destroy();
                delete xs;
            }

            // Finally, draw frames
            aa = s->set_antialiasing(true);
            s->wire_round_rect(1.5f, 1.5f, w - 3, h - 3, 4.0f, SURFMASK_ALL_CORNER, 1.0f, bg_color);
            s->wire_round_rect(0.5f, 0.5f, w - 1, h - 1, 4.0f, SURFMASK_ALL_CORNER, 1.0f, color);
            s->set_antialiasing(aa);
        }

        void LSPProgressBar::size_request(size_request_t *r)
        {
            LSPWidget::size_request(r);

            ssize_t h = sFont.height() + 4;

            ssize_t minw = (nMinWidth < 16) ? 16 : nMinWidth;
            if (h < nMinHeight)
                h           = nMinHeight;

            if (r->nMinWidth < minw)
                r->nMinWidth    = minw;
            if (r->nMinHeight < h)
                r->nMinHeight   = h;
            if ((r->nMaxWidth >= 0) && (r->nMaxWidth < r->nMinWidth))
                r->nMaxWidth    = r->nMinWidth;
            if ((r->nMaxHeight >= 0) && (r->nMaxHeight < r->nMinHeight))
                r->nMaxHeight   = r->nMinHeight;
        }
    } /* namespace tk */
} /* namespace lsp */
