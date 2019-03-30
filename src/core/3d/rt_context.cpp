/*
 * RTContext.cpp
 *
 *  Created on: 15 янв. 2019 г.
 *      Author: sadko
 */

#include <core/3d/common.h>
#include <core/3d/rt_context.h>

namespace lsp
{
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

    rt_context_t::rt_context_t():
        triangle(1024)
    {
        this->state     = S_SCAN_OBJECTS;
        this->debug     = NULL;

        // Initialize point of view
        view.amplitude  = 0.0f;
        view.location   = 0.0f; // Undefined
        view.face       = -1;
        view.oid        = -1;
        view.speed      = SOUND_SPEED_M_S;
        view.rnum       = 0;

        dsp::init_point_xyz(&view.s, 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&view.p[0], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&view.p[1], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&view.p[2], 0.0f, 0.0f, 0.0f);
    }
    
    rt_context_t::rt_context_t(const rt_view_t *view):
        triangle(1024)
    {
        this->state     = S_SCAN_OBJECTS;
        this->debug     = NULL;
        this->view      = *view;
    }

    rt_context_t::rt_context_t(const rt_view_t *view, rt_context_state_t state):
        triangle(1024)
    {
        this->state     = state;
        this->debug     = NULL;
        this->view      = *view;
    }

    rt_context_t::~rt_context_t()
    {
        debug           = NULL;
        plan.flush();
        triangle.flush();

        IF_RT_TRACE_Y(
            ignored.flush();
            trace.clear_all();
        )
    }

#if defined(LSP_DEBUG) && defined(LSP_RT_TRACE)
    status_t rt_context_t::ignore(const rtm_triangle_t *t)
    {
        v_triangle3d_t vt;
        vt.p[0]     = *(t->v[0]);
        vt.p[1]     = *(t->v[1]);
        vt.p[2]     = *(t->v[2]);

        vt.n[0]     = t->n;
        vt.n[1]     = t->n;
        vt.n[2]     = t->n;

        return (ignored.add(&vt)) ? STATUS_OK : STATUS_NO_MEM;
    }

    status_t rt_context_t::ignore(const rt_triangle_t *t)
    {
        v_triangle3d_t vt;
        vt.p[0]     = t->v[0];
        vt.p[1]     = t->v[1];
        vt.p[2]     = t->v[2];

        vt.n[0]     = t->n;
        vt.n[1]     = t->n;
        vt.n[2]     = t->n;

        return (ignored.add(&vt)) ? STATUS_OK : STATUS_NO_MEM;
    }
#endif

    int rt_context_t::compare_triangles(const void *p1, const void *p2)
    {
        const rt_triangle_sort_t *t1 = reinterpret_cast<const rt_triangle_sort_t *>(p1);
        const rt_triangle_sort_t *t2 = reinterpret_cast<const rt_triangle_sort_t *>(p2);

        float x = t1->w - t2->w;
        if (x < -DSP_3D_TOLERANCE)
            return -1;
        return (x > DSP_3D_TOLERANCE) ? 1 : 0;
    }

    status_t rt_context_t::fetch_objects(rt_mesh_t *src, size_t n, const size_t *ids)
    {
        // Check size
        if (n <= 0)
        {
            triangle.flush();
            plan.flush();
            return STATUS_OK;
        }

        //--------------------------------------------
        // Prepare sorted list of matched triangles
        size_t cap              = (src->triangle.size() + 0x3f) & (~0x3f);
        size_t t_total          = 0;
        rt_triangle_sort_t *vt  = reinterpret_cast<rt_triangle_sort_t *>(malloc(cap * sizeof(rt_triangle_sort_t)));
        if (vt == NULL)
            return STATUS_NO_MEM;

        RT_FOREACH(rtm_triangle_t, t, src->triangle)
            // Skip triangles that should be ignored
            if ((t->oid == view.oid) && (t->face == view.face))
            {
                RT_TRACE(debug, ignore(t); );
                continue;
            }

            // Check that triangle matches specified object
            for (size_t i=0; i<n; ++i)
                if (t->oid == ssize_t(ids[i]))
                {
                    // Mark edges as required to be added to the plan
                    t->e[0]->itag       = 0;
                    t->e[1]->itag       = 0;
                    t->e[2]->itag       = 0;

                    // Add triangle to list and increment total counter
                    vt[t_total].t       = t;
                    vt[t_total].w       = dsp::calc_min_distance_p3(&view.s, t->v[0], t->v[1], t->v[2]);
                    ++t_total;
                }
        RT_FOREACH_END;

        if (t_total <= 0)
        {
            free(vt);
            triangle.flush();
            plan.flush();
            return STATUS_OK;
        }

        // Perform sort
        RT_TRACE_BREAK(debug,
            lsp_trace("Prepare sort (%d triangles)", int(t_total));
            trace.add_view_1c(&view, &C_MAGENTA);

            color3d_t c;
            c.r     = 1.0f;
            c.g     = 0.0f;
            c.b     = 0.0f;
            c.a     = 0.0f;

            for (size_t i=0; i<t_total; ++i)
            {
                c.r = float(t_total - i)/t_total;
                trace.add_triangle_1c(vt[i].t, &c);
            }
            free(vt);
        )

        ::qsort(vt, t_total, sizeof(rt_triangle_sort_t), compare_triangles);

        RT_TRACE_BREAK(debug,
            lsp_trace("After sort (%d triangles)", int(t_total));
            trace.add_view_1c(&view, &C_MAGENTA);

            color3d_t c;
            c.r     = 0.0f;
            c.g     = 1.0f;
            c.b     = 0.0f;
            c.a     = 0.0f;

            for (size_t i=0; i<t_total; ++i)
            {
                c.g = float(t_total - i)/t_total;
                trace.add_triangle_1c(vt[i].t, &c);
            }
            free(vt);
        )

        //--------------------------------------------
        // Build the plan
        rt_triangle_t *dt;
        rt_plan_t   xplan;
        Allocator3D<rt_triangle_t> xtriangle(1024);
        status_t res = STATUS_OK;

        for (size_t i=0; i<t_total; ++i)
        {
            rtm_triangle_t *t = vt[i].t;

            // Add edges to plan
            if (!(t->e[0]->itag++))
            {
                if (!xplan.add_edge(t->v[0], t->v[1]))
                {
                    res = STATUS_NO_MEM;
                    break;
                }
            }
            if (!(t->e[1]->itag++))
            {
                if (!xplan.add_edge(t->v[1], t->v[2]))
                {
                    res = STATUS_NO_MEM;
                    break;
                }
            }
            if (!(t->e[2]->itag++))
            {
                if (!xplan.add_edge(t->v[2], t->v[0]))
                {
                    res = STATUS_NO_MEM;
                    break;
                }
            }

            // Add triangle to list
            if (!(dt = xtriangle.alloc()))
            {
                res     = STATUS_NO_MEM;
                break;
            }

            dt->v[0]    = *(t->v[0]);
            dt->v[1]    = *(t->v[1]);
            dt->v[2]    = *(t->v[2]);
            dt->n       = t->n;
            dt->oid     = t->oid;
            dt->face    = t->face;
            dt->m       = t->m;
        }

        free(vt);
        if (res != STATUS_OK)
            return res;

        RT_VALIDATE(
            if (!src->validate())
                return STATUS_CORRUPTED;
        );

        xtriangle.swap(&this->triangle);
        xplan.swap(&this->plan);

        return STATUS_OK;
    }

    status_t rt_context_t::cull_view()
    {
        vector3d_t pl[4]; // Split plane
        status_t res;

        // Initialize cull planes
        dsp::calc_rev_oriented_plane_p3(&pl[0], &view.s, &view.p[0], &view.p[1], &view.p[2]);
        dsp::calc_oriented_plane_p3(&pl[1], &view.p[2], &view.s, &view.p[0], &view.p[1]);
        dsp::calc_oriented_plane_p3(&pl[2], &view.p[0], &view.s, &view.p[1], &view.p[2]);
        dsp::calc_oriented_plane_p3(&pl[3], &view.p[1], &view.s, &view.p[2], &view.p[0]);

        RT_TRACE_BREAK(debug,
            lsp_trace("Culling space with planes (%d triangles)", int(triangle.size()));

            for (size_t j=0, n=triangle.size(); j<n; ++j)
               trace.add_triangle_1c(triangle.get(j), &C_DARKGREEN);

            trace.add_plane_3pn1c(&view.p[0], &view.p[1], &view.p[2], &pl[0], &C_YELLOW);
            trace.add_plane_3pn1c(&view.s, &view.p[0], &view.p[1], &pl[1], &C_RED);
            trace.add_plane_3pn1c(&view.s, &view.p[1], &view.p[2], &pl[2], &C_GREEN);
            trace.add_plane_3pn1c(&view.s, &view.p[2], &view.p[0], &pl[3], &C_BLUE);
        )

        for (size_t pi=0; pi<4; ++pi)
        {
            res = cut(&pl[pi]);
            if (res != STATUS_OK)
                return res;

            // Check that there is data for processing and take it for next iteration
            if (triangle.size() <= 0)
                break;
        }

        RT_TRACE_BREAK(debug,
            lsp_trace("Data after culling (%d triangles)", int(triangle.size()));
            trace.add_view_1c(&view, &C_MAGENTA);
            for (size_t j=0,n=triangle.size(); j<n; ++j)
                trace.add_triangle_1c(triangle.get(j), &C_YELLOW);
            trace.dump(&plan, &C_RED);
        );

        return STATUS_OK;
    }

