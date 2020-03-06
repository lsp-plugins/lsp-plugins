/*
 * LSPGrid.cpp
 *
 *  Created on: 20 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPGrid::metadata = { "LSPGrid", &LSPWidgetContainer::metadata };
        
        LSPGrid::LSPGrid(LSPDisplay *dpy, bool horizontal): LSPWidgetContainer(dpy)
        {
            nHSpacing   = 0;
            nVSpacing   = 0;
            nCurrRow    = 0;
            nCurrCol    = 0;
            bVertical   = !horizontal;
            pClass      = &metadata;

            pKey        = NULL;
            pMouse      = NULL;
        }
        
        LSPGrid::~LSPGrid()
        {
            destroy_cells();
        }

        status_t LSPGrid::init()
        {
            status_t result = LSPWidgetContainer::init();
            if (result != STATUS_OK)
                return result;

            return STATUS_OK;
        }

        void LSPGrid::destroy()
        {
            destroy_cells();
            LSPWidgetContainer::destroy();
        }

        void LSPGrid::destroy_cells()
        {
            size_t n_items  = vCells.size();
            for (size_t i=0; i<n_items; ++i)
            {
                // Get widget
                cell_t *w = vCells.at(i);
                if (w->pWidget == NULL)
                    continue;

                unlink_widget(w->pWidget);
                w->pWidget = NULL;
            }

            vCells.flush();
            vRows.flush();
            vCols.flush();
        }

        LSPWidget *LSPGrid::find_widget(ssize_t x, ssize_t y)
        {
            size_t items = vRows.size() * vCols.size();
            if (items < vCells.size())
                items = vCells.size();

            for (size_t i=0; i<items; ++i)
            {
                cell_t *w = vCells.at(i);
                if ((hidden_widget(w)) || (w->nRows <= 0))
                    continue;
                if ((w->s.nLeft <= x) &&
                    (w->s.nTop <= y) &&
                    (ssize_t(w->s.nLeft + w->s.nWidth) > x) &&
                    (ssize_t(w->s.nTop + w->s.nHeight) > y))
                    return w->pWidget;
            }

            return NULL;
        }

        bool LSPGrid::hidden_widget(const cell_t *w)
        {
            if (w == NULL)
                return true;
            if (w->pWidget == NULL)
                return true;
            return !w->pWidget->visible();
        }

        void LSPGrid::split_size(header_t *h, size_t items, size_t size)
        {
            // Increase the size of each row
            size_t delta    = size / items;
            if (delta > 0)
            {
                for (size_t k=0; k < items; ++k)
                    h[k].nSize += delta;

                size       -= delta * items;
            }

            // Increment the size of other cells
            for (size_t k=0; size > 0; k = (k+1) % items, size--)
                h[k].nSize ++;
        }

        void LSPGrid::distribute_size(cstorage<header_t> &vh, size_t idx, size_t items, size_t rq_size)
        {
            ssize_t size    = estimate_size(vh, idx, items, NULL);
            ssize_t left    = rq_size - size;

            if (left <= 0)
                return;

            size_t expanded = 0;
            for (size_t k=0; k<items; ++k)
            {
                header_t *h     = vh.at(idx + k);
                if (h->bExpand)
                    expanded ++;
            }

            // Distribute size between expanded first
            if (expanded > 0)
            {
                ssize_t total = 0;
                for (size_t k=0; (k<items); ++k)
                {
                    header_t *h     = vh.at(idx + k);
                    if (!h->bExpand)
                        continue;
                    size_t delta    = (h->nSize * left) / size;
                    h->nSize       += delta;
                    total          += delta;
                }
                left           -= total;

                // Add equal size to each element
                if (left > 0)
                {
                    size_t delta    = left / expanded;
                    if (delta > 0)
                    {
                        for (size_t k=0; k < items; ++k)
                        {
                            header_t *h     = vh.at(idx + k);
                            if (!h->bExpand)
                                continue;
                            h->nSize   += delta;
                            left       -= delta;
                        }
                    }
                }

                // Distribute the non-distributed size
                if (left > 0)
                {
                    for (size_t k=0; left > 0; k = (k+1) % items)
                    {
                        header_t *h     = vh.at(idx + k);
                        if (!h->bExpand)
                            continue;
                        h->nSize ++;
                        left --;
                    }
                }
            }
            else
            {
                if (size > 0)
                {
                    // Distribute left size proportionally
                    ssize_t total = 0;
                    for (size_t k=0; (k<items); ++k)
                    {
                        header_t *h     = vh.at(idx + k);
                        size_t delta    = (h->nSize * left) / size;
                        h->nSize       += delta;
                        total          += delta;
                    }
                    left           -= total;
                }

                // Add equal size to each element
                if (left > 0)
                {
                    size_t delta    = left / items;
                    if (delta > 0)
                    {
                        for (size_t k=0; k < items; ++k)
                        {
                            header_t *h     = vh.at(idx + k);
                            h->nSize       += delta;
                        }
                        left       -= items * delta;
                    }
                }

                // Distribute the non-distributed size
                if (left > 0)
                {
                    for (size_t k=0; left > 0; k = (k+1) % items)
                    {
                        header_t *h     = vh.at(idx + k);
                        h->nSize ++;
                        left --;
                    }
                }
            }
        }

        size_t LSPGrid::estimate_size(cstorage<header_t> &vh, size_t idx, size_t items, size_t *spacing)
        {
            size_t size = 0, last = 0;

            for (size_t i=0, k=idx; i < items; ++i, ++k)
            {
                header_t *h = vh.at(k);
                size       += h->nSize + last;
                last        = h->nSpacing;
            }

            if (spacing != NULL)
                *spacing = last;

            return size;
        }

        void LSPGrid::assign_coords(header_t *h, size_t items, size_t start)
        {
            for (size_t i=0; i<items; ++i)
            {
                h[i].nOffset    = start;
                start          += h[i].nSize + h[i].nSpacing;
            }
        }

        status_t LSPGrid::set_rows(size_t rows)
        {
            size_t r    = vRows.size();
            if (rows == r)
                return STATUS_OK;

            if (rows < r)
            {
                // Need to remove extra rows
                size_t start_idx = r * vCols.size();
                if (!vCells.remove_n(start_idx, vCells.size() - start_idx))
                    return STATUS_UNKNOWN_ERR;
                if (!vRows.remove_n(rows, rows - r))
                    return STATUS_UNKNOWN_ERR;
            }
            else // r < rows
            {
                size_t n  = (rows - r);

                // Add set of cells for new rows
                if (vCols.size() > 0)
                {
                    cell_t *x = vCells.append_n(n * vCols.size());
                    if (!x)
                        return STATUS_NO_MEM;
                    for (size_t i=0; i<n; ++i)
                    {
                        x->pWidget      = NULL;
                        x->nRows        = 1;
                        x->nCols        = 1;
                        x++;
                    }
                }

                // Add set of rows to the rows
                if (!vRows.append_n(n))
                    return STATUS_NO_MEM;
            }

            // Reset iterator
            nCurrRow        = 0;
            nCurrCol        = 0;

            query_resize();

            return STATUS_OK;
        }

        status_t LSPGrid::set_columns(size_t cols)
        {
            size_t c    = vCols.size();
            if (cols == c)
                return STATUS_OK;

            size_t r    = vRows.size();

            if (cols < c)
            {
                // Need to remove extra columns
                for (size_t i=0, j=cols; i<r; ++i, j += cols)
                {
                    if (!vCells.remove_n(j, c - cols))
                        return STATUS_UNKNOWN_ERR;
                }

                if (!vCols.remove_n(cols, c - cols))
                    return STATUS_UNKNOWN_ERR;
            }
            else
            {
                size_t n  = (cols - c);

                // Need to add extra columns
                for (size_t i=0, j=c; i<r; ++i, j += cols)
                {
                    cell_t *x = vCells.insert_n(j, n);
                    if (x == NULL)
                        return STATUS_NO_MEM;

                    for (size_t i=0; i<n; ++i)
                    {
                        x->pWidget      = NULL;
                        x->nRows        = 1;
                        x->nCols        = 1;
                        x++;
                    }
                }

                // Add extra column headers
                if (!vCols.append_n(n))
                    return STATUS_NO_MEM;
            }

            // Reset iterator
            nCurrRow        = 0;
            nCurrCol        = 0;

            query_resize();

            return STATUS_OK;
        }

        status_t LSPGrid::set_orientation(orientation_t value)
        {
            if ((value == O_VERTICAL) && (bVertical))
                return STATUS_OK;
            else if ((value == O_HORIZONTAL) && (!bVertical))
                return STATUS_OK;

            bVertical   = value == O_VERTICAL;

            // Reset iterator
            nCurrRow        = 0;
            nCurrCol        = 0;

            return STATUS_OK;
        }

        status_t LSPGrid::set_hspacing(size_t value)
        {
            nHSpacing       = value;
            query_resize();
            return STATUS_OK;
        }

        status_t LSPGrid::set_vspacing(size_t value)
        {
            nVSpacing       = value;
            query_resize();
            return STATUS_OK;
        }

        status_t LSPGrid::set_spacing(size_t hor, size_t vert)
        {
            nHSpacing       = hor;
            nVSpacing       = vert;
            query_resize();
            return STATUS_OK;
        }

        void LSPGrid::render(ISurface *s, bool force)
        {
            size_t items = vCells.size();

            // Check dirty flag
            if (nFlags & REDRAW_SURFACE)
                force = true;

            // Estimate palette
            Color bg_color;

            // Render nested widgets
            size_t visible = 0;
            for (size_t i=0; i<items; ++i)
            {
                cell_t *w = vCells.at(i);
                if (w->nRows <= 0)
                    continue;
                if (hidden_widget(w))
                {
                    if (w->pWidget != NULL)
                        bg_color.copy(w->pWidget->bg_color()->color());
                    else
                        bg_color.copy(sBgColor);
                    s->fill_rect(w->a.nLeft, w->a.nTop, w->a.nWidth, w->a.nHeight, bg_color);
                    continue;
                }

                visible ++;
                if ((force) || (w->pWidget->redraw_pending()))
                {
//                    lsp_trace("render child=%p (%s), force=%d, pending=%d",
//                            w->pWidget, w->pWidget->get_class()->name,
//                            int(force), int(w->pWidget->redraw_pending()));
                    if (force)
                    {
                        bg_color.copy(w->pWidget->bg_color()->color());
                        s->fill_frame(
                            w->a.nLeft, w->a.nTop, w->a.nWidth, w->a.nHeight,
                            w->s.nLeft, w->s.nTop, w->s.nWidth, w->s.nHeight,
                            bg_color
                        );
//                        s->wire_rect(w->a.nLeft, w->a.nTop, w->a.nWidth, w->a.nHeight, 1, red);
                    }

    //                lsp_trace("Rendering this=%p, tgt=%p, force=%d", this, w->pWidget, int(force));
                    w->pWidget->render(s, force);
                    w->pWidget->commit_redraw();
                }
            }

            // Draw background if needed
            if ((!visible) && (force))
            {
                bg_color.copy(sBgColor);
                s->fill_rect(sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight, bg_color);
            }

//            s->wire_rect(sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight, 1, red);
        }

        LSPGrid::cell_t *LSPGrid::alloc_cell()
        {
            size_t n_cols   = vCols.size();
            if (n_cols <= 0)
                return NULL;
            size_t n_rows   = vRows.size();
            if (n_rows <= 0)
                return NULL;

            // Move iterator
            while (true)
            {
                // Get cell
                cell_t *cell = vCells.get(nCurrRow*n_cols + nCurrCol);
                if (cell == NULL)
                    return NULL;

                // Check that cell is not tagged as ignore
                if ((cell->pWidget != NULL) || (cell->nRows <= 0))
                    cell = NULL;

                // Iterate to new row and column
                if (bVertical)
                {
                    if ((++nCurrRow) >= n_rows)
                    {
                        nCurrRow = 0;
                        if ((++nCurrCol) >= n_cols)
                            return cell;
                    }
                }
                else
                {
                    if ((++nCurrCol) >= n_cols)
                    {
                        nCurrCol = 0;
                        if ((++nCurrRow) >= n_rows)
                            return cell;
                    }
                }

                // Check that cell is valid
                if (cell != NULL)
                    return cell;
            }
        }

        status_t LSPGrid::tag_cell(cell_t *c, bool main)
        {
            ssize_t idx = vCells.indexof(c);
            if (idx < 0)
                return STATUS_NOT_FOUND;

            size_t row          = idx / vCols.size();
            size_t col          = idx % vCols.size();
            ssize_t max_rows    = vRows.size() - row;
            ssize_t max_cols    = vCols.size() - col;

            if (c->nRows > max_rows)
                c->nRows        = max_rows;
            if (c->nCols > max_cols)
                c->nCols        = max_cols;
            ssize_t rc          = (main) ? 1 : -1;

            for (ssize_t j=0; j<c->nRows; ++j)
            {
                for (ssize_t i=0; i<c->nCols; ++i)
                {
                    cell_t *x   = vCells.get(idx + i);
                    if ((x != NULL) && (x != c))
                    {
                        x->nRows    = rc;
                        x->nCols    = rc;
                    }
                }
                idx    += vCols.size();
            }

            return STATUS_OK;
        }

        status_t LSPGrid::add(LSPWidget *widget)
        {
            return add(widget, 1, 1);
        }

        status_t LSPGrid::add(LSPWidget *widget, size_t rowspan, size_t colspan)
        {
            // Allocate cell
            cell_t *cell    = alloc_cell();
            if (cell == NULL)
                return STATUS_OVERFLOW;

            if (cell->pWidget != NULL)
            {
                unlink_widget(cell->pWidget);
                cell->pWidget       = NULL;
            }

            cell->nRows     = rowspan;
            cell->nCols     = colspan;
            cell->pWidget   = widget;

            if (cell->pWidget != NULL)
                cell->pWidget->set_parent(this);

            return tag_cell(cell, false);
        }

        status_t LSPGrid::remove(LSPWidget *widget)
        {
            size_t n = vCells.size();
            for (size_t i=0; i<n; ++i)
            {
                cell_t *cell    = vCells.at(i);

                if (cell->pWidget == widget)
                {
                    cell->pWidget       = NULL;
                    unlink_widget(cell->pWidget);
                    return tag_cell(cell, true);
                }
            }

            return STATUS_NOT_FOUND;
        }

        void LSPGrid::realize(const realize_t *r)
        {
            cell_t *w;
            header_t *h, *v;
            size_t hs = 0, vs = 0;
            realize_t alloc;

            size_t n_rows   = vRows.size();
            size_t n_cols   = vCols.size();
            
            // Distribute size between cells
            distribute_size(vRows, 0, n_rows, r->nHeight);
            distribute_size(vCols, 0, n_cols, r->nWidth);

            assign_coords(vRows.get_array(), n_rows, r->nTop);
            assign_coords(vCols.get_array(), n_cols, r->nLeft);

            // Now we are ready to realize cells
            w           = vCells.get_array();
            for (size_t i=0; i<n_rows; ++i)
            {
                h               = vRows.at(i);

                for (size_t j=0; j<n_cols; ++j, ++w)
                {
                    v               = vCols.at(j);

                    if ((w->nRows <= 0) || (w->nCols <= 0))
                        continue;

                    w->a.nLeft      = v->nOffset;
                    w->a.nTop       = h->nOffset;
                    w->a.nWidth     = estimate_size(vCols, j, w->nCols, &hs);
                    w->a.nHeight    = estimate_size(vRows, i, w->nRows, &vs);

                    alloc           = w->a;
                    if ((j + w->nCols) < n_cols)
                        w->a.nWidth    += hs;
                    if ((i + w->nRows) < n_rows)
                        w->a.nHeight   += vs;

                    if (hidden_widget(w))
                        continue;

                    w->s            = alloc; // Copy cell parameters to cell size attributes
                    w->s.nWidth    -= w->p.nLeft + w->p.nRight;
                    w->s.nHeight   -= w->p.nTop  + w->p.nBottom;

                    // Do not fill horizontally
                    if (!w->pWidget->hfill())
                    {
                        ssize_t nw      = (w->r.nMinWidth >= 0) ? w->r.nMinWidth : 0;
                        w->s.nLeft     += (w->s.nWidth - nw) >> 1;
                        w->s.nWidth     = nw;
                    }
                    else
                    {
                        if ((w->r.nMaxWidth >= 0) && (w->s.nWidth > w->r.nMaxWidth))
                        {
                            w->s.nLeft     +=   (w->s.nWidth - w->r.nMaxWidth) >> 1;
                            w->s.nWidth     =   w->r.nMaxWidth;
                        }
                    }

                    // Do not fill vertically
                    if (!w->pWidget->vfill())
                    {
                        ssize_t nh      = (w->r.nMinHeight >= 0) ? w->r.nMinHeight : 0;
                        w->s.nTop      += (w->s.nHeight - nh) >> 1;
                        w->s.nHeight    = nh;
                    }
                    else
                    {
                        if ((w->r.nMaxHeight >= 0) && (w->s.nHeight > w->r.nMaxHeight))
                        {
                            w->s.nTop      +=   (w->s.nHeight - w->r.nMaxHeight) >> 1;
                            w->s.nHeight    =   w->r.nMaxHeight;
                        }
                    }

                    w->s.nLeft     += w->p.nLeft;
                    w->s.nTop      += w->p.nTop;

                    // Issue realize
                    w->pWidget->realize(&w->s);
                    w->pWidget->query_draw();
                }
            }

            // Save geometry
            LSPWidgetContainer::realize(r);
        }

        void LSPGrid::size_request(size_request_t *r)
        {
            header_t *h, *v;
            cell_t *w;

            r->nMinWidth    = 0;
            r->nMinHeight   = 0;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;

            if (vCells.size() <= 0)
                return;

            size_t n_rows   = vRows.size();
            size_t n_cols   = vCols.size();

//            #ifdef LSP_TRACE
//            for (size_t i=0; i<n_rows; ++i)
//                for (size_t j=0; j<n_cols; ++j)
//                {
//                    cell_t *c = vCells.get(i*n_cols + j);
//                    lsp_trace("c[%d,%d] = %p { rows=%d, cols=%d, widget=%p }",
//                            int(i), int(j), c, int(c->nRows), int(c->nCols), c->pWidget);
//                }
//            #endif /* LSP_TRACE */

            // Perform size request for all visible child widgets
            size_t n_cells = vCells.size();
            for (size_t i=0; i<n_cells; ++i)
            {
                // Get widget
                w           = vCells.get(i);
                if ((w->nRows <= 0) || (hidden_widget(w)))
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
            }

            // Estimate minimum size of each row by using 1xM (single) cells
            for (size_t i=0; i<n_rows; ++i)
            {
                h           = vRows.get(i);
                h->nSize    = 0;
                h->nSpacing = 0;
                h->nOffset  = 0;
                h->bExpand  = false;
            }
            for (size_t i=0; i<n_cols; ++i)
            {
                h           = vCols.get(i);
                h->nSize    = 0;
                h->nSpacing = 0;
                h->nOffset  = 0;
                h->bExpand  = false;
            }

            // Estimate minimum row/column size for 1xN and Mx1 cells
            w           = vCells.get_array();
            for (size_t i=0; i<n_rows; ++i)
            {
                h          = vRows.get(i);
                for (size_t j=0; j<n_cols; ++j, ++w)
                {
                    v           = vCols.get(j);
                    if (hidden_widget(w))
                        continue;

                    if (w->nRows == 1)
                    {
                        ssize_t space   = w->p.nTop + w->p.nBottom;
                        if (w->r.nMinHeight >= 0)
                            space          += w->r.nMinHeight;
                        if (h->nSize < space)
                            h->nSize        = space;
                        h->nSpacing     = nVSpacing;
                    }
                    if (w->nCols == 1)
                    {
                        ssize_t space   = w->p.nLeft + w->p.nRight;
                        if (w->r.nMinWidth >= 0)
                            space          += w->r.nMinWidth;
                        if (v->nSize < space)
                            v->nSize    = space;
                        v->nSpacing     = nHSpacing;
                    }
                }
            }

            // Estimate minimum row/column size for N x M cells
            w           = vCells.get(0);
            for (size_t i=0; i<n_rows; ++i)
            {
                h          = vRows.get(i);
                for (size_t j=0; j<n_cols; ++j, ++w)
                {
                    v           = vCols.get(j);
                    if (hidden_widget(w))
                        continue;

                    if (w->nRows > 1)
                    {
                        ssize_t space   = w->p.nTop + w->p.nBottom;
                        if (w->r.nMinHeight >= 0)
                            space          += space;
                        distribute_size(vRows, i, w->nRows, space);
                    }
                    if (w->nCols > 1)
                    {
                        ssize_t space   = w->p.nLeft + w->p.nRight;
                        if (w->r.nMinWidth >= 0)
                            space          += w->r.nMinWidth;
                        distribute_size(vCols, j, w->nCols, space);
                    }
                }
            }

            // Mark some rows/cols as expanded
            for (size_t i=0, n=vCells.size(); i<n; ++i)
            {
                w           = vCells.get(i);
                if (hidden_widget(w))
                    continue;
                if (!w->pWidget->expand())
                    continue;
                size_t row = i / n_cols;
                size_t col = i % n_cols;
                for (ssize_t i=0; i<w->nRows; ++i)
                    vRows.get(row + i)->bExpand = true;
                for (ssize_t i=0; i<w->nCols; ++i)
                    vCols.get(col + i)->bExpand = true;
            }

            // Calculate the size of table
            r->nMinHeight  += estimate_size(vRows, 0, n_rows, NULL);
            r->nMinWidth   += estimate_size(vCols, 0, n_cols, NULL);
            for (size_t i=0; i<n_rows; ++i)
            {
                h   = vRows.at(i);
                h->nMinSize   = h->nSize;
            }
            for (size_t i=0; i<n_cols; ++i)
            {
                h   = vCols.at(i);
                h->nMinSize   = h->nSize;
            }
//            lsp_trace("MinWidth = %d, MinHeight=%d", int(r->nMinWidth), int(r->nMinHeight));
        }
    
    } /* namespace tk */
} /* namespace lsp */
