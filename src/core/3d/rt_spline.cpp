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

    void rt_spline_t::clear()
    {
        start   = NULL;
        end     = NULL;
        vertex.flush();
    }

    /**
     * Take all data from source spline and clear source spline
     * @param src source spline
     */
    void rt_spline_t::take(rt_spline_t *src)
    {
        start   = src->start;
        end     = src->end;
        vertex.take_from(&src->vertex);
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

    ssize_t rt_spline_t::find_matching_edge(rtm_vertex_t *v0, rtm_vertex_t *v1)
    {
        size_t n = vertex.size();

        rtm_vertex_t *x0 = vertex.at(0);
        for (size_t i=1; i<=n; ++i)
        {
            rtm_vertex_t *x1 = vertex.at((i < n) ? i : i-n);
            if ((x0 == v0) && (x1 == v1))
                return i-1;
        }

        return -1;
    }

    status_t rt_spline_t::test(rtm_triangle_t *t)
    {
        // Check state of spline
        if (!closed())
            return STATUS_BAD_STATE;
        size_t n = vertex.size();
        if (n < 3)
            return STATUS_BAD_STATE;

        ssize_t ei[3], ni = 0;
        if ((ei[ni] = find_matching_edge(t->v[0], t->v[1])) >= 0)
            ++ni;
        if ((ei[ni] = find_matching_edge(t->v[1], t->v[2])) >= 0)
            ++ni;
        if ((ei[ni] = find_matching_edge(t->v[2], t->v[0])) >= 0)
            ++ni;



        return STATUS_NOT_FOUND;
    }

}