    status_t rt_context_t::cut(const vector3d_t *pl)
    {
        Allocator3D<rt_triangle_t> in(1024);
        rt_triangle_t *nt1, *nt2;

        RT_FOREACH(rt_triangle_t, t, triangle)
            size_t tag  = dsp::colocation_v1pv(pl, t->v);

            switch (tag)
            {
                case 0x00:  // 0 0 0
                case 0x01:  // 0 0 1
                case 0x04:  // 0 1 0
                case 0x05:  // 0 1 1
                case 0x10:  // 1 0 0
                case 0x11:  // 1 0 1
                case 0x14:  // 1 1 0
                    // Triangle is above, skip
                    break;

                case 0x15:  // 1 1 1
                    // Triangle is on the plane, skip
                    break;

                case 0x16:  // 1 1 2
                case 0x19:  // 1 2 1
                case 0x1a:  // 1 2 2
                case 0x25:  // 2 1 1
                case 0x26:  // 2 1 2
                case 0x29:  // 2 2 1
                case 0x2a:  // 2 2 2
                    // Triangle is below, add and continue
                    if (!in.alloc(t))
                        return STATUS_NO_MEM;
                    break;

                case 0x06:  // 0 1 2
                    if (!(nt1 = in.alloc(t)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[2], &t->v[0], &t->v[2], pl);
                    break;
                case 0x24:  // 2 1 0
                    if (!(nt1 = in.alloc(t)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[0], &t->v[0], &t->v[2], pl);
                    break;

                case 0x12:  // 1 0 2
                    if (!(nt1 = in.alloc(t)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[1], &t->v[0], &t->v[1], pl);
                    break;
                case 0x18:  // 1 2 0
                    if (!(nt1 = in.alloc(t)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[0], &t->v[0], &t->v[1], pl);
                    break;

                case 0x09:  // 0 2 1
                    if (!(nt1 = in.alloc(t)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[2], &t->v[1], &t->v[2], pl);
                    break;
                case 0x21:  // 2 0 1
                    if (!(nt1 = in.alloc(t)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[1], &t->v[1], &t->v[2], pl);
                    break;

                case 0x02:  // 0 0 2
                    if (!(nt1 = in.alloc(t)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[1], &t->v[0], &t->v[1], pl);
                    dsp::calc_split_point_p2v1(&nt1->v[2], &t->v[0], &t->v[2], pl);
                    break;
                case 0x08:  // 0 2 0
                    if (!(nt1 = in.alloc(t)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[0], &t->v[1], &t->v[0], pl);
                    dsp::calc_split_point_p2v1(&nt1->v[2], &t->v[1], &t->v[2], pl);
                    break;
                case 0x20:  // 2 0 0
                    if (!(nt1 = in.alloc(t)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[0], &t->v[2], &t->v[0], pl);
                    dsp::calc_split_point_p2v1(&nt1->v[1], &t->v[2], &t->v[1], pl);
                    break;

                case 0x28:  // 2 2 0
                    if ((!(nt1 = in.alloc(t))) || (!(nt2 = in.alloc(t))))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[0], &t->v[0], &t->v[1], pl);
                    dsp::calc_split_point_p2v1(&nt2->v[0], &t->v[0], &t->v[2], pl);
                    nt2->v[1]   = nt1->v[0];
                    break;

                case 0x22:  // 2 0 2
                    if ((!(nt1 = in.alloc(t))) || (!(nt2 = in.alloc(t))))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[1], &t->v[1], &t->v[2], pl);
                    dsp::calc_split_point_p2v1(&nt2->v[1], &t->v[1], &t->v[0], pl);
                    nt2->v[2]   = nt1->v[1];
                    break;

                case 0x0a:  // 0 2 2
                    if ((!(nt1 = in.alloc(t))) || (!(nt2 = in.alloc(t))))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[2], &t->v[2], &t->v[0], pl);
                    dsp::calc_split_point_p2v1(&nt2->v[2], &t->v[2], &t->v[1], pl);
                    nt2->v[0]   = nt1->v[2];
                    break;

                default:
                    return STATUS_UNKNOWN_ERR;
            }
        RT_FOREACH_END

        // Swap data and proceed
        in.swap(&this->triangle);

        return plan.cut_out(pl);
    }

    status_t rt_context_t::cullback(const vector3d_t *pl)
    {
        Allocator3D<rt_triangle_t> in(1024);
        rt_triangle_t *nt1, *nt2;

        RT_FOREACH(rt_triangle_t, t, triangle)
            size_t tag  = dsp::colocation_v1pv(pl, t->v);

            switch (tag)
            {
                case 0x00:  // 0 0 0
                case 0x01:  // 0 0 1
                case 0x04:  // 0 1 0
                case 0x05:  // 0 1 1
                case 0x10:  // 1 0 0
                case 0x11:  // 1 0 1
                case 0x14:  // 1 1 0
                    // Triangle is above, skip
                    break;

                case 0x15:  // 1 1 1
                    // Triangle is on the plane, add and continue
                    if (!in.alloc(t))
                        return STATUS_NO_MEM;
                    break;

                case 0x16:  // 1 1 2
                case 0x19:  // 1 2 1
                case 0x1a:  // 1 2 2
                case 0x25:  // 2 1 1
                case 0x26:  // 2 1 2
                case 0x29:  // 2 2 1
                case 0x2a:  // 2 2 2
                    // Triangle is below, add and continue
                    if (!in.alloc(t))
                        return STATUS_NO_MEM;
                    break;

                case 0x06:  // 0 1 2
                    if (!(nt1 = in.alloc(t)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[2], &t->v[0], &t->v[2], pl);
                    break;
                case 0x24:  // 2 1 0
                    if (!(nt1 = in.alloc(t)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[0], &t->v[0], &t->v[2], pl);
                    break;

                case 0x12:  // 1 0 2
                    if (!(nt1 = in.alloc(t)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[1], &t->v[0], &t->v[1], pl);
                    break;
                case 0x18:  // 1 2 0
                    if (!(nt1 = in.alloc(t)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[0], &t->v[0], &t->v[1], pl);
                    break;

                case 0x09:  // 0 2 1
                    if (!(nt1 = in.alloc(t)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[2], &t->v[1], &t->v[2], pl);
                    break;
                case 0x21:  // 2 0 1
                    if (!(nt1 = in.alloc(t)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[1], &t->v[1], &t->v[2], pl);
                    break;

                case 0x02:  // 0 0 2
                    if (!(nt1 = in.alloc(t)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[1], &t->v[0], &t->v[1], pl);
                    dsp::calc_split_point_p2v1(&nt1->v[2], &t->v[0], &t->v[2], pl);
                    break;
                case 0x08:  // 0 2 0
                    if (!(nt1 = in.alloc(t)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[0], &t->v[1], &t->v[0], pl);
                    dsp::calc_split_point_p2v1(&nt1->v[2], &t->v[1], &t->v[2], pl);
                    break;
                case 0x20:  // 2 0 0
                    if (!(nt1 = in.alloc(t)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[0], &t->v[2], &t->v[0], pl);
                    dsp::calc_split_point_p2v1(&nt1->v[1], &t->v[2], &t->v[1], pl);
                    break;

                case 0x28:  // 2 2 0
                    if ((!(nt1 = in.alloc(t))) || (!(nt2 = in.alloc(t))))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[0], &t->v[0], &t->v[1], pl);
                    dsp::calc_split_point_p2v1(&nt2->v[0], &t->v[0], &t->v[2], pl);
                    nt2->v[1]   = nt1->v[0];
                    break;

                case 0x22:  // 2 0 2
                    if ((!(nt1 = in.alloc(t))) || (!(nt2 = in.alloc(t))))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[1], &t->v[1], &t->v[2], pl);
                    dsp::calc_split_point_p2v1(&nt2->v[1], &t->v[1], &t->v[0], pl);
                    nt2->v[2]   = nt1->v[1];
                    break;

                case 0x0a:  // 0 2 2
                    if ((!(nt1 = in.alloc(t))) || (!(nt2 = in.alloc(t))))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[2], &t->v[2], &t->v[0], pl);
                    dsp::calc_split_point_p2v1(&nt2->v[2], &t->v[2], &t->v[1], pl);
                    nt2->v[0]   = nt1->v[2];
                    break;

                default:
                    return STATUS_UNKNOWN_ERR;
            }
        RT_FOREACH_END

        // Swap data and proceed
        in.swap(&this->triangle);

        return plan.cut_out(pl);
    }

    status_t rt_context_t::split(rt_context_t *out, const vector3d_t *pl)
    {
        Allocator3D<rt_triangle_t> xin(1024), xout(1024);
        rt_triangle_t *nt1, *nt2, *nt3;

        RT_FOREACH(rt_triangle_t, t, triangle)
            size_t tag  = dsp::colocation_v1pv(pl, t->v);

            switch (tag)
            {
                case 0x00:  // 0 0 0
                case 0x01:  // 0 0 1
                case 0x04:  // 0 1 0
                case 0x05:  // 0 1 1
                case 0x10:  // 1 0 0
                case 0x11:  // 1 0 1
                case 0x14:  // 1 1 0
                    // Triangle is above, add to xout
                    if (!xout.alloc(t))
                        return STATUS_NO_MEM;
                    break;

                case 0x15:  // 1 1 1
                    // Triangle is on the plane, skip
                    break;

                case 0x16:  // 1 1 2
                case 0x19:  // 1 2 1
                case 0x1a:  // 1 2 2
                case 0x25:  // 2 1 1
                case 0x26:  // 2 1 2
                case 0x29:  // 2 2 1
                case 0x2a:  // 2 2 2
                    // Triangle is below, add to xin
                    if (!xin.alloc(t))
                        return STATUS_NO_MEM;
                    break;

                // Split into 2 triangles
                case 0x06:  // 0 1 2
                    if ((!(nt1 = xin.alloc(t))) || (!(nt2 = xout.alloc(t))))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[2], &t->v[0], &t->v[2], pl);
                    nt2->v[0]   = nt1->v[2];
                    break;
                case 0x24:  // 2 1 0
                    if ((!(nt1 = xin.alloc(t))) || (!(nt2 = xout.alloc(t))))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[0], &t->v[0], &t->v[2], pl);
                    nt2->v[2]   = nt1->v[0];
                    break;

                case 0x12:  // 1 0 2
                    if ((!(nt1 = xin.alloc(t))) || (!(nt2 = xout.alloc(t))))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[1], &t->v[0], &t->v[1], pl);
                    nt2->v[0]   = nt1->v[1];
                    break;
                case 0x18:  // 1 2 0
                    if ((!(nt1 = xin.alloc(t))) || (!(nt2 = xout.alloc(t))))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[0], &t->v[0], &t->v[1], pl);
                    nt2->v[1]   = nt1->v[0];
                    break;

                case 0x09:  // 0 2 1
                    if ((!(nt1 = xin.alloc(t))) || (!(nt2 = xout.alloc(t))))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[2], &t->v[1], &t->v[2], pl);
                    nt2->v[1]   = nt1->v[2];
                    break;
                case 0x21:  // 2 0 1
                    if ((!(nt1 = xin.alloc(t))) || (!(nt2 = xout.alloc(t))))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[1], &t->v[1], &t->v[2], pl);
                    nt2->v[2]   = nt1->v[1];
                    break;


                // 2 triangles over the plane
                case 0x02:  // 0 0 2
                    if ((!(nt1 = xin.alloc(t))) || (!(nt2 = xout.alloc(t))) || (!(nt3 = xout.alloc(t))))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[1], &t->v[0], &t->v[1], pl);
                    dsp::calc_split_point_p2v1(&nt1->v[2], &t->v[0], &t->v[2], pl);
                    nt2->v[0]   = nt1->v[2];
                    nt3->v[0]   = nt1->v[1];
                    nt3->v[2]   = nt1->v[2];
                    break;
                case 0x08:  // 0 2 0
                    if ((!(nt1 = xin.alloc(t))) || (!(nt2 = xout.alloc(t))) || (!(nt3 = xout.alloc(t))))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[0], &t->v[1], &t->v[0], pl);
                    dsp::calc_split_point_p2v1(&nt1->v[2], &t->v[1], &t->v[2], pl);
                    nt2->v[1]   = nt1->v[0];
                    nt3->v[0]   = nt1->v[0];
                    nt3->v[1]   = nt1->v[2];
                    break;
                case 0x20:  // 2 0 0
                    if ((!(nt1 = xin.alloc(t))) || (!(nt2 = xout.alloc(t))) || (!(nt3 = xout.alloc(t))))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[0], &t->v[2], &t->v[0], pl);
                    dsp::calc_split_point_p2v1(&nt1->v[1], &t->v[2], &t->v[1], pl);
                    nt2->v[2]   = nt1->v[0];
                    nt3->v[0]   = nt1->v[0];
                    nt3->v[2]   = nt1->v[1];
                    break;

                // 2 triangles under the plane
                case 0x28:  // 2 2 0
                    if ((!(nt1 = xout.alloc(t))) || (!(nt2 = xin.alloc(t))) || (!(nt3 = xin.alloc(t))))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[1], &t->v[0], &t->v[1], pl);
                    dsp::calc_split_point_p2v1(&nt1->v[2], &t->v[0], &t->v[2], pl);
                    nt2->v[0]   = nt1->v[2];
                    nt3->v[0]   = nt1->v[1];
                    nt3->v[2]   = nt1->v[2];
                    break;

                case 0x22:  // 2 0 2
                    if ((!(nt1 = xout.alloc(t))) || (!(nt2 = xin.alloc(t))) || (!(nt3 = xin.alloc(t))))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[0], &t->v[1], &t->v[0], pl);
                    dsp::calc_split_point_p2v1(&nt1->v[2], &t->v[1], &t->v[2], pl);
                    nt2->v[1]   = nt1->v[0];
                    nt3->v[0]   = nt1->v[0];
                    nt3->v[1]   = nt1->v[2];
                    break;

                case 0x0a:  // 0 2 2
                    if ((!(nt1 = xout.alloc(t))) || (!(nt2 = xin.alloc(t))) || (!(nt3 = xin.alloc(t))))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&nt1->v[0], &t->v[2], &t->v[0], pl);
                    dsp::calc_split_point_p2v1(&nt1->v[1], &t->v[2], &t->v[1], pl);
                    nt2->v[2]   = nt1->v[0];
                    nt3->v[0]   = nt1->v[0];
                    nt3->v[2]   = nt1->v[1];
                    break;

                default:
                    return STATUS_UNKNOWN_ERR;
            }
        RT_FOREACH_END

        // Swap data and proceed
        xin.swap(&this->triangle);
        xout.swap(&out->triangle);

        return plan.split(&out->plan, pl);
    }

