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
            nBorder     = 0;
            pClass      = &metadata;

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

        void LSPMarker::set_border(ssize_t value)
        {
            if (nBorder == value)
                return;
            nBorder = value;
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

            // Translate point and get the coordinates of point that lays on the target line
            if (!basis->apply(&x, &y, &fValue, 1))
                return;
            if (fOffset != 0.0f)
            {
                if (!parallel->apply(&x, &y, &fOffset, 1))
                    return;
            }

            // Get equation of the line that contains calculated point
            float a, b, c;
            float nx, ny;
            float a2, b2, c2;

            if (fAngle == 0.0f)
            {
                if (!parallel->parallel(x, y, a, b, c))
                    return;
                if (nBorder != 0)
                {
                    parallel->ortogonal_shift(x, y, nBorder, nx, ny);
                    if (!parallel->parallel(nx, ny, a2, b2, c2))
                        return;
                }
            }
            else
            {
                if (!parallel->angle(x, y, fAngle * M_PI, a, b, c))
                    return;
                if (nBorder != 0)
                {
                    parallel->rotate_shift(x, y, fAngle * M_PI, nBorder, nx, ny);
                    if (!parallel->angle(x, y, fAngle * M_PI, a2, b2, c2))
                        return;
                }
            }

            // Draw line
            bool aa = s->set_antialiasing(bSmooth);
            Color col(sColor, 0.0f);

            if (nBorder != 0)
            {
                IGradient *g = s->linear_gradient(x, y, nx, ny);
                if (g != NULL)
                {
                    g->add_color(0.0f, sColor, 0.25f  + 0.5f * (1.0f - sColor.alpha()));
                    g->add_color(1.0f, sColor, 1.0f);

                    s->parametric_bar(
                        a, b, c, a2, b2, c2,
                        cv->area_left(), cv->area_right(), cv->area_top(), cv->area_bottom(),
                        g
                    );
                    s->parametric_line(a, b, c, cv->area_left(), cv->area_right(), cv->area_top(), cv->area_bottom(), nWidth, col);
//                    s->parametric_line(a2, b2, c2, cv->area_left(), cv->area_right(), cv->area_top(), cv->area_bottom(), nWidth, col);

                    delete g;
                }
            }
            else {
                s->parametric_line(a, b, c, cv->area_left(), cv->area_right(), cv->area_top(), cv->area_bottom(), nWidth, col);
            }
            s->set_antialiasing(aa);
        }
    } /* namespace tk */
} /* namespace lsp */
