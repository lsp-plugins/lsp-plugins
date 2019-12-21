/*
 * LSPDot.cpp
 *
 *  Created on: 19 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPDot::metadata = { "LSPDot", &LSPGraphItem::metadata };

        LSPDot::LSPDot(LSPDisplay *dpy): LSPGraphItem(dpy),
            sColor(this)
        {
            sLeft.fMin          = 0.0f;
            sLeft.fMax          = 1.0f;
            sLeft.fValue        = 0.0f;
            sLeft.fLast         = 0.0f;
            sLeft.fBigStep      = 0.1f;
            sLeft.fStep         = 0.01f;
            sLeft.fTinyStep     = 0.001f;

            sTop.fMin           = 0.0f;
            sTop.fMax           = 1.0f;
            sTop.fValue         = 0.0f;
            sTop.fLast          = 0.0f;
            sTop.fBigStep       = 0.1f;
            sTop.fStep          = 0.01f;
            sTop.fTinyStep      = 0.001f;

            sScroll.fMin        = 0.0f;
            sScroll.fMax        = 1.0f;
            sScroll.fValue      = 0.0f;
            sScroll.fLast       = 0.0f;
            sScroll.fBigStep    = 0.1f;
            sScroll.fStep       = 0.01f;
            sScroll.fTinyStep   = 0.001f;

            nCenter     = 0;
            nFlags      = 0;
            nBasisID    = 0;
            nParallelID = 1;
            nRealX      = -1;
            nRealY      = -1;
            nMouseX     = -1;
            nMouseY     = -1;
            nDMouseX    = 0;
            nDMouseY    = 0;
            nMouseBtn   = 0;
            nBorder     = 12;
            nPadding    = 4;
            nSize       = 4;

            pClass          = &metadata;
        }

        LSPDot::~LSPDot()
        {
        }

        status_t LSPDot::init()
        {
            status_t result = LSPGraphItem::init();
            if (result != STATUS_OK)
                return result;

            init_color(C_GRAPH_MESH, &sColor);

            if (!sSlots.add(LSPSLOT_CHANGE))
                return STATUS_NO_MEM;

            return STATUS_OK;
        }

        void LSPDot::set_value(float *v, float value)
        {
            if (*v == value)
                return;
            *v = value;
            query_draw();
        }

        void LSPDot::set_limit_value(param_t *v, size_t flag, float value)
        {
            if (nFlags & flag)
                value       = limit_value(v, value);
            if (v->fValue == value)
                return;

            v->fValue   = value;
            query_draw();
        }

        void LSPDot::set_flag(size_t flag, bool value)
        {
            size_t flags = nFlags;
            if (value)
                nFlags      |= flag;
            else
                nFlags      &= ~flag;
            if (flags != nFlags)
                query_draw();
        }

        void LSPDot::set_size(size_t value)
        {
            if (nSize == value)
                return;
            nSize = value;
            query_draw();
        }

        void LSPDot::set_border(size_t value)
        {
            if (nBorder == value)
                return;
            nBorder = value;
            query_draw();
        }

        void LSPDot::set_padding(size_t value)
        {
            if (nPadding == value)
                return;
            nPadding = value;
            query_draw();
        }

        void LSPDot::set_basis_id(size_t value)
        {
            if (nBasisID == value)
                return;
            nBasisID    = value;
            query_draw();
        }

        void LSPDot::set_parallel_id(size_t value)
        {
            if (nParallelID == value)
                return;
            nParallelID = value;
            query_draw();
        }

        void LSPDot::set_center_id(size_t value)
        {
            if (nCenter == value)
                return;
            nCenter = value;
            query_draw();
        }

        float LSPDot::limit_value(const param_t *param, float value)
        {
            if (param->fMin < param->fMax)
            {
                if (value < param->fMin)
                    value   = param->fMin;
                else if (value > param->fMax)
                    value   = param->fMax;
            }
            else
            {
                if (value < param->fMax)
                    value   = param->fMax;
                else if (value > param->fMin)
                    value   = param->fMin;
            }
            return value;
        }

        void LSPDot::apply_motion(ssize_t x, ssize_t y)
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
            lsp_trace("xy=(%d, %d), mxy=(%d, %d), dxy=(%d, %d)",
                    int(x), int(y), int(nMouseX), int(nMouseY), int(nDMouseX), int(nDMouseY));

            float rx = x, ry = y;
            if (nFlags & F_FINE_TUNE)
            {
                float dx = x - nMouseX, dy = y - nMouseY;
                rx      = nMouseX - cv->canvas_left() - nDMouseX + 0.1f * dx;
                ry      = nMouseY - cv->canvas_top() - nDMouseY + 0.1f * dy;
            }
            else
            {
                rx  -= cv->canvas_left() + nDMouseX;
                ry  -= cv->canvas_top() + nDMouseY;
            }

            // Modify the value according to X coordinate
            bool changed = false;

            if (nFlags & F_X_EDITABLE)
            {
                float old       = sLeft.fValue;
                if (nMouseX == x)
                    sLeft.fValue    = sLeft.fLast;
                else if (basis != NULL)
                    sLeft.fValue    = basis->project(rx, ry);
                sLeft.fValue    = limit_value(&sLeft, sLeft.fValue);
                changed        |= bool(sLeft.fValue != old);
            }

            // Modify the value according to Y  coordinate
            if (nFlags & F_Y_EDITABLE)
            {
                float old       = sTop.fValue;
                if (nMouseY == y)
                    sTop.fValue     = sTop.fLast;
                else if (parallel != NULL)
                    sTop.fValue     = parallel->project(rx, ry);
                sTop.fValue     = limit_value(&sTop, sTop.fValue);
                changed        |= bool(sTop.fValue != old);
            }

            // Query widget for redraw
            if (changed)
                sSlots.execute(LSPSLOT_CHANGE, this);
            query_draw();
        }

        void LSPDot::render(ISurface *s, bool force)
        {
            if (!visible())
                return;

            // Get graph
            LSPGraph *cv = graph();
            if (cv == NULL)
                return;

            // Generate palette
            Color color(sColor);
            color.scale_lightness(brightness());

            // Prepare arguments
            float left  = sLeft.fValue;
            float top   = sTop.fValue;

            // Get axises
            LSPAxis *basis     = cv->axis(nBasisID);
            if (basis == NULL)
                return;
            LSPAxis *parallel  = cv->axis(nParallelID);
            if (parallel == NULL)
                return;

            // Locate the point at the center
            float x = 0.0f, y = 0.0f;
            cv->center(nCenter, &x, &y);

            // Translate point and get the owner line
            basis->apply(&x, &y, &left, 1);
            parallel->apply(&x, &y, &top, 1);

            // Store real coordinates
            nRealX  = x;
            nRealY  = y;

            x = truncf(x);
            y = truncf(y);

            // Draw the dot
            if (nFlags & (F_X_EDITABLE | F_Y_EDITABLE | F_Z_EDITABLE))
            {
                float radius = (nFlags & F_HIGHLIGHT) ? nBorder + nPadding : nBorder;
                bool aa = s->set_antialiasing(true);

                if (radius > 0.0f)
                {
                    Color c2(sColor, 0.9f);

                    IGradient *gr = s->radial_gradient(x, y, 0.0f, x, y, radius);
                    gr->add_color(0.0f, color);
                    gr->add_color(1.0f, c2);
                    s->fill_circle(x, y, radius, gr);

                    delete gr;
                }

                Color hole(0.0f, 0.0f, 0.0f);
                s->set_antialiasing(bSmooth);
                s->fill_circle(x, y, nSize, hole);
                s->fill_circle(x, y, nSize-1, color);
                s->set_antialiasing(aa);
            }
            else
            {
                bool aa = s->set_antialiasing(true);
                if (nFlags & F_HIGHLIGHT)
                {
                    Color c1(sColor), c2(sColor);
                    c2.alpha(0.9);

                    if (nBorder > 0)
                    {
                        Color c2(sColor, 0.9f);

                        IGradient *gr = s->radial_gradient(x, y, 0.0f, x, y, nBorder);
                        gr->add_color(0.0f, color);
                        gr->add_color(1.0f, c2);
                        s->fill_circle(x, y, nBorder, gr);

                        delete gr;
                    }

                    Color hole(0.0f, 0.0f, 0.0f);
                    s->set_antialiasing(bSmooth);
                    s->fill_circle(x, y, nSize, hole);
                }

                s->set_antialiasing(bSmooth);
                s->fill_circle(x, y, nSize-1, color);
                s->set_antialiasing(aa);
            }
        }

        bool LSPDot::inside(ssize_t x, ssize_t y)
        {
            if (!visible())
                return false;
            else if (!(nFlags & (F_X_EDITABLE | F_Y_EDITABLE | F_Z_EDITABLE)))
                return false;

            // Get graph
            LSPGraph *cv = graph();
            if (cv == NULL)
                return false;

    //        lsp_trace("x=%d, y=%d, cv_left=%d, cv_top=%d, real_x=%d, real_y=%d",
    //                int(x), int(y), int(cv->canvasLeft()), int(cv->canvasTop()), int(nRealX), int(nRealY)
    //               );

            float dx    = x - cv->canvas_left() - nRealX;
            float dy    = y - cv->canvas_top() - nRealY;
            float R     = nSize;

    //        lsp_trace("dx=%f, dy=%f", dx, dy);

            return (dx*dx + dy*dy) <= R*R;
        }

        status_t LSPDot::on_mouse_down(const ws_event_t *e)
        {
            if (nMouseBtn == 0)
            {
                if (!inside(e->nLeft, e->nTop))
                    return STATUS_OK;

                if ((e->nCode == MCB_LEFT) || (e->nCode == MCB_RIGHT))
                {
                    nMouseX     = e->nLeft;
                    nMouseY     = e->nTop;

                    LSPGraph *cv = graph();
                    nDMouseX    = (cv != NULL) ? nMouseX - cv->canvas_left() - nRealX : 0;
                    nDMouseY    = (cv != NULL) ? nMouseY - cv->canvas_top()  - nRealY : 0;
                    lsp_trace("dmouse = (%d, %d)", int(nDMouseX), int(nDMouseY));
                    sLeft.fLast = sLeft.fValue;
                    sTop.fLast  = sTop.fValue;
                    nFlags     |= F_EDITING;
                    if (e->nCode == MCB_RIGHT)
                        nFlags     |= F_FINE_TUNE;
                }
            }

            nMouseBtn  |= 1 << e->nCode;

            size_t bflag    = (nFlags & F_FINE_TUNE) ? (1 << MCB_RIGHT) : (1 << MCB_LEFT);
            if (nMouseBtn == bflag)
                apply_motion(e->nLeft, e->nTop);
            else
                apply_motion(nMouseX, nMouseY);

            return STATUS_OK;
        }

        status_t LSPDot::on_mouse_up(const ws_event_t *e)
        {
            if ((!(nFlags & F_EDITING)) || (nMouseBtn == 0))
                return STATUS_OK;

            size_t button   = 1 << e->nCode;
            size_t bflag    = (nFlags & F_FINE_TUNE) ? (1 << MCB_RIGHT) : (1 << MCB_LEFT);

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

                nFlags     &= ~F_FINE_TUNE;
            }

            return STATUS_OK;
        }

        status_t LSPDot::on_mouse_move(const ws_event_t *e)
        {
            if (nMouseBtn == 0)
                return STATUS_OK;

            size_t bflag    = (nFlags & F_FINE_TUNE) ? (1 << MCB_RIGHT) : (1 << MCB_LEFT);

            if (nMouseBtn == bflag)
                apply_motion(e->nLeft, e->nTop);
            else
                apply_motion(nMouseX, nMouseY);

            return STATUS_OK;
        }

        status_t LSPDot::on_mouse_in(const ws_event_t *e)
        {
            lsp_trace("mouse in");
            nFlags      |= F_HIGHLIGHT;
            query_draw();

            return LSPGraphItem::on_mouse_in(e);
        }

        status_t LSPDot::on_mouse_out(const ws_event_t *e)
        {
            nFlags      &= ~F_HIGHLIGHT;
            query_draw();

            return STATUS_OK;
        }

        status_t LSPDot::on_mouse_scroll(const ws_event_t *e)
        {
            // Process generic mouse event
            if (!inside(e->nLeft, e->nTop))
                return STATUS_OK;
            else if (!(nFlags & F_Z_EDITABLE))
                return STATUS_OK;

            float delta = 0.0f;
            if ((e->nCode == MCD_UP) || (e->nCode == MCD_DOWN))
            {
                delta   = (e->nState & MCF_SHIFT)   ? sScroll.fTinyStep :
                          (e->nState & MCF_CONTROL) ? sScroll.fBigStep : sScroll.fStep;
                if (e->nCode == MCD_DOWN)
                    delta   = - delta;
            }
            else
                return STATUS_OK;

            // Update value
            sScroll.fValue  = limit_value(&sScroll, sScroll.fValue + delta);

            // Notify about changes
            sSlots.execute(LSPSLOT_CHANGE, this);
            query_draw();

            return STATUS_OK;
        }
    } /* namespace tk */
} /* namespace lsp */