    status_t rt_context_t::edge_split(rt_context_t *out)
    {
        // Find edge to apply split
        if (plan.items.size() <= 0)
            return STATUS_NOT_FOUND;

        vector3d_t pl;

        RT_FOREACH(rt_split_t, se, plan.items)
            if (se->flags & SF_REMOVE)
                continue;
            se->flags      |= SF_REMOVE;        // Mark edge for removal

            rt_split_t xe   = *se;

//            if (!(xe.flags & SF_APPLIED))
//            {
                // Process split only with valid plane
                if (dsp::calc_plane_p3(&pl, &view.s, &xe.p[0], &xe.p[1]) > DSP_3D_TOLERANCE)
                {
                    status_t res = split(out, &pl);
                    if (res != STATUS_OK)
                        return res;
                    RT_TRACE_BREAK(debug,
                        lsp_trace("Split context into triangles in(GREEN)/out(RED) = %d/%d",
                                int(triangle.size()), int(out->triangle.size()));
                        trace.add_view_1c(&view, &C_MAGENTA);
                        trace.add_plane_3p1c(&view.s, &xe.p[0], &xe.p[1], &C_MAGENTA);

                        for (size_t i=0, n=triangle.size(); i<n; ++i)
                            trace.add_triangle_1c(triangle.get(i), &C_GREEN);
                        for (size_t i=0, n=out->triangle.size(); i<n; ++i)
                            trace.add_triangle_1c(out->triangle.get(i), &C_RED);

                        trace.dump(&plan, &C_RED);
                        trace.dump(&out->plan, &C_GREEN);
                    )
                }
//            }
//            if (xe.flags & SF_CULLBACK)
//            {
//                dsp::orient_plane_v1p1(&pl, &view.s, &xe.sp);
//                RT_TRACE_BREAK(debug,
//                    lsp_trace("Cullback context");
//                    trace.add_view_1c(&view, &C_MAGENTA);
//
//                    for (size_t i=0, n=triangle.size(); i<n; ++i)
//                        trace.add_triangle_1c(triangle.get(i), &C_GREEN);
//                    trace.add_plane_2pn1c(&xe.p[0], &xe.p[1], &pl, &C_YELLOW);
//                )
//
//                status_t res = cullback(&pl);
//                if (res != STATUS_OK)
//                    return res;
//
//                RT_TRACE_BREAK(debug,
//                    lsp_trace("Cullback context into triangles in(BLUE) = %d",
//                            int(triangle.size()));
//                    trace.add_view_1c(&view, &C_MAGENTA);
//
//                    for (size_t i=0, n=triangle.size(); i<n; ++i)
//                        trace.add_triangle_1c(triangle.get(i), &C_BLUE);
//                )
//            }

            return STATUS_OK;
        RT_FOREACH_END

        return STATUS_NOT_FOUND;
    }

    status_t rt_context_t::depth_test()
    {
        if (triangle.size() <= 1)
            return STATUS_OK;

        // Find the nearest to the point of view triangle
        rt_triangle_t *st = triangle.get(0);
        float dmin = dsp::calc_min_distance_pv(&view.s, st->v);

        RT_FOREACH(rt_triangle_t, t, triangle)
            float d = dsp::calc_min_distance_pv(&view.s, t->v);
            if (d < dmin)
            {
                st = t;
                dmin = d;
            }
        RT_FOREACH_END;

        // Build plane equation and perform cull-back
        vector3d_t v;
        dsp::orient_plane_v1p1(&v, &view.s, &st->n);
        return cullback(&v);
    }

#if 0
    bool rt_context_t::unlink_triangle(rtm_triangle_t *t, rtm_edge_t *e)
    {
        for (rtm_triangle_t **pcurr = &e->vt; *pcurr != NULL; )
        {
            rtm_triangle_t *curr = *pcurr;
            rtm_triangle_t **pnext = (curr->e[0] == e) ? &curr->elnk[0] :
                                    (curr->e[1] == e) ? &curr->elnk[1] :
                                    (curr->e[2] == e) ? &curr->elnk[2] :
                                    NULL;
            if (pnext == NULL) // Unexpected behaviour
                return false;
            if (curr == t)
            {
                *pcurr = *pnext;
                return true;
            }
            pcurr = pnext;
        }
        return false;
    }

    status_t rt_context_t::arrange_triangle(rtm_triangle_t *ct, rtm_edge_t *e)
    {
        rtm_vertex_t *tv;
        rtm_edge_t *te;
        rtm_triangle_t *tt;

        // Rotate triangle to make ct->e[0] == e
        if (ct->e[1] == e) // Rotate clockwise
        {
            tv              = ct->v[0];
            ct->v[0]        = ct->v[1];
            ct->v[1]        = ct->v[2];
            ct->v[2]        = tv;

            te              = ct->e[0];
            ct->e[0]        = ct->e[1];
            ct->e[1]        = ct->e[2];
            ct->e[2]        = te;

            tt              = ct->elnk[0];
            ct->elnk[0]     = ct->elnk[1];
            ct->elnk[1]     = ct->elnk[2];
            ct->elnk[2]     = tt;
        }
        else if (ct->e[2] == e) // Rotate counter-clockwise
        {
            tv              = ct->v[2];
            ct->v[2]        = ct->v[1];
            ct->v[1]        = ct->v[0];
            ct->v[0]        = tv;

            te              = ct->e[2];
            ct->e[2]        = ct->e[1];
            ct->e[1]        = ct->e[0];
            ct->e[0]        = te;

            tt              = ct->elnk[2];
            ct->elnk[2]     = ct->elnk[1];
            ct->elnk[1]     = ct->elnk[0];
            ct->elnk[0]     = tt;
        }
        else if (ct->e[0] != e)
            return STATUS_BAD_STATE;

        return STATUS_OK;
    }

