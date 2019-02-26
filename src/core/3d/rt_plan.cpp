/*
 * rt_plan.cpp
 *
 *  Created on: 26 февр. 2019 г.
 *      Author: sadko
 */

#include <core/3d/rt_plan.h>

#define RT_FOREACH(type, var, collection) \
    for (size_t __ci=0,__ne=collection.size(), __nc=collection.chunks(); (__ci<__nc) && (__ne>0); ++__ci) \
    { \
        type *var       = collection.chunk(__ci); \
        size_t __loops  = collection.chunk_size(); \
        if (__loops > __ne) __loops = __ne; \
        __ne -= __loops; \
        for ( ; __loops > 0; ++var, --__loops) \
        {

#define RT_FOREACH_BREAK    { __ne = 0; break; }

#define RT_FOREACH_END      } }

namespace lsp
{
    rt_plan_t::rt_plan_t():
        items(1024)
    {
    }

    rt_plan_t::~rt_plan_t()
    {
        items.flush();
    }

    status_t rt_plan_t::perform_split(const vector3d_t *pl)
    {
        point3d_t sp;
        rt_split_t *sx;
        float k[2];

        RT_FOREACH(rt_split_t, s, items)
            k[0] = s->p[0].x * pl->dx + s->p[0].y * pl->dy + s->p[0].z*pl->dz + pl->dw;
            k[1] = s->p[0].x * pl->dx + s->p[0].y * pl->dy + s->p[0].z*pl->dz + pl->dw;

            if (k[0] <= -DSP_3D_TOLERANCE) // p[0] is under the plane
            {
                if (k[1] >= DSP_3D_TOLERANCE) // p[1] is over the plane
                {
                    sx          = items.alloc();
                    if (sx == NULL)
                        return STATUS_NO_MEM;

                    dsp::calc_split_point_p2v1(&sp, &s->p[0], &s->p[1], pl);

                    sx->p[0]    = sp;
                    sx->p[1]    = s->p[1];
                    sx->itag    = 2;
                    s->p[1]     = sp;
                    s->itag     = 0;
                }
                else
                    s->itag     = 2; // Edge is under the plane
            }
            else if (k[0] >= DSP_3D_TOLERANCE) // p[0] is over the plane
            {
                if (k[1] <= DSP_3D_TOLERANCE) // p[1] is under the plane
                {
                    sx          = items.alloc();
                    if (sx == NULL)
                        return STATUS_NO_MEM;

                    dsp::calc_split_point_p2v1(&sp, &s->p[0], &s->p[1], pl);

                    sx->p[0]    = sp;
                    sx->p[1]    = s->p[1];
                    sx->itag    = 0;
                    s->p[1]     = sp;
                    s->itag     = 2;
                }
                else
                    s->itag     = 2; // Edge is under the plane
            }
            else // consider p[0] lays on the plane
            {
                if (k[1] <= -DSP_3D_TOLERANCE)
                    s->itag     = 2;
                else if (k[1] >= DSP_3D_TOLERANCE)
                    s->itag     = 0;
                else
                    s->itag     = 1;
            }
        RT_FOREACH_END

        return STATUS_OK;
    }

    status_t rt_plan_t::split_out(const vector3d_t *pl)
    {
        status_t res = perform_split(pl);
        if (res != STATUS_OK)
            return res;

        rt_plan_t tmp;
        RT_FOREACH(rt_split_t, s, items)
            if (s->itag != 2)
                continue;
            if (tmp.items.alloc(s) == NULL)
                return STATUS_NO_MEM;
        RT_FOREACH_END;

        tmp.swap(this);
        return STATUS_OK;
    }

    status_t rt_plan_t::split_in(const vector3d_t *pl)
    {
        status_t res = perform_split(pl);
        if (res != STATUS_OK)
            return res;

        rt_plan_t tmp;
        RT_FOREACH(rt_split_t, s, items)
            if (s->itag != 0)
                continue;
            if (tmp.items.alloc(s) == NULL)
                return STATUS_NO_MEM;
        RT_FOREACH_END;

        tmp.swap(this);
        return STATUS_OK;
    }

    status_t rt_plan_t::split(rt_plan_t *out, const vector3d_t *pl)
    {
        status_t res = perform_split(pl);
        if (res != STATUS_OK)
            return res;

        rt_plan_t xin, xout;
        RT_FOREACH(rt_split_t, s, items)
            if (s->itag == 2)
            {
                if (xin.items.alloc(s) == NULL)
                    return STATUS_NO_MEM;
            }
            else if (s->itag == 0)
            {
                if (xout.items.alloc(s) == NULL)
                    return STATUS_NO_MEM;
            }
        RT_FOREACH_END;

        xin.swap(this);
        xout.swap(out);

        return STATUS_OK;
    }

    status_t rt_plan_t::add_triangle(const point3d_t *pv)
    {
        rt_split_t *asp[3];
        if (items.alloc_n(asp, 3) != 3)
            return STATUS_NO_MEM;

        asp[0]->p[0]    = pv[0];
        asp[0]->p[1]    = pv[1];
        asp[0]->itag    = 0;

        asp[1]->p[0]    = pv[1];
        asp[1]->p[1]    = pv[2];
        asp[1]->itag    = 0;

        asp[2]->p[0]    = pv[2];
        asp[2]->p[1]    = pv[0];
        asp[2]->itag    = 0;

        return STATUS_OK;
    }

    status_t rt_plan_t::add_edge(const point3d_t *pv)
    {
        rt_split_t *asp = items.alloc();
        if (asp == NULL)
            return STATUS_NO_MEM;

        asp->p[0]       = pv[0];
        asp->p[1]       = pv[1];
        asp->itag       = 0;

        return STATUS_OK;
    }

    void rt_plan_t::swap(rt_plan_t *dst)
    {
        items.swap(&dst->items);
    }
}

