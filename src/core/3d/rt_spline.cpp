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
        vertex.flush();
    }

    void rt_spline_t::swap(rt_spline_t *dst)
    {
        ::swap(start, dst->start);
        ::swap(end, dst->end);
        vertex.swap_data(&dst->vertex);
    }

    status_t rt_spline_t::copy(rt_spline_t *src)
    {
        cvector<rtm_vertex_t> xv;
        for (size_t i=0, n=src->vertex.size(); i<n; ++i)
        {
            if (!xv.add(vertex.at(i)))
                return STATUS_NO_MEM;
        }
        vertex.swap_data(&xv);
        start       = src->start;
        end         = src->end;
        return STATUS_OK;
    }

    void rt_spline_t::reverse()
    {
        ::swap(start, end);
        rtm_vertex_t **first = vertex.get_array();
        rtm_vertex_t **last  = &first[vertex.size()-1];

        while (first < last)
            ::swap(*(first++), *(last--));
    }

    status_t rt_spline_t::link(rt_spline_t *src)
    {
        status_t res;
        rt_spline_t tmp;

        if ((closed()) || (src->closed()))
            return STATUS_FAILED;

        if ((start == src->start) || (start == src->end))
        {
            res = tmp.copy(src);
            if (res != STATUS_OK)
                return res;
            if (start == src->start)
                tmp.reverse();

            rtm_vertex_t *v0 = vertex.at(0);
            for (size_t i=1, n=vertex.size(); i<n; ++i)
            {
                if (res == STATUS_CLOSED)
                    return STATUS_FAILED;
                rtm_vertex_t *v1 = vertex.at(i);
                res = tmp.add(v0, v1);
                if ((res != STATUS_OK) && (res != STATUS_CLOSED))
                    return res;
                v0 = v1;
            }
        }
        else if ((end == src->start) || (end == src->end))
        {
            res = tmp.copy(this);
            if (res != STATUS_OK)
                return res;
            if (end == src->end)
                tmp.reverse();

            rtm_vertex_t *v0 = src->vertex.at(0);
            for (size_t i=1, n=src->vertex.size(); i<n; ++i)
            {
                if (res == STATUS_CLOSED)
                    return STATUS_FAILED;
                rtm_vertex_t *v1 = src->vertex.at(i);
                res = tmp.add(v0, v1);
                if ((res != STATUS_OK) && (res != STATUS_CLOSED))
                    return res;
                v0 = v1;
            }
        }
        else
            return STATUS_FAILED;

        if ((res == STATUS_OK) || (res == STATUS_CLOSED))
            tmp.swap(this);

        return res;
    }

    status_t rt_spline_t::add(rtm_vertex_t *v0, rtm_vertex_t *v1)
    {
        rtm_edge_t *e;

        // Empty state?
        if ((start == NULL) && (end == NULL))
        {
            if (!vertex.add(v0))
                return STATUS_NO_MEM;
            if (!vertex.add(v1))
            {
                vertex.clear();
                return STATUS_NO_MEM;
            }
            return STATUS_OK;
        }
        else if (closed()) // Allow append only to non-closed splines
            return STATUS_FAILED;

        // Check for duplicated edges
        rtm_vertex_t *xv0 = vertex.at(0);
        for (size_t i=1, n=vertex.size(); i<n; ++i)
        {
            rtm_vertex_t *xv1 = vertex.at(i);
            if ((xv0 == v0) && (xv1 == v1))
                return STATUS_DUPLICATED;
            if ((xv0 == v1) && (xv1 == v0))
                return STATUS_DUPLICATED;
        }

        // Analyze edge state
        if (end == v0) // [start ... end] + [v[0] .. v[1]]
        {
            if (start == v1)
            {
                end = start;
                return STATUS_CLOSED;
            }
            else if (!vertex.add(v1))
                return STATUS_NO_MEM;
        }
        else if (end == v1) // [start ... end] + [v[1] .. v[0]]
        {
            if (start == v0)
            {
                end = start;
                return STATUS_CLOSED;
            }
            else if (!vertex.add(v0))
                return STATUS_NO_MEM;
        }
        else if (start == v0) // [v[1] .. v[0]] + [start ... end]
        {
            if (end == v1)
            {
                start = end;
                return STATUS_CLOSED;
            }
            else if (!vertex.insert(v1, 0))
                return STATUS_NO_MEM;
        }
        else if (start == v1) // [v[0] .. v[1]] + [start ... end]
        {
            if (end == v0)
            {
                start = end;
                return STATUS_CLOSED;
            }
            else if (!vertex.insert(v0, 0))
                return STATUS_NO_MEM;
        }
        else
            return STATUS_INVALID_VALUE;

        return (end == start) ? STATUS_CLOSED : STATUS_OK;
    }

    status_t rt_spline_t::test(rtm_triangle_t *t)
    {
#if 0 // TODO
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
#endif

        return STATUS_NOT_FOUND;
    }

}