    status_t rt_context_t::split_edge(rtm_edge_t* e, rtm_vertex_t* sp)
    {
        status_t res;
        rtm_triangle_t *ct, *nt, *pt;
        rtm_edge_t *ne, *se;

//        RT_TRACE_BREAK(this,
//            lsp_trace("Splitting edge");
//            for (rtm_triangle_t *t = e->vt; t != NULL;)
//            {
//                trace.add_triangle_3c(t, &C_RED, &C_GREEN, &C_BLUE);
//                t = (t->e[0] == e) ? t->elnk[0] :
//                    (t->e[1] == e) ? t->elnk[1] :
//                    (t->e[2] == e) ? t->elnk[2] :
//                    NULL;
//            }
//            trace.add_segment(e, &C_RED, &C_YELLOW);
//        );

        // Rearrange first triangle
        if ((ct = e->vt) == NULL)
            return STATUS_OK;
        res             = arrange_triangle(ct, e);
        if (res != STATUS_OK)
            return res;

        // Allocate edges
        ne              = edge.alloc();
        if (ne == NULL)
            return STATUS_NO_MEM;

        // Initialize culled edge and link to corresponding vertexes
        ne->v[0]        = sp;
        ne->v[1]        = e->v[1];
        ne->vt          = NULL;
        ne->ptag        = NULL;
        ne->itag        = e->itag;

        RT_VALIDATE(
            if ((ne->v[0] == NULL) || (ne->v[1] == NULL))
                return STATUS_CORRUPTED;
        )

        // Unlink current edge from vertexes
//        if (!unlink_edge(e, e->v[0]))
//            return STATUS_CORRUPTED;
//        if (!unlink_edge(e, e->v[1]))
//            return STATUS_CORRUPTED;
        RT_VALIDATE(
            if (linked_count(e, e->v[0]) != 0)
                return STATUS_CORRUPTED;
            if (linked_count(e, e->v[1]) != 0)
                return STATUS_CORRUPTED;
        )

        // Process all triangles
        while (true)
        {
//            RT_TRACE_BREAK(this,
//                lsp_trace("Splitting triangle");
//                trace.add_triangle_3c(ct, &C_RED, &C_GREEN, &C_BLUE);
//                trace.add_segment(e, &C_RED, &C_YELLOW);
//            );

            // Save pointer to triangle to move forward
            pt              = ct->elnk[0];  // Save pointer to pending triangle, splitting edge is always rearranged to have index 0

            // Allocate triangle and splitting edge
            nt              = triangle.alloc();
            se              = edge.alloc();
            if ((nt == NULL) || (se == NULL))
                return STATUS_NO_MEM;

            // Initialize splitting edge and link to it's vertexes
            se->v[0]        = ct->v[2];
            se->v[1]        = sp;
            se->vt          = NULL;
            se->ptag        = NULL;
            se->itag        = 0;

            // Unlink current triangle from all edges
            if (!unlink_triangle(ct, ct->e[0]))
                return STATUS_CORRUPTED;
            if (!unlink_triangle(ct, ct->e[1]))
                return STATUS_CORRUPTED;
            if (!unlink_triangle(ct, ct->e[2]))
                return STATUS_CORRUPTED;

            if (ct->v[0] == e->v[0])
            {
                // Initialize new triangle
                nt->v[0]        = sp;
                nt->v[1]        = ct->v[1];
                nt->v[2]        = ct->v[2];
                nt->e[0]        = ne;
                nt->e[1]        = ct->e[1];
                nt->e[2]        = se;
                nt->n           = ct->n;
                nt->ptag        = NULL;
                nt->itag        = ct->itag;
                nt->oid         = ct->oid;
                nt->face        = ct->face;
                nt->m           = ct->m;

                // Update current triangle
              //ct->v[0]        = ct->v[0];
                ct->v[1]        = sp;
              //ct->v[2]        = ct->v[2];
              //ct->e[0]        = e;
                ct->e[1]        = se;
              //ct->e[2]        = ct->e[2];
              //ct->n           = ct->n;
              //ct->itag        = ct->itag;
            }
            else if (ct->v[0] == e->v[1])
            {
                // Initialize new triangle
                nt->v[0]        = sp;
                nt->v[1]        = ct->v[2];
                nt->v[2]        = ct->v[0];
                nt->e[0]        = se;
                nt->e[1]        = ct->e[2];
                nt->e[2]        = ne;
                nt->n           = ct->n;
                nt->ptag        = NULL;
                nt->itag        = ct->itag;
                nt->oid         = ct->oid;
                nt->face        = ct->face;
                nt->m           = ct->m;

                // Update current triangle
                ct->v[0]        = sp;
              //ct->v[1]        = ct->v[1];
              //ct->v[2]        = ct->v[2];
              //ct->e[0]        = e;
              //ct->e[1]        = ct->e[1];
                ct->e[2]        = se;
              //ct->n           = ct->n;
              //ct->itag        = ct->itag;
//
//                RT_TRACE_BREAK(this,
//                    lsp_trace("Drawing new triangles");
//                    trace.add_triangle_1c(ct, &C_CYAN);
//                    trace.add_triangle_1c(nt, &C_MAGENTA);
//                    trace.add_segment(e[0].v[0], sp, &C_RED);
//                    trace.add_segment(ne, &C_GREEN);
//                    trace.add_segment(se, &C_BLUE);
//                );
            }
            else
                return STATUS_BAD_STATE;

            // Link edges to new triangles
            nt->elnk[0]     = nt->e[0]->vt;
            nt->elnk[1]     = nt->e[1]->vt;
            nt->elnk[2]     = nt->e[2]->vt;
            nt->e[0]->vt    = nt;
            nt->e[1]->vt    = nt;
            nt->e[2]->vt    = nt;

            ct->elnk[0]     = ct->e[0]->vt;
            ct->elnk[1]     = ct->e[1]->vt;
            ct->elnk[2]     = ct->e[2]->vt;
            ct->e[0]->vt    = ct;
            ct->e[1]->vt    = ct;
            ct->e[2]->vt    = ct;

//            RT_TRACE_BREAK(this,
//                lsp_trace("Splitted triangle");
//                trace.add_triangle_1c(ct, &C_GREEN);
//                trace.add_triangle_1c(nt, &C_BLUE);
//            );

            // Move to next triangle
            if (pt == NULL)
            {
                // Re-link edge to vertexes and leave cycle
              //e->v[0]         = e->v[0];
                e->v[1]         = sp;

                if ((e->v[0] == NULL) || (e->v[1] == NULL))
                    return STATUS_CORRUPTED;
                break;
            }
            else
                ct = pt;

            // Re-arrange next triangle and edges
            res             = arrange_triangle(ct, e);
            if (res != STATUS_OK)
                return res;
        }

        // Now the edge 'e' is stored in context but not linked to any primitive
        return STATUS_OK;
    }

    status_t rt_context_t::split_triangle(rtm_triangle_t* t, rtm_vertex_t* sp)
    {
        // Unlink triangle from all it's edges
        unlink_triangle(t, t->e[0]);
        unlink_triangle(t, t->e[1]);
        unlink_triangle(t, t->e[2]);

        // Create additional edges and link them to vertexes
        rtm_edge_t *ne[3];
        for (size_t i=0; i<3; ++i)
        {
            rtm_edge_t *e    = edge.alloc();
            if (e == NULL)
                return STATUS_NO_MEM;
            ne[i]           = e;

            e->v[0]         = t->v[i];
            e->v[1]         = sp;
            e->vt           = NULL;
            e->ptag         = NULL;
            e->itag         = 0;
        }

        // Allocate additional triangles
        rtm_triangle_t *nt[3];
        nt[0]       = triangle.alloc();
        nt[1]       = triangle.alloc();
        nt[2]       = t;
        if ((nt[0] == NULL) || (nt[1] == NULL))
            return STATUS_NO_MEM;

        // Now bind edges and vertexes to triangles
        nt[0]->v[0]     = t->v[1];
        nt[0]->v[1]     = t->v[2];
        nt[0]->v[2]     = sp;
        nt[0]->e[0]     = t->e[1];
        nt[0]->e[1]     = ne[2];
        nt[0]->e[2]     = ne[1];
        nt[0]->n        = t->n;
        nt[0]->ptag     = NULL;
        nt[0]->itag     = t->itag;
        nt[0]->oid      = t->oid;
        nt[0]->face     = t->face;
        nt[0]->m        = t->m;

        nt[1]->v[0]     = t->v[2];
        nt[1]->v[1]     = t->v[0];
        nt[1]->v[2]     = sp;
        nt[1]->e[0]     = t->e[2];
        nt[1]->e[1]     = ne[0];
        nt[1]->e[2]     = ne[2];
        nt[1]->n        = t->n;
        nt[1]->ptag     = NULL;
        nt[1]->itag     = t->itag;
        nt[1]->oid      = t->oid;
        nt[1]->face     = t->face;
        nt[1]->m        = t->m;

      //nt[2]->v[0]     = t->v[0];
      //nt[2]->v[1]     = t->v[1];
        nt[2]->v[2]     = sp;
      //nt[2]->e[0]     = t->e[0];
        nt[2]->e[1]     = ne[1];
        nt[2]->e[2]     = ne[0];
      //nt[2]->n        = t->n;
      //nt[2]->ptag     = NULL;
      //nt[2]->itag     = t->itag;
      //nt[2]->oid      = t->oid;
      //nt[2]->face     = t->face;

        // Re-link triangles to edges
        for (size_t i=0; i<3; ++i)
        {
            rtm_triangle_t *ct   = nt[i];

            ct->elnk[0]     = ct->e[0]->vt;
            ct->elnk[1]     = ct->e[1]->vt;
            ct->elnk[2]     = ct->e[2]->vt;
            ct->e[0]->vt    = ct;
            ct->e[1]->vt    = ct;
            ct->e[2]->vt    = ct;
        }

        return STATUS_OK;
    }

    void rt_context_t::cleanup_tag_pointers()
    {
        // Cleanup pointers
        RT_FOREACH(rtm_vertex_t, v, vertex)
            v->ptag = NULL;
        RT_FOREACH_END

        RT_FOREACH(rtm_edge_t, e, edge)
            e->ptag = NULL;
        RT_FOREACH_END

        RT_FOREACH(rtm_triangle_t, t, triangle)
            t->ptag = NULL;
        RT_FOREACH_END
    }

    status_t rt_context_t::fetch_triangle(rt_context_t *dst, rtm_triangle_t *st)
    {
        rtm_vertex_t *sv, *vx;
        rtm_edge_t *se, *ex;
        rtm_triangle_t *tx;

        // Allocate new triangle
        tx          = dst->triangle.alloc();
        if (tx == NULL)
            return STATUS_NO_MEM;

        tx->n       = st->n;
        tx->m       = st->m;
        tx->itag    = st->itag;
        tx->oid     = st->oid;
        tx->face    = st->face;
        tx->ptag    = st;
        st->ptag    = tx;

        // Process each element in triangle
        for (size_t j=0; j<3; ++j)
        {
            // Allocate vertex if required
            sv      = st->v[j];
            vx      = reinterpret_cast<rtm_vertex_t *>(sv->ptag);

            if (vx == NULL)
            {
                vx              = dst->vertex.alloc();
                if (vx == NULL)
                    return STATUS_NO_MEM;

                vx->x           = sv->x;
                vx->y           = sv->y;
                vx->z           = sv->z;
                vx->w           = sv->w;
                vx->itag        = 0;
//                vx->ve          = NULL;

                // Link together
                vx->ptag        = sv;
                sv->ptag        = vx;
            }

            // Allocate edge if required
            se      = st->e[j];
            ex      = reinterpret_cast<rtm_edge_t *>(se->ptag);
            if (ex == NULL)
            {
                ex              = dst->edge.alloc();
                if (ex == NULL)
                    return STATUS_NO_MEM;

                ex->v[0]        = se->v[0];
                ex->v[1]        = se->v[1];
                ex->vt          = NULL;
                ex->itag        = se->itag;

                // Link together
                ex->ptag        = se;
                se->ptag        = ex;
            }

            // Store pointers
            tx->v[j]        = vx;
            tx->e[j]        = ex;
            tx->elnk[j]     = NULL;
        }

        return STATUS_OK;
    }

    status_t rt_context_t::fetch_triangle_safe(rt_context_t *dst, rtm_triangle_t *st)
    {
        if (dst == NULL)
            return STATUS_OK;
        return fetch_triangle(dst, st);
    }

    status_t rt_context_t::fetch_triangles(rt_context_t *dst, ssize_t itag)
    {
        // Iterate all triangles
        RT_FOREACH(rtm_triangle_t, st, triangle)
            if (st->itag != itag)
                continue;

            status_t res    = fetch_triangle(dst, st);
            if (res != STATUS_OK)
                return res;
        RT_FOREACH_END

        return STATUS_OK;
    }

    status_t rt_context_t::vfetch_triangles(rt_context_t *dst, size_t n, const ssize_t *itag)
    {
        rtm_triangle_t *st;
        status_t res = STATUS_OK;

        // Iterate all triangles
        size_t nt = triangle.size();

        for (size_t j=0; j<n; ++j)
        {
            ssize_t xitag   = itag[j];

            for (size_t i=0; i<nt; ++i)
            {
                // Fetch triangle while skipping current one
                st          = triangle.get(i);
                if (st->itag != xitag)
                    continue;

                res     = fetch_triangle(dst, st);
                if (res != STATUS_OK)
                    break;
            } // for
        }

        return res;
    }

