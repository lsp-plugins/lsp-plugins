/*
 * LSPCenter.cpp
 *
 *  Created on: 19 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPCenter::metadata = { "LSPCenter", &LSPGraphItem::metadata };

        LSPCenter::LSPCenter(LSPDisplay *dpy): LSPGraphItem(dpy),
            sColor(this)
        {
            fLeft       = 0.0f;
            fTop        = 0.0f;
            fRadius     = 4.0f;
            pClass          = &metadata;
        }

        LSPCenter::~LSPCenter()
        {
        }

        status_t LSPCenter::init()
        {
            status_t result = LSPGraphItem::init();
            if (result != STATUS_OK)
                return result;

            set_visible(false);
            init_color(C_GRAPH_AXIS, &sColor);

            return STATUS_OK;
        }

        void LSPCenter::set_radius(float value)
        {
            if (fRadius == value)
                return;
            fRadius = value;
            query_draw();
        }

        void LSPCenter::set_canvas_left(float value)
        {
            if (fLeft == value)
                return;
            fLeft = value;
            query_draw();
        }

        void LSPCenter::set_canvas_top(float value)
        {
            if (fTop == value)
                return;
            fTop = value;
            query_draw();
        }

        void LSPCenter::render(ISurface *s, bool force)
        {
            // Get graph
            LSPGraph *cv = graph();
            if (cv == NULL)
                return;

            // Generate palette
            Color color(sColor);
            color.scale_lightness(brightness());

            // Draw circle
            float x=0.0, y=0.0;
            cv->center(this, &x, &y);
            bool aa = s->set_antialiasing(bSmooth);
            s->fill_circle(x, y, fRadius, color);
            s->set_antialiasing(aa);
        }
    } /* namespace tk */
} /* namespace lsp */
