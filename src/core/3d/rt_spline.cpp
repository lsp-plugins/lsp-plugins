/*
 * rt_spline.cpp
 *
 *  Created on: 25 мар. 2019 г.
 *      Author: sadko
 */

#include <core/3d/rt_spline.h>

namespace lsp
{
    rt_spline_t::rt_spline_t()
    {
        start   = NULL;
        end     = NULL;
    }

    rt_spline_t::~rt_spline_t()
    {
        start   = NULL;
        end     = NULL;
        edges.flush();
    }

    void rt_spline_t::swap(rt_spline_t *dst)
    {
        ::swap(start, dst->start);
        ::swap(end, dst->end);
        edges.swap(&dst->edges);
    }

    status_t rt_spline_t::copy(rt_spline_t *src)
    {
        cstorage<rtm_edge_t> xedges;
        for (size_t i=0, n=src->edges.size(); i<n; ++i)
        {
            rtm_edge_t *e = xedges.add(edges.at(i));
            if (e == NULL)
                return STATUS_NO_MEM;
            e->vlnk[0]  = NULL;
            e->vlnk[1]  = NULL;
        }
        edges.swap(&xedges);
        start       = src->start;
        end         = src->end;
        return STATUS_OK;
    }

    void rt_spline_t::reverse()
    {
        ::swap(start, end);
        rtm_edge_t **e = edges.get_array();
        for (size_t i=0, n=edges.size(); i<n; ++i)
            ::swap(e[i]->v[0], e[i]->v[1]);
    }

    status_t rt_spline_t::add(rtm_edge_t *edge)
    {
        rtm_edge_t *e;

        // Empty state?
        if ((start == NULL) && (end == NULL))
        {
            if (!(e = edges.add(edge)))
                return STATUS_NO_MEM;
            start   = e->v[0];
            end     = e->v[1];
            return STATUS_OK;
        }

        // Analyze edge state
        if (end == edge->v[0]) // [start ... end] + [v[0] .. v[1]]
        {
            if (!(e = edges.add(edge)))
                return STATUS_NO_MEM;
            end     = e->v[1];
            if (start == NULL)
                start   = e->v[0];
        }
        else if (end == edge->v[1]) // [start ... end] + [v[1] .. v[0]]
        {
            if (!edges.add(edge))
                return STATUS_NO_MEM;

            // Change orientation of the edge
            ::swap(e->v[0], e->v[1]);

            end     = e->v[1];
            if (start == NULL)
                start   = e->v[0];
        }
        else if (start == edge->v[0]) // [v[1] .. v[0]] + [start ... end]
        {
            if (!(e = edges.insert(0, edge)))
                return STATUS_NO_MEM;

            // Change orientation of the edge
            ::swap(e->v[0], e->v[1]);

            start       = e->v[0];
            if (end == NULL)
                end     = e->v[1];
        }
        else if (start == edge->v[1]) // [v[0] .. v[1]] + [start ... end]
        {
            if (!(e = edges.insert(0, edge)))
                return STATUS_NO_MEM;
            start       = e->v[0];
            if (end == NULL)
                end     = e->v[1];
        }
        else
            return STATUS_INVALID_VALUE;

        e->vlnk[0]  = NULL;
        e->vlnk[1]  = NULL;
        e->ptag     = edge;

        return (end == start) ? STATUS_CLOSED : STATUS_OK;
    }

    status_t rt_spline_t::test(rtm_triangle_t *t)
    {
        size_t n = edges.size();
        if (n < 3)
            return STATUS_BAD_STATE;

        // Check size of spline
        rtm_triangle_t tt;
        tt = *t;

        for (size_t i=0; i<n; ++i)
        {
            rtm_edge_t *e = edges.at(i);

            if ((e->v[0] == tt.v[1]) && (e->v[1] == tt.v[2])) // edge matches edge 1 of triangle
            {
                // Rotate clockwise
                rtm_vertex_t *vt = tt.v[0];
                tt.v[0]     = tt.v[1];
                tt.v[1]     = tt.v[2];
                tt.v[2]     = vt;

                rtm_edge_t *et = tt.e[0];
                tt.e[0]     = tt.e[1];
                tt.e[1]     = tt.e[2];
                tt.e[2]     = et;
            }
            else if ((e->v[0] == tt.v[2]) && (e->v[1] == tt.v[0])) // edge matches edge 2 of triangle
            {
                // Rotate counter-clockwise
                rtm_vertex_t *vt = tt.v[0];
                tt.v[0]     = tt.v[2];
                tt.v[2]     = tt.v[1];
                tt.v[1]     = vt;

                rtm_edge_t *et = tt.e[0];
                tt.e[0]     = tt.e[2];
                tt.e[2]     = tt.e[1];
                tt.e[1]     = et;
            }
            else if ((e->v[0] != tt.v[0]) || (e->v[1] != tt.v[1])) // edge matches edge 0 of triangle
                continue;

            // Now edge 0 of tt is guaranteed to be matching the current edge
            // Perform heuristics
//            size_t i1       =
//            rtm_edge_t *e1  = edges.at((i + 1) % n);
//            if (e1->v[1] == tt.v[2])
//            {
//                rtm_edge_t *e2  = edges.at((i + 2) % n);
//                if (e2->v[1] == tt.v[0])
//                {
//
//                }
//            }
//
//            if ((i+1) < n)
//            {
//                rtm_edge_t *ne  = edges.at(i+1);
//                if ((ne->v[1] == tt.v[1]) || (ne->v[1] == tt.v[0]))
//                    return STATUS_CORRUPTED;
//                else if (ne->v[1] == tt.v[2]) // next edge matches the triangle's edge
//                {
//                    e->v[1]     = tt.v[2];
//                    edges.remove(i+1);  // Remove the edge
//                    continue;
//                }
//            }
//
//            // Only one edge matches the triangle, we need to replace it and add new one
//            e->v[1]     = tt.v[2];
        }

        return STATUS_NOT_FOUND;
    }

}