    void rt_context_t::complete_fetch(rt_context_t *dst)
    {
        // Patch edge structures and link to vertexes
        RT_FOREACH(rtm_edge_t, ex, dst->edge)
            rtm_edge_t *se   = reinterpret_cast<rtm_edge_t *>(ex->ptag);
            if (se == NULL) // Edge does not need patching
                continue;

            // Patch vertex pointers if needed
            ex->v[0]        = (se->v[0]->ptag != NULL) ? reinterpret_cast<rtm_vertex_t *>(se->v[0]->ptag) : se->v[0];
            ex->v[1]        = (se->v[1]->ptag != NULL) ? reinterpret_cast<rtm_vertex_t *>(se->v[1]->ptag) : se->v[1];
        RT_FOREACH_END

        // Link triangle structures to edges
        RT_FOREACH(rtm_triangle_t, tx, dst->triangle)
            if (tx->ptag == NULL) // Triangle does not need patching
                continue;

            // Link triangle to the edge
            tx->elnk[0]         = tx->e[0]->vt;
            tx->elnk[1]         = tx->e[1]->vt;
            tx->elnk[2]         = tx->e[2]->vt;

            tx->e[0]->vt        = tx;
            tx->e[1]->vt        = tx;
            tx->e[2]->vt        = tx;
        RT_FOREACH_END
    }

    status_t rt_context_t::fetch_triangles_safe(rt_context_t *dst, ssize_t itag)
    {
        if (dst == NULL)
            return STATUS_OK;

        cleanup_tag_pointers();

        status_t res = fetch_triangles(dst, itag);
        if (res != STATUS_OK)
            return res;

        complete_fetch(dst);
        return STATUS_OK;
    }

    status_t rt_context_t::vfetch_triangles_safe(rt_context_t *dst, size_t n, const ssize_t *itag)
    {
        if (dst == NULL)
            return STATUS_OK;

        cleanup_tag_pointers();

        status_t res = vfetch_triangles(dst, n, itag);
        if (res != STATUS_OK)
            return res;

        complete_fetch(dst);
        return STATUS_OK;
    }

    int rt_context_t::compare_edges(const void *p1, const void *p2)
    {
        const rt_edge_sort_t *t1 = reinterpret_cast<const rt_edge_sort_t *>(p1);
        const rt_edge_sort_t *t2 = reinterpret_cast<const rt_edge_sort_t *>(p2);

        float x = t1->w - t2->w;
        if (x < -DSP_3D_TOLERANCE)
            return -1;
        return (x > DSP_3D_TOLERANCE) ? 1 : 0;
    }

    int rt_context_t::compare_triangles(const void *p1, const void *p2)
    {
        const rt_triangle_sort_t *t1 = reinterpret_cast<const rt_triangle_sort_t *>(p1);
        const rt_triangle_sort_t *t2 = reinterpret_cast<const rt_triangle_sort_t *>(p2);

        float x = t1->w - t2->w;
        if (x < -DSP_3D_TOLERANCE)
            return -1;
        return (x > DSP_3D_TOLERANCE) ? 1 : 0;
    }

    status_t rt_context_t::sort_edges()
    {
        // Compute number of triangles
        size_t ne       = edge.size();
        if (ne <= 1)
            return STATUS_OK;

        // Allocate temporary array for sort
        size_t cap          = (ne + 0x3f) & (~0x3f); // Round capacity to some granular size
        rt_edge_sort_t *ve  = reinterpret_cast<rt_edge_sort_t *>(malloc(cap * sizeof(rt_edge_sort_t)));
        if (ve == NULL)
            return STATUS_NO_MEM;

        // Prepare data for sorting
        size_t ei   = 0;
        RT_FOREACH(rtm_edge_t, ce, edge)
            float d0    = dsp::calc_sqr_distance_p2(&view.s, ce->v[0]);
            float d1    = dsp::calc_sqr_distance_p2(&view.s, ce->v[1]);

            ve[ei].e    = ce;
            ve[ei].w    = (d0 > d1) ? d1 : d0;
            ++ei;
        RT_FOREACH_END

        RT_TRACE_BREAK(debug,
            lsp_trace("Prepare sort (%d edges)", int(ne));

            color3d_t c;
            c.r     = 1.0f;
            c.g     = 0.0f;
            c.b     = 0.0f;
            c.a     = 0.0f;

            for (size_t i=0; i<ne; ++i)
            {
                c.r = float(ne - i)/ne;
                trace.add_segment(ve[i].e, &c);
            }
            free(ve);
        );

        // Call sorting function
        ::qsort(ve, ne, sizeof(rt_edge_sort_t), compare_edges);

        Allocator3D<rtm_edge_t> new_edges(edge.chunk_size());
        for (size_t i=0; i<ne; ++i)
        {
            rtm_edge_t *se   = ve[i].e;
            rtm_edge_t *de   = new_edges.alloc(se);
            if (de == NULL)
            {
                free(ve);
                return STATUS_NO_MEM;
            }

            se->ptag        = de;
        }

        // Now we can remove the temporary array
        free(ve);
        ve          = NULL;
        edge.swap(&new_edges);

        // Patch all triangle pointers
        RT_FOREACH(rtm_triangle_t, t, triangle)
            t->e[0]         = reinterpret_cast<rtm_edge_t *>(t->e[0]->ptag);
            t->e[1]         = reinterpret_cast<rtm_edge_t *>(t->e[1]->ptag);
            t->e[2]         = reinterpret_cast<rtm_edge_t *>(t->e[2]->ptag);
        RT_FOREACH_END

        RT_VALIDATE(
            if (!validate())
                return STATUS_CORRUPTED;
        );

        RT_TRACE_BREAK(debug,
            lsp_trace("Edges have been sorted (%d edges)", int(ne));

            color3d_t c;
            c.r     = 0.0f;
            c.g     = 1.0f;
            c.b     = 0.0f;
            c.a     = 0.0f;

            for (size_t i=0; i<ne; ++i)
            {
                c.g = float(ne - i)/ne;
                trace.add_segment(edge.get(i), &c);
            }
        );

        return STATUS_OK;
    }

    status_t rt_context_t::sort_triangles()
    {
        // Compute number of triangles
        size_t nt       = triangle.size();
        if (nt <= 1)
            return STATUS_OK;

        // Allocate temporary array for sort
        size_t cap              = (nt + 0x3f) & (~0x3f); // Round capacity to some granular size
        rt_triangle_sort_t *vt  = reinterpret_cast<rt_triangle_sort_t *>(malloc(cap * sizeof(rt_triangle_sort_t)));
        if (vt == NULL)
            return STATUS_NO_MEM;

        // Prepare data for sorting
        size_t ti   = 0;
        RT_FOREACH(rtm_triangle_t, ct, triangle)
            vt[ti].t    = ct;
            vt[ti].w    = dsp::calc_avg_distance_p3(&view.s, ct->v[0], ct->v[1], ct->v[2]);
            ++ti;
        RT_FOREACH_END

        RT_TRACE_BREAK(debug,
            lsp_trace("Prepare sort (%d triangles)", int(nt));

            color3d_t c;
            c.r     = 1.0f;
            c.g     = 0.0f;
            c.b     = 0.0f;
            c.a     = 0.0f;

            for (size_t i=0; i<nt; ++i)
            {
                c.r = float(nt - i)/nt;
                trace.add_triangle_1c(vt[i].t, &c);
            }
            free(vt);
        );

        // Call sorting function
        ::qsort(vt, nt, sizeof(rt_triangle_sort_t), compare_triangles);

        Allocator3D<rtm_triangle_t> new_triangles(triangle.chunk_size());
        for (size_t i=0; i<nt; ++i)
        {
            rtm_triangle_t *st   = vt[i].t;
            rtm_triangle_t *dt   = new_triangles.alloc(st);
            if (dt == NULL)
            {
                free(vt);
                return STATUS_NO_MEM;
            }
            st->ptag        = dt;
        }

        // Now we can remove the temporary array
        free(vt);
        vt          = NULL;
        triangle.swap(&new_triangles);

        // Patch all edge pointers
        RT_FOREACH(rtm_edge_t, e, edge)
            e->vt           = (e->vt != NULL) ? reinterpret_cast<rtm_triangle_t *>(e->vt->ptag) : NULL;
        RT_FOREACH_END

        // Patch all triangle pointers
        RT_FOREACH(rtm_triangle_t, t, triangle)
            t->elnk[0]      = (t->elnk[0] != NULL) ? reinterpret_cast<rtm_triangle_t *>(t->elnk[0]->ptag) : NULL;
            t->elnk[1]      = (t->elnk[1] != NULL) ? reinterpret_cast<rtm_triangle_t *>(t->elnk[1]->ptag) : NULL;
            t->elnk[2]      = (t->elnk[2] != NULL) ? reinterpret_cast<rtm_triangle_t *>(t->elnk[2]->ptag) : NULL;
        RT_FOREACH_END

        RT_VALIDATE(
            if (!validate())
                return STATUS_CORRUPTED;
        );

        RT_TRACE_BREAK(debug,
            lsp_trace("Triangles have been sorted (%d edges)", int(nt));

            color3d_t c;
            c.r     = 0.0f;
            c.g     = 1.0f;
            c.b     = 0.0f;
            c.a     = 0.0f;

            for (size_t i=0; i<nt; ++i)
            {
                c.g = float(nt - i)/nt;
                trace.add_triangle_1c(triangle.get(i), &c);
            }
        );

        return STATUS_OK;
    }

