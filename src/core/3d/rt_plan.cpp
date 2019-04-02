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

    status_t rt_plan_t::cut_out(const vector3d_t *pl)
    {
        rt_plan_t tmp;
        rt_split_t *sp;

        RT_FOREACH(rt_split_t, s, items)
            if (s->flags & SF_REMOVE) // Do not analyze the edge, it will be automatically removed
                continue;

            size_t tag = dsp::colocation_x2_v1pv(pl, s->p);

            switch (tag)
            {
                case 0x06: // 1 2
                case 0x09: // 2 1
                case 0x0a: // 2 2
                    if (!tmp.items.alloc(s)) // Just copy segment to output
                        return STATUS_NO_MEM;
                    break;

                case 0x02: // 0 2 -- p[0] is under the plane, p[1] is over the plane, cut p[1]
                    if (!(sp = tmp.items.alloc(s)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_pvv1(&sp->p[1], sp->p, pl);
                    break;

                case 0x08: // 2 0 -- p[1] is under the plane, p[0] is over the plane, cut p[0]
                    if (!(sp = tmp.items.alloc(s)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_pvv1(&sp->p[0], sp->p, pl);
                    break;

                default:
                    break;
            }
        RT_FOREACH_END

        tmp.swap(this);
        return STATUS_OK;
    }

    status_t rt_plan_t::cut_in(const vector3d_t *pl)
    {
        rt_plan_t tmp;
        rt_split_t *sp;

        RT_FOREACH(rt_split_t, s, items)
            if (s->flags & SF_REMOVE) // Do not analyze the edge, it will be automatically removed
                continue;

            size_t tag = dsp::colocation_x2_v1pv(pl, s->p);

            switch (tag)
            {
                case 0x04: // 1 0
                case 0x01: // 0 1
                case 0x00: // 0 0
                    if (!tmp.items.alloc(s)) // Just copy segment to output
                        return STATUS_NO_MEM;
                    break;

                case 0x02: // 0 2 -- p[0] is under the plane, p[1] is over the plane, cut p[0]
                    if (!(sp = tmp.items.alloc(s)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_pvv1(&sp->p[0], sp->p, pl);
                    break;

                case 0x08: // 2 0 -- p[1] is under the plane, p[0] is over the plane, cut p[1]
                    if (!(sp = tmp.items.alloc(s)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_pvv1(&sp->p[1], sp->p, pl);
                    break;

                default:
                    break;
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

        RT_FOREACH(rt_split_t, s, items)
            size_t tag = dsp::colocation_x2_v1pv(pl, s->p);

            switch (tag)
            {
                case 0x00: // 0 0
                case 0x01: // 0 1
                case 0x04: // 1 0
                    // Edge is over the plane
                    if (!xout.items.alloc(s))
                        return STATUS_NO_MEM;
                    break;

                case 0x06: // 1 2
                case 0x09: // 2 1
                case 0x0a: // 2 2
                    // Edge is under the plane
                    if (!xin.items.alloc(s))
                        return STATUS_NO_MEM;
                    break;

                case 0x02: // 0 2
                    // p[1] is over the plane, p[0] is under
                    si          = xin.items.alloc();
                    so          = xout.items.alloc();
                    if ((!si) || (!so))
                        return STATUS_NO_MEM;

                    dsp::calc_split_point_pvv1(&sp, s->p, pl);
                    si->p[0]    = s->p[0];
                    si->p[1]    = sp;
                    si->flags   = s->flags;

                    so->p[0]    = sp;
                    so->p[1]    = s->p[1];
                    so->flags   = s->flags;
                    break;

                case 0x08: // 2 0
                    // p[0] is over the plane, p[1] is under
                    si          = xin.items.alloc();
                    so          = xout.items.alloc();
                    if ((!si) || (!so))
                        return STATUS_NO_MEM;

                    dsp::calc_split_point_pvv1(&sp, s->p, pl);

                    si->p[0]    = sp;
                    si->p[1]    = s->p[1];
                    si->flags   = s->flags;

                    so->p[0]    = s->p[0];
                    so->p[1]    = sp;
                    so->flags   = s->flags;
                    break;

                default:
                    break;
            }
        RT_FOREACH_END

        // Swap contents
        xin.swap(this);
        xout.swap(out);

        return STATUS_OK;
    }

//    status_t rt_plan_t::add_triangle(const rtm_triangle_t *t, const vector3d_t *sp)
    status_t rt_plan_t::add_triangle(const rtm_triangle_t *t)
    {
        rt_split_t *asp[3];
        if (items.alloc_n(asp, 3) != 3)
            return STATUS_NO_MEM;

        asp[0]->p[0]    = *(t->v[0]);
        asp[0]->p[1]    = *(t->v[1]);
//        asp[0]->sp      = *sp;
        asp[0]->flags   = 0;

        asp[1]->p[0]    = *(t->v[1]);
        asp[1]->p[1]    = *(t->v[2]);
//        asp[1]->sp      = *sp;
        asp[1]->flags   = 0;

        asp[2]->p[0]    = *(t->v[2]);
        asp[2]->p[1]    = *(t->v[0]);
//        asp[2]->sp      = *sp;
        asp[2]->flags   = 0; //SF_CULLBACK;      // After split of this edge, we need to perform a cull-back

        return STATUS_OK;
    }

//    status_t rt_plan_t::add_triangle(const point3d_t *pv, const vector3d_t *sp)
    status_t rt_plan_t::add_triangle(const point3d_t *pv)
    {
        rt_split_t *asp[3];
        if (items.alloc_n(asp, 3) != 3)
            return STATUS_NO_MEM;

        asp[0]->p[0]    = pv[0];
        asp[0]->p[1]    = pv[1];
//        asp[0]->sp      = *sp;
        asp[0]->flags   = 0;

        asp[1]->p[0]    = pv[1];
        asp[1]->p[1]    = pv[2];
//        asp[1]->sp      = *sp;
        asp[1]->flags   = 0;

        asp[2]->p[0]    = pv[2];
        asp[2]->p[1]    = pv[0];
//        asp[2]->sp      = *sp;
        asp[2]->flags   = 0; //SF_CULLBACK;      // After split of this edge, we need to perform a cull-back

        return STATUS_OK;
    }

//    rt_split_t *rt_plan_t::add_edge(const point3d_t *pv, const vector3d_t *sp)
    rt_split_t *rt_plan_t::add_edge(const point3d_t *pv)
    {
        rt_split_t *asp     = items.alloc();
        if (asp != NULL)
        {
            asp->p[0]       = pv[0];
            asp->p[1]       = pv[1];
//            asp->sp         = *sp;
            asp->flags      = 0;
        }

        return asp;
    }

//    rt_split_t *rt_plan_t::add_edge(const point3d_t *p1, const point3d_t *p2, const vector3d_t *sp)
    rt_split_t *rt_plan_t::add_edge(const point3d_t *p1, const point3d_t *p2)
    {
        rt_split_t *asp     = items.alloc();
        if (asp != NULL)
        {
            asp->p[0]       = *p1;
            asp->p[1]       = *p2;
//            asp->sp         = *sp;
            asp->flags      = 0;
        }

        return asp;
    }
}

