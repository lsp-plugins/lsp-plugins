/*
 * LSPMarker.cpp
 *
 *  Created on: 19 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPMarker::metadata = { "LSPMarker", &LSPGraphItem::metadata };

        LSPMarker::LSPMarker(LSPDisplay *dpy): LSPGraphItem(dpy)
        {
            nBasisID    = 0;
            nParallelID = 1;
            fValue      = 0.0f;
            fOffset     = 0.0f;
            fAngle      = 0.0f;
            nWidth      = 1;
            nCenter     = 0;
            pClass          = &metadata;

            set_smooth(false);
        }

        LSPMarker::~LSPMarker()
        {
        }

        status_t LSPMarker::init()
        {
            status_t result = LSPGraphItem::init();
            if (result != STATUS_OK)
                return result;

            init_color(C_GRAPH_MARKER, &sColor);
            return STATUS_OK;
        }

        void LSPMarker::set_basis_id(size_t value)
        {
            if (nBasisID == value)
                return;
            nBasisID = value;
            query_draw();
        }

        void LSPMarker::set_parallel_id(size_t value)
        {
            if (nParallelID == value)
                return;
            nParallelID = value;
            query_draw();
        }

        void LSPMarker::set_value(float value)
        {
            if (fValue == value)
                return;
            fValue = value;
            query_draw();
        }

        void LSPMarker::set_offset(float value)
        {
            if (fOffset == value)
                return;
            fOffset = value;
            query_draw();
        }

        void LSPMarker::set_angle(float value)
        {
            if (fAngle == value)
                return;
            fAngle = value;
            query_draw();
        }

        void LSPMarker::set_width(size_t value)
        {
            if (nWidth == value)
                return;
            nWidth = value;
            query_draw();
        }

        void LSPMarker::set_center(size_t value)
        {
            if (nCenter == value)
                return;
            nCenter = value;
            query_draw();
        }

        void LSPMarker::render(ISurface *s, bool force)
        {
            // Get graph
            LSPGraph *cv        = graph();
            if (cv == NULL)
                return;

            // Get basis
            LSPAxis *basis      = cv->axis(nBasisID);
            if (basis == NULL)
                return;
            LSPAxis *parallel   = cv->axis(nParallelID);
            if (parallel == NULL)
                return;

            float x = 0.0f, y = 0.0f;
            cv->center(nCenter, &x, &y);

            // Translate point and get the owner line
            if (!basis->apply(&x, &y, &fValue, 1))
                return;
            if (fOffset != 0.0f)
            {
                if (!parallel->apply(&x, &y, &fOffset, 1))
                    return;
            }

            float a, b, c;
            if (fAngle == 0.0f)
            {
                if (!parallel->parallel(x, y, a, b, c))
                    return;
            }
            else
            {
                if (!parallel->angle(x, y, fAngle * M_PI, a, b, c))
                    return;
            }

            // Draw line
            bool aa = s->set_antialiasing(bSmooth);
            s->parametric_line(a, b, c, cv->area_left(), cv->area_right(), cv->area_top(), cv->area_bottom(), nWidth, sColor);
            s->set_antialiasing(aa);
        }
    } /* namespace tk */
} /* namespace lsp */