    status_t rt_context_t::solve_conflicts()
    {
        status_t res;
        vector3d_t pl;
        vector3d_t spl[3]; // Scissor planes
        float k[3];
        ssize_t l[3];

        for (size_t i=0; i<triangle.size(); ++i)
        {
            rtm_triangle_t *ct   = triangle.get(i);
            dsp::calc_plane_p3(&pl, ct->v[0], ct->v[1], ct->v[2]);
            dsp::calc_plane_v1p2(&spl[0], &pl, ct->v[0], ct->v[1]);
            dsp::calc_plane_v1p2(&spl[1], &pl, ct->v[1], ct->v[2]);
            dsp::calc_plane_v1p2(&spl[2], &pl, ct->v[2], ct->v[0]);

//            RT_TRACE_BREAK(this,
//                lsp_trace("Solving conflicts for triangle %d/%d", int(i), int(triangle.size()));
//
//                for (size_t i=0,n=triangle.size(); i<n; ++i)
//                {
//                    rtm_triangle_t *t = triangle.get(i);
//                    trace.add_triangle_1c(t, (t == ct) ? &C_ORANGE : &C_YELLOW);
//                }
//
//                trace.add_plane_3pn1c(ct->v[0], ct->v[1], ct->v[2], &pl, &C_MAGENTA);
//                trace.add_plane_2pn1c(ct->v[0], ct->v[1], &spl[0], &C_RED);
//                trace.add_plane_2pn1c(ct->v[1], ct->v[2], &spl[1], &C_GREEN);
//                trace.add_plane_2pn1c(ct->v[2], ct->v[0], &spl[2], &C_BLUE);
//            )

            // Estimate location of each vertex relative to the plane
            RT_FOREACH(rtm_vertex_t, cv, vertex)
                float k = cv->x * pl.dx + cv->y * pl.dy + cv->z*pl.dz + pl.dw;
                cv->itag = (k < -DSP_3D_TOLERANCE) ? 2 : (k > DSP_3D_TOLERANCE) ? 0 : 1;
            RT_FOREACH_END

            // Split each edge with triangle, do not process new edges
            RT_FOREACH(rtm_edge_t, ce, edge)
                if ((ce == ct->e[0]) || (ce == ct->e[1]) || (ce == ct->e[2]))
                    continue;

                ssize_t x1      = ce->v[0]->itag;
                ssize_t x2      = ce->v[1]->itag;

                // Ensure that edge intersects the plane
                if ((x1 <= 1) && (x2 <= 1))
                    continue;
                else if ((x1 >= 1) && (x2 >=1))
                    continue;

//                RT_TRACE_BREAK(this,
//                    lsp_trace("Solving conflicts for triangle %d/%d, edge %d/%d",
//                                int(i), int(triangle.size()),
//                                int(j), int(n)
//                    );
//
//                    for (size_t i=0,n=triangle.size(); i<n; ++i)
//                    {
//                        rtm_triangle_t *t = triangle.get(i);
//                        trace.add_triangle_1c(t, (t == ct) ? &C_ORANGE : &C_YELLOW);
//                    }
//
//                    trace.add_plane_3pn1c(ct->v[0], ct->v[1], ct->v[2], &pl, &C_MAGENTA);
//                    trace.add_plane_2pn1c(ct->v[0], ct->v[1], &spl[0], &C_MAGENTA);
//                    trace.add_plane_2pn1c(ct->v[1], ct->v[2], &spl[1], &C_MAGENTA);
//                    trace.add_plane_2pn1c(ct->v[2], ct->v[0], &spl[2], &C_MAGENTA);
//
//                    trace.add_segment(ce, &C_GREEN);
//                )

                // But now we need to check that intersection point lays on the triangle
                rtm_vertex_t sp, *spp;
                dsp::calc_split_point_p2v1(&sp, ce->v[0], ce->v[1], &pl);
                sp.itag     = 0;
                sp.ptag     = NULL;
//                sp.ve       = NULL;

                k[0]        = sp.x*spl[0].dx + sp.y*spl[0].dy + sp.z*spl[0].dz + spl[0].dw;
                k[1]        = sp.x*spl[1].dx + sp.y*spl[1].dy + sp.z*spl[1].dz + spl[1].dw;
                k[2]        = sp.x*spl[2].dx + sp.y*spl[2].dy + sp.z*spl[2].dz + spl[2].dw;

                l[0]        = (k[0] <= -DSP_3D_TOLERANCE) ? 2 : (k[0] > DSP_3D_TOLERANCE) ? 0 : 1;
                l[1]        = (k[1] <= -DSP_3D_TOLERANCE) ? 2 : (k[1] > DSP_3D_TOLERANCE) ? 0 : 1;
                l[2]        = (k[2] <= -DSP_3D_TOLERANCE) ? 2 : (k[2] > DSP_3D_TOLERANCE) ? 0 : 1;

                switch ((l[0]) | (l[1] << 2) | (l[2] << 4))
                {
                    case 0x16: // Point matches edges 1 and 2 (vertex 2)
                        res     = split_edge(ce, ct->v[2]); // Need to perform only split of crossing edge
                        if (res != STATUS_OK)
                            return res;
                        continue;

                    case 0x19: // Point matches edges 0 and 2 (vertex 0)
                        res     = split_edge(ce, ct->v[0]); // Need to perform only split of crossing edge
                        if (res != STATUS_OK)
                            return res;
                        continue;

                    case 0x25: // Point matches edges 0 and 1 (vertex 1)
                        res     = split_edge(ce, ct->v[1]); // Need to perform only split of crossing edge
                        if (res != STATUS_OK)
                            return res;
                        continue;

                    case 0x1a: // Point lays on edge 2, split triangle's edge
                        spp         = vertex.alloc(&sp);
                        if (spp == NULL)
                            return STATUS_NO_MEM;
//                        spp->d      = dsp::calc_sqr_distance_p2(spp, &view.s);
                        res         = split_edge(ct->e[2], spp);
                        if (res == STATUS_OK)
                            res         = split_edge(ce, spp);
                        break;

                    case 0x26: // Point lays on edge 1, split triangle's edge
                        spp         = vertex.alloc(&sp);
                        if (spp == NULL)
                            return STATUS_NO_MEM;
//                        spp->d      = dsp::calc_sqr_distance_p2(spp, &view.s);
                        res         = split_edge(ct->e[1], spp);
                        if (res == STATUS_OK)
                            res         = split_edge(ce, spp);
                        break;

                    case 0x29: // Point lays on edge 0, split triangle's edge
                        spp         = vertex.alloc(&sp);
                        if (spp == NULL)
                            return STATUS_NO_MEM;
//                        spp->d      = dsp::calc_sqr_distance_p2(spp, &view.s);
                        res         = split_edge(ct->e[0], spp);
                        if (res == STATUS_OK)
                            res         = split_edge(ce, spp);
                        break;

                    case 0x2a: // Point lays inside of the triangle, split triangle's edge
                        spp         = vertex.alloc(&sp);
                        if (spp == NULL)
                            return STATUS_NO_MEM;
//                        spp->d      = dsp::calc_sqr_distance_p2(spp, &view.s);
                        res         = split_triangle(ct, spp);
                        if (res == STATUS_OK)
                            res         = split_edge(ce, spp);
                        break;

                    default: // Point is not crossing triangle
                        continue;
                }

                // Check final result
                if (res != STATUS_OK)
                    return res;

                // Current triangle's structure has been modified, update split planes' equations
                dsp::calc_plane_v1p2(&spl[0], &pl, ct->v[0], ct->v[1]);
                dsp::calc_plane_v1p2(&spl[1], &pl, ct->v[1], ct->v[2]);
                dsp::calc_plane_v1p2(&spl[2], &pl, ct->v[2], ct->v[0]);
            RT_FOREACH_END
        }
        return STATUS_OK;
    }



    status_t rt_context_t::cutoff(const vector3d_t *pl)
    {
        status_t res;

        // Is there data for processing ?
        if (triangle.size() <= 0)
            return STATUS_OK;

        rt_context_t in;
        RT_TRACE(debug, in.set_debug_context(debug); );

        // State of itag for vertex:
        //  0   = vertex lays over the plane
        //  1   = vertex lays on the plane
        //  2   = vertex lays below the plane

        // State of itag for triangle:
        //  0   = triangle is 'out'
        //  1   = triangle is 'on'
        //  2   = triangle is 'in'
        RT_FOREACH(rtm_vertex_t, v, vertex)
            float t         = v->x*pl->dx + v->y*pl->dy + v->z*pl->dz + pl->dw;
            v->ptag         = NULL; // Cleanup tag pointer
            v->itag         = (t < -DSP_3D_TOLERANCE) ? 2 : (t > DSP_3D_TOLERANCE) ? 0 : 1;
		RT_FOREACH_END

        // First step: split edges
        // Perform split of edges
		RT_FOREACH(rtm_edge_t, e, edge)
		    e->ptag     = NULL; // Cleanup tag pointer

            // Analyze state of edge
            // 00 00    - edge is over the plane
            // 00 01    - edge is over the plane
            // 00 10    - edge is crossing the plane
            // 01 00    - edge is over the plane
            // 01 01    - edge is laying on the plane
            // 01 10    - edge is under the plane
            // 10 00    - edge is crossing the plane
            // 10 01    - edge is under the plane
            // 10 10    - edge is under the plane
            switch ((e->v[0]->itag << 2) | e->v[1]->itag)
            {
                case 0: case 1: case 4: // edge is over the plane, skip
                case 6: case 9: case 10: // edge is under the plane, skip
                    break;
                case 5: // edge lays on the plane, mark as split edge and skip
                    e->itag    |= RT_EF_PLANE;
                    break;

                case 2: // edge is crossing the plane, v0 is over, v1 is under
                case 8: // edge is crossing the plane, v0 is under, v1 is over
                {
                    // Allocate split point
                    rtm_vertex_t *sp     = vertex.alloc();
                    if (sp == NULL)
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(sp, e->v[0], e->v[1], pl);

//                    sp->ve      = NULL;
                    sp->ptag    = NULL;
                    sp->itag    = 1;        // Split-point lays on the plane

                    res         = split_edge(e, sp);
                    if (res != STATUS_OK)
                        return res;
                    break;
                }

                default:
                    return STATUS_BAD_STATE;
            }
		RT_FOREACH_END

        RT_VALIDATE(
            if (!validate())
                return STATUS_CORRUPTED;
        )

        // Toggle state of all triangles
		size_t num_inside = 0;
		RT_FOREACH(rtm_triangle_t, st, triangle)
		    st->ptag    = NULL; // Cleanup tag pointer

            // Detect position of triangle: over the plane or under the plane
            if (st->v[0]->itag != 1)
                st->itag    = st->v[0]->itag;
            else if (st->v[1]->itag != 1)
                st->itag    = st->v[1]->itag;
            else
                st->itag    = st->v[2]->itag;

		    if (st->itag == 2)
		        ++num_inside;

            RT_TRACE(debug,
                if (st->itag != 2)
                    ignore(st);
            )
		RT_FOREACH_END

		if (num_inside <= 0)
		{
		    vertex.clear();
		    triangle.clear();
		    edge.clear();
		    return STATUS_OK;
		}

        // Now we can fetch triangles
        res    = fetch_triangles(&in, 2);
        if (res != STATUS_OK)
            return res;
        complete_fetch(&in);
        this->swap(&in);

        RT_VALIDATE(
            if (!in.validate())
                return STATUS_CORRUPTED;
            if (!validate())
                return STATUS_CORRUPTED;
        )

        return STATUS_OK;
    }

    status_t rt_context_t::edge_split(rt_context_t *out)
    {
        // Is there data for processing ?
        if (triangle.size() <= 1)
        {
            state   = S_REFLECT;
            return STATUS_OK;
        }

        // Find edge to apply split
        vector3d_t pl;
        rtm_edge_t *ce = NULL;

        RT_FOREACH(rtm_edge_t, se, edge)
            if (se->itag & RT_EF_PLANE)
                continue;
            if (!(se->itag & RT_EF_APPLY))
                continue;

            // We need to check that edge can be applied
            se->itag       |= RT_EF_PLANE; // Mark current edge as processed
            if (dsp::calc_plane_p3(&pl, &view.s, se->v[0], se->v[1]) > DSP_3D_TOLERANCE) // Ensure that the normal vector is valid
            {
                ce = se;
                RT_FOREACH_BREAK;
            }
        RT_FOREACH_END

        return (ce != NULL) ? apply_edge_split(out, ce, &pl) : STATUS_NOT_FOUND;
    }

