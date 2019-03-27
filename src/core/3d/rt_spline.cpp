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
        edge.flush();
    }

    void rt_spline_t::clear()
    {
        start   = NULL;
        end     = NULL;
        edge.flush();
    }

    /**
     * Take all data from source spline and clear source spline
     * @param src source spline
     */
    void rt_spline_t::take(rt_spline_t *src)
    {
        start   = src->start;
        end     = src->end;
        edge.take_from(&src->edge);
    }

    void rt_spline_t::swap(rt_spline_t *dst)
    {
        ::swap(start, dst->start);
        ::swap(end, dst->end);
        edge.swap_data(&dst->edge);
    }

    status_t rt_spline_t::copy(rt_spline_t *src)
    {
        cvector<rtm_edge_t> xv;
        for (size_t i=0, n=src->edge.size(); i<n; ++i)
        {
            if (!xv.add(edge.at(i)))
                return STATUS_NO_MEM;
        }
        edge.swap_data(&xv);
        start       = src->start;
        end         = src->end;
        return STATUS_OK;
    }

    void rt_spline_t::reverse()
    {
        ::swap(start, end);

        for (size_t i=0, n=edge.size(); i<n; ++i)
        {
            rtm_edge_t *ce = edge.at(i);
            ::swap(ce->v[0], ce->v[1]);
        }
    }

    void rt_spline_t::arrange()
    {
        size_t n = edge.size();
        if (n <= 1)
            return;

        rtm_edge_t *e = edge.get(0);
        for (size_t i=1; i<n; ++i)
        {
            rtm_edge_t *ne = edge.get(i);
            if (ne->v[1] == e->v[1]) // Need to reorder edge?
                ::swap(ne->v[0], ne->v[1]);
            e = ne;
        }
    }

    status_t rt_spline_t::link(rt_spline_t *src)
    {
        if ((closed()) || (src->closed()))
            return STATUS_FAILED;

        // Need to reverse source spline?
        if ((start == src->start) && (end == src->end))
            src->reverse();

        // The source spline continues current spline?
        if ((start == src->end) && (end == src->start))
        {
            for (size_t i=0, n=src->edge.size(); i<n; ++i)
            {
                rtm_edge_t *e = src->edge.at(i);
                if (!edge.add(e))
                    return STATUS_NO_MEM;
            }

            start = end;
            return STATUS_CLOSED;
        }

        return STATUS_FAILED;
    }

    status_t rt_spline_t::add(rtm_edge_t *e)
    {
        // Empty state?
        if ((start == NULL) && (end == NULL))
        {
            if (!edge.add(e))
                return STATUS_NO_MEM;
            start   = e->v[0];
            end     = e->v[1];
            return STATUS_OK;
        }
        else if (closed()) // Allow append only to non-closed splines
            return STATUS_FAILED;

        // Check for duplicated edges
        if (contains(e))
            return STATUS_DUPLICATED;

        // Analyze edge state
        if (end == e->v[0]) // [start ... end] + [v[0] .. v[1]]
        {
            if (!edge.add(e))
                return STATUS_NO_MEM;
            end = e->v[1];
        }
        else if (end == e->v[1]) // [start ... end] + [v[1] .. v[0]]
        {
            if (!edge.add(e))
                return STATUS_NO_MEM;
            ::swap(e->v[0], e->v[1]);
            end = e->v[1];
        }
        else if (start == e->v[0]) // [v[1] .. v[0]] + [start ... end]
        {
            if (!edge.insert(e, 0))
                return STATUS_NO_MEM;
            ::swap(e->v[0], e->v[1]);
            start = e->v[0];
        }
        else if (start == e->v[1]) // [v[0] .. v[1]] + [start ... end]
        {
            if (!edge.insert(e, 0))
                return STATUS_NO_MEM;
            start = e->v[0];
        }
        else
            return STATUS_INVALID_VALUE;

        return (end == start) ? STATUS_CLOSED : STATUS_OK;
    }

    bool rt_spline_t::contains(rtm_edge_t *e)
    {
        for (size_t i=0, n=edge.size(); i<n; ++i)
        {
            rtm_edge_t *ce = edge.at(i);
            if (e == ce)
                return true;
            if ((e->v[0] == ce->v[0]) && (e->v[1] == ce->v[1]))
                return true;
            if ((e->v[0] == ce->v[1]) && (e->v[1] == ce->v[0]))
                return true;
        }

        return false;
    }

}


