/*
 * LSPMarker.cpp
 *
 *  Created on: 19 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <ui/graphics.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPMarker::metadata = { "LSPMarker", &LSPGraphItem::metadata };

        LSPMarker::LSPMarker(LSPDisplay *dpy): LSPGraphItem(dpy),
            sColor(this)
        {
            nBasisID    = 0;
            nParallelID = 1;
            fValue      = 0.0f;
            fLast       = 0.0f;
            fOffset     = 0.0f;
            fAngle      = 0.0f;
            fDX         = 1.0f;
            fDY         = 0.0f;
            fMin        = -1.0f;
            fMax        = 1.0f;
            nWidth      = 1;
            nCenter     = 0;
            nBorder     = 0;
            pClass      = &metadata;
            nXFlags     = 0;
            nMouseX     = 0; // debug
            nMouseY     = 0; // debug
//            nDMouseX    = 0; // debug
//            nDMouseY    = 0; // debug
            nMouseBtn   = 0;

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

            if (!sSlots.add(LSPSLOT_CHANGE))
                return STATUS_NO_MEM;

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
            fDX     = cosf(value);
            fDY     = sinf(value);
            fAngle  = value;
            query_draw();
        }

        void LSPMarker::set_direction(float dx, float dy)
        {
            fDX         = dx;
            fDY         = dy;
            fAngle      = get_angle_2d(0.0f, 0.0f, dx, dy);

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

        void LSPMarker::set_editable(bool value)
        {
            size_t flags = nXFlags;
            if (value)
                nXFlags     |= F_EDITABLE;
            else
                nXFlags     &= ~F_EDITABLE;
            if (flags != nXFlags)
                query_draw();
        }

        void LSPMarker::set_minimum(float value)
        {
            if (fMin == value)
                return;
            fMin = value;
            query_draw();
        }

        void LSPMarker::set_maximum(float value)
        {
            if (fMax == value)
                return;
            fMax = value;
            query_draw();
        }

        void LSPMarker::render(ISurface *s, bool force)
        {
            // Get graph
            LSPGraph *cv        = graph();
            if (cv == NULL)
                return;

            // Prepare palette
            Color color(sColor);
            color.scale_lightness(brightness());

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
                if (!parallel->angle(x, y, fAngle, a, b, c))
                    return;
                if (nBorder != 0)
                {
                    parallel->rotate_shift(x, y, fAngle, nBorder, nx, ny);
                    if (!parallel->angle(x, y, fAngle, a2, b2, c2))
                        return;
                }
            }

            // Draw line
            bool aa = s->set_antialiasing(bSmooth);
            Color col(sColor, 0.0f);

            ssize_t l_width = nWidth;
            if (nXFlags & F_HIGHLIGHT)
                l_width += 2;

            if (nBorder != 0)
            {
                IGradient *g = s->linear_gradient(x, y, nx, ny);
                if (g != NULL)
                {
                    g->add_color(0.0f, color, 0.25f  + 0.5f * (1.0f - color.alpha()));
                    g->add_color(1.0f, color, 1.0f);

                    s->parametric_bar(
                        a, b, c, a2, b2, c2,
                        cv->area_left(), cv->area_right(), cv->area_top(), cv->area_bottom(),
                        g
                    );
                    s->parametric_line(a, b, c, cv->area_left(), cv->area_right(), cv->area_top(), cv->area_bottom(), l_width, col);
//                    s->parametric_line(a2, b2, c2, cv->area_left(), cv->area_right(), cv->area_top(), cv->area_bottom(), nWidth, col);

                    delete g;
                }
            }
            else {
                s->parametric_line(a, b, c, cv->area_left(), cv->area_right(), cv->area_top(), cv->area_bottom(), l_width, col);
            }
            s->set_antialiasing(aa);

            // Debug
//            if (nXFlags & F_EDITABLE)
//            {
//                float a1, b1, c1;
//                float a2, b2, c2;
//                float mx = nDMouseX, my = nDMouseY;
//                float nx, ny;
//
//                if (!parallel->parallel(x, y, a1, b1, c1))
//                    return;
//                if (!basis->parallel(mx, my, a2, b2, c2))
//                    return;
//                if (!line2d_intersection(a1, b1, c1, a2, b2, c2, nx, ny))
//                    return;
//
//                s->line(nx, ny, mx, my, 2, col);
//            }
        }

        bool LSPMarker::inside(ssize_t mx, ssize_t my)
        {
            if (!(nXFlags & F_EDITABLE))
                return false;

            // Get graph
            LSPGraph *cv = graph();
            if (cv == NULL)
                return false;

            mx     -= cv->canvas_left();
            my     -= cv->canvas_top();

//            nDMouseX = mx; // DEBUG
//            nDMouseY = my; // DEBUG
//            query_draw(); // DEBUG

            // Get basis
            LSPAxis *basis      = cv->axis(nBasisID);
            if (basis == NULL)
                return false;
            LSPAxis *parallel   = cv->axis(nParallelID);
            if (parallel == NULL)
                return false;

            float x = 0.0f, y = 0.0f;
            cv->center(nCenter, &x, &y);

            // Translate point and get the coordinates of point that lays on the target line
            if (!basis->apply(&x, &y, &fValue, 1))
                return false;
            if (fOffset != 0.0f)
            {
                if (!parallel->apply(&x, &y, &fOffset, 1))
                    return false;
            }

            // Get equation of the line that contains calculated point
            float a1, b1, c1;
            float a2, b2, c2;
            float nx, ny;
//            ssize_t border = (nBorder > 0) ? nBorder : -nBorder;
//            if (border > 3)
//                border

            if (!parallel->parallel(x, y, a1, b1, c1))
                return false;
            if (!basis->parallel(mx, my, a2, b2, c2))
                return false;
            if (!line2d_intersection(a1, b1, c1, a2, b2, c2, nx, ny))
                return false;

            return distance2d(nx, ny, mx, my) <= 3.0f;
        }

        status_t LSPMarker::on_mouse_in(const ws_event_t *e)
        {
            nXFlags |= F_HIGHLIGHT;
            query_draw();

            if (!(nXFlags & F_EDITABLE))
                return STATUS_OK;

            LSPGraph *cv = graph();
            if (cv == NULL)
                return STATUS_OK;
            LSPAxis *basis      = cv->axis(nBasisID);
            if (basis == NULL)
                return STATUS_OK;

            float x = 0.0f, y = 0.0f;
            if (!basis->apply(&x, &y, &fValue, 1))
                return STATUS_OK;

            if (fabs(x) > fabs(y))
                set_cursor(MP_HSIZE);
            else
                set_cursor(MP_VSIZE);

            return LSPGraphItem::on_mouse_in(e);
        }

        status_t LSPMarker::on_mouse_out(const ws_event_t *e)
        {
            nXFlags &= ~F_HIGHLIGHT;
            query_draw();
            lsp_trace("this = %p", this);
            return STATUS_OK;
        }

        status_t LSPMarker::on_mouse_down(const ws_event_t *e)
        {
            if (nMouseBtn == 0)
            {
                if (!inside(e->nLeft, e->nTop))
                    return STATUS_OK;

                if ((e->nCode == MCB_LEFT) || (e->nCode == MCB_RIGHT))
                {
                    nMouseX     = e->nLeft;
                    nMouseY     = e->nTop;

//                    LSPGraph *cv = graph();
//                    nDMouseX    = (cv != NULL) ? nMouseX - cv->canvas_left() : 0;
//                    nDMouseY    = (cv != NULL) ? nMouseY - cv->canvas_top() : 0;
//                    lsp_trace("dmouse = (%d, %d)", int(nDMouseX), int(nDMouseY));
                    fLast       = fValue;
                    nXFlags    |= F_EDITING;
                    if (e->nCode == MCB_RIGHT)
                        nXFlags    |= F_FINE_TUNE;
                }
            }

            nMouseBtn  |= 1 << e->nCode;

            size_t bflag    = (nXFlags & F_FINE_TUNE) ? (1 << MCB_RIGHT) : (1 << MCB_LEFT);
            if (nMouseBtn == bflag)
                apply_motion(e->nLeft, e->nTop);
            else
                apply_motion(nMouseX, nMouseY);

            return STATUS_OK;
        }

        status_t LSPMarker::on_mouse_up(const ws_event_t *e)
        {
            if ((!(nXFlags & F_EDITING)) || (nMouseBtn == 0))
                return STATUS_OK;

            size_t button   = 1 << e->nCode;
            size_t bflag    = (nXFlags & F_FINE_TUNE) ? (1 << MCB_RIGHT) : (1 << MCB_LEFT);

            nMouseBtn      &= ~button;

            if (nMouseBtn != 0)
            {
                if (nMouseBtn == bflag)
                    apply_motion(e->nLeft, e->nTop);
                else
                    apply_motion(nMouseX, nMouseY);
            }
            else
            {
                if (button == bflag)
                    apply_motion(e->nLeft, e->nTop);
                else
                    apply_motion(nMouseX, nMouseY);

                nXFlags    &= ~F_FINE_TUNE;
            }

            return STATUS_OK;
        }

        status_t LSPMarker::on_mouse_move(const ws_event_t *e)
        {
            if (nMouseBtn == 0)
                return STATUS_OK;

            size_t bflag    = (nXFlags & F_FINE_TUNE) ? (1 << MCB_RIGHT) : (1 << MCB_LEFT);

            if (nMouseBtn == bflag)
                apply_motion(e->nLeft, e->nTop);
            else
                apply_motion(nMouseX, nMouseY);

            return STATUS_OK;
        }

        void LSPMarker::apply_motion(ssize_t x, ssize_t y)
        {
            // Get graph
            LSPGraph *cv = graph();
            if (cv == NULL)
                return;

            // Ignore canvas coordinates

            // Get axises
            LSPAxis *basis      = cv->axis(nBasisID);
            if (basis == NULL)
                return;
            LSPAxis *parallel   = cv->axis(nParallelID);
            if (parallel == NULL)
                return;

            // Update the difference relative to the sensitivity
            lsp_trace("xy=(%d, %d), mxy=(%d, %d)",
                    int(x), int(y), int(nMouseX), int(nMouseY));

            float rx, ry;
            if (nXFlags & F_FINE_TUNE)
            {
                float dx = x - nMouseX, dy = y - nMouseY;
                rx      = nMouseX - cv->canvas_left() + 0.1f * dx;
                ry      = nMouseY - cv->canvas_top() + 0.1f * dy;
            }
            else
            {
                rx      = x - cv->canvas_left();
                ry      = y - cv->canvas_top();
            }

            lsp_trace("rxy=(%f, %f)", rx, ry);

            // Modify the value according to X coordinate
            if ((rx != 0.0f) && (ry != 0.0f))
                lsp_trace("debug");

            float old       = fValue;
            if ((nMouseX == x) && (nMouseY == y))
                fValue          = fLast;
            else if (basis != NULL)
                fValue          = basis->project(rx, ry);
            fValue          = limit_value(fValue);

            // Query widget for redraw
            if (fValue != old)
                sSlots.execute(LSPSLOT_CHANGE, this);
            query_draw();
        }

        float LSPMarker::limit_value(float value)
        {
            if (fMin < fMax)
            {
                if (value < fMin)
                    value   = fMin;
                else if (value > fMax)
                    value   = fMax;
            }
            else
            {
                if (value < fMax)
                    value   = fMax;
                else if (value > fMin)
                    value   = fMin;
            }
            return value;
        }
    } /* namespace tk */
} /* namespace lsp */