    status_t rt_context_t::triangle_split(rt_context_t *out)
    {
        rtm_triangle_t *ct = triangle.get(0);
        if (ct == NULL)
            return STATUS_NOT_FOUND;

        vector3d_t pl;
        rtm_edge_t *ce = NULL;

        for (size_t i=0; i<3; ++i)
        {
            rtm_edge_t *se = ct->e[i];
            if (se->itag & RT_EF_PLANE)
                continue;
            if (!(se->itag & RT_EF_APPLY))
                continue;

            // We need to check that edge can be applied
            se->itag       |= RT_EF_PLANE; // Mark current edge as processed
            if (dsp::calc_plane_p3(&pl, &view.s, se->v[0], se->v[1]) > DSP_3D_TOLERANCE) // Ensure that the normal vector is valid
            {
                ce = se;
                break;
            }
        }

        return (ce != NULL) ? apply_edge_split(out, ce, &pl) : STATUS_NOT_FOUND;
    }

    status_t rt_context_t::apply_edge_split(rt_context_t *out, rtm_edge_t *ce, const vector3d_t *pl)
    {
        status_t res;
        if (out != NULL)
            out->clear();

        // Perform split
        RT_TRACE_BREAK(debug,
            lsp_trace("Prepare split by edge (%f, %f, %f, %f) (%d triangles)",
                    pl->dx, pl->dy, pl->dz, pl->dw,
                    int(triangle.size()));

            for (size_t i=0,n=triangle.size(); i<n; ++i)
                trace.add_triangle_1c(triangle.get(i), &C_YELLOW);
            trace.add_plane_3pn1c(&view.s, ce->v[0], ce->v[1], pl, &C_MAGENTA);

            for (size_t i=0,n=edge.size(); i<n; ++i)
            {
                rtm_edge_t *se = edge.get(i);
                if (se->itag & RT_EF_PLANE)
                    trace.add_segment(se, &C_GREEN);
                else if (se == ce)
                    trace.add_segment(se, &C_MAGENTA);
                else if (se->itag & RT_EF_APPLY)
                    trace.add_segment(se, &C_RED);
                else
                    trace.add_segment(se, &C_GREEN);
            }
        );

        // State of itag for vertex:
        //  0   = vertex lays over the plane
        //  1   = vertex lays on the plane
        //  2   = vertex lays below the plane

        // State of itag for triangle:
        //  0   = triangle is 'out'
        //  1   = triangle is 'on'
        //  2   = triangle is 'in'
        RT_FOREACH(rtm_vertex_t, v, vertex)
            float t         = v->x*pl->dx + v->y*pl->dy + v->z*pl->dz + pl->dw;
            v->ptag         = NULL; // Cleanup tag pointer
            v->itag         = (t < -DSP_3D_TOLERANCE) ? 2 : (t > DSP_3D_TOLERANCE) ? 0 : 1;
        RT_FOREACH_END

        // Reset all flags of edges


        // First step: split edges
        // Perform split of edges
        RT_FOREACH(rtm_edge_t, e, edge)
            e->ptag         = NULL; // Cleanup tag pointer
            if (e == ce) // Skip current edge
                continue;

            // Analyze state of edge
            // 00 00    - edge is over the plane
            // 00 01    - edge is over the plane
            // 00 10    - edge is crossing the plane
            // 01 00    - edge is over the plane
            // 01 01    - edge is laying on the plane
            // 01 10    - edge is under the plane
            // 10 00    - edge is crossing the plane
            // 10 01    - edge is under the plane
            // 10 10    - edge is under the plane
            switch ((e->v[0]->itag << 2) | e->v[1]->itag)
            {
                case 0x0: case 0x1: case 0x4: // edge is over the plane, skip
                case 0x6: case 0x9: case 0xa: // edge is under the plane, skip
                    break;
                case 0x5: // edge lays on the plane, mark as part of the split plane and skip
                    e->itag    |= RT_EF_PLANE;
                    break;

                case 0x2: // edge is crossing the plane, v0 is over, v1 is under
                case 0x8: // edge is crossing the plane, v0 is under, v1 is over
                {
                    // Allocate split point
                    rtm_vertex_t *sp     = vertex.alloc();
                    if (sp == NULL)
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(sp, e->v[0], e->v[1], pl);

//                    sp->ve      = NULL;
                    sp->ptag    = NULL;
                    sp->itag    = 1;        // Split-point lays on the plane

                    res         = split_edge(e, sp);
                    if (res != STATUS_OK)
                        return res;
                    break;
                }

                default:
                    return STATUS_BAD_STATE;
            }
        RT_FOREACH_END

        RT_VALIDATE(
            if (!validate())
                return STATUS_CORRUPTED;
        )

        // Toggle state of all triangles
        RT_FOREACH(rtm_triangle_t, st, triangle)
            st->ptag        = NULL; // Cleanup tag pointer
            // Detect position of triangle: over the plane or under the plane
            if (st->v[0]->itag != 1)
                st->itag    = st->v[0]->itag;
            else if (st->v[1]->itag != 1)
                st->itag    = st->v[1]->itag;
            else
                st->itag    = st->v[2]->itag;
        RT_FOREACH_END

        // Now we can fetch triangles
        rt_context_t in;
        RT_TRACE(debug, in.set_debug_context(debug); );

        // Fetch 'in' triangles in unsafe mode (we have cleaned up all tag pointers)
        res     = fetch_triangles(&in, 2);
        if (res != STATUS_OK)
            return res;
        complete_fetch(&in);

        // Fetch 'out' triangles in safe mode
        res    = fetch_triangles_safe(out, 0);
        if (res != STATUS_OK)
            return res;

        RT_TRACE(debug,
            RT_FOREACH(rtm_triangle_t, t, triangle)
                if (t->itag == 1)
                    ignore(t);
            RT_FOREACH_END
        )

        RT_VALIDATE(
            if (!in.validate())
                return STATUS_CORRUPTED;
            if (!out->validate())
                return STATUS_CORRUPTED;
            if (!validate())
                return STATUS_CORRUPTED;
        )
        this->swap(&in);

        RT_TRACE_BREAK(debug,
            lsp_trace("After split by edge");

            lsp_trace("IN context is GREEN (%d triangles)", int(triangle.size()));
            for (size_t i=0,n=triangle.size(); i<n; ++i)
                trace.add_triangle_1c(triangle.get(i), &C_GREEN);

            if (out != NULL)
            {
                lsp_trace("OUT context is RED (%d triangles)", int(out->triangle.size()));
                for (size_t i=0,n=out->triangle.size(); i<n; ++i)
                    trace.add_triangle_1c(out->triangle.get(i), &C_RED);
            }
        );

        return STATUS_OK;
    }

    /**
     * Perform depth-testing cullback of faces
     * @return status of operation
     */
    status_t rt_context_t::depth_cullback()
    {
        /* itag status:
             0 = out (above culling plane)
             1 = on the culling plane
             2 = in (below culling plane), default
         */
        RT_FOREACH(rtm_triangle_t, t, triangle)
            t->itag     = 2;
        RT_FOREACH_END

        rtm_triangle_t *ct;
        vector3d_t pl;
        size_t itag;

        RT_TRACE_BREAK(debug,
            lsp_trace("Prepare depth test");
            trace.add_view_1c(&view, &C_MAGENTA);
            for (size_t j=0; j<triangle.size(); ++j)
                trace.add_triangle_1c(triangle.get(j), &C_YELLOW);
            for (size_t j=0; j<edge.size(); ++j)
                trace.add_segment(edge.get(j), &C_GREEN);
        )

        while (true)
        {
            // Select triangle for culling
            ct = NULL;

            RT_FOREACH(rtm_triangle_t, st, triangle)
                if (st->itag == 2)
                {
                    ct = st;
                    RT_FOREACH_BREAK;
                }
            RT_FOREACH_END

            // Is there a triangle for processing?
            if (ct == NULL)
                break;

            // Prepare culling plane
            dsp::orient_plane_v1p1(&pl, &view.s, &ct->n);

            RT_TRACE_BREAK(debug,
                lsp_trace("Doing depth test for triangle %d/%d", int(triangle.index_of(ct)), int(triangle.size()));
                trace.add_view_1c(&view, &C_MAGENTA);
                for (size_t j=0; j<triangle.size(); ++j)
                {
                    rtm_triangle_t *t = triangle.get(j);
                    trace.add_triangle_1c(t, (t == ct) ? &C_ORANGE : &C_YELLOW);
                }
                for (size_t j=0; j<edge.size(); ++j)
                    trace.add_segment(edge.get(j), &C_GREEN);
                trace.add_plane_sp3p1c(&view.s, ct->v[0], ct->v[1], ct->v[2], &C_MAGENTA);
            )

            // Estimate location of each vertex relative to the plane
            RT_FOREACH(rtm_vertex_t, sv, vertex)
                float t         = sv->x*pl.dx + sv->y*pl.dy + sv->z*pl.dz + pl.dw;
                sv->itag        = (t < -DSP_3D_TOLERANCE) ? 2 : (t > DSP_3D_TOLERANCE) ? 0 : 1;
            RT_FOREACH_END

            // Now process all triangles
            RT_FOREACH(rtm_triangle_t, st, triangle)
                if (st->itag == 0) // Triangle is already 'out', skip
                    continue;
                else if (st->face == ct->face) // Triangles laying on the same face are always considered to be 'on'
                {
                    st->itag = 1;
                    continue;
                }

                // Detect new itag
                if (st->v[0]->itag != 1)
                    itag        = st->v[0]->itag;
                else if (st->v[1]->itag != 1)
                    itag        = st->v[1]->itag;
                else
                    itag        = st->v[2]->itag;

                // Update itag of triangle
                if (!((st->itag == 1) && (itag == 2))) // 'on' -> 'in' change not possible
                    st->itag    = itag;
            RT_FOREACH_END

            RT_TRACE_BREAK(debug,
                lsp_trace("After depth test for triangle %d/%d, in=GREEN, on=BLUE, out=RED", int(triangle.index_of(ct)), int(triangle.size()));
                trace.add_view_1c(&view, &C_MAGENTA);
                for (size_t j=0; j<triangle.size(); ++j)
                {
                    rtm_triangle_t *t = triangle.get(j);
                    trace.add_triangle_1c(t,
                            (t->itag == 0) ? &C_RED :
                            (t->itag == 1) ? &C_BLUE : &C_GREEN
                        );
                }
                trace.add_plane_sp3p1c(&view.s, ct->v[0], ct->v[1], ct->v[2], &C_YELLOW);
            )
        }

        // Trace all ignored triangles
        RT_TRACE(debug,
            RT_FOREACH(rtm_triangle_t, t, triangle)
                if (t->itag != 1)
                    ignore(t);
            RT_FOREACH_END
        )

        // Now we have all triangles only in 'out' and 'on' state
        rt_context_t tmp;
        RT_TRACE(debug,  tmp.set_debug_context(debug); )
        status_t res = fetch_triangles_safe(&tmp, 1);
        if (res != STATUS_OK)
            return res;
        this->swap(&tmp);

        RT_TRACE_BREAK(debug,
            lsp_trace("After depth test triangles=%d", int(triangle.size()));
            trace.add_view_1c(&view, &C_MAGENTA);
            for (size_t j=0; j<triangle.size(); ++j)
            {
                rtm_triangle_t *t = triangle.get(j);
                trace.add_triangle_1c(t, &C_GREEN);
            }
            for (size_t j=0; j<edge.size(); ++j)
                trace.add_segment(edge.get(j), &C_YELLOW);
        )

        return STATUS_OK;
    }

