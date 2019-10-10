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
            sColor(this),
            sHoleColor(this)
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

            sHoleColor.bind("hole_color");
            init_color(C_GREEN, &sColor);

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

            // Estimate palette
            Color bg_color(sBgColor);
            Color hole(sHoleColor);
            Color col(sColor);
            Color glass(sGlassColor);

            col.scale_lightness(brightness());
            glass.scale_lightness(brightness());

            // Draw background
            s->fill_rect(0, 0, sSize.nWidth, sSize.nHeight, bg_color);

            // Move to center of the led
            ssize_t cx = (sSize.nWidth >> 1);
            ssize_t cy = (sSize.nHeight >> 1);

            // Draw hole
            s->fill_circle(cx, cy, (nSize >> 1) + 1, hole);

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
                c.blend(col, glass, 0.4);

                // Draw led glass
                cp = s->radial_gradient(cx, cy, nSize >> 3, cx, cy, nSize >> 1);
                cp->add_color(0.0, col);
                cp->add_color(1.0, glass);
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
