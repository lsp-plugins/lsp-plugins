/*
 * LSPBox.cpp
 *
 *  Created on: 20 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPBox::metadata = { "LSPBox", &LSPWidgetContainer::metadata };

        LSPBox::LSPBox(LSPDisplay *dpy, bool horizontal): LSPWidgetContainer(dpy)
        {
            nSpacing        = 0;
            bProportional   = false;
            enOrientation   = (horizontal) ? O_HORIZONTAL : O_VERTICAL;
            pClass          = &metadata;
        }
        
        LSPBox::~LSPBox()
        {
            do_destroy();
        }

        status_t LSPBox::init()
        {
            status_t result = LSPWidgetContainer::init();
            if (result != STATUS_OK)
                return result;

            return STATUS_OK;
        }

        void LSPBox::destroy()
        {
            do_destroy();
            LSPWidgetContainer::destroy();
        }

        void LSPBox::do_destroy()
        {
            size_t n_items  = vItems.size();
            for (size_t i=0; i<n_items; ++i)
            {
                // Get widget
                cell_t *w = vItems.at(i);
                if (w->pWidget == NULL)
                    continue;

                unlink_widget(w->pWidget);
                w->pWidget = NULL;
            }

            vItems.flush();
        }

        bool LSPBox::hidden_widget(const cell_t *w)
        {
            if (w == NULL)
                return true;
            if (w->pWidget == NULL)
                return true;
            return !w->pWidget->visible();
        }

        size_t LSPBox::visible_items()
        {
            size_t n_items  = vItems.size();
            size_t visible  = 0;

            // Estimate number of visible items
            for (size_t i=0; i<n_items; ++i)
            {
                // Get widget
                cell_t *w = vItems.at(i);
                if (!hidden_widget(w))
                    visible ++;
            }

            return visible;
        }

        LSPWidget *LSPBox::find_widget(ssize_t x, ssize_t y)
        {
            size_t items = vItems.size();
            for (size_t i=0; i<items; ++i)
            {
                cell_t *w = vItems.at(i);
                if (hidden_widget(w))
                    continue;
                if ((w->s.nLeft <= x) &&
                    (w->s.nTop <= y) &&
                    (ssize_t(w->s.nLeft + w->s.nWidth) > x) &&
                    (ssize_t(w->s.nTop + w->s.nHeight) > y))
                    return w->pWidget;
            }

            return NULL;
        }

        void LSPBox::set_border(size_t value)
        {
            query_resize();
        }

        void LSPBox::set_spacing(size_t value)
        {
            nSpacing        = value;
            query_resize();
        }

        void LSPBox::set_proportional(bool value)
        {
            bProportional   = value;
            query_resize();
        }

        void LSPBox::set_orientation(orientation_t value)
        {
            if (enOrientation == value)
                return;

            enOrientation   = value;
            query_resize();
        }

        void LSPBox::render(ISurface *s, bool force)
        {
            size_t items = vItems.size();

            // Check dirty flag
            if (nFlags & REDRAW_SURFACE)
                force = true;

            // Estimate palette
            Color bg_color(sBgColor);

            // Render child widgets
            size_t visible = visible_items();

            // Draw background if needed
            if ((!visible) && (force))
            {
                s->fill_rect(sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight, bg_color);
                return;
            }

            // Draw items
            for (size_t i=0; i<items; ++i)
            {
                cell_t *wc = vItems.at(i);
                if (hidden_widget(wc))
                    continue;

                LSPWidget *w = wc->pWidget;

                if ((force) || (w->redraw_pending()))
                {
                    // Fill unused space with background
                    if (force)
                    {
                        bg_color.copy(w->bg_color()->color());
                        s->fill_frame(
                            wc->a.nLeft, wc->a.nTop, wc->a.nWidth, wc->a.nHeight,
                            wc->s.nLeft, wc->s.nTop, wc->s.nWidth, wc->s.nHeight,
                            bg_color
                        );
                    }
                    w->render(s, force);
                    w->commit_redraw();
                }
            }
        }

        status_t LSPBox::add(LSPWidget *widget)
        {
            cell_t *cell = vItems.append();
            if (cell == NULL)
                return STATUS_NO_MEM;

            cell->r.nMinWidth   = -1;
            cell->r.nMinHeight  = -1;
            cell->r.nMaxWidth   = -1;
            cell->r.nMaxHeight  = -1;
            cell->a.nLeft       = 0;
            cell->a.nTop        = 0;
            cell->a.nWidth      = 0;
            cell->a.nHeight     = 0;
            cell->s.nLeft       = 0;
            cell->s.nTop        = 0;
            cell->s.nWidth      = 0;
            cell->s.nHeight     = 0;
            cell->pWidget       = widget;

            if (widget != NULL)
                widget->set_parent(this);

            query_resize();
            return STATUS_SUCCESS;
        }

        status_t LSPBox::remove(LSPWidget *child)
        {
            size_t n            = vItems.size();
            for (size_t i=0; i<n; ++i)
            {
                cell_t *cell        = vItems.at(i);
                if (cell->pWidget == child)
                {
                    if (!vItems.remove(i))
                        return STATUS_UNKNOWN_ERR;
                    query_resize();
                    child->set_parent(NULL);
                    return STATUS_OK;
                }
            }

            return STATUS_NOT_FOUND;
        }

        status_t LSPBox::remove_all()
        {
            if (vItems.size() <= 0)
                return STATUS_OK;

            cstorage<cell_t> tmp;
            vItems.swap(&tmp);

            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                cell_t *cell        = vItems.at(i);
                if (cell->pWidget != NULL)
                    cell->pWidget->set_parent(NULL);
            }

            tmp.flush();
            query_resize();
            return STATUS_OK;
        }

        void LSPBox::realize(const realize_t *r)
        {
            size_t n_items  = vItems.size();
            if (n_items <= 0)
            {
                LSPWidgetContainer::realize(r);
                return;
            }
            size_t visible = visible_items();

//            // Reset settings
//            for (size_t i=0; i<n_items; ++i)
//            {
//                // Get widget
//                cell_t *w = vItems.at(i);
//                if (hidden_widget(w))
//                    continue;
//
//                w->a.nLeft      = -1;
//                w->a.nTop       = -1;
//                w->a.nWidth     = -1;
//                w->a.nHeight    = -1;
//            }

            ssize_t n_left  = (enOrientation == O_HORIZONTAL) ? r->nWidth : r->nHeight;
            if (visible > 0)
                n_left         -= (visible-1)*nSpacing;
            size_t n_size   = n_left;

            // FIRST PASS: Initialize widgets with their minimum widths
            size_t expand       = 0;
            size_t n_expand     = 0;
            for (size_t i=0; i<n_items; ++i)
            {
                // Get widget
                cell_t *w = vItems.at(i);
                if (hidden_widget(w))
                    continue;

                if (enOrientation == O_HORIZONTAL)
                {
                    w->a.nWidth     = w->p.nLeft + w->p.nRight;
                    if (w->r.nMinWidth >= 0)
                        w->a.nWidth    += w->r.nMinWidth;
                    w->a.nHeight    = r->nHeight;
                    n_left         -= w->a.nWidth;

                    // Calculate number of expanded widgets
                    if (w->pWidget->expand())
                    {
                        expand      ++;
                        n_expand   += w->a.nWidth;
                    }
                }
                else    // VBOX
                {
                    w->a.nHeight    = w->p.nTop + w->p.nBottom;
                    if (w->r.nMinHeight)
                        w->a.nHeight    += w->r.nMinHeight;
                    w->a.nWidth     = r->nWidth;
                    n_left         -= w->a.nHeight;

                    // Calculate number of expanded widgets
                    if (w->pWidget->expand())
                    {
                        expand      ++;
                        n_expand   += w->a.nHeight;
                    }
                }
            }

            // SECOND PASS: Split unused space between widgets
            if (n_left > 0)
            {
                ssize_t total = 0;

                if (expand > 0)
                {
                    // Update expand value
                    if (n_expand == 0)
                    {
                        // Split unused space between all expanded widgets
                        ssize_t delta   = n_left / expand;
                        for (size_t i=0; i<n_items; ++i)
                        {
                            // Get widget
                            cell_t *w = vItems.at(i);
                            if (hidden_widget(w))
                                continue;
                            else if (!w->pWidget->expand())
                                continue;

                            if (enOrientation == O_HORIZONTAL)
                            {
                                w->a.nWidth    += delta;
                                total          += delta;
                            }
                            else // VBOX
                            {
                                w->a.nHeight   += delta;
                                total          += delta;
                            }
                        }
                    }
                    else
                    {
                        // Split unused space between all expanded widgets
                        for (size_t i=0; i<n_items; ++i)
                        {
                            // Get widget
                            cell_t *w = vItems.at(i);
                            if (hidden_widget(w))
                                continue;
                            else if (!w->pWidget->expand())
                                continue;

                            if (enOrientation == O_HORIZONTAL)
                            {
                                ssize_t delta   = (w->a.nWidth * n_left) / n_expand;
                                w->a.nWidth    += delta;
                                total          += delta;
                            }
                            else // VBOX
                            {
                                ssize_t delta   = (w->a.nHeight * n_left) / n_expand;
                                w->a.nHeight   += delta;
                                total          += delta;
                            }
                        }
                    } // n_expand
                }
                else
                {
                    // Split unused space between all visible widgets
                    for (size_t i=0; i<n_items; ++i)
                    {
                        // Get widget
                        cell_t *w = vItems.at(i);
                        if (hidden_widget(w))
                            continue;

                        if (enOrientation == O_HORIZONTAL)
                        {
                            ssize_t delta   = (w->a.nWidth * n_left) / n_size;
                            w->a.nWidth    += delta;
                            total          += delta;
                        }
                        else // VBOX
                        {
                            ssize_t delta   = (w->a.nHeight * n_left) / n_size;
                            w->a.nHeight   += delta;
                            total          += delta;
                        }
                    }
                }

                n_left     -= total;
            }

            // FOURTH PASS: utilize unused pixels
            if (n_left > 0)
            {
                bool any_visible = false;
                do
                {
                    any_visible = false;

                    for (size_t i=0; i<n_items; ++i)
                    {
                        // Get widget
                        cell_t *w = vItems.at(i);
                        if (hidden_widget(w))
                            continue;
                        if (enOrientation == O_HORIZONTAL)
                            w->a.nWidth     ++;
                        else
                            w->a.nHeight    ++;

                        any_visible = true;
                        if ((n_left--) <= 0)
                            break;
                    }
                } while ((any_visible) && (n_left > 0));
            }

            // Now we have n_left=0, now need to generate proper Left and Top coordinates of widget
            ssize_t l = r->nLeft, t = r->nTop; // Left-Top corner
            size_t counter = 0;

            // Now completely apply geometry to each widget
            for (size_t i=0; i<n_items; ++i)
            {
                // Get widget
                cell_t *w = vItems.at(i);
                if (hidden_widget(w))
                    continue;

                // Initial coordinates
                w->a.nLeft      = l;
                w->a.nTop       = t;

                w->s            = w->a;
                w->s.nWidth    -= w->p.nLeft + w->p.nRight;
                w->s.nHeight   -= w->p.nTop  + w->p.nBottom;

                if (enOrientation == O_HORIZONTAL)
                {
                    if ((++counter) < visible)
                        w->a.nWidth    += nSpacing;
                    l              += w->a.nWidth;
                }
                else
                {
                    if ((++counter) < visible)
                        w->a.nHeight   += nSpacing;
                    t              += w->a.nHeight;
                }

                // Do not fill
                if (!w->pWidget->fill())
                {
                    ssize_t nw      = (w->r.nMinWidth >= 0) ? w->r.nMinWidth : 0;
                    ssize_t nh      = (w->r.nMinHeight >= 0) ? w->r.nMinHeight : 0;
                    w->s.nLeft     += (w->s.nWidth - nw) >> 1;
                    w->s.nTop      += (w->s.nHeight - nh) >> 1;
                    w->s.nWidth     = nw;
                    w->s.nHeight    = nh;
                }
                else // Fixup coordinates
                {
                    if (enOrientation == O_HORIZONTAL)
                    {
                        // Fixup width
                        if ((w->r.nMaxWidth >= 0) && (w->r.nMaxWidth >= w->r.nMinWidth) && (w->r.nMaxWidth < ssize_t(w->s.nWidth)))
                        {
                            w->s.nLeft     += (w->s.nWidth - w->r.nMaxWidth) >> 1;
                            w->s.nWidth     = w->r.nMaxWidth;
                        }

                        // Fixup height
                        if ((w->r.nMaxHeight >= 0) && (w->r.nMaxHeight >= w->r.nMinHeight) && (w->r.nMaxHeight < ssize_t(w->s.nHeight)))
                        {
                            w->s.nTop      += (w->s.nHeight - w->r.nMaxHeight) >> 1;
                            w->s.nHeight    = w->r.nMaxHeight;
                        }
                    }
                    else
                    {
                        // Fixup height
                        if ((w->r.nMaxHeight >= 0) && (w->r.nMaxHeight >= w->r.nMinHeight) && (w->r.nMaxHeight < ssize_t(w->s.nHeight)))
                        {
                            w->s.nTop      += (w->s.nHeight - w->r.nMaxHeight) >> 1;
                            w->s.nHeight    = w->r.nMaxHeight;
                        }

                        // Fixup height
                        if ((w->r.nMaxWidth >= 0) && (w->r.nMaxWidth >= w->r.nMinWidth) && (w->r.nMaxWidth < ssize_t(w->s.nWidth)))
                        {
                            w->s.nLeft     += (w->s.nWidth - w->r.nMaxWidth) >> 1;
                            w->s.nWidth     = w->r.nMaxWidth;
                        }
                    }
                }

                w->s.nLeft     += w->p.nLeft;
                w->s.nTop      += w->p.nTop;

                // Output message
//                lsp_trace("realize id=%d, parameters = {%d, %d, %d, %d}", int(i), int(w->s.nLeft), int(w->s.nTop), int(w->s.nWidth), int(w->s.nHeight));
                w->pWidget->realize(&w->s);
                w->pWidget->query_draw();
            }

            // Call parent method
            LSPWidgetContainer::realize(r);
        }

        void LSPBox::size_request(size_request_t *r)
        {
            r->nMinWidth    = -1;
            r->nMinHeight   = -1;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;

            size_t n_items  = vItems.size();
            if (n_items <= 0)
                return;

            ssize_t e_width = 0, e_height = 0; // Estimated width and height

            // Estimate self size
            for (size_t i=0; i<n_items; ++i)
            {
                // Get widget
                cell_t *w = vItems.at(i);
                if (hidden_widget(w))
                    continue;

                // Perform size request
                w->r.nMinWidth      = -1;
                w->r.nMinHeight     = -1;
                w->r.nMaxWidth      = -1;
                w->r.nMaxHeight     = -1;
                if (w->pWidget == NULL)
                    continue;

                w->pWidget->size_request(&w->r);
                w->pWidget->padding()->get(&w->p);
//                lsp_trace("size_request id=%d, parameters = {%d, %d, %d, %d}",
//                    int(i), int(w->r.nMinWidth), int(w->r.nMinHeight), int(w->r.nMaxWidth), int(w->r.nMaxHeight));

                // Analyze widget class
                ssize_t x_width     = w->p.nLeft + w->p.nRight;
                ssize_t x_height    = w->p.nTop  + w->p.nBottom;
                if (w->r.nMinWidth >= 0)
                    x_width            += w->r.nMinWidth;
                if (w->r.nMinHeight >= 0)
                    x_height           += w->r.nMinHeight;

                if (enOrientation == O_HORIZONTAL)
                {
                    if (x_height > e_height)
                        e_height        = x_height;
                    e_width            += x_width;
                    if (i > 0)
                        e_width        += nSpacing;
                }
                else // VBOX
                {
                    if (x_width > e_width)
                        e_width         = x_width;
                    e_height           += x_height;
                    if (i > 0)
                        e_height       += nSpacing;
                }
            }

            r->nMinWidth        = e_width;
            r->nMinHeight       = e_height;
        }
    
    } /* namespace tk */
} /* namespace lsp */
