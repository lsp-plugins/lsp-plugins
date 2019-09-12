/*
 * LSPLed.cpp
 *
 *  Created on: 10 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPLed::metadata = { "LSPLed", &LSPWidget::metadata };

        LSPLed::LSPLed(LSPDisplay *dpy):
            LSPWidget(dpy),
            sColor(this)
        {
            nSize       = 8;
            bOn         = false;
            pClass      = &metadata;
        }

        LSPLed::~LSPLed()
        {
        }

        status_t LSPLed::init()
        {
            status_t result = LSPWidget::init();
            if (result != STATUS_OK)
                return result;

            override_color(C_GREEN, &sColor);

            return STATUS_OK;
        }

        void LSPLed::set_on(bool on)
        {
            if (bOn == on)
                return;
            bOn   = on;
            query_draw();
        }

        void LSPLed::set_off(bool off)
        {
            bool on = ! off;
            if (bOn == on)
                return;
            bOn   = on;
            query_draw();
        }

        void LSPLed::set_size(size_t size)
        {
            if (nSize == size)
                return;

            nSize = size;
            query_resize();
        }

        void LSPLed::draw(ISurface *s)
        {
            IGradient *cp;

            // Draw background
            s->fill_rect(0, 0, sSize.nWidth, sSize.nHeight, sBgColor);

            // Move to center of the led
            ssize_t cx = (sSize.nWidth >> 1);
            ssize_t cy = (sSize.nHeight >> 1);

            // Draw hole
            Color hole;
            pDisplay->theme()->get_color(C_HOLE, &hole);

            s->fill_circle(cx, cy, (nSize >> 1) + 1, hole);

            Color col(sColor);

            bool aa = s->set_antialiasing(true);

            if (bOn)
            {
                // Draw light
                cp = s->radial_gradient(cx, cy, 0, cx, cy, nSize);
                cp->add_color(0.0, col, 0.5f);
                cp->add_color(1.0, col, 1.0f);
                s->fill_circle(cx, cy, nSize, cp);
                delete cp;

                // Draw led spot
                Color c_light(col);
                c_light.lightness(c_light.lightness() * 1.5);

                cp = s->radial_gradient(cx, cy, nSize >> 3, cx, cy, nSize >> 1);
                cp->add_color(0.0f, c_light);
                cp->add_color(1.0f, col);
                s->fill_circle(cx, cy, nSize >> 1, cp);
                delete cp;

                // Add blink
                cp = s->radial_gradient(cx + (nSize >> 3), cy - ssize_t(nSize >> 3), 0, cx, cy, nSize >> 1);
                cp->add_color(0.0, 1.0, 1.0, 1.0, 0.0f);
                cp->add_color(1.0, 1.0, 1.0, 1.0, 1.0f);
                s->fill_circle(cx, cy, (nSize >> 1) - 1, cp);
                delete cp;
            }
            else
            {
                Color c;
                pDisplay->theme()->get_color(C_GLASS, &c);

                float r=c.red() + (col.red() - c.red()) * 0.4;
                float g=c.green() + (col.green() - c.green()) * 0.4;
                float b=c.blue() + (col.blue() - c.blue()) * 0.4;

                // Draw led glass
                cp = s->radial_gradient(cx, cy, nSize >> 3, cx, cy, nSize >> 1);
                cp->add_color(0.0, r, g, b);
                cp->add_color(1.0, c);
                s->fill_circle(cx, cy, (nSize >> 1)+1, cp);
                delete cp;

                // Add blink
                cp = s->radial_gradient(cx + (nSize >> 3), cy - ssize_t(nSize >> 3), cx, cy, 0, nSize >> 1);
                cp->add_color(0.0, 1.0, 1.0, 1.0, 0.8);
                cp->add_color(1.0, 1.0, 1.0, 1.0, 1.0);
                s->fill_circle(cx, cy, (nSize >> 1) - 1, cp);
                delete cp;
            }

            s->set_antialiasing(aa);
        }

        void LSPLed::size_request(size_request_t *r)
        {
            r->nMinWidth        = (nSize << 1);
            r->nMinHeight       = (nSize << 1);
            r->nMaxWidth        = r->nMinWidth;
            r->nMaxHeight       = r->nMinHeight;
        }
    } /* namespace tk */
} /* namespace lsp */