    status_t rt_context_t::add_object(Object3D *obj, ssize_t oid, rt_material_t *material)
    {
        return add_object(obj, oid, obj->matrix(), material);
    }

    status_t rt_context_t::add_object(Object3D *obj, ssize_t oid, const matrix3d_t *transform, rt_material_t *material)
    {
        // Reset tags
        obj->scene()->init_tags(NULL, 0);
        RT_VALIDATE(
            if (!obj->scene()->validate())
                return STATUS_CORRUPTED;
        )

//        lsp_trace("Processing object \"%s\"", obj->get_name());
        size_t start_t  = triangle.size();
        size_t start_e  = edge.size();

        // Clone triangles and apply object matrix to vertexes
        for (size_t i=0, n=obj->num_triangles(); i<n; ++i)
        {
            obj_triangle_t *st = obj->triangle(i);
            if (st == NULL)
                return STATUS_BAD_STATE;
            else if (st->ptag != NULL) // Skip already emitted triangle
                continue;

            if ((view.oid == oid) && (view.face == st->face))
            {
                RT_TRACE(debug,
                    v_triangle3d_t it;
                    it.p[0] = *(st->v[0]);
                    it.p[1] = *(st->v[1]);
                    it.p[2] = *(st->v[2]);
                    it.n[0] = *(st->n[0]);
                    it.n[1] = *(st->n[1]);
                    it.n[2] = *(st->n[2]);
                    ignored.add(&it);
                );
                continue;
            }

            // Allocate triangle and store pointer
            rtm_triangle_t *dt = triangle.alloc();
            if (dt == NULL)
                return STATUS_NO_MEM;

            dt->elnk[0] = NULL;
            dt->elnk[1] = NULL;
            dt->elnk[2] = NULL;
            dt->ptag    = st;
            dt->itag    = 0;
            dt->oid     = oid;
            dt->face    = st->face;
            st->ptag    = dt;
            dt->m       = material;

//            lsp_trace("Link rt_triangle[%p] to obj_triangle[%p]", dt, st);

            // Copy data
            for (size_t j=0; j<3; ++j)
            {
                // Allocate vertex
                rtm_vertex_t *vx     = reinterpret_cast<rtm_vertex_t *>(st->v[j]->ptag);
                if (st->v[j]->ptag == NULL)
                {
                    vx              = vertex.alloc();
                    if (vx == NULL)
                        return STATUS_NO_MEM;

                    dsp::apply_matrix3d_mp2(vx, st->v[j], transform);
//                    vx->d           = dsp::calc_sqr_distance_p2(vx, &view.s);
//                    vx->ve          = NULL;
                    vx->ptag        = st->v[j];
                    vx->itag        = 0;

                    st->v[j]->ptag  = vx;
//                    lsp_trace("Link #%d rt_vertex[%p] to obj_vertex[%p]", int(j), vx, st->v[j]);
                }

                // Allocate edge
                rtm_edge_t *ex       = reinterpret_cast<rtm_edge_t *>(st->e[j]->ptag);
                if (ex == NULL)
                {
                    ex              = edge.alloc();
                    if (ex == NULL)
                        return STATUS_NO_MEM;

                    ex->v[0]        = NULL;
                    ex->v[1]        = NULL;
                    ex->vt          = NULL;
                    ex->ptag        = st->e[j];
                    ex->itag        = 0;

                    st->e[j]->ptag  = ex;
//                    lsp_trace("Link #%d rt_edge[%p] to obj_edge[%p]", int(j), ex, st->e[j]);
                }

                dt->v[j]        = vx;
                dt->e[j]        = ex;
            }

            // Update normals
            dsp::apply_matrix3d_mv2(&dt->n, st->n[0], transform);
            dt->n.dw    = - (dt->n.dx * dt->v[0]->x + dt->n.dy * dt->v[0]->y + dt->n.dz * dt->v[0]->z);
        }

        // Patch edge structures and link to vertexes
        for (size_t i=start_e, n=edge.size(); i<n; ++i)
        {
            rtm_edge_t *de       = edge.get(i);
            obj_edge_t *se      = reinterpret_cast<obj_edge_t *>(de->ptag);

//            lsp_trace("patching rt_edge[%p] with obj_edge[%p]", de, se);
            de->v[0]            = reinterpret_cast<rtm_vertex_t *>(se->v[0]->ptag);
            de->v[1]            = reinterpret_cast<rtm_vertex_t *>(se->v[1]->ptag);
        }

        // Patch triangle structures and link to edges
        for (size_t i=start_t, n=triangle.size(); i<n; ++i)
        {
            rtm_triangle_t *dt   = triangle.get(i);
            obj_triangle_t *st  = reinterpret_cast<obj_triangle_t *>(dt->ptag);

//            lsp_trace("patching rt_triangle[%p] with obj_triangle[%p]", dt, st);

            dt->v[0]            = reinterpret_cast<rtm_vertex_t *>(st->v[0]->ptag);
            dt->v[1]            = reinterpret_cast<rtm_vertex_t *>(st->v[1]->ptag);
            dt->v[2]            = reinterpret_cast<rtm_vertex_t *>(st->v[2]->ptag);

            dt->e[0]            = reinterpret_cast<rtm_edge_t *>(st->e[0]->ptag);
            dt->e[1]            = reinterpret_cast<rtm_edge_t *>(st->e[1]->ptag);
            dt->e[2]            = reinterpret_cast<rtm_edge_t *>(st->e[2]->ptag);

            // Link triangle to the edge
            dt->elnk[0]         = dt->e[0]->vt;
            dt->elnk[1]         = dt->e[1]->vt;
            dt->elnk[2]         = dt->e[2]->vt;

            dt->e[0]->vt        = dt;
            dt->e[1]->vt        = dt;
            dt->e[2]->vt        = dt;
        }

        RT_VALIDATE(
            if (!obj->scene()->validate())
                return STATUS_CORRUPTED;

            if (!validate())
                return STATUS_CORRUPTED;
        )

        return STATUS_OK;
    }

    status_t rt_context_t::fetch_objects(rt_mesh_t *src, size_t n, const size_t *ids)
    {
        status_t res;
        rt_context_t tmp;
        RT_TRACE(debug, tmp.set_debug_context(debug); );

        if (n > 0)
        {
            // Cleanup tag pointers
            RT_FOREACH(rtm_vertex_t, v, src->vertex)
                v->ptag = NULL;
            RT_FOREACH_END

            RT_FOREACH(rtm_edge_t, e, src->edge)
                e->ptag = NULL;
            RT_FOREACH_END

            // Match triangles by object identifier
            size_t fetched = 0;
            RT_FOREACH(rtm_triangle_t, t, src->triangle)
                t->ptag         = NULL; // Clean ptag

                // Skip triangles that should be ignored
                if ((t->oid == view.oid) && (t->face == view.face))
                {
                    RT_TRACE(debug, ignore(t); );
                    continue;
                }

                // Check that triangle matches specified object
                for (size_t i=0; i<n; ++i)
                    if (t->oid == ssize_t(ids[i]))
                    {
                        res     = src->fetch_triangle(&tmp, t);
                        if (res != STATUS_OK)
                            return res;
                        ++fetched;
                        break;
                    }
            RT_FOREACH_END;

            // Complete fetch if there are any matched triangles
            if (fetched > 0)
                src->complete_fetch(&tmp);
        }

        RT_VALIDATE(
            if (!tmp.validate())
                return STATUS_CORRUPTED;
            if (!src->validate())
                return STATUS_CORRUPTED;
            if (!validate())
                return STATUS_CORRUPTED;
        );

        tmp.swap(this);
        return STATUS_OK;
    }

    ssize_t rt_context_t::linked_count(rtm_triangle_t *t, rtm_edge_t *e)
    {
        if ((t == NULL) || (e == NULL))
            return -1;

        size_t n = 0;
        for (rtm_triangle_t *p = e->vt; p != NULL; )
        {
            if ((p->e[0] == p->e[1]) || (p->e[0] == p->e[2]) || (p->e[1] == p->e[2]))
                return -1;

            if (p == t)
                ++n;

            if (p->e[0] == e)
                p = p->elnk[0];
            else if (p->e[1] == e)
                p = p->elnk[1];
            else if (p->e[2] == e)
                p = p->elnk[2];
            else
                return -1;
        }

        return n;
    }

    bool rt_context_t::validate_list(rtm_edge_t *e)
    {
        rtm_triangle_t *t = e->vt;
        size_t n = 0;

        while (t != NULL)
        {
            if (!triangle.validate(t))
                return false;

            ++n;
            if (t->e[0] == e)
                t   = t->elnk[0];
            else if (t->e[1] == e)
                t   = t->elnk[1];
            else if (t->e[2] == e)
                t   = t->elnk[2];
            else
                return false;
        }

        if (n <= 0)
        {
            lsp_trace("Edge has no link with triangle");
        }

        return n > 0; // The edge should be linked at least to one triangle
    }

    bool rt_context_t::validate()
    {
        for (size_t i=0, n=vertex.size(); i<n; ++i)
        {
            rtm_vertex_t *v      = vertex.get(i);
            if (v == NULL)
                return false;
//            if (!validate_list(v))
//                return false;
        }

        for (size_t i=0, n=edge.size(); i<n; ++i)
        {
            rtm_edge_t *e        = edge.get(i);
            if (e == NULL)
                return false;
            if (!validate_list(e))
                return false;

            for (size_t j=0; j<2; ++j)
            {
                if (e->v[j] == NULL)
                    return false;
                if (!vertex.validate(e->v[j]))
                    return false;
            }
        }

        for (size_t i=0, n=triangle.size(); i<n; ++i)
        {
            rtm_triangle_t *t    = triangle.get(i);
            if (t == NULL)
                return false;

            for (size_t j=0; j<3; ++j)
            {
                if (t->v[j] == NULL)
                    return false;
                if (t->e[j] == NULL)
                    return false;
                if (!vertex.validate(t->v[j]))
                    return false;
                if (!edge.validate(t->e[j]))
                    return false;
                if (!triangle.validate(t->elnk[j]))
                    return false;
                if (linked_count(t, t->e[j]) != 1)
                    return false;
            }
        }

        return true;
    }
#endif


} /* namespace mtest */
