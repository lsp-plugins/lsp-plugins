/*
 * LSPAlign.cpp
 *
 *  Created on: 17 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPAlign::metadata = { "LSPAlign", &LSPWidgetContainer::metadata };

        LSPAlign::LSPAlign(LSPDisplay *dpy): LSPWidgetContainer(dpy)
        {
            nVertPos        = 0.5f;
            nHorPos         = 0.5f;
            nVertScale      = 0.0f;
            nHorScale       = 0.0f;
            pWidget         = NULL;
            pClass          = &metadata;
        }

        LSPAlign::~LSPAlign()
        {
            do_destroy();
        }

        status_t LSPAlign::init()
        {
            status_t result = LSPWidgetContainer::init();
            if (result != STATUS_OK)
                return result;

            return STATUS_OK;
        }

        void LSPAlign::destroy()
        {
            do_destroy();
            LSPWidgetContainer::destroy();
        }

        void LSPAlign::do_destroy()
        {
            if (pWidget != NULL)
            {
                unlink_widget(pWidget);
                pWidget = NULL;
            }
        }

        LSPWidget *LSPAlign::find_widget(ssize_t x, ssize_t y)
        {
            if (pWidget == NULL)
                return NULL;

            return (pWidget->inside(x, y)) ? pWidget : NULL;
        }

        void LSPAlign::set_vpos(float value)
        {
            if (value < 0.0f)
                value = 0.0f;
            else if (value > 1.0f)
                value = 1.0f;
            if (nVertPos == value)
                return;
            nVertPos = value;
            query_draw();
        }

        void LSPAlign::set_hpos(float value)
        {
            if (value < 0.0f)
                value = 0.0f;
            else if (value > 1.0f)
                value = 1.0f;
            if (nHorPos == value)
                return;
            nHorPos = value;
            query_draw();
        }

        void LSPAlign::set_pos(float h, float v)
        {
            set_hpos(h);
            set_vpos(v);
        }

        void LSPAlign::set_vscale(float value)
        {
            if (value < 0.0f)
                value = 0.0f;
            else if (value > 1.0f)
                value = 1.0f;

            if (nVertScale == value)
                return;
            nVertScale = value;
            query_draw();
        }

        void LSPAlign::set_hscale(float value)
        {
            if (value < 0.0f)
                value = 0.0f;
            else if (value > 1.0f)
                value = 1.0f;

            if (nHorScale == value)
                return;
            nHorScale = value;
            query_draw();
        }

        void LSPAlign::set_scale(float h, float v)
        {
            set_hscale(h);
            set_vscale(v);
        }

        void LSPAlign::render(ISurface *s, bool force)
        {
            if (nFlags & REDRAW_SURFACE)
                force = true;

            // Initialize palette
            Color bg_color(sBgColor);

            // Draw background if child is invisible or not present
            if ((pWidget == NULL) || (!pWidget->visible()))
            {
                s->fill_rect(sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight, bg_color);
                return;
            }

            if ((force) || (pWidget->redraw_pending()))
            {
                if (force)
                {
                    realize_t r;
                    pWidget->get_dimensions(&r);
                    s->fill_frame(
                        sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight,
                        r.nLeft, r.nTop, r.nWidth, r.nHeight,
                        bg_color
                    );
                }

                pWidget->render(s, force);
                pWidget->commit_redraw();
            }
        }

        status_t LSPAlign::add(LSPWidget *widget)
        {
            if (pWidget != NULL)
                return STATUS_ALREADY_EXISTS;

            widget->set_parent(this);
            pWidget = widget;
            query_resize();
            return STATUS_OK;
        }

        status_t LSPAlign::remove(LSPWidget *widget)
        {
            if (pWidget != widget)
                return STATUS_NOT_FOUND;

            unlink_widget(pWidget);
            pWidget  = NULL;

            return STATUS_OK;
        }

        void LSPAlign::size_request(size_request_t *r)
        {
            if (pWidget == NULL)
            {
                r->nMinWidth        = -1;
                r->nMinHeight       = -1;
                r->nMaxWidth        = -1;
                r->nMaxHeight       = -1;
            }
            else
                pWidget->size_request(r);

            float  w            = 0;
            float  h            = 0;

            r->nMinWidth        = (r->nMinWidth < 0) ? w : w + r->nMinWidth;
            r->nMinHeight       = (r->nMinHeight < 0) ? h : h + r->nMinHeight;
            r->nMaxWidth        = -1;
            r->nMaxHeight       = -1;
        }

        void LSPAlign::realize(const realize_t *r)
        {
            LSPWidgetContainer::realize(r);

            if (pWidget == NULL)
                return;

            // Query for size
            size_request_t sr;
            sr.nMinWidth        = -1;
            sr.nMinHeight       = -1;
            sr.nMaxWidth        = -1;
            sr.nMaxHeight       = -1;
            pWidget->size_request(&sr);

            // Calculate realize parameters
            realize_t rc;

            // Dimensions
            ssize_t xs          = r->nWidth;
            ssize_t ys          = r->nHeight;
            rc.nWidth           = (sr.nMinWidth >= 0)   ? sr.nMinWidth  + (xs - sr.nMinWidth)   * nHorScale     : xs * nHorScale;
            rc.nHeight          = (sr.nMinHeight >= 0)  ? sr.nMinHeight + (ys - sr.nMinHeight)  * nVertScale    : ys * nVertScale;

            // Location
            if (rc.nWidth > xs)
                rc.nWidth           = xs;
            if (rc.nHeight > ys)
                rc.nHeight          = ys;
            xs             -= rc.nWidth;
            ys             -= rc.nHeight;
            rc.nLeft        = r->nLeft + xs * nHorPos;
            rc.nTop         = r->nTop + ys * nVertPos;

            // Call for realize
            pWidget->realize(&rc);
        }
    } /* namespace tk */
} /* namespace lsp */
