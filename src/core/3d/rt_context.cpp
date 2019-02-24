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
        vertex(1024),
        edge(1024),
        triangle(1024)
    {
        this->state     = S_SCAN_OBJECTS;
        this->debug     = NULL;

        // Initialize point of view
        view.amplitude  = 0.0f;
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
        vertex(1024),
        edge(1024),
        triangle(1024)
    {
        this->state     = S_SCAN_OBJECTS;
        this->debug     = NULL;
        this->view      = *view;
    }

    rt_context_t::rt_context_t(const rt_view_t *view, rt_context_state_t state):
        vertex(1024),
        edge(1024),
        triangle(1024)
    {
        this->state     = state;
        this->debug     = NULL;
        this->view      = *view;
    }

    rt_context_t::~rt_context_t()
    {
        debug           = NULL;
        flush();
    }

    void rt_context_t::set_debug_context(rt_debug_t *debug)
    {
        this->debug     = debug;
    }

    void rt_context_t::flush()
    {
        vertex.destroy();
        edge.destroy();
        triangle.destroy();
        matched.flush();
        ignored.flush();
        trace.clear_all();
    }

    void rt_context_t::init_view(const point3d_t *sp, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
    {
        view.s          = *sp;
        view.p[0]       = *p0;
        view.p[1]       = *p1;
        view.p[2]       = *p2;
        view.face       = -1;
        view.time[0]    = 0.0f;
        view.time[1]    = 0.0f;
        view.time[2]    = 0.0f;
        view.amplitude  = 1.0f;
        view.speed      = SOUND_SPEED_M_S;
        view.rnum       = 0;
    }

    void rt_context_t::init_view(const point3d_t *sp, const point3d_t *pv)
    {
        view.s          = *sp;
        view.p[0]       = pv[0];
        view.p[1]       = pv[1];
        view.p[2]       = pv[2];
        view.face       = -1;
        view.time[0]    = 0.0f;
        view.time[1]    = 0.0f;
        view.time[2]    = 0.0f;
        view.amplitude  = 1.0f;
        view.speed      = SOUND_SPEED_M_S;
        view.rnum       = 0;
    }

    void rt_context_t::clear()
    {
        vertex.clear();
        edge.clear();
        triangle.clear();
    }

    void rt_context_t::swap(rt_context_t *src)
    {
        vertex.swap(&src->vertex);
        edge.swap(&src->edge);
        triangle.swap(&src->triangle);
    }

//    bool rt_context_t::unlink_edge(rt_edge_t *e, rt_vertex_t *v)
//    {
//        for (rt_edge_t **pcurr = &v->ve; *pcurr != NULL; )
//        {
//            rt_edge_t *curr = *pcurr;
//            rt_edge_t **pnext = (curr->v[0] == v) ? &curr->vlnk[0] :
//                                (curr->v[1] == v) ? &curr->vlnk[1] :
//                                NULL;
//            if (pnext == NULL) // Unexpected behaviour
//                return false;
//
//            if (curr == e)
//            {
//                *pcurr = *pnext;
//                return true;
//            }
//            pcurr = pnext;
//        }
//        return false;
//    }

    bool rt_context_t::unlink_triangle(rt_triangle_t *t, rt_edge_t *e)
    {
        for (rt_triangle_t **pcurr = &e->vt; *pcurr != NULL; )
        {
            rt_triangle_t *curr = *pcurr;
            rt_triangle_t **pnext = (curr->e[0] == e) ? &curr->elnk[0] :
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

    status_t rt_context_t::arrange_triangle(rt_triangle_t *ct, rt_edge_t *e)
    {
        rt_vertex_t *tv;
        rt_edge_t *te;
        rt_triangle_t *tt;

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

    status_t rt_context_t::split_edge(rt_edge_t* e, rt_vertex_t* sp)
    {
        status_t res;
        rt_triangle_t *ct, *nt, *pt;
        rt_edge_t *ne, *se;

//        RT_TRACE_BREAK(this,
//            lsp_trace("Splitting edge");
//            for (rt_triangle_t *t = e->vt; t != NULL;)
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
      //ne->vlnk[0]     = NULL;
      //ne->vlnk[1]     = NULL;
        ne->ptag        = NULL;
        ne->itag        = e->itag;

//        ne->vlnk[0]     = ne->v[0]->ve;
//        ne->vlnk[1]     = ne->v[1]->ve;
//        ne->v[0]->ve    = ne;
//        ne->v[1]->ve    = ne;

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

//            se->vlnk[0]     = se->v[0]->ve;
//            se->vlnk[1]     = se->v[1]->ve;
//            se->v[0]->ve    = se;
//            se->v[1]->ve    = se;

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

//                e->vlnk[0]      = e->v[0]->ve;
//                e->vlnk[1]      = e->v[1]->ve;
//                e->v[0]->ve     = e;
//                e->v[1]->ve     = e;

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

    status_t rt_context_t::split_triangle(rt_triangle_t* t, rt_vertex_t* sp)
    {
        // Unlink triangle from all it's edges
        unlink_triangle(t, t->e[0]);
        unlink_triangle(t, t->e[1]);
        unlink_triangle(t, t->e[2]);

        // Create additional edges and link them to vertexes
        rt_edge_t *ne[3];
        for (size_t i=0; i<3; ++i)
        {
            rt_edge_t *e    = edge.alloc();
            if (e == NULL)
                return STATUS_NO_MEM;
            ne[i]           = e;

            e->v[0]         = t->v[i];
            e->v[1]         = sp;
            e->vt           = NULL;
            e->ptag         = NULL;
            e->itag         = 0;
//            e->vlnk[0]      = e->v[0]->ve;
//            e->vlnk[1]      = e->v[1]->ve;
//            e->v[0]->ve     = e;
//            e->v[1]->ve     = e;
        }

        // Allocate additional triangles
        rt_triangle_t *nt[3];
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
            rt_triangle_t *ct   = nt[i];

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
        RT_FOREACH(rt_vertex_t, v, vertex)
            v->ptag = NULL;
        RT_FOREACH_END

        RT_FOREACH(rt_edge_t, e, edge)
            e->ptag = NULL;
        RT_FOREACH_END

        RT_FOREACH(rt_triangle_t, t, triangle)
            t->ptag = NULL;
        RT_FOREACH_END
    }

    status_t rt_context_t::fetch_triangle(rt_context_t *dst, rt_triangle_t *st)
    {
        rt_vertex_t *sv, *vx;
        rt_edge_t *se, *ex;
        rt_triangle_t *tx;

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
            vx      = reinterpret_cast<rt_vertex_t *>(sv->ptag);

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
            ex      = reinterpret_cast<rt_edge_t *>(se->ptag);
            if (ex == NULL)
            {
                ex              = dst->edge.alloc();
                if (ex == NULL)
                    return STATUS_NO_MEM;

                ex->v[0]        = se->v[0];
                ex->v[1]        = se->v[1];
                ex->vt          = NULL;
//                ex->vlnk[0]     = NULL;
//                ex->vlnk[1]     = NULL;
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

    status_t rt_context_t::fetch_triangle_safe(rt_context_t *dst, rt_triangle_t *st)
    {
        if (dst == NULL)
            return STATUS_OK;
        return fetch_triangle(dst, st);
    }

    status_t rt_context_t::fetch_triangles(rt_context_t *dst, ssize_t itag)
    {
        // Iterate all triangles
        RT_FOREACH(rt_triangle_t, st, triangle)
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
        rt_triangle_t *st;
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
        RT_FOREACH(rt_edge_t, ex, dst->edge)
            rt_edge_t *se   = reinterpret_cast<rt_edge_t *>(ex->ptag);
            if (se == NULL) // Edge does not need patching
                continue;

            // Patch vertex pointers if needed
            ex->v[0]        = (se->v[0]->ptag != NULL) ? reinterpret_cast<rt_vertex_t *>(se->v[0]->ptag) : se->v[0];
            ex->v[1]        = (se->v[1]->ptag != NULL) ? reinterpret_cast<rt_vertex_t *>(se->v[1]->ptag) : se->v[1];

            // Link edge to vertexes
//            ex->vlnk[0]     = ex->v[0]->ve;
//            ex->vlnk[1]     = ex->v[1]->ve;
//            ex->v[0]->ve    = ex;
//            ex->v[1]->ve    = ex;
        RT_FOREACH_END

        // Link triangle structures to edges
        RT_FOREACH(rt_triangle_t, tx, dst->triangle)
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
        RT_FOREACH(rt_edge_t, ce, edge)
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

        Allocator3D<rt_edge_t> new_edges(edge.chunk_size());
        for (size_t i=0; i<ne; ++i)
        {
            rt_edge_t *se   = ve[i].e;
            rt_edge_t *de   = new_edges.alloc(se);
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

        // Patch all vertex pointers
//        RT_FOREACH(rt_vertex_t, v, vertex)
//            v->ve           = (v->ve != NULL) ? reinterpret_cast<rt_edge_t *>(v->ve->ptag) : NULL;
//        RT_FOREACH_END

        // Patch all edge pointers
//        RT_FOREACH(rt_edge_t, e, edge)
//            e->vlnk[0]      = (e->vlnk[0] != NULL) ? reinterpret_cast<rt_edge_t *>(e->vlnk[0]->ptag) : NULL;
//            e->vlnk[1]      = (e->vlnk[1] != NULL) ? reinterpret_cast<rt_edge_t *>(e->vlnk[1]->ptag) : NULL;
//        RT_FOREACH_END

        // Patch all triangle pointers
        RT_FOREACH(rt_triangle_t, t, triangle)
            t->e[0]         = reinterpret_cast<rt_edge_t *>(t->e[0]->ptag);
            t->e[1]         = reinterpret_cast<rt_edge_t *>(t->e[1]->ptag);
            t->e[2]         = reinterpret_cast<rt_edge_t *>(t->e[2]->ptag);
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
        RT_FOREACH(rt_triangle_t, ct, triangle)
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

        Allocator3D<rt_triangle_t> new_triangles(triangle.chunk_size());
        for (size_t i=0; i<nt; ++i)
        {
            rt_triangle_t *st   = vt[i].t;
            rt_triangle_t *dt   = new_triangles.alloc(st);
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
        RT_FOREACH(rt_edge_t, e, edge)
            e->vt           = (e->vt != NULL) ? reinterpret_cast<rt_triangle_t *>(e->vt->ptag) : NULL;
        RT_FOREACH_END

        // Patch all triangle pointers
        RT_FOREACH(rt_triangle_t, t, triangle)
            t->elnk[0]      = (t->elnk[0] != NULL) ? reinterpret_cast<rt_triangle_t *>(t->elnk[0]->ptag) : NULL;
            t->elnk[1]      = (t->elnk[1] != NULL) ? reinterpret_cast<rt_triangle_t *>(t->elnk[1]->ptag) : NULL;
            t->elnk[2]      = (t->elnk[2] != NULL) ? reinterpret_cast<rt_triangle_t *>(t->elnk[2]->ptag) : NULL;
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
            rt_triangle_t *ct   = triangle.get(i);
            dsp::calc_plane_p3(&pl, ct->v[0], ct->v[1], ct->v[2]);
            dsp::calc_plane_v1p2(&spl[0], &pl, ct->v[0], ct->v[1]);
            dsp::calc_plane_v1p2(&spl[1], &pl, ct->v[1], ct->v[2]);
            dsp::calc_plane_v1p2(&spl[2], &pl, ct->v[2], ct->v[0]);

//            RT_TRACE_BREAK(this,
//                lsp_trace("Solving conflicts for triangle %d/%d", int(i), int(triangle.size()));
//
//                for (size_t i=0,n=triangle.size(); i<n; ++i)
//                {
//                    rt_triangle_t *t = triangle.get(i);
//                    trace.add_triangle_1c(t, (t == ct) ? &C_ORANGE : &C_YELLOW);
//                }
//
//                trace.add_plane_3pn1c(ct->v[0], ct->v[1], ct->v[2], &pl, &C_MAGENTA);
//                trace.add_plane_2pn1c(ct->v[0], ct->v[1], &spl[0], &C_RED);
//                trace.add_plane_2pn1c(ct->v[1], ct->v[2], &spl[1], &C_GREEN);
//                trace.add_plane_2pn1c(ct->v[2], ct->v[0], &spl[2], &C_BLUE);
//            )

            // Estimate location of each vertex relative to the plane
            RT_FOREACH(rt_vertex_t, cv, vertex)
                float k = cv->x * pl.dx + cv->y * pl.dy + cv->z*pl.dz + pl.dw;
                cv->itag = (k < -DSP_3D_TOLERANCE) ? 2 : (k > DSP_3D_TOLERANCE) ? 0 : 1;
            RT_FOREACH_END

            // Split each edge with triangle, do not process new edges
            RT_FOREACH(rt_edge_t, ce, edge)
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
//                        rt_triangle_t *t = triangle.get(i);
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
                rt_vertex_t sp, *spp;
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

    status_t rt_context_t::cull_view()
    {
        vector3d_t pl[4]; // Split plane
        status_t res;

        // Mark all edges to be split
        RT_FOREACH(rt_edge_t, e, edge)
            e->itag        |= RT_EF_APPLY;
        RT_FOREACH_END

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
            res = cutoff(&pl[pi]);
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
                trace.add_triangle_3c(triangle.get(j), &C_CYAN, &C_MAGENTA, &C_YELLOW);
        );

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
        RT_FOREACH(rt_vertex_t, v, vertex)
            float t         = v->x*pl->dx + v->y*pl->dy + v->z*pl->dz + pl->dw;
            v->itag         = (t < -DSP_3D_TOLERANCE) ? 2 : (t > DSP_3D_TOLERANCE) ? 0 : 1;
		RT_FOREACH_END

        // First step: split edges
        // Perform split of edges
		RT_FOREACH(rt_edge_t, e, edge)
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
                    rt_vertex_t *sp     = vertex.alloc();
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
		RT_FOREACH(rt_triangle_t, st, triangle)
            // Detect position of triangle: over the plane or under the plane
            if (st->v[0]->itag != 1)
                st->itag    = st->v[0]->itag;
            else if (st->v[1]->itag != 1)
                st->itag    = st->v[1]->itag;
            else
                st->itag    = st->v[2]->itag;

            RT_TRACE(debug,
                if (st->itag != 2)
                    ignore(st);
            )
		RT_FOREACH_END

        // Now we can fetch triangles
        res    = fetch_triangles_safe(&in, 2);
        if (res != STATUS_OK)
            return res;

        RT_VALIDATE(
            if (!in.validate())
                return STATUS_CORRUPTED;
            if (!validate())
                return STATUS_CORRUPTED;
        )

        this->swap(&in);
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
        rt_edge_t *ce = NULL;

        RT_FOREACH(rt_edge_t, se, edge)
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
        rt_triangle_t *ct = triangle.get(0);
        if (ct == NULL)
            return STATUS_NOT_FOUND;

        vector3d_t pl;
        rt_edge_t *ce = NULL;

        for (size_t i=0; i<3; ++i)
        {
            rt_edge_t *se = ct->e[i];
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

    status_t rt_context_t::apply_edge_split(rt_context_t *out, rt_edge_t *ce, const vector3d_t *pl)
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
                rt_edge_t *se = edge.get(i);
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
        RT_FOREACH(rt_vertex_t, v, vertex)
            float t         = v->x*pl->dx + v->y*pl->dy + v->z*pl->dz + pl->dw;
            v->itag         = (t < -DSP_3D_TOLERANCE) ? 2 : (t > DSP_3D_TOLERANCE) ? 0 : 1;
        RT_FOREACH_END

        // Reset all flags of edges


        // First step: split edges
        // Perform split of edges
        RT_FOREACH(rt_edge_t, e, edge)
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
                    rt_vertex_t *sp     = vertex.alloc();
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
        RT_FOREACH(rt_triangle_t, st, triangle)
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
        res    = fetch_triangles_safe(&in, 2);
        if (res != STATUS_OK)
            return res;
        res    = fetch_triangles_safe(out, 0);
        if (res != STATUS_OK)
            return res;

        RT_TRACE(debug,
            RT_FOREACH(rt_triangle_t, t, triangle)
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
        RT_FOREACH(rt_triangle_t, t, triangle)
            t->itag     = 2;
        RT_FOREACH_END

        rt_triangle_t *ct;
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

            RT_FOREACH(rt_triangle_t, st, triangle)
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
                    rt_triangle_t *t = triangle.get(j);
                    trace.add_triangle_1c(t, (t == ct) ? &C_ORANGE : &C_YELLOW);
                }
                for (size_t j=0; j<edge.size(); ++j)
                    trace.add_segment(edge.get(j), &C_GREEN);
                trace.add_plane_sp3p1c(&view.s, ct->v[0], ct->v[1], ct->v[2], &C_MAGENTA);
            )

            // Estimate location of each vertex relative to the plane
            RT_FOREACH(rt_vertex_t, sv, vertex)
                float t         = sv->x*pl.dx + sv->y*pl.dy + sv->z*pl.dz + pl.dw;
                sv->itag        = (t < -DSP_3D_TOLERANCE) ? 2 : (t > DSP_3D_TOLERANCE) ? 0 : 1;
            RT_FOREACH_END

            // Now process all triangles
            RT_FOREACH(rt_triangle_t, st, triangle)
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
                    rt_triangle_t *t = triangle.get(j);
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
            RT_FOREACH(rt_triangle_t, t, triangle)
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
                rt_triangle_t *t = triangle.get(j);
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
            rt_triangle_t *dt = triangle.alloc();
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
                rt_vertex_t *vx     = reinterpret_cast<rt_vertex_t *>(st->v[j]->ptag);
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
                rt_edge_t *ex       = reinterpret_cast<rt_edge_t *>(st->e[j]->ptag);
                if (ex == NULL)
                {
                    ex              = edge.alloc();
                    if (ex == NULL)
                        return STATUS_NO_MEM;

                    ex->v[0]        = NULL;
                    ex->v[1]        = NULL;
                    ex->vt          = NULL;
//                    ex->vlnk[0]     = NULL;
//                    ex->vlnk[1]     = NULL;
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
            rt_edge_t *de       = edge.get(i);
            obj_edge_t *se      = reinterpret_cast<obj_edge_t *>(de->ptag);

//            lsp_trace("patching rt_edge[%p] with obj_edge[%p]", de, se);
            de->v[0]            = reinterpret_cast<rt_vertex_t *>(se->v[0]->ptag);
            de->v[1]            = reinterpret_cast<rt_vertex_t *>(se->v[1]->ptag);

//            de->vlnk[0]         = de->v[0]->ve;
//            de->vlnk[1]         = de->v[1]->ve;
//            de->v[0]->ve        = de;
//            de->v[1]->ve        = de;
        }

        // Patch triangle structures and link to edges
        for (size_t i=start_t, n=triangle.size(); i<n; ++i)
        {
            rt_triangle_t *dt   = triangle.get(i);
            obj_triangle_t *st  = reinterpret_cast<obj_triangle_t *>(dt->ptag);

//            lsp_trace("patching rt_triangle[%p] with obj_triangle[%p]", dt, st);

            dt->v[0]            = reinterpret_cast<rt_vertex_t *>(st->v[0]->ptag);
            dt->v[1]            = reinterpret_cast<rt_vertex_t *>(st->v[1]->ptag);
            dt->v[2]            = reinterpret_cast<rt_vertex_t *>(st->v[2]->ptag);

            dt->e[0]            = reinterpret_cast<rt_edge_t *>(st->e[0]->ptag);
            dt->e[1]            = reinterpret_cast<rt_edge_t *>(st->e[1]->ptag);
            dt->e[2]            = reinterpret_cast<rt_edge_t *>(st->e[2]->ptag);

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

    status_t rt_context_t::fetch_objects(rt_context_t *src, size_t n, const size_t *ids)
    {
        rt_context_t tmp;
        RT_TRACE(debug, tmp.set_debug_context(debug); );

        if (n > 0)
        {
            // Match triangles by object identifier
            RT_FOREACH(rt_triangle_t, t, src->triangle)
                t->itag         = 0; // Not matched

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
                        t->itag     = 1;
                        break;
                    }
            RT_FOREACH_END;

            status_t res    = src->fetch_triangles_safe(&tmp, 1);
            if (res != STATUS_OK)
                return res;
        }

        tmp.swap(this);
        return STATUS_OK;
    }

//    bool rt_context_t::validate_list(rt_vertex_t *v)
//    {
//        rt_edge_t *e = v->ve;
//        size_t n = 0;
//
//        while (e != NULL)
//        {
//            if (!edge.validate(e))
//                return false;
//
//            ++n;
//            if (e->v[0] == v)
//                e   = e->vlnk[0];
//            else if (e->v[1] == v)
//                e   = e->vlnk[1];
//            else
//                return false;
//        }
//
//        return n > 0; // The vertex should be linked at least to one edge
//    }

//    ssize_t rt_context_t::linked_count(rt_edge_t *e, rt_vertex_t *v)
//    {
//        if ((e == NULL) || (v == NULL))
//            return -1;
//
//        size_t n = 0;
//        for (rt_edge_t *p = v->ve; p != NULL; )
//        {
//            if (p->v[0] == p->v[1])
//                return -1;
//            if (p == e)
//                ++n;
//
//            if (p->v[0] == v)
//                p = p->vlnk[0];
//            else if (p->v[1] == v)
//                p = p->vlnk[1];
//            else
//                return -1;
//        }
//
//        return n;
//    }

    ssize_t rt_context_t::linked_count(rt_triangle_t *t, rt_edge_t *e)
    {
        if ((t == NULL) || (e == NULL))
            return -1;

        size_t n = 0;
        for (rt_triangle_t *p = e->vt; p != NULL; )
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

    bool rt_context_t::validate_list(rt_edge_t *e)
    {
        rt_triangle_t *t = e->vt;
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
            rt_vertex_t *v      = vertex.get(i);
            if (v == NULL)
                return false;
//            if (!validate_list(v))
//                return false;
        }

        for (size_t i=0, n=edge.size(); i<n; ++i)
        {
            rt_edge_t *e        = edge.get(i);
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
//                if (!edge.validate(e->vlnk[j]))
//                    return false;
//                if (linked_count(e, e->v[j]) != 1)
//                    return false;
            }
        }

        for (size_t i=0, n=triangle.size(); i<n; ++i)
        {
            rt_triangle_t *t    = triangle.get(i);
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

    status_t rt_context_t::ignore(const rt_triangle_t *t)
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

    status_t rt_context_t::match(const rt_triangle_t *t)
    {
        v_triangle3d_t vt;
        vt.p[0]     = *(t->v[0]);
        vt.p[1]     = *(t->v[1]);
        vt.p[2]     = *(t->v[2]);

        vt.n[0]     = t->n;
        vt.n[1]     = t->n;
        vt.n[2]     = t->n;

        return (matched.add(&vt)) ? STATUS_OK : STATUS_NO_MEM;
    }

    void rt_context_t::dump()
    {
        printf("Vertexes (%d items):\n", int(vertex.size()));
        for (size_t i=0,n=vertex.size(); i<n; ++i)
        {
            rt_vertex_t *vx = vertex.get(i);
            printf("  [%3d]: %p\n"
                   "    p:  (%.6f, %.6f, %.6f)\n",
                    int(i), vx,
                    vx->x, vx->y, vx->z
                );
//            dump_edge_list(4, vx->ve);
        }

        printf("Edges (%d items):\n", int(edge.size()));
        for (size_t i=0,n=edge.size(); i<n; ++i)
        {
            rt_edge_t *ex = edge.get(i);
            printf("  [%3d]: %p\n"
                   "    v:  [%d]-[%d]\n",
//                   "    l:  [%d]-[%d]\n",
                   int(i), ex,
                   int(vertex.index_of(ex->v[0])),
                   int(vertex.index_of(ex->v[1]))
//                   int(edge.index_of(ex->vlnk[0])),
//                   int(edge.index_of(ex->vlnk[1]))
               );
            dump_triangle_list(4, ex->vt);
        }

        printf("Triangles (%d items):\n", int(triangle.size()));
        for (size_t i=0,n=triangle.size(); i<n; ++i)
        {
            rt_triangle_t *vx = triangle.get(i);
            printf("  [%3d]: %p\n"
                   "    v:  [%d]-[%d]-[%d]\n"
                   "    e:  [%d]-[%d]-[%d]\n"
                   "    n:  (%.6f, %.6f, %.6f)\n"
                   "    l:  [%d]-[%d]-[%d]\n",
                    int(i), vx,
                    int(vertex.index_of(vx->v[0])),
                    int(vertex.index_of(vx->v[1])),
                    int(vertex.index_of(vx->v[2])),
                    int(edge.index_of(vx->e[0])),
                    int(edge.index_of(vx->e[1])),
                    int(edge.index_of(vx->e[2])),
                    vx->n.dx, vx->n.dy, vx->n.dz,
                    int(triangle.index_of(vx->elnk[0])),
                    int(triangle.index_of(vx->elnk[1])),
                    int(triangle.index_of(vx->elnk[2]))
                );
        }
    }

    void rt_context_t::dump_edge_list(size_t lvl, rt_edge_t *e)
    {
        for (size_t i=0; i<lvl; ++i)
            printf(" ");

        if (e == NULL)
        {
            printf("-1\n");
            return;
        }
        else
            printf("e[%d]:\n", int(edge.index_of(e)));
//        dump_edge_list(lvl+2, e->vlnk[0]);
//        dump_edge_list(lvl+2, e->vlnk[1]);
    }

    void rt_context_t::dump_triangle_list(size_t lvl, rt_triangle_t *t)
    {
        for (size_t i=0; i<lvl; ++i)
            printf(" ");

        if (t == NULL)
        {
            printf("-1\n");
            return;
        }
        else
            printf("t[%d]:\n", int(triangle.index_of(t)));
        dump_triangle_list(lvl+2, t->elnk[0]);
        dump_triangle_list(lvl+2, t->elnk[1]);
        dump_triangle_list(lvl+2, t->elnk[2]);
    }

} /* namespace mtest */
