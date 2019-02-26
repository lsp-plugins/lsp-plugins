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

    status_t rt_plan_t::split_out(const vector3d_t *pl)
    {
        rt_plan_t tmp;
        float k[2];

        RT_FOREACH(rt_split_t, s, items)
            if (s->flags & SF_REMOVE) // Do not analyze the edge, it will be automatically removed
                continue;

            k[0] = s->p[0].x * pl->dx + s->p[0].y * pl->dy + s->p[0].z*pl->dz + pl->dw;
            k[1] = s->p[0].x * pl->dx + s->p[0].y * pl->dy + s->p[0].z*pl->dz + pl->dw;

            if (k[0] <= -DSP_3D_TOLERANCE) // p[0] is under the plane
            {
                if (k[1] >= DSP_3D_TOLERANCE) // p[1] is over the plane, cut p[1]
                    dsp::calc_split_point_pvv1(&s->p[1], s->p, pl);
                if (!tmp.items.alloc(s))
                    return STATUS_NO_MEM;
            }
            else if (k[0] >= DSP_3D_TOLERANCE) // p[0] is over the plane
            {
                if (k[1] <= DSP_3D_TOLERANCE) // p[1] is under the plane, cut p[0]
                    dsp::calc_split_point_pvv1(&s->p[0], s->p, pl);
                if (!tmp.items.alloc(s))
                    return STATUS_NO_MEM;
            }
            else // consider p[0] lays on the plane
            {
                if ((k[1] > -DSP_3D_TOLERANCE) && (k[1] < DSP_3D_TOLERANCE)) // p[1] also lies on the plane?
                    s->flags   |= SF_APPLIED;
            }
        RT_FOREACH_END

        tmp.swap(this);
        return STATUS_OK;
    }

    status_t rt_plan_t::split_in(const vector3d_t *pl)
    {
        rt_plan_t tmp;
        float k[2];

        RT_FOREACH(rt_split_t, s, items)
            if (s->flags & SF_REMOVE) // Do not analyze the edge, it will be automatically removed
                continue;

            k[0] = s->p[0].x * pl->dx + s->p[0].y * pl->dy + s->p[0].z*pl->dz + pl->dw;
            k[1] = s->p[0].x * pl->dx + s->p[0].y * pl->dy + s->p[0].z*pl->dz + pl->dw;

            if (k[0] <= -DSP_3D_TOLERANCE) // p[0] is under the plane
            {
                if (k[1] >= DSP_3D_TOLERANCE) // p[1] is over the plane, cut p[0]
                    dsp::calc_split_point_pvv1(&s->p[0], s->p, pl);
                if (!tmp.items.alloc(s))
                    return STATUS_NO_MEM;
            }
            else if (k[0] >= DSP_3D_TOLERANCE) // p[0] is over the plane
            {
                if (k[1] <= DSP_3D_TOLERANCE) // p[1] is under the plane, cut p[1]
                    dsp::calc_split_point_pvv1(&s->p[1], s->p, pl);
                if (!tmp.items.alloc(s))
                    return STATUS_NO_MEM;
            }
            else // consider p[0] lays on the plane
            {
                if ((k[1] > -DSP_3D_TOLERANCE) && (k[1] < DSP_3D_TOLERANCE)) // p[1] also lies on the plane?
                    s->flags   |= SF_APPLIED;
            }
        RT_FOREACH_END

        tmp.swap(this);
        return STATUS_OK;
    }

    status_t rt_plan_t::split(rt_plan_t *out, const vector3d_t *pl)
    {
        rt_plan_t xin, xout;

        point3d_t sp;
        rt_split_t *si, *so;
        float k[2];

        RT_FOREACH(rt_split_t, s, items)
            k[0] = s->p[0].x * pl->dx + s->p[0].y * pl->dy + s->p[0].z*pl->dz + pl->dw;
            k[1] = s->p[0].x * pl->dx + s->p[0].y * pl->dy + s->p[0].z*pl->dz + pl->dw;

            if (k[0] <= -DSP_3D_TOLERANCE) // p[0] is under the plane
            {
                if (k[1] >= DSP_3D_TOLERANCE) // p[1] is over the plane, perform split
                {
                    si          = xin.items.alloc();
                    so          = xout.items.alloc();
                    if ((!si) || (!so))
                        return STATUS_NO_MEM;

                    dsp::calc_split_point_p2v1(&sp, &s->p[0], &s->p[1], pl);

                    si->p[0]    = s->p[0];
                    si->p[1]    = sp;
                    si->flags   = s->flags;

                    so->p[0]    = sp;
                    so->p[1]    = s->p[1];
                    so->flags   = s->flags;
                }
                else // Edge is under the plane, just copy to xin
                {
                    if (!xin.items.alloc(s))
                        return STATUS_NO_MEM;
                }
            }
            else if (k[0] >= DSP_3D_TOLERANCE) // p[0] is over the plane
            {
                if (k[1] <= DSP_3D_TOLERANCE) // p[1] is under the plane, perform split
                {
                    si          = xin.items.alloc();
                    so          = xout.items.alloc();
                    if ((!si) || (!so))
                        return STATUS_NO_MEM;

                    dsp::calc_split_point_p2v1(&sp, &s->p[0], &s->p[1], pl);

                    si->p[0]    = sp;
                    si->p[1]    = s->p[1];
                    si->flags   = s->flags;

                    so->p[0]    = s->p[0];
                    so->p[1]    = sp;
                    so->flags   = s->flags;
                }
                else // Edge is over the plane, just copy to xout
                {
                    if (!xout.items.alloc(s))
                        return STATUS_NO_MEM;
                }
            }
            else // consider p[0] lays on the plane
            {
                if (k[1] <= -DSP_3D_TOLERANCE) // p[1] is under the plane
                {
                    if (!xin.items.alloc(s))
                        return STATUS_NO_MEM;
                }
                else if (k[1] >= DSP_3D_TOLERANCE) // p[1] is over the plane
                {
                    if (!xout.items.alloc(s))
                        return STATUS_NO_MEM;
                }
                else if (s->flags & SF_CULLBACK) // Edge lays on the plane, keep it only if has a SF_CULLBACK flag
                {
                    si          = xin.items.alloc();
                    so          = xout.items.alloc();
                    if ((!si) || (!so))
                        return STATUS_NO_MEM;

                    si->p[0]    = s->p[0];
                    si->p[1]    = s->p[1];
                    si->flags   = s->flags | SF_APPLIED;

                    so->p[0]    = s->p[0];
                    so->p[1]    = s->p[1];
                    so->flags   = s->flags | SF_APPLIED;
                }
            }
        RT_FOREACH_END

        // Swap contents
        xin.swap(this);
        xout.swap(out);

        return STATUS_OK;
    }

    status_t rt_plan_t::add_triangle(const point3d_t *pv, const vector3d_t *sp)
    {
        rt_split_t *asp[3];
        if (items.alloc_n(asp, 3) != 3)
            return STATUS_NO_MEM;

        asp[0]->p[0]    = pv[0];
        asp[0]->p[1]    = pv[1];
        asp[0]->sp      = *sp;
        asp[0]->flags   = 0;

        asp[1]->p[0]    = pv[1];
        asp[1]->p[1]    = pv[2];
        asp[1]->sp      = *sp;
        asp[1]->flags   = 0;

        asp[2]->p[0]    = pv[2];
        asp[2]->p[1]    = pv[0];
        asp[2]->sp      = *sp;
        asp[2]->flags   = SF_CULLBACK;      // After split of this edge, we need to perform a cull-back

        return STATUS_OK;
    }

    void rt_plan_t::swap(rt_plan_t *dst)
    {
        items.swap(&dst->items);
    }
}

