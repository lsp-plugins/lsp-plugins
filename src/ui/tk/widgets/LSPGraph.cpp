/*
 * LSPGraph.cpp
 *
 *  Created on: 18 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <ui/tk/helpers/draw.h>
#include <time.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPGraph::metadata = { "LSPGraph", &LSPWidgetContainer::metadata };

        LSPGraph::LSPGraph(LSPDisplay *dpy):
            LSPWidgetContainer(dpy),
            sIPadding(this),
            sColor(this)
        {
            nMinWidth       = 0;
            nMinHeight      = 0;
            nBorder         = 12;
            nRadius         = 4;
            fCanvasLeft     = 0.0f;
            fCanvasTop      = 0.0f;
            fCanvasWidth    = 0.0f;
            fCanvasHeight   = 0.0f;
            pCanvas         = NULL;
            pGlass          = NULL;
            pClass          = &metadata;

            sIPadding.set(1, 1, 1, 1);

            #ifdef LSP_TRACE
            sClock.tv_sec   = 0;
            sClock.tv_nsec  = 0;
            nFrames         = 0;
            #endif /* LSP_TRACE */
        }

        LSPGraph::~LSPGraph()
        {
            do_destroy();
        }

        status_t LSPGraph::init()
        {
            status_t result = LSPWidgetContainer::init();
            if (result != STATUS_OK)
                return result;

            init_color(C_GLASS, &sColor);

            return STATUS_OK;
        }

        void LSPGraph::do_destroy()
        {
            size_t count = vObjects.size();
            for (size_t i=0; i<count; ++i)
                unlink_widget(vObjects.at(i));

            vObjects.flush();
            vAxises.flush();
            vBasises.flush();
            vCenters.flush();

            if (pCanvas != NULL)
            {
                pCanvas->destroy();
                delete pCanvas;
                pCanvas = NULL;
            }
            if (pGlass != NULL)
            {
                pGlass->destroy();
                delete pGlass;
                pGlass = NULL;
            }
        }

        void LSPGraph::destroy()
        {
            do_destroy();
            LSPWidgetContainer::destroy();
        }

        ISurface *LSPGraph::get_canvas(ISurface *s, ssize_t w, ssize_t h, const Color & color)
        {
            // Check surface
            if (pCanvas != NULL)
            {
                if ((w != ssize_t(pCanvas->width())) || (h != ssize_t(pCanvas->height())))
                {
                    pCanvas->destroy();
                    delete pCanvas;
                    pCanvas    = NULL;
                }
            }

            // Create new surface if needed
            if (pCanvas == NULL)
            {
                if (s == NULL)
                    return NULL;
                pCanvas        = s->create(w, h);
                if (pCanvas == NULL)
                    return NULL;
            }

            // Clear canvas
            pCanvas->clear(color);

            // Draw all objects
            size_t n_objects = vObjects.size();
            for (size_t i=0; i<n_objects; ++i)
            {
                LSPGraphItem *obj = vObjects.at(i);
                if ((obj == NULL) || (!obj->visible()))
                    continue;
                obj->render(pCanvas, true);
                obj->commit_redraw();
            }

            return pCanvas;
        }

        LSPWidget *LSPGraph::find_widget(ssize_t x, ssize_t y)
        {
            size_t n = vObjects.size();
            for (size_t i=0; i<n; ++i)
            {
                LSPGraphItem *obj = vObjects.at(i);
                if (obj == NULL)
                    continue;
                if (obj->inside(x, y))
                    return obj;
            }
            return NULL;
        }

        bool LSPGraph::center(size_t index, float *x, float *y)
        {
            return center(vCenters.get(index), x, y);
        }

        bool LSPGraph::center(LSPCenter *c, float *x, float *y)
        {
            if ((c == NULL) || (pCanvas == NULL))
            {
                *x      = 0.0f;
                *y      = 0.0f;
                return false;
            }

            float c_width   = ssize_t(pCanvas->width()) - ssize_t(sIPadding.horizontal()) - 2;
            float c_height  = ssize_t(pCanvas->height()) - ssize_t(sIPadding.vertical()) - 2;

            *x  = area_left() + float(sIPadding.left()) + (c->canvas_left() + 1.0f) * c_width * 0.5f;
            *y  = area_bottom() + float(sIPadding.top()) + (1.0f - c->canvas_top()) * c_height * 0.5f;
            return true;
        }

        size_t LSPGraph::get_axes(LSPAxis **dst, size_t start, size_t count)
        {
            size_t n_count = vAxises.size();

            for (size_t i=0; i<count; ++i)
            {
                if (start >= n_count)
                    return i;
                dst[i]      = vAxises.at(start++);
            }

            return count;
        }

        size_t LSPGraph::get_basis_axes(LSPAxis **dst, size_t start, size_t count)
        {
            size_t n_count = vBasises.size();

            for (size_t i=0; i<count; ++i)
            {
                if (start >= n_count)
                    return i;
                dst[i]      = vBasises.at(start++);
            }

            return count;
        }

        size_t LSPGraph::get_items(LSPGraphItem **dst, size_t start, size_t count)
        {
            size_t n_count = vObjects.size();

            for (size_t i=0; i<count; ++i)
            {
                if (start >= n_count)
                    return i;
                dst[i]      = vObjects.at(start++);
            }

            return count;
        }

        void LSPGraph::set_min_width(size_t value)
        {
            if (nMinWidth == value)
                return;
            nMinWidth = value;
            query_resize();
        }

        void LSPGraph::set_min_height(size_t value)
        {
            if (nMinHeight == value)
                return;
            nMinHeight = value;
            query_resize();
        }

        void LSPGraph::set_border(size_t value)
        {
            if (nBorder == value)
                return;
            nBorder = value;
            query_resize();
        }

        void LSPGraph::set_radius(size_t value)
        {
            if (nRadius == value)
                return;
            nRadius = value;
            query_resize();
        }

        void LSPGraph::query_draw(size_t flags)
        {
            LSPWidgetContainer::query_draw(flags | REDRAW_SURFACE);
        }

        void LSPGraph::size_request(size_request_t *r)
        {
            r->nMinWidth    = nMinWidth + sIPadding.horizontal() + (nBorder << 1);
            r->nMinHeight   = nMinHeight + sIPadding.vertical() + (nBorder << 1);
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;
        }

        status_t LSPGraph::add(LSPWidget *widget)
        {
            LSPGraphItem *obj = widget_cast<LSPGraphItem>(widget);
            if (obj == NULL)
                return STATUS_BAD_ARGUMENTS;

            obj->set_parent(this);
            vObjects.add(obj);

            LSPAxis *axis = widget_cast<LSPAxis>(widget);
            if (axis != NULL)
            {
                vAxises.add(axis);
                if (axis->is_basis())
                    vBasises.add(axis);
                return STATUS_OK;
            }

            LSPCenter *center = widget_cast<LSPCenter>(widget);
            if (center != NULL)
                vCenters.add(center);

            return STATUS_OK;
        }

        status_t LSPGraph::remove(LSPWidget *widget)
        {
            LSPGraphItem *obj = widget_cast<LSPGraphItem>(widget);
            if (obj == NULL)
                return STATUS_BAD_ARGUMENTS;

            if (!vObjects.remove(obj))
                return STATUS_NOT_FOUND;
            unlink_widget(obj);

            LSPAxis *axis = widget_cast<LSPAxis>(widget);
            if (axis != NULL)
            {
                vAxises.remove(axis);
                vBasises.remove(axis);
                return STATUS_OK;
            }

            LSPCenter *center = widget_cast<LSPCenter>(widget);
            if (center != NULL)
                vCenters.remove(center);

            return STATUS_OK;
        }

        status_t LSPGraph::on_mouse_down(const ws_event_t *e)
        {
            if (e->nType == UIE_MOUSE_DOWN)
                take_focus();
            return LSPWidgetContainer::on_mouse_down(e);
        }

        void LSPGraph::realize(const realize_t *r)
        {
            size_t bw       = nBorder;
            size_t bs       = bw * M_SQRT2 * 0.5;
            ssize_t gw      = r->nWidth  - (bs << 1);
            ssize_t gh      = r->nHeight - (bs << 1);

            fCanvasLeft     = sSize.nLeft + bs;
            fCanvasTop      = sSize.nTop + bs;
            fCanvasWidth    = gw;
            fCanvasHeight   = gh;

            LSPWidgetContainer::realize(r);
        }

        status_t LSPGraph::on_resize(const realize_t *r)
        {
            status_t res = STATUS_OK;

            for (size_t i=0, n=vObjects.size(); i<n; ++i)
            {
                LSPGraphItem *item = vObjects.at(i);
                realize_t tmp = *r;
                if ((res = item->slots()->execute(LSPSLOT_RESIZE_PARENT, this, &tmp)) != STATUS_OK)
                    break;
            }

            return res;
        }

        void LSPGraph::draw(ISurface *s)
        {
            // Prepare palette
            Color color(sColor);
            Color bg_color(sBgColor);
            color.scale_lightness(brightness());

            // Draw background
            ssize_t pr  = (nBorder + 1) >> 1;
            s->fill_frame(0, 0, sSize.nWidth, sSize.nHeight,
                    pr, pr, sSize.nWidth - 2*pr, sSize.nHeight - 2*pr,
                    bg_color);

            size_t bw   = nBorder;
            size_t bs   = bw * M_SQRT2 * 0.5;

            s->fill_round_rect(0, 0, sSize.nWidth, sSize.nHeight, nBorder, SURFMASK_ALL_CORNER, color);

            // Draw the internals
            ISurface *cv = get_canvas(s, fCanvasWidth, fCanvasHeight, color);
            if (cv != NULL)
                s->draw(cv, bs, bs);

            // Draw the glass and the border
            cv = create_border_glass(s, &pGlass, sSize.nWidth, sSize.nHeight, nRadius, nBorder, SURFMASK_ALL_CORNER, color);
            if (cv != NULL)
                s->draw(cv, 0, 0);
        }
    } /* namespace tk */
} /* namespace lsp */
