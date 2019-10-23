/*
 * LSPScrollBox.cpp
 *
 *  Created on: 11 окт. 2019 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPScrollBox::metadata = { "LSPScrollBox", &LSPWidgetContainer::metadata };

        LSPScrollBox::LSPScrollBox(LSPDisplay *dpy, bool horizontal): LSPWidgetContainer(dpy),
            sConstraints(this),
            sHBar(dpy, true),
            sVBar(dpy, false)
        {
            nSpacing        = 0;
            bProportional   = false;
            bHSBypass       = true;
            bVSBypass       = true;
            enOrientation   = (horizontal) ? O_HORIZONTAL : O_VERTICAL;
            enHScroll		= SCROLL_NONE;
            enVScroll		= SCROLL_NONE;
            nAreaX          = 0;
            nAreaY          = 0;

            pClass          = &metadata;
        }
        
        LSPScrollBox::~LSPScrollBox()
        {
            do_destroy();
        }

        status_t LSPScrollBox::init()
        {
            status_t result = LSPWidgetContainer::init();
            if (result != STATUS_OK)
                return result;

            result = sHBar.init();
            if (result != STATUS_OK)
                return result;
            result = sVBar.init();
            if (result != STATUS_OK)
                return result;

            sHBar.set_parent(this);
            sHBar.set_step(16.0f);
            sHBar.set_tiny_step(1.0f);
            sHBar.hide();

            sVBar.set_parent(this);
            sVBar.set_step(16.0f);
            sVBar.set_tiny_step(1.0f);
            sVBar.hide();


            sHBar.slot(LSPSLOT_CHANGE)->bind(slot_on_scroll, self());
            sVBar.slot(LSPSLOT_CHANGE)->bind(slot_on_scroll, self());

            return STATUS_OK;
        }

        void LSPScrollBox::destroy()
        {
            do_destroy();
            LSPWidgetContainer::destroy();
        }

        void LSPScrollBox::do_destroy()
        {
            // Destroy scroll bars
            sHBar.destroy();
            sVBar.destroy();

            // Unlink widgets
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

        bool LSPScrollBox::hidden_widget(const cell_t *w)
        {
            if (w == NULL)
                return true;
            if (w->pWidget == NULL)
                return true;
            return !w->pWidget->visible();
        }

        size_t LSPScrollBox::visible_items()
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

        LSPWidget *LSPScrollBox::find_widget(ssize_t x, ssize_t y)
        {
            if ((sHBar.visible()) && (sHBar.inside(x, y)))
                return &sHBar;
            if ((sVBar.visible()) && (sVBar.inside(x, y)))
                return &sVBar;

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

        void LSPScrollBox::set_spacing(size_t value)
        {
            if (nSpacing == value)
                return;
            nSpacing        = value;
            query_resize();
        }

        void LSPScrollBox::set_proportional(bool value)
        {
            if (bProportional == value)
                return;
            bProportional   = value;
            query_resize();
        }

        void LSPScrollBox::set_orientation(orientation_t value)
        {
            if (enOrientation == value)
                return;

            enOrientation   = value;
            query_resize();
        }

        void LSPScrollBox::set_hscroll(scrolling_t mode)
        {
            if (enHScroll == mode)
                return;

            enHScroll   = mode;
            query_resize();
        }

        void LSPScrollBox::set_vscroll(scrolling_t mode)
        {
            if (enVScroll == mode)
                return;

            enVScroll   = mode;
            query_resize();
        }

        void LSPScrollBox::render(ISurface *s, bool force)
        {
            size_t items = vItems.size();

            // Check dirty flag
            if (nFlags & REDRAW_SURFACE)
                force = true;

            // Estimate palette
            Color bg_color(sBgColor);

            realize_t area;
            area.nLeft      = left();
            area.nTop       = top();
            area.nWidth     = width();
            area.nHeight    = height();

            // Render scroll bars
            if (sVBar.visible())
            {
                area.nWidth    -= sVBar.width();
                if ((force) || (sVBar.redraw_pending()))
                    sVBar.render(s, force);
            }
            if (sHBar.visible())
            {
                area.nHeight   -= sHBar.height();
                if ((force) || (sHBar.redraw_pending()))
                    sHBar.render(s, force);
            }
            if ((sHBar.visible()) && (sVBar.visible()))
                s->fill_rect(sVBar.left(), sHBar.top(), sVBar.width(), sHBar.height(), bg_color);

            // Render child widgets
            size_t n_visible = visible_items();

            // Draw background if needed
            if ((!n_visible) && (force))
            {
                s->fill_rect(area.nLeft, area.nTop, area.nWidth, area.nHeight, bg_color);
                return;
            }

            // Draw items clipped
            s->clip_begin(area.nLeft, area.nTop, area.nWidth, area.nHeight);

            ssize_t w_right     = area.nLeft + area.nWidth;
            ssize_t w_bottom    = area.nTop + area.nHeight;

            for (size_t i=0; i<items; ++i)
            {
                cell_t *wc = vItems.at(i);
                if (hidden_widget(wc))
                    continue;

                // Do not draw widget area if it is outside the clip area
                if ((wc->a.nLeft >= w_right) ||
                    (wc->a.nTop >= w_bottom) ||
                    ((wc->a.nLeft + wc->a.nWidth) <= area.nLeft) ||
                    ((wc->a.nTop + wc->a.nHeight) <= area.nTop))
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

                    // Skip clipped widgets
                    if (!((w->left() >= w_right) ||
                        (w->top() >= w_bottom) ||
                        (w->right() <= area.nLeft) ||
                        (w->bottom() <= area.nTop)))
                    {
                        w->render(s, force);
                        w->commit_redraw();
                    }
                }
            }

            s->clip_end();
        }

        status_t LSPScrollBox::add(LSPWidget *widget)
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

        status_t LSPScrollBox::remove(LSPWidget *child)
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

        status_t LSPScrollBox::remove_all()
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

        void LSPScrollBox::realize(const realize_t *r)
        {
            // Call parent widget
            LSPWidgetContainer::realize(r);

            // Estimate scroll bar sizes (if needed)
            allocation_t alloc;
            estimate_allocation(&alloc, r);

            size_request_t hbar, vbar;
            hbar.nMinWidth   = -1;
            hbar.nMinHeight  = -1;
            hbar.nMaxWidth   = -1;
            hbar.nMaxHeight  = -1;
            vbar.nMinWidth   = -1;
            vbar.nMinHeight  = -1;
            vbar.nMaxWidth   = -1;
            vbar.nMaxHeight  = -1;

            sHBar.size_request(&hbar);
            sVBar.size_request(&vbar);

            ssize_t havail = alloc.aw;
            ssize_t vavail = alloc.ah;

            // Place horizontal scroll bar if enabled
            havail  = (alloc.vs) ? r->nWidth - vbar.nMinWidth : r->nWidth;
            vavail  = (alloc.hs) ? r->nHeight - hbar.nMinHeight : r->nHeight;

            if (alloc.aw < havail)
                alloc.aw    = havail;
            if (alloc.ah < vavail)
                alloc.ah    = vavail;

            if (alloc.hs)
            {
                if (hbar.nMaxWidth < 0)
                    hbar.nMaxWidth = havail;

                realize_t hbr;
                hbr.nLeft   = r->nLeft + ((havail - hbar.nMaxWidth) >> 1);
                hbr.nTop    = r->nTop + r->nHeight - hbar.nMinHeight;
                hbr.nWidth  = hbar.nMaxWidth;
                hbr.nHeight = hbar.nMinHeight;

                sHBar.set_min_value(0.0f);
                sHBar.set_max_value(alloc.aw - havail);
                sHBar.show();
                sHBar.query_draw();
                sHBar.realize(&hbr);
            }
            else
            {
                sHBar.hide();
                sHBar.set_value(0.0f);
                sHBar.set_min_value(0.0f);
                sHBar.set_max_value(0.0f);
            }

            // Place vertical scroll bar if enabled
            if (alloc.vs)
            {
                if (vbar.nMaxHeight < 0)
                    vbar.nMaxHeight = vavail;

                realize_t vbr;
                vbr.nLeft   = r->nLeft + r->nWidth - vbar.nMinWidth;
                vbr.nTop    = r->nTop + ((vavail - vbar.nMaxHeight) >> 1);
                vbr.nWidth  = vbar.nMinWidth;
                vbr.nHeight = vbar.nMaxHeight;

                sVBar.set_min_value(0.0f);
                sVBar.set_max_value(alloc.ah - vavail);
                sVBar.show();
                sVBar.query_draw();
                sVBar.realize(&vbr);
            }
            else
            {
                sVBar.hide();
                sVBar.set_value(0.0f);
                sVBar.set_min_value(0.0f);
                sVBar.set_max_value(0.0f);
            }

            nAreaX  = alloc.aw;
            nAreaY  = alloc.ah;

            // Realize all children at their actual positions
            realize_children();
        }

        void LSPScrollBox::realize_children()
        {
            // Estimate number of items for allocation
            size_t n_items  = vItems.size();
            if (n_items <= 0)
                return;

            size_t visible = visible_items();

            ssize_t n_left  = (enOrientation == O_HORIZONTAL) ? nAreaX : nAreaY;
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
                    w->a.nHeight    = nAreaY;
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
                    w->a.nWidth     = nAreaX;
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
                if (expand > 0)
                {
                    // Update expand value
                    if (n_expand == 0)
                        n_expand = 1;

                    // Split unused space between all expanded widgets
                    ssize_t total = 0;
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
                    n_left     -= total;
                }
                else
                {
                    // Split unused space between all visible widgets
                    ssize_t total = 0;
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
                    n_left     -= total;
                }
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
            // Left-Top corner
            ssize_t l   = left() - ssize_t(sHBar.value());
            ssize_t t   = top() - ssize_t(sVBar.value());
            size_t counter = 0;

            // Apply geometry to each widget
            for (size_t i=0, n_items=vItems.size(); i<n_items; ++i)
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
        }

        void LSPScrollBox::estimate_allocation(allocation_t *alloc, const realize_t *realize)
        {
            // Initialize allocation
            alloc->aw           = -1;
            alloc->ah           = -1;
            alloc->hs           = false;
            alloc->vs           = false;

            size_request_t *r   = &alloc->r;
            if (realize != NULL)
            {
                r->nMinWidth    = realize->nWidth;
                r->nMinHeight   = realize->nHeight;
                r->nMaxWidth    = realize->nWidth;
                r->nMaxHeight   = realize->nHeight;
            }
            else
                sConstraints.get(r);

            // Estimated width and height of widget area
            alloc->aw = 0;
            alloc->ah = 0;

            for (size_t i=0, n_items=vItems.size(); i<n_items; ++i)
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
                    if (x_height > alloc->ah)
                        alloc->ah       = x_height;
                    alloc->aw          += x_width;
                    if (i > 0)
                        alloc->aw      += nSpacing;
                }
                else // VBOX
                {
                    if (x_width > alloc->aw)
                        alloc->aw       = x_width;
                    alloc->ah          += x_height;
                    if (i > 0)
                        alloc->ah      += nSpacing;
                }
            }

            // Check scroll bar allocations
            size_request_t hsb, vsb;
            sHBar.size_request(&hsb);
            sVBar.size_request(&vsb);

            if (hsb.nMinWidth < 0)
                hsb.nMinWidth        = 0;
            if (hsb.nMinHeight < 0)
                hsb.nMinHeight       = 0;
            if (vsb.nMinWidth < 0)
                vsb.nMinWidth        = 0;
            if (vsb.nMinHeight < 0)
                vsb.nMinHeight       = 0;

            ssize_t minw    = (r->nMinWidth >= 0) ? r->nMinWidth : 0;
            ssize_t minh    = (r->nMinHeight >= 0) ? r->nMinHeight : 0;
            ssize_t hsize   = 0;
            ssize_t vsize   = 0;

            alloc->hs       = (enHScroll == SCROLL_ALWAYS) ||
                              ((enHScroll == SCROLL_OPTIONAL) && (r->nMaxWidth >= 0) && (alloc->aw > r->nMaxWidth));
            alloc->vs       = (enVScroll == SCROLL_ALWAYS) ||
                              ((enVScroll == SCROLL_OPTIONAL) && (r->nMaxHeight >= 0) && (alloc->ah > r->nMaxHeight));

            if (alloc->hs)
                alloc->vs       = (enVScroll == SCROLL_ALWAYS) ||
                                  ((enVScroll == SCROLL_OPTIONAL) && (r->nMaxHeight >= 0) && ((alloc->ah + hsb.nMinHeight) > r->nMaxHeight));
            else if (alloc->vs)
                alloc->hs       = (enHScroll == SCROLL_ALWAYS) ||
                                  ((enHScroll == SCROLL_OPTIONAL) && (r->nMaxWidth >= 0) && ((alloc->aw + vsb.nMinWidth) > r->nMaxWidth));

            if (alloc->hs)
            {
                if (alloc->vs)
                {
                    hsize           = lsp_max(minw, hsb.nMinWidth + vsb.nMinWidth);
                    vsize           = lsp_max(minh, hsb.nMinHeight + vsb.nMinHeight);
                }
                else
                {
                    hsize           = lsp_max(minw, hsb.nMinWidth);
                    vsize           = alloc->ah + hsb.nMinHeight;
                }
            }
            else if (alloc->vs)
            {
                hsize           = alloc->aw + vsb.nMinWidth;
                vsize           = lsp_max(minh, vsb.nMinHeight);
            }
            else
            {
                hsize           = alloc->aw;
                vsize           = alloc->ah;
            }

            // Update size constraints
            if ((r->nMinWidth >= 0) && (r->nMinWidth < hsize))
                r->nMinWidth    = hsize;
            if ((r->nMinHeight >= 0) && (r->nMinHeight < vsize))
                r->nMinHeight   = vsize;
            if ((r->nMaxWidth >= 0) && (r->nMaxWidth < hsize))
                r->nMaxWidth    = hsize;
            if ((r->nMaxHeight >= 0) && (r->nMaxHeight < vsize))
                r->nMaxHeight   = vsize;
        }

        void LSPScrollBox::size_request(size_request_t *r)
        {
            allocation_t alloc;
            estimate_allocation(&alloc, NULL);
            *r              = alloc.r;
        }

        status_t LSPScrollBox::handle_event(const ws_event_t *e)
        {
            if (e->nType != UIE_MOUSE_SCROLL)
                return LSPWidgetContainer::handle_event(e);

            ws_event_t xe = *e;
            LSPScrollBar *bar1 = &sVBar, *bar2 = &sHBar;
            bool b1 = bVSBypass, b2 = bHSBypass;

            if ((xe.nCode == MCD_LEFT) || (xe.nCode == MCD_RIGHT))
            {
                xe.nCode = (xe.nCode == MCD_LEFT) ? MCD_DOWN : MCD_UP;
                bar1 = &sHBar;
                bar2 = &sVBar;
                b1 = bHSBypass;
                b2 = bVSBypass;
            }

            if ((xe.nCode == MCD_UP) || (xe.nCode == MCD_DOWN))
            {
                if (bar1->visible())
                {
                    // Shift overrides vertical scroll with horizontal scroll
                    if ((xe.nState & MCF_SHIFT) && (bar2->visible()))
                    {
                        if (xe.nState & MCF_ALT)
                        {
                            xe.nState &= ~MCF_ALT;
                            xe.nState |= MCF_SHIFT;
                        }
                        else
                            xe.nState &= ~MCF_SHIFT;

                        return (b2) ? LSPWidgetContainer::handle_event(e) : bar2->handle_event(&xe);
                    }

                    return (b1) ? LSPWidgetContainer::handle_event(e) : bar1->handle_event(&xe);
                }
                else if (bar2->visible())
                    return (b2) ? LSPWidgetContainer::handle_event(e) : bar2->handle_event(&xe);
            }

            return STATUS_OK;
        }

        status_t LSPScrollBox::slot_on_scroll(LSPWidget *sender, void *ptr, void *data)
        {
            LSPScrollBox *_this = widget_ptrcast<LSPScrollBox>(ptr);
            if (_this == NULL)
                return STATUS_BAD_STATE;

            _this->realize_children();
            _this->query_draw();
            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */
