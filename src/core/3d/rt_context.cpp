/*
 * RTContext.cpp
 *
 *  Created on: 15 янв. 2019 г.
 *      Author: sadko
 */

#include <core/3d/rt_context.h>

namespace lsp
{
    rt_context_t::rt_context_t(rt_shared_t *shared):
        vertex(256),
        edge(256),
        triangle(256)
    {
        this->state     = S_SCAN_OBJECTS;
        this->loop      = 0;
        this->shared    = shared;

        // Initialize point of view
        dsp::init_point_xyz(&view.s, 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&view.p[0], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&view.p[1], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&view.p[2], 0.0f, 0.0f, 0.0f);
        view.l[0]       = -1.0f;
        view.l[1]       = -1.0f;
        view.l[2]       = -1.0f;
        view.area       = 0.0f;
    }
    
    rt_context_t::~rt_context_t()
    {
        shared          = NULL;
        flush();
    }

    void rt_context_t::flush()
    {
        vertex.destroy();
        edge.destroy();
        triangle.destroy();
    }

    void rt_context_t::init_view(const point3d_t *sp, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
    {
        view.s      = *sp;
        view.p[0]   = *p0;
        view.p[1]   = *p1;
        view.p[2]   = *p2;

        view.l[0]   = dsp::calc_distance_p2(p0, p1);
        view.l[1]   = dsp::calc_distance_p2(p1, p2);
        view.l[2]   = dsp::calc_distance_p2(p0, p2);
        view.area   = calc_area(&view);
    }

    void rt_context_t::init_view(const point3d_t *sp, const point3d_t *pv)
    {
        view.s      = *sp;
        view.p[0]   = pv[0];
        view.p[1]   = pv[1];
        view.p[2]   = pv[2];

        view.l[0]   = dsp::calc_distance_pv(&pv[0]);
        view.l[1]   = dsp::calc_distance_pv(&pv[1]);
        view.l[2]   = dsp::calc_distance_p2(&pv[0], &pv[2]);
        view.area   = calc_area(&view);
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

    bool rt_context_t::unlink_edge(rt_edge_t *e, rt_vertex_t *v)
    {
        for (rt_edge_t **pcurr = &v->ve; *pcurr != NULL; )
        {
            rt_edge_t *curr = *pcurr;
            rt_edge_t **pnext = (curr->v[0] == v) ? &curr->vlnk[0] :
                                (curr->v[1] == v) ? &curr->vlnk[1] :
                                NULL;
            if (pnext == NULL) // Unexpected behaviour
                return false;

            if (curr == e)
            {
                *pcurr = *pnext;
                return true;
            }
            pcurr = pnext;
        }
        return false;
    }

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
//                shared->view->add_triangle_3c(t, &C_RED, &C_GREEN, &C_BLUE);
//                t = (t->e[0] == e) ? t->elnk[0] :
//                    (t->e[1] == e) ? t->elnk[1] :
//                    (t->e[2] == e) ? t->elnk[2] :
//                    NULL;
//            }
//            shared->view->add_segment(e, &C_RED, &C_YELLOW);
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
        ne->vlnk[0]     = NULL;
        ne->vlnk[1]     = NULL;
        ne->ptag        = NULL;
        ne->itag        = e->itag | RT_EF_PROCESSED;

        ne->vlnk[0]     = ne->v[0]->ve;
        ne->vlnk[1]     = ne->v[1]->ve;
        ne->v[0]->ve    = ne;
        ne->v[1]->ve    = ne;

        if ((ne->v[0] == NULL) || (ne->v[1] == NULL))
            return STATUS_CORRUPTED;

        // Unlink current edge from vertexes
        if (!unlink_edge(e, e->v[0]))
            return STATUS_CORRUPTED;
        if (!unlink_edge(e, e->v[1]))
            return STATUS_CORRUPTED;
        RT_TRACE(
            if (linked_count(e, e->v[0]) != 0)
                return STATUS_CORRUPTED;
            if (linked_count(e, e->v[1]) != 0)
                return STATUS_CORRUPTED;
        )

        e->itag        |= RT_EF_PROCESSED;

        cvector<rt_triangle_t> dbg_out;

        // Process all triangles
        while (true)
        {
//            RT_TRACE_BREAK(this,
//                lsp_trace("Splitting triangle");
//                shared->view->add_triangle_3c(ct, &C_RED, &C_GREEN, &C_BLUE);
//                shared->view->add_segment(e, &C_RED, &C_YELLOW);
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

            se->vlnk[0]     = se->v[0]->ve;
            se->vlnk[1]     = se->v[1]->ve;
            se->v[0]->ve    = se;
            se->v[1]->ve    = se;

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
                nt->face        = ct->face;

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
                nt->face        = ct->face;

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
//                    shared->view->add_triangle_1c(ct, &C_CYAN);
//                    shared->view->add_triangle_1c(nt, &C_MAGENTA);
//                    shared->view->add_segment(e[0].v[0], sp, &C_RED);
//                    shared->view->add_segment(ne, &C_GREEN);
//                    shared->view->add_segment(se, &C_BLUE);
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

            dbg_out.add(ct);
            dbg_out.add(nt);

//            RT_TRACE_BREAK(this,
//                lsp_trace("Splitted triangle");
//                shared->view->add_triangle_1c(ct, &C_GREEN);
//                shared->view->add_triangle_1c(nt, &C_BLUE);
//            );

            // Move to next triangle
            if (pt == NULL)
            {
                // Re-link edge to vertexes and leave cycle
              //e->v[0]         = e->v[0];
                e->v[1]         = sp;

                e->vlnk[0]      = e->v[0]->ve;
                e->vlnk[1]      = e->v[1]->ve;
                e->v[0]->ve     = e;
                e->v[1]->ve     = e;

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

//        RT_TRACE_BREAK(this,
//            lsp_trace("Final result for edge");
//            for (size_t i=0,n=dbg_out.size();i<n; ++i)
//                shared->view->add_triangle_1c(dbg_out.get(i), &C_GREEN);
//        );

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
            e->vlnk[0]      = e->v[0]->ve;
            e->vlnk[1]      = e->v[1]->ve;
            e->v[0]->ve     = e;
            e->v[0]->ve     = e;
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
        nt[0]->face     = t->face;

        nt[1]->v[0]     = t->v[2];
        nt[1]->v[1]     = t->v[0];
        nt[1]->v[2]     = sp;
        nt[1]->e[0]     = t->e[2];
        nt[1]->e[1]     = ne[0];
        nt[1]->e[2]     = ne[2];
        nt[1]->n        = t->n;
        nt[1]->ptag     = NULL;
        nt[1]->itag     = t->itag;
        nt[1]->face     = t->face;

      //nt[2]->v[0]     = t->v[0];
      //nt[2]->v[1]     = t->v[1];
        nt[2]->v[2]     = sp;
      //nt[2]->e[0]     = t->e[0];
        nt[2]->e[1]     = ne[1];
        nt[2]->e[2]     = ne[0];
      //nt[2]->n        = t->n;
      //nt[2]->ptag     = NULL;
      //nt[2]->itag     = t->itag;
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
    }

    status_t rt_context_t::split_edges(const vector3d_t *pl)
    {
        float t;
        size_t s;
        vector3d_t d;
        rt_vertex_t *sp;
        status_t res;

        // Try to split all edges (including new ones)
        for (size_t i=0; i< edge.size(); ++i)
        {
            rt_edge_t *e    = edge.get(i);

//            RT_TRACE_BREAK(this,
//                lsp_trace("Testing edge %d", int(i));
//                for (size_t i=0,n=triangle.size(); i<n; ++i)
//                    shared->view->add_triangle_1c(triangle.get(i), &C_RED);
//                for (size_t j=0,n=edge.size(); j<n; ++j)
//                {
//                    rt_edge_t *xe    = edge.get(j);
//                    if (xe == e)
//                        continue;
//                    shared->view->add_segment(xe,
//                            (xe->itag & RT_EF_SPLIT) ? &C_GREEN : &C_CYAN
//                    );
//                }
//                shared->view->add_segment(e, &C_YELLOW);
//            );

            if (e->itag & RT_EF_PROCESSED) // Skip already splitted eges
                continue;

            /*
             State of each vertex:
                 0 if point is over the plane
                 1 if point is on the plane
                 2 if point is above the plane

             The chart of edge state 's':
                 s=0   s=1   s=2   s=4   s=5   s=6   s=8   s=9   s=10   Normal
               | 0 1 | 0   | 0   |   1 |     |     |   1 |     |     |  ^
               |     |     |     |     |     |     |     |     |     |  |
             ==|=====|===1=|=====|=0===|=0=1=|=0===|=====|===1=|=====|==== <- Splitting plane
               |     |     |     |     |     |     |     |     |     |
               |     |     |   1 |     |     |   1 | 0   | 0   | 0 1 |
             */
            s               = (e->v[0]->itag << 2) | e->v[1]->itag;

            // Analyze ks[0] and ks[1]
            switch (s)
            {
                case 0: case 1: case 4: // edge is over the plane, skip
                    e->itag    |= RT_EF_PROCESSED;
                    break;
                case 6: case 9: case 10: // edge is under the plane, skip
                    e->itag    |= RT_EF_PROCESSED;
                    break;
                case 5: // edge lays on the plane, mark as split edge and skip
                    e->itag    |= RT_EF_PLANE | RT_EF_PROCESSED;
                    break;

                case 2: // edge is crossing the plane, v0 is over, v1 is under
                case 8: // edge is crossing the plane, v0 is under, v1 is over
                    // Find intersection with plane
                    d.dx        = e->v[1]->x - e->v[0]->x;
                    d.dy        = e->v[1]->y - e->v[0]->y;
                    d.dz        = e->v[1]->z - e->v[0]->z;
                    d.dw        = 0.0f;

                    t           = (e->v[0]->x*pl->dx + e->v[0]->y*pl->dy + e->v[0]->z*pl->dz + pl->dw) /
                                  (pl->dx*d.dx + pl->dy*d.dy + pl->dz*d.dz);

                    // Allocate split point
                    sp          = vertex.alloc();
                    if (sp == NULL)
                        return STATUS_NO_MEM;

                    // Compute split point
                    sp->x       = e->v[0]->x - d.dx * t;
                    sp->y       = e->v[0]->y - d.dy * t;
                    sp->z       = e->v[0]->z - d.dz * t;
                    sp->w       = 1.0f;

                    sp->ve      = NULL;
                    sp->ptag    = NULL;
                    sp->itag    = 1;

                    res         = split_edge(e, sp);
                    if (res != STATUS_OK)
                        return res;
                    break;

                default:
                    return STATUS_BAD_STATE;
            }
        }

//        RT_TRACE_BREAK(this,
//            lsp_trace("Triangle edges have been split, out=RED, in=GREEN");
//            for (size_t i=0, n=triangle.size(); i<n; ++i)
//            {
//                rt_triangle_t *t = triangle.get(i);
//                bool out         = (t->v[0]->itag != 1) ? (t->v[0]->itag <= 1) :
//                                   (t->v[1]->itag != 1) ? (t->v[1]->itag <= 1) :
//                                   (t->v[2]->itag <= 1);
//
//                shared->view->add_triangle_1c(t, (out) ? &C_RED : &C_GREEN);
//            }
//        );

        return STATUS_OK;
    }

    void rt_context_t::cleanup_tag_pointers()
    {
        // Cleanup pointers
        for (size_t i=0, n=vertex.size(); i<n; ++i)
            vertex.get(i)->ptag     = NULL;
        for (size_t i=0, n=edge.size(); i<n; ++i)
            edge.get(i)->ptag       = NULL;
        for (size_t i=0, n=triangle.size(); i<n; ++i)
            triangle.get(i)->ptag   = NULL;
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
        tx->itag    = st->itag;
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
                vx->ve          = NULL;

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
                ex->vlnk[0]     = NULL;
                ex->vlnk[1]     = NULL;
                ex->itag        = se->itag & ~RT_EF_TEMP;

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
        rt_triangle_t *st;
        status_t res = STATUS_OK;

        // Iterate all triangles
        for (size_t i=0, n=triangle.size(); i<n; ++i)
        {
            // Fetch triangle while skipping current one
            st          = triangle.get(i);
            if (st->itag != itag)
                continue;

            res         = fetch_triangle(dst, st);
            if (res != STATUS_OK)
                break;
        } // for

        return res;
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
        rt_edge_t *se, *ex;
        rt_triangle_t *tx;

        // Patch edge structures and link to vertexes
        for (size_t i=0, n=dst->edge.size(); i<n; ++i)
        {
            ex              = dst->edge.get(i);
            se              = reinterpret_cast<rt_edge_t *>(ex->ptag);
            if (se == NULL) // Edge does not need patching
                continue;

            // Patch vertex pointers if needed
            ex->v[0]        = (se->v[0]->ptag != NULL) ? reinterpret_cast<rt_vertex_t *>(se->v[0]->ptag) : se->v[0];
            ex->v[1]        = (se->v[1]->ptag != NULL) ? reinterpret_cast<rt_vertex_t *>(se->v[1]->ptag) : se->v[1];

            // Link edge to vertexes
            ex->vlnk[0]     = ex->v[0]->ve;
            ex->vlnk[1]     = ex->v[1]->ve;
            ex->v[0]->ve    = ex;
            ex->v[1]->ve    = ex;
        }

        // Link triangle structures to edges
        for (size_t i=0, n=dst->triangle.size(); i<n; ++i)
        {
            tx                  = dst->triangle.get(i);
            if (tx->ptag == NULL) // Triangle does not need patching
                continue;

            // Link triangle to the edge
            tx->elnk[0]         = tx->e[0]->vt;
            tx->elnk[1]         = tx->e[1]->vt;
            tx->elnk[2]         = tx->e[2]->vt;

            tx->e[0]->vt        = tx;
            tx->e[1]->vt        = tx;
            tx->e[2]->vt        = tx;
        }
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

    status_t rt_context_t::filter(rt_context_t *out, rt_context_t *in, const vector3d_t *pl)
    {
        status_t res;
        float t;

        // Always clear target context before proceeding
        if (out != NULL)
            out->clear();
        if (in != NULL)
            in->clear();

        // Cleanup itag for all triangles
        for (size_t i=0, n=triangle.size(); i<n; ++i)
        {
            rt_triangle_t  *t   = triangle.get(i);
            t->itag             = 1;        // Mark all triangles as outside
            t->ptag             = NULL;
        }

        // Initialize state of all vertexes
        for (size_t i=0, n=vertex.size(); i<n; ++i)
        {
            rt_vertex_t *v  = vertex.get(i);
            t               = v->x*pl->dx + v->y*pl->dy + v->z*pl->dz + pl->dw;

            v->ptag         = NULL;
            v->itag         = (t < 0.0f);
        }

        // Reset all flags of edges
        for (size_t i=0, n=edge.size(); i<n; ++i)
        {
            rt_edge_t *e    = edge.get(i);
            e->itag        &= ~RT_EF_PROCESSED; // Clear split flag
            e->ptag         = NULL;
        }

        // Toggle state of all triangles
        for (size_t i=0, n=triangle.size(); i<n; ++i)
        {
            rt_triangle_t  *st  = triangle.get(i);
            if (!st->itag) // Skip triangle?
                continue;

            // Is there at least one point below the plane?
            if (st->v[0]->itag | st->v[1]->itag | st->v[2]->itag)
                st->itag    = 0;
        }

        // Now we can fetch triangles
        res    = fetch_triangles_safe(in, 0);
        if (res != STATUS_OK)
            return res;

        res    = fetch_triangles_safe(out, 1);
        if (res != STATUS_OK)
            return res;

        RT_TRACE(
            if (!in->validate())
                return STATUS_CORRUPTED;
            if (!out->validate())
                return STATUS_CORRUPTED;
            if (!validate())
                return STATUS_CORRUPTED;
        )

        return STATUS_OK;
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

    float rt_context_t::calc_area(const rt_view_t *v)
    {
        float p     = 0.5f * (v->l[0] + v->l[1] + v->l[2]); // Semi-permeter
        return sqrtf(p * (p - v->l[0]) * (p - v->l[1]) * (p - v->l[2]));
    }

    status_t rt_context_t::sort()
    {
        // Compute number of triangles
        size_t nt       = triangle.size();
        if (nt <= 1)
            return STATUS_OK;

        // Allocate temporary array for sort
        size_t cap      = (nt + 0x3f) & (~0x3f); // Round capacity to some granular size
        rt_triangle_sort_t *vt = reinterpret_cast<rt_triangle_sort_t *>(malloc(cap * sizeof(rt_triangle_sort_t)));
        if (vt == NULL)
            return STATUS_NO_MEM;

        // Prepare data for sorting
        for (size_t i=0; i<nt; ++i)
        {
            rt_triangle_t *ct = triangle.get(i);
//            float d     = dsp::calc_avg_distance_p3(&view.s, ct->v[0], ct->v[1], ct->v[2]); // User reverse distance: the far locations should give lesser values
//            float a     = 1.0f / dsp::calc_area_p3(ct->v[0], ct->v[1], ct->v[2]);
            point3d_t c;
            c.x         = (ct->v[0]->x + ct->v[1]->x + ct->v[2]->x) / 3.0f;
            c.y         = (ct->v[0]->y + ct->v[1]->y + ct->v[2]->y) / 3.0f;
            c.z         = (ct->v[0]->z + ct->v[1]->z + ct->v[2]->z) / 3.0f;
            c.w         = 1.0f;

            vector3d_t d;
            d.dx        = view.s.x - c.x;
            d.dx        = view.s.y - c.y;
            d.dx        = view.s.z - c.z;
            d.dz        = 0.0f;

            dsp::normalize_vector(&d);

            vt[i].t     = ct;
            vt[i].w     = - (ct->n.dx * d.dx + ct->n.dy * d.dy + ct->n.dz * d.dz);
            //sqrtf(d*d + a); // Finding balance between distance and area
        }

        RT_TRACE_BREAK(this,
            lsp_trace("Prepare sort (%d triangles)", int(nt));

            color3d_t c;
            c.r     = 1.0f;
            c.g     = 0.0f;
            c.b     = 0.0f;
            c.a     = 0.0f;

            for (size_t i=0; i<nt; ++i)
            {
                c.r = float(nt - i)/nt;
                shared->view->add_triangle_1c(vt[i].t, &c);
            }
            free(vt);
        );

        // Call sorting function
        ::qsort(vt, nt, sizeof(rt_triangle_sort_t), compare_triangles);

        RT_TRACE_BREAK(this,
            lsp_trace("Triangles have been sorted (%d triangles)", int(nt));

            color3d_t c;
            c.r     = 0.0f;
            c.g     = 1.0f;
            c.b     = 0.0f;
            c.a     = 0.0f;

            for (size_t i=0; i<nt; ++i)
            {
                c.g = float(nt - i)/nt;
                shared->view->add_triangle_1c(vt[i].t, &c);
            }
            free(vt);
        );

        // Now we are ready to fetch triangles in new order
        rt_context_t tmp(shared);
        status_t res;

        cleanup_tag_pointers();

        // Fetch all triangles
        for (size_t i=0; i<nt; ++i)
        {
            // Fetch triangle
            res         = fetch_triangle(&tmp, vt[i].t);
            if (res != STATUS_OK)
            {
                free(vt);
                return res;
            }
        }
        complete_fetch(&tmp);

        // Free resources
        swap(&tmp);
        free(vt);

        return STATUS_OK;
    }

    status_t rt_context_t::solve_conflicts()
    {
        status_t res;
        vector3d_t pl;
        vector3d_t spl[3]; // Scissor planes
        float k[3];
        ssize_t l[3];

        for (size_t i=0; i<triangle.size()-1; ++i)
        {
            rt_triangle_t *t1 = triangle.get(i);
            dsp::calc_plane_p3(&pl, t1->v[0], t1->v[1], t1->v[2]);
            dsp::calc_plane_v1p2(&spl[0], t1->v[0], t1->v[1]);
            dsp::calc_plane_v1p2(&spl[1], t1->v[1], t1->v[2]);
            dsp::calc_plane_v1p2(&spl[2], t1->v[2], t1->v[0]);

            // Estimate location of each vertex relative to the plane
            for (size_t j=0,n=vertex.size(); j<n; ++j)
            {
                rt_vertex_t *cv = vertex.get(j);
                float k = cv->x * pl.dx + cv->y * pl.dy + cv->z*pl.dz + pl.dw;
                cv->itag = (k < -DSP_3D_TOLERANCE) ? 2 : (k > DSP_3D_TOLERANCE) ? 0 : 1;
            }

            // Split each edge with triangle, do not process new edges
            for (size_t j=0,n=edge.size(); j<n; ++j)
            {
                rt_edge_t *ce   = edge.get(j);
                ssize_t x1      = ce->v[0]->itag;
                ssize_t x2      = ce->v[1]->itag;

                // Ensure that edge intersects the plane
                if ((x1 <= 1) && (x2 <= 1))
                    continue;
                else if ((x1 >= 1) && (x2 >=1))
                    continue;

                // But now we need to check that intersection point lays on the triangle
                rt_vertex_t sp, *spp;
                dsp::calc_split_point_p2v1(&sp, ce->v[0], ce->v[1], &pl);
                sp.itag     = 0;
                sp.ptag     = NULL;
                sp.ve       = NULL;

                k[0]        = sp.x*spl[0].dx + sp.y*spl[0].dy + sp.z*spl[0].dz + spl[0].dw;
                k[1]        = sp.x*spl[1].dx + sp.y*spl[1].dy + sp.z*spl[1].dz + spl[1].dw;
                k[2]        = sp.x*spl[2].dx + sp.y*spl[2].dy + sp.z*spl[2].dz + spl[2].dw;

                l[0]        = (k[0] <= -DSP_3D_TOLERANCE) ? 2 : (k[0] > DSP_3D_TOLERANCE) ? 0 : 1;
                l[1]        = (k[1] <= -DSP_3D_TOLERANCE) ? 2 : (k[1] > DSP_3D_TOLERANCE) ? 0 : 1;
                l[2]        = (k[2] <= -DSP_3D_TOLERANCE) ? 2 : (k[2] > DSP_3D_TOLERANCE) ? 0 : 1;

                switch ((l[0]) | (l[1] << 2) | (l[2] << 4))
                {
                    case 0x16: // Point matches edges 1 and 2 (vertex 2)
                        res     = split_edge(ce, t1->v[2]); // Need to perform only split of crossing edge
                        if (res != STATUS_OK)
                            return res;
                        continue;

                    case 0x19: // Point matches edges 0 and 2 (vertex 0)
                        res     = split_edge(ce, t1->v[0]); // Need to perform only split of crossing edge
                        if (res != STATUS_OK)
                            return res;
                        continue;

                    case 0x25: // Point matches edges 0 and 1 (vertex 1)
                        res     = split_edge(ce, t1->v[1]); // Need to perform only split of crossing edge
                        if (res != STATUS_OK)
                            return res;
                        continue;

                    case 0x1a: // Point lays on edge 2, split triangle's edge
                        spp         = vertex.alloc(&sp);
                        if (spp == NULL)
                            return STATUS_NO_MEM;
                        res     = split_edge(t1->e[2], spp);
                        if (res == STATUS_OK)
                            res = split_edge(ce, spp);
                        break;

                    case 0x26: // Point lays on edge 1, split triangle's edge
                        spp         = vertex.alloc(&sp);
                        if (spp == NULL)
                            return STATUS_NO_MEM;
                        res     = split_edge(t1->e[1], spp);
                        if (res == STATUS_OK)
                            res = split_edge(ce, spp);
                        break;

                    case 0x29: // Point lays on edge 0, split triangle's edge
                        spp         = vertex.alloc(&sp);
                        if (spp == NULL)
                            return STATUS_NO_MEM;
                        res     = split_edge(t1->e[0], spp);
                        if (res == STATUS_OK)
                            res = split_edge(ce, spp);
                        break;

                    case 0x2a: // Point lays inside of the triangle, split triangle's edge
                        spp         = vertex.alloc(&sp);
                        if (spp == NULL)
                            return STATUS_NO_MEM;
                        res     = split_triangle(t1, spp);
                        if (res == STATUS_OK)
                            res = split_edge(ce, spp);
                        break;

                    default: // Point is not crossing triangle
                        continue;
                }

                // Check final result
                if (res != STATUS_OK)
                    return res;

                // Current triangle's structure has been modified, update split planes' equations
                dsp::calc_plane_v1p2(&spl[0], t1->v[0], t1->v[1]);
                dsp::calc_plane_v1p2(&spl[1], t1->v[1], t1->v[2]);
                dsp::calc_plane_v1p2(&spl[2], t1->v[2], t1->v[0]);
            }
        }
        return STATUS_OK;
    }

    status_t rt_context_t::split(rt_context_t *out, rt_context_t *on, rt_context_t *in, const vector3d_t *pl)
    {
        status_t res;

        // Always clear target context before proceeding
        if (out != NULL)
            out->clear();
        if (in != NULL)
            in->clear();
        if (on != NULL)
            on->clear();

        // Is there data for processing ?
        if (triangle.size() <= 0)
            return STATUS_OK;

        // State of itag for vertex:
        //  0   = vertex lays over the plane
        //  1   = vertex lays on the plane
        //  2   = vertex lays below the plane

        // State of itag for triangle:
        // -1   = currently unknown state
        //  0   = triangle is 'in'
        //  1   = triangle is 'out'
        //  2   = triangle is 'on'
        for (size_t i=0, n=vertex.size(); i<n; ++i)
        {
            rt_vertex_t *v  = vertex.get(i);
            float t         = v->x*pl->dx + v->y*pl->dy + v->z*pl->dz + pl->dw;
            v->itag         = (t < -DSP_3D_TOLERANCE) ? 2 : (t > DSP_3D_TOLERANCE) ? 0 : 1;
        }

        // Reset all flags of edges
        for (size_t i=0, n=edge.size(); i<n; ++i)
            edge.get(i)->itag &= ~RT_EF_TEMP; // Clear processed flag

        // First step: split edges
        // Perform split of edges
        for (size_t i=0; i<edge.size(); ++i) // The edge array can grow
        {
            rt_edge_t *e    = edge.get(i);
            if (e->itag & RT_EF_PROCESSED) // Skip already splitted eges
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
                case 0: case 1: case 4: // edge is over the plane, skip
                case 6: case 9: case 10: // edge is under the plane, skip
                    e->itag    |= RT_EF_PROCESSED;
                    break;
                case 5: // edge lays on the plane, mark as split edge and skip
                    e->itag    |= RT_EF_PLANE | RT_EF_PROCESSED;
                    break;

                case 2: // edge is crossing the plane, v0 is over, v1 is under
                case 8: // edge is crossing the plane, v0 is under, v1 is over
                {
                    // Allocate split point
                    rt_vertex_t *sp     = vertex.alloc();
                    if (sp == NULL)
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(sp, e->v[0], e->v[1], pl);

                    sp->ve      = NULL;
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
        }

//        RT_TRACE(
            if (!validate())
                return STATUS_CORRUPTED;
//        )

        // Toggle state of all triangles
        for (size_t i=0, n=triangle.size(); i<n; ++i)
        {
            rt_triangle_t  *st  = triangle.get(i);

            // Detect position of triangle: over the plane or under the plane
            if (st->v[0]->itag != 1)
                st->itag    = (st->v[0]->itag < 1) ? 1 : 0;
            else if (st->v[1]->itag != 1)
                st->itag    = (st->v[1]->itag < 1) ? 1 : 0;
            else if (st->v[2]->itag != 1)
                st->itag    = (st->v[2]->itag < 1) ? 1 : 0;
            else
                st->itag    = 2; // Triangle is on the plane
        }

        // Now we can fetch triangles
        res    = fetch_triangles_safe(in, 0);
        if (res != STATUS_OK)
            return res;
        res    = fetch_triangles_safe(out, 1);
        if (res != STATUS_OK)
            return res;
        res    = fetch_triangles_safe(on, 2);
        if (res != STATUS_OK)
            return res;

//        RT_TRACE(
            if (!in->validate())
                return STATUS_CORRUPTED;
            if (!out->validate())
                return STATUS_CORRUPTED;
            if (!on->validate())
                return STATUS_CORRUPTED;
            if (!validate())
                return STATUS_CORRUPTED;
//        )

        return STATUS_OK;
    }

    status_t rt_context_t::split(rt_context_t *out, rt_context_t *on, const vector3d_t *pl)
    {
        rt_context_t tmp(shared);
        status_t res = split(out, on, &tmp, pl);
        if (res == STATUS_OK)
            tmp.swap(this);
        return res;
    }

    status_t rt_context_t::split(rt_context_t *out, rt_context_t *on, rt_context_t *in)
    {
        static const ssize_t in_itags[] = { 0, 1 };
        static const ssize_t out_itags[] = { 2, 3 };

        status_t res;

        // Clear all target contexts
        if (out != NULL)
            out->clear();
        if (on != NULL)
            on->clear();
        if (in != NULL)
            in->clear();

        // Is there data for processing ?
        if (triangle.size() <= 1)
        {
            if (in != NULL)
                in->swap(this);
            return STATUS_OK;
        }

        // State of itag for vertex:
        //  0   = vertex lays over the plane
        //  1   = vertex lays on the plane
        //  2   = vertex lays below the plane

        // State of itag for triangle:
        // -1   = currently unknown state
        // -2   = unknown state with low priority (need to be placed at the end of array)
        //  0   = triangle is 'in'
        //  1   = triangle is 'in' but needs to be placed at the end of array
        //  2   = triangle is 'out'
        //  2   = triangle is 'out' but needs to be placed at the end of array
        //  4   = triangle is 'on'

        // Select current triangle for processing
        rt_triangle_t *ct   = NULL; // Current triangle
        rt_edge_t     *se   = NULL; // Splitting edge
        vector3d_t     pl;      // Culling plane

        for (size_t i=0,n=triangle.size(); i<n; ++i)
            triangle.get(i)->itag = -1;

        for (size_t i=0,n=triangle.size(); i<n; ++i)
        {
            rt_triangle_t *st   = triangle.get(i);

            // Find unprocessed edge in the triangle
            if (!(st->e[0]->itag & RT_EF_PLANE))
            {
                se      = st->e[0];
                dsp::calc_oriented_plane_p3(&pl, st->v[2], &view.s, st->v[0], st->v[1]);
            }
            else if (!(st->e[1]->itag & RT_EF_PLANE))
            {
                se      = st->e[1];
                dsp::calc_oriented_plane_p3(&pl, st->v[0], &view.s, st->v[1], st->v[2]);
            }
            else if (!(st->e[2]->itag & RT_EF_PLANE))
            {
                se      = st->e[2];
                dsp::calc_oriented_plane_p3(&pl, st->v[1], &view.s, st->v[2], st->v[0]);
            }
            else
                st->itag    = -2; // We need to put this triangle to the end of queue because there is nothing to split

            if (se != NULL)
            {
                se->itag   |= RT_EF_PLANE;
                ct          = st;
                break;
            }
        }

        // Nothing to do?
        if (ct == NULL)
        {
            if (in != NULL)
                in->swap(this);
            return STATUS_OK;
        }

        // Determine co-location of plane and vertexes
        for (size_t i=0, n=vertex.size(); i<n; ++i)
        {
            rt_vertex_t *v  = vertex.get(i);
            float t         = v->x*pl.dx + v->y*pl.dy + v->z*pl.dz + pl.dw;
            v->itag         = (t < -DSP_3D_TOLERANCE) ? 2 : (t > DSP_3D_TOLERANCE) ? 0 : 1;
        }

        // Reset all temporary flags of edges
        for (size_t i=0, n=edge.size(); i<n; ++i)
            edge.get(i)->itag &= ~RT_EF_TEMP; // Clear temporary flags

        // Mark current edge as processed and laying on the split plane
        se->itag       |= RT_EF_PLANE | RT_EF_PROCESSED;
        se->v[0]->itag  = 1;
        se->v[1]->itag  = 1;

        RT_TRACE_BREAK(this,
            lsp_trace("Prepare split with edge (%d triangles)", int(triangle.size()));

            for (size_t i=0,n=triangle.size(); i<n; ++i)
            {
                rt_triangle_t *t = triangle.get(i);
                shared->view->add_triangle_1c(t, (t == ct) ? &C_ORANGE : &C_YELLOW);
            }
            shared->view->add_plane_3pn1c(&view.s, se->v[0], se->v[1], &pl, &C_MAGENTA);

            for (size_t i=0,n=edge.size(); i<n; ++i)
            {
                rt_edge_t *e = edge.get(i);
                shared->view->add_segment(e,
                        (e == se) ? &C_MAGENTA :
                        (e->itag & RT_EF_PLANE) ? &C_GREEN : &C_RED
                    );
            }
        );

        // Perform split of edges
        for (size_t i=0; i<edge.size(); ++i) // The edge array can grow
        {
            rt_edge_t *e    = edge.get(i);
            if (e->itag & RT_EF_PROCESSED) // Skip already splitted eges
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
                case 0: case 1: case 4: // edge is over the plane, skip
                case 6: case 9: case 10: // edge is under the plane, skip
                    e->itag    |= RT_EF_PROCESSED;
                    break;
                case 5: // edge lays on the plane, mark as split edge and skip
                    e->itag    |= RT_EF_PLANE | RT_EF_PROCESSED;
                    break;

                case 2: // edge is crossing the plane, v0 is over, v1 is under
                case 8: // edge is crossing the plane, v0 is under, v1 is over
                {
                    // Allocate split point
                    rt_vertex_t *sp     = vertex.alloc();
                    if (sp == NULL)
                        return STATUS_NO_MEM;

                    dsp::calc_split_point_p2v1(sp, e->v[0], e->v[1], &pl);
                    sp->ve      = NULL;
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
        }

        RT_TRACE(
            if (!validate())
                return STATUS_CORRUPTED;
        )

        // Toggle state of all triangles
        for (size_t i=0, n=triangle.size(); i<n; ++i)
        {
            rt_triangle_t  *st  = triangle.get(i);
            if (st->itag >= 0)
                continue;
            size_t intag     = (st->itag == -1) ? 0 : 1;
            size_t outtag    = (st->itag == -1) ? 2 : 3;

            // Detect position of triangle: over the plane or under the plane
            if (st->v[0]->itag != 1)
                st->itag    = (st->v[0]->itag < 1) ? outtag : intag;
            else if (st->v[1]->itag != 1)
                st->itag    = (st->v[1]->itag < 1) ? outtag : intag;
            else if (st->v[2]->itag != 1)
                st->itag    = (st->v[2]->itag < 1) ? outtag : intag;
            else
                st->itag    = 4; // Triangle is on the plane
        }

        // Now we can fetch triangles
        res     = vfetch_triangles_safe(in, 2, in_itags);
        if (res != STATUS_OK)
            return res;

        res     = vfetch_triangles_safe(out, 2, out_itags);
        if (res != STATUS_OK)
            return res;

        res     = fetch_triangles_safe(on, 4);
        if (res != STATUS_OK)
            return res;

        RT_TRACE_BREAK(this,
            lsp_trace("After split");

            if (in != NULL)
            {
                lsp_trace("In context is GREEN (%d triangles)", int(in->triangle.size()));
                for (size_t i=0,n=in->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(in->triangle.get(i), &C_GREEN);
            }
            if (out != NULL)
            {
                lsp_trace("Out context is RED (%d triangles)", int(out->triangle.size()));
                for (size_t i=0,n=out->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(out->triangle.get(i), &C_RED);
            }
            if (on != NULL)
            {
                lsp_trace("On context is BLUE (%d triangles)", int(on->triangle.size()));
                for (size_t i=0,n=on->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(on->triangle.get(i), &C_BLUE);
            }

            shared->view->add_plane_3pn1c(&view.s, se->v[0], se->v[1], &pl, &C_YELLOW);
        );

        RT_TRACE(
            if (!in->validate())
                return STATUS_CORRUPTED;
            if (!out->validate())
                return STATUS_CORRUPTED;
            if (!on->validate())
                return STATUS_CORRUPTED;
            if (!validate())
                return STATUS_CORRUPTED;
        )

        return STATUS_OK;
    }

    status_t rt_context_t::split(rt_context_t *out, rt_context_t *on)
    {
        rt_context_t tmp(shared);
        status_t res = split(out, on, &tmp);
        if (res == STATUS_OK)
            tmp.swap(this);
        return res;
    }

    status_t rt_context_t::edge_split(cvector<rt_context_t> &dst)
    {
        // Is there data for processing ?
        if (triangle.size() <= 1)
        {
            state   = S_REFLECT;
            return STATUS_OK;
        }

        // Find edge to apply split
        vector3d_t pl;
        rt_triangle_t *ct = NULL;
        status_t res;

        for (size_t i=0, n=triangle.size(); i<n; ++i)
        {
            rt_triangle_t *st = triangle.get(i);
            if (rearrange_triangle(st))
            {
                ct  = st;
                break;
            }
        }
        if (ct == NULL)
            return STATUS_OK;

        // Perform split
        dsp::calc_oriented_plane_p3(&pl, ct->v[2], &view.s, ct->v[0], ct->v[1]);

        RT_TRACE_BREAK(this,
            lsp_trace("Prepare split with edge (%d triangles)", int(triangle.size()));

            for (size_t i=0,n=triangle.size(); i<n; ++i)
            {
                rt_triangle_t *t = triangle.get(i);
                shared->view->add_triangle_1c(t, (t == ct) ? &C_ORANGE : &C_YELLOW);
            }
            shared->view->add_plane_3pn1c(&view.s, ct->v[0], ct->v[1], &pl, &C_MAGENTA);

            for (size_t i=0,n=edge.size(); i<n; ++i)
            {
                rt_edge_t *e = edge.get(i);
                shared->view->add_segment(e, (e->itag & RT_EF_PLANE) ? &C_GREEN : &C_RED);
            }
        );

        // Do the split
        rt_context_t in(shared), out(shared), ign(shared);
        res = split(&out, &ign, &in, &pl);
        if (res != STATUS_OK)
            return res;

        // Now perform split of view
        size_t n_in = 0, n_out = 0;
        raw_triangle_t vin[2], vout[2], src;
        rt_context_t *ctin[2], *ctout[2], *ctign;
        src.p[0]    = view.p[0];
        src.p[1]    = view.p[1];
        src.p[2]    = view.p[2];

        dsp::split_triangle_raw(vout, &n_out, vin, &n_in, &pl, &src);
        ctin[0]     = NULL;
        ctin[1]     = NULL;
        ctout[0]    = NULL;
        ctout[1]    = NULL;
        ctign       = NULL;

        if ((n_out > 0) && (in.triangle.size() > 0))
        {
            if (n_out >= 2)
            {
                // Allocate additional context
                ctout[1] = new rt_context_t(shared);
                if (ctout[1] == NULL)
                    return STATUS_NO_MEM;
                else if (!dst.add(ctout[1]))
                {
                    delete ctout[1];
                    return STATUS_NO_MEM;
                }

                // Perform additional split
                dsp::calc_plane_p3(&pl, &view.s, &vout[1].p[2], &vout[1].p[1]);
                res     = out.split(ctout[1], &ign, &pl);
                if (res != STATUS_OK)
                    return res;

                // Set context state
                ctout[1]->init_view(&view.s, vout[1].p);
                if ((ctout[1]->triangle.size() < 1) || (ctout[1]->view.area < 1e-3f))
                    ctout[1]->state     = S_IGNORE;
                else if (ctout[1]->triangle.size() > 1)
                    ctout[1]->state     = S_CULL_BACK;
                else
                    ctout[1]->state     = S_REFLECT;
            }

            ctout[0] = new rt_context_t(shared);
            if (ctout[0] == NULL)
                return STATUS_NO_MEM;
            else if (!dst.add(ctout[0]))
            {
                delete ctout[0];
                return STATUS_NO_MEM;
            }
            ctout[0]->init_view(&view.s, vout[0].p);
            ctout[0]->swap(&out);

            if ((ctout[0]->triangle.size() < 1) || (ctout[0]->view.area < 1e-3f))
                ctout[0]->state     = S_IGNORE;
            else if (ctout[0]->triangle.size() > 1)
                ctout[0]->state     = S_CULL_BACK;
            else
                ctout[0]->state     = S_REFLECT;
        }

        if ((n_in > 0) && (in.triangle.size() > 0))
        {
            if (n_in >= 2)
            {
                // Allocate additional context
                ctin[1] = new rt_context_t(shared);
                if (ctin[1] == NULL)
                    return STATUS_NO_MEM;
                else if (!dst.add(ctin[1]))
                {
                    delete ctin[1];
                    return STATUS_NO_MEM;
                }

                // Perform additional split
                dsp::calc_plane_p3(&pl, &view.s, &vin[1].p[2], &vin[1].p[1]);
                res     = in.split(ctin[1], &ign, &pl);
                if (res != STATUS_OK)
                    return res;

                // Set context state
                ctin[1]->init_view(&view.s, vin[1].p);
                if ((ctin[1]->triangle.size() < 1) || (ctin[1]->view.area <= 1e-3f))
                    ctin[1]->state      = S_IGNORE;
                else if (ctin[1]->triangle.size() > 1)
                    ctin[1]->state      = S_CULL_BACK;
                else
                    ctin[1]->state      = S_REFLECT;
            }

            ctin[0] = new rt_context_t(shared);
            if (ctin[0] == NULL)
                return STATUS_NO_MEM;
            else if (!dst.add(ctin[0]))
            {
                delete ctin[0];
                return STATUS_NO_MEM;
            }
            ctin[0]->init_view(&view.s, vin[0].p);
            ctin[0]->swap(&in);

            if ((ctin[0]->triangle.size() < 1) || (ctin[0]->view.area < 1e-3f))
                ctin[0]->state      = S_IGNORE;
            else if (ctin[0]->triangle.size() > 1)
                ctin[0]->state      = S_CULL_BACK;
            else
                ctin[0]->state      = S_REFLECT;
        }

        if (ign.triangle.size() > 0)
        {
            ctign = new rt_context_t(shared);
            if (ctign == NULL)
                return STATUS_NO_MEM;
            else if (!dst.add(ctign))
            {
                delete ctign;
                return STATUS_NO_MEM;
            }
            ctign->view       = view;
            ctign->state      = S_IGNORE;
            ctign->swap(&ign);
        }

        RT_TRACE_BREAK(this,
            lsp_trace("Prepare split with edge result in[0]=GREEN, in[1]=BLUE, out[0]=RED, out[1]=MAGENTA, ign=CYAN");

            if (ctin[0] != NULL)
            {
                for (size_t i=0,n=ctin[0]->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(ctin[0]->triangle.get(i), &C_GREEN);
                shared->view->add_view_1c(&ctin[0]->view, &C_GREEN);
            }
            lsp_trace("Prepare split with edge result in[0]=GREEN, in[1]=BLUE, out[0]=RED, out[1]=MAGENTA, ign=CYAN");
            if (ctin[1] != NULL)
            {
                for (size_t i=0,n=ctin[1]->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(ctin[1]->triangle.get(i), &C_BLUE);
                shared->view->add_view_1c(&ctin[1]->view, &C_BLUE);
            }
            lsp_trace("Prepare split with edge result in[0]=GREEN, in[1]=BLUE, out[0]=RED, out[1]=MAGENTA, ign=CYAN");
            if (ctout[0] != NULL)
            {
                for (size_t i=0,n=ctout[0]->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(ctout[0]->triangle.get(i), &C_RED);
                shared->view->add_view_1c(&ctout[0]->view, &C_RED);
            }
            lsp_trace("Prepare split with edge result in[0]=GREEN, in[1]=BLUE, out[0]=RED, out[1]=MAGENTA, ign=CYAN");
            if (ctout[1] != NULL)
            {
                for (size_t i=0,n=ctout[1]->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(ctout[1]->triangle.get(i), &C_MAGENTA);
                shared->view->add_view_1c(&ctout[1]->view, &C_MAGENTA);
            }
            lsp_trace("Prepare split with edge result in[0]=GREEN, in[1]=BLUE, out[0]=RED, out[1]=MAGENTA, ign=CYAN");
            if (ctign != NULL)
            {
                for (size_t i=0,n=ctign->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(ctign->triangle.get(i), &C_CYAN);
//                shared->view->add_view_1c(&ctign->view, &C_MAGENTA);
            }
        );

        return STATUS_OK;
    }

    bool rt_context_t::rearrange_triangle(rt_triangle_t *t)
    {
        rt_vertex_t *tv;
        rt_edge_t *te;
        rt_triangle_t *tt;

        if (!(t->e[0]->itag & RT_EF_PLANE))
            return true;
        else if (!(t->e[1]->itag & RT_EF_PLANE)) // Rotate clockwise
        {
            tv          = t->v[0];
            t->v[0]     = t->v[1];
            t->v[1]     = t->v[2];
            t->v[2]     = tv;

            te          = t->e[0];
            t->e[0]     = t->e[1];
            t->e[1]     = t->e[2];
            t->e[2]     = te;

            tt          = t->elnk[0];
            t->elnk[0]  = t->elnk[1];
            t->elnk[1]  = t->elnk[2];
            t->elnk[2]  = tt;
        }
        else if (!(t->e[2]->itag & RT_EF_PLANE)) // Rotate counter-clockwise
        {
            tv          = t->v[2];
            t->v[2]     = t->v[1];
            t->v[1]     = t->v[0];
            t->v[0]     = tv;

            te          = t->e[2];
            t->e[2]     = t->e[1];
            t->e[1]     = t->e[0];
            t->e[0]     = te;

            tt          = t->elnk[2];
            t->elnk[2]  = t->elnk[1];
            t->elnk[1]  = t->elnk[0];
            t->elnk[0]  = tt;
        }
        else
            return false;

        return true;
    }

    void rt_context_t::rearrange_view()
    {
        if ((view.l[0] > view.l[1]) && (view.l[0] > view.l[2]))
            return;

        point3d_t tp;
        float tl;

        if (view.l[1] > view.l[2]) // Rotate clockwise
        {
            tp          = view.p[0];
            view.p[0]   = view.p[1];
            view.p[1]   = view.p[2];
            view.p[2]   = tp;

            tl          = view.l[0];
            view.l[0]   = view.l[1];
            view.l[1]   = view.l[2];
            view.l[2]   = tl;
        }
        else // Rotate counter-clockwise
        {
            tp          = view.p[2];
            view.p[2]   = view.p[1];
            view.p[1]   = view.p[0];
            view.p[0]   = tp;

            tl          = view.l[2];
            view.l[2]   = view.l[1];
            view.l[1]   = view.l[0];
            view.l[0]   = tl;
        }
    }

    status_t rt_context_t::binary_split(rt_context_t *out)
    {
        status_t res;
        point3d_t sp;
        vector3d_t pl;
        rt_context_t in(shared);

        // Always clear target context before proceeding
        if (out != NULL)
            out->clear();

        // Is there data for processing ?
        if (triangle.size() <= 0)
            return STATUS_OK;

        // Make the longest edge to have index 0
        rearrange_view();

        // Estimate coordinates of split-point
        sp.x        = 0.5f * (view.p[0].x + view.p[1].x);
        sp.y        = 0.5f * (view.p[0].y + view.p[1].y);
        sp.z        = 0.5f * (view.p[0].z + view.p[1].z);
        sp.w        = 1.0f;

        // Compute split plane
        dsp::calc_oriented_plane_p3(&pl, &view.p[0], &view.s, &view.p[2], &sp);

        // State of itag for vertex:
        //  0   = vertex lays over the plane
        //  1   = vertex lays on the plane
        //  2   = vertex lays below the plane

        // State of itag for triangle:
        //  0   = triangle is 'in'
        //  1   = triangle is 'out'
        for (size_t i=0, n=vertex.size(); i<n; ++i)
        {
            rt_vertex_t *v  = vertex.get(i);
            float t         = v->x*pl.dx + v->y*pl.dy + v->z*pl.dz + pl.dw;
            v->itag         = (t < -DSP_3D_TOLERANCE) ? 2 : (t > DSP_3D_TOLERANCE) ? 0 : 1;
        }

        // Reset all flags of edges
        for (size_t i=0, n=edge.size(); i<n; ++i)
            edge.get(i)->itag &= ~RT_EF_TEMP; // Clear processed flag

        for (size_t i=0, n=triangle.size(); i<n; ++i)
            triangle.get(i)->itag = 0;  // By default, all triangles are considered to be 'in'

        RT_TRACE_BREAK(this,
            lsp_trace("Prepare binary_split (%d triangles)", int(triangle.size()));

            for (size_t i=0,n=triangle.size(); i<n; ++i)
                shared->view->add_triangle_1c(triangle.get(i), &C_YELLOW);

            shared->view->add_plane_sp3p1c(&view.p[0], &view.s, &view.p[2], &sp, &C_MAGENTA);
            shared->view->add_view_3c(&view, &C_RED, &C_GREEN, &C_BLUE);
        );

        // Call for binary split
        // First step: split edges
        // Perform split of edges
        for (size_t i=0; i<edge.size(); ++i) // The edge array can grow
        {
            rt_edge_t *e    = edge.get(i);
            if (e->itag & RT_EF_PROCESSED) // Skip already split edges
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
                case 0: case 1: case 4: // edge is over the plane, skip
                case 6: case 9: case 10: // edge is under the plane, skip
                    e->itag    |= RT_EF_PROCESSED;
                    break;
                case 5: // edge lays on the plane, mark as split edge and skip
                    e->itag    |= RT_EF_PLANE | RT_EF_PROCESSED;
                    break;

                case 2: // edge is crossing the plane, v0 is over, v1 is under
                case 8: // edge is crossing the plane, v0 is under, v1 is over
                {
                    // Allocate split point
                    rt_vertex_t *sp     = vertex.alloc();
                    if (sp == NULL)
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(sp, e->v[0], e->v[1], &pl);

                    sp->ve      = NULL;
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
        }

        RT_TRACE(
            if (!validate())
                return STATUS_CORRUPTED;
        )

        // Toggle state of all triangles
        for (size_t i=0, n=triangle.size(); i<n; ++i)
        {
            rt_triangle_t  *st  = triangle.get(i);

            // Detect position of triangle: over the plane or under the plane
            if (st->v[0]->itag != 1)
                st->itag    = (st->v[0]->itag < 1) ? 1 : 0;
            else if (st->v[1]->itag != 1)
                st->itag    = (st->v[1]->itag < 1) ? 1 : 0;
            else if (st->v[2]->itag != 1)
                st->itag    = (st->v[2]->itag < 1) ? 1 : 0;
            else
                st->itag    = 1; // Mark triangle as out
        }

        // Now we can fetch triangles
        float l     = dsp::calc_distance_p2(&view.p[2], &sp);
        if (out != NULL)
        {
            out->view       = this->view;
            out->view.p[0]  = sp;
            out->view.l[0] *= 0.5f;
            out->view.l[2]  = l;
            out->view.area  = calc_area(&out->view);
            out->state      = (out->view.area >= 0.0001f) ? S_CULL_BACK : S_IGNORE;
            res             = fetch_triangles_safe(out, 1);
            if (res != STATUS_OK)
                return res;
        }

        // Update self state
        view.p[1]       = sp;
        view.l[0]      *= 0.5f;
        view.l[1]       = l;
        view.area       = calc_area(&view);
        state           = (view.area >= 0.0001f) ? S_CULL_BACK : S_IGNORE;
        res             = fetch_triangles_safe(&in, 0);
        if (res != STATUS_OK)
            return res;
        this->swap(&in);

        RT_TRACE_BREAK(this,
            lsp_trace("Processed binary_split");

            lsp_trace("IN content is YELLOW/GREEN (%d triangles)", int(this->triangle.size()));
            for (size_t i=0,n=this->triangle.size(); i<n; ++i)
                shared->view->add_triangle_1c(this->triangle.get(i), &C_GREEN);
            shared->view->add_view_3c(&view, &C_YELLOW, &C_YELLOW, &C_YELLOW);

            if (out != NULL)
            {
                lsp_trace("OUT content is RED/BLUE (%d triangles)", int(out->triangle.size()));
                for (size_t i=0,n=out->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(out->triangle.get(i), &C_BLUE);
                shared->view->add_view_3c(&out->view, &C_RED, &C_RED, &C_RED);
            }

            shared->view->add_plane_sp3p1c(&view.p[0], &view.s, &view.p[2], &sp, &C_MAGENTA);
        );

        RT_TRACE(
            if (!in.validate())
                return STATUS_CORRUPTED;
            if (!out->validate())
                return STATUS_CORRUPTED;
            if (!validate())
                return STATUS_CORRUPTED;
        )

        return STATUS_OK;
    }

    bool rt_context_t::match_face(rt_edge_t *e, ssize_t face_id)
    {
        for (rt_triangle_t *ct = e->vt; ct != NULL; )
        {
            if (ct->face == face_id)
                return true;

            if (ct->e[0] == e)
                ct  = ct->elnk[0];
            else if (ct->e[1] == e)
                ct  = ct->elnk[1];
            else if (ct->e[2] == e)
                ct  = ct->elnk[2];
        }
        return false;
    }

    bool rt_context_t::check_face(rt_triangle_t *ct, rt_edge_t *e)
    {
        if (!(e->itag & RT_EF_PLANE))
            return false;

        rt_edge_t *ce   = e;
        rt_edge_t *ne;
        rt_vertex_t *cv = e->v[0];

//        lsp_trace("start edge=%p, start vertex=%p[%d]", ce, cv, 0);
//        RT_CALL_DEBUGGER(this, 403);
        cvector<rt_edge_t> path;

        while (true)
        {
            // Analyze edge list
            for (ne = cv->ve; ne != NULL; ne = (ne->v[0] == cv) ? ne->vlnk[0] : ne->vlnk[1])
            {
                if (!(ne->itag & RT_EF_PLANE)) // Check that edge has proper tag
                    continue;
                else if (ne == ce)
                    continue;
                else if (path.index_of(ne) >= 0)//(ne == e)
                    return true;

                // Check that edge lays on the proper face
                if (match_face(ne, ct->face))
                    break;
            }
            if (!path.add(ce))
                return false;
            if (ne == NULL)
                return false;

            // Change current edge
            ce      = ne;
            cv      = (ne->v[0] == cv) ? ne->v[1] : ne->v[0];

//            lsp_trace("new edge=%p (start=%p, v[0]=%p, v[1]=%p), new vertex=%p[%d]",
//                    ne, e, e->v[0], e->v[1], cv, int(ce->v[1] == cv));
        }
    }

    rt_triangle_t *rt_context_t::find_cullback_triangle()
    {
        size_t nt = triangle.size();
        if (nt <= 1)
        {
            state = (nt < 1) ? S_IGNORE : S_REFLECT;
            return NULL;
        }

        size_t n_similar = 0, n_pending = 0;
        rt_triangle_t *ct = NULL;

        // Check that all triangles lay on the same plane
        for (size_t i=0; i<nt; ++i)
        {
            rt_triangle_t *st = triangle.get(i);
            if (st->itag == 1) // 'out'
                continue;
            else if (ct == NULL)
                ct = st;
            ++n_pending;
            if (st->face == ct->face)
                ++n_similar;
        }
        if (n_similar >= n_pending)
        {
            state   = S_REFLECT;    // All triangles are laying on the same plane
            return NULL;
        }

        // Find co-planar triangles
        for (size_t i=0; i < nt; ++i)
        {
            rt_triangle_t *st = triangle.get(i);
            if (st->itag > 0) // skip 'current' and 'out' faces
                continue;
            if (check_face(st, st->e[0]))
                return st;
            if (check_face(st, st->e[1]))
                return st;
            if (check_face(st, st->e[2]))
                return st;
        }

        return NULL;
    }

    status_t rt_context_t::binary_cullback(rt_context_t *out)
    {
        vector3d_t pl;
        rt_triangle_t *ct;
        rt_context_t in(shared);
        status_t res;
        static const ssize_t in_itags[] = { 0, 2 };

        // Always clear target context before proceeding
        if (out != NULL)
            out->clear();

        // State of itag for vertex:
        //  0   = vertex lays over the plane
        //  1   = vertex lays on the plane
        //  2   = vertex lays below the plane

        // State of itag for triangle:
        // -1   = unknown state (default)
        //  0   = triangle is 'in'
        //  1   = triangle is 'out'
        //  2   = triangle is 'current'

        // Clear itag for all triangles
        for (size_t i=0,n=triangle.size(); i<n; ++i)
            triangle.get(i)->itag = -1; // By default, unknown state

        RT_TRACE_BREAK(this,
            lsp_trace("Prepare cullback (%d triangles)", int(triangle.size()));

            for (size_t i=0,n=triangle.size(); i<n; ++i)
                shared->view->add_triangle_1c(triangle.get(i), &C_YELLOW);

            for (size_t i=0,n=edge.size(); i<n; ++i)
            {
                rt_edge_t *se = edge.get(i);
                shared->view->add_segment(se, (se->itag & RT_EF_PLANE) ? &C_GREEN : &C_RED);
            }
        );

        // Is there triangle for cull-back ?
        state   = S_SPLIT;
        ct      = find_cullback_triangle();
        if (ct == NULL)
            return STATUS_OK;

        do
        {
            // Compute split plane
            dsp::calc_oriented_plane_p3(&pl, &view.s, ct->v[0], ct->v[1], ct->v[2]);

            // Perform cull-back with plane
            for (size_t i=0, n=vertex.size(); i<n; ++i)
            {
                rt_vertex_t *v  = vertex.get(i);
                float t         = v->x*pl.dx + v->y*pl.dy + v->z*pl.dz + pl.dw;
                v->itag         = (t < -DSP_3D_TOLERANCE) ? 2 : (t > DSP_3D_TOLERANCE) ? 0 : 1;
            }

            // Reset all flags of edges
            for (size_t i=0, n=edge.size(); i<n; ++i)
                edge.get(i)->itag &= ~RT_EF_TEMP; // Clear processed flag

            // Initialize edges for all triangles laying on the same plane
            for (size_t i=0,n=triangle.size(); i<n; ++i)
            {
                rt_triangle_t *st   = triangle.get(i);
                if (st->face == ct->face)
                {
                    st->itag            = 2; // Mark as current
                    st->v[0]->itag      = 1;
                    st->v[1]->itag      = 1;
                    st->v[2]->itag      = 1;
                    st->e[0]->itag     |= RT_EF_PROCESSED;
                    st->e[1]->itag     |= RT_EF_PROCESSED;
                    st->e[2]->itag     |= RT_EF_PROCESSED;
                }
            }

            RT_TRACE_BREAK(this,
                lsp_trace("Prepare cullback step (%d triangles)", int(triangle.size()));

                for (size_t i=0,n=triangle.size(); i<n; ++i)
                {
                    rt_triangle_t *t = triangle.get(i);
                    shared->view->add_triangle_1c(t, (t == ct) ? &C_ORANGE : &C_YELLOW);
                }
                shared->view->add_plane_sp3p1c(&view.s, ct->v[0], ct->v[1], ct->v[2], &C_MAGENTA);
                shared->view->add_view_3c(&view, &C_RED, &C_GREEN, &C_BLUE);
            );

            // Call for binary split
            // First step: split edges
            // Perform split of edges
            for (size_t i=0; i<edge.size(); ++i) // The edge array can grow
            {
                rt_edge_t *e    = edge.get(i);
                if (e->itag & RT_EF_PROCESSED) // Skip already split edges
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
                    case 0: case 1: case 4: // edge is over the plane, skip
                    case 6: case 9: case 10: // edge is under the plane, skip
                    case 5: // edge lays on the plane, skip
                        e->itag    |= RT_EF_PROCESSED;
                        break;

                    case 2: // edge is crossing the plane, v0 is over, v1 is under
                    case 8: // edge is crossing the plane, v0 is under, v1 is over
                    {
                        // Allocate split point
                        rt_vertex_t *sp     = vertex.alloc();
                        if (sp == NULL)
                            return STATUS_NO_MEM;
                        dsp::calc_split_point_p2v1(sp, e->v[0], e->v[1], &pl);

                        sp->ve      = NULL;
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
            }

            RT_TRACE(
                if (!validate())
                    return STATUS_CORRUPTED;
            )

            // Toggle state of all triangles
            for (size_t i=0, n=triangle.size(); i<n; ++i)
            {
                rt_triangle_t  *st  = triangle.get(i);

                // Detect position of triangle: over the plane or under the plane
                if (st->itag <= 0) // Check 'unknown' and 'in' states
                {
                    if (st->v[0]->itag != 1)
                        st->itag    = (st->v[0]->itag < 1) ? 1 : 0;
                    else if (st->v[1]->itag != 1)
                        st->itag    = (st->v[1]->itag < 1) ? 1 : 0;
                    else if (st->v[2]->itag != 1)
                        st->itag    = (st->v[2]->itag < 1) ? 1 : 0;
                    else
                        st->itag    = 0; // Triangle is 'in'
                }
                else if (st->itag == 2) // Check 'Current' state
                {
                    if ((st->v[0]->itag < 1) ||
                        (st->v[1]->itag < 1) ||
                        (st->v[2]->itag < 1))
                        st->itag    = 1; // Triangle is 'out'
                }
            }

            RT_TRACE_BREAK(this,
                lsp_trace("After cullback step");

                lsp_trace("In is GREEN/BLUE, out is RED");
                for (size_t i=0,n=this->triangle.size(); i<n; ++i)
                {
                    rt_triangle_t *st = triangle.get(i);
                    shared->view->add_triangle_1c(st,
                            (st->itag == 0) ? &C_GREEN :
                            (st->itag == 2) ? &C_BLUE :
                            &C_RED
                        );
                }

                shared->view->add_plane_sp3p1c(&view.s, ct->v[0], ct->v[1], ct->v[2], &C_YELLOW);
            );

            RT_TRACE_BREAK(this,
                lsp_trace("Prepare cullback (%d triangles)", int(triangle.size()));

                for (size_t i=0,n=triangle.size(); i<n; ++i)
                {
                    rt_triangle_t *st = triangle.get(i);
                    if (st->itag > 0)
                        continue;
                    shared->view->add_triangle_1c(triangle.get(i), &C_YELLOW);
                }

                for (size_t i=0,n=edge.size(); i<n; ++i)
                {
                    rt_edge_t *se = edge.get(i);
                    shared->view->add_segment(se, (se->itag & RT_EF_PLANE) ? &C_GREEN : &C_RED);
                }
            );

            // Fetch new cull-back triangle
            ct = find_cullback_triangle();

        } while (ct != NULL);

        // Now we can fetch triangles
        if (out != NULL)
        {
            out->view       = this->view;
            out->state      = S_IGNORE;
            res             = fetch_triangles_safe(out, 1);
            if (res != STATUS_OK)
                return res;
        }

        // Update self state
        res             = vfetch_triangles_safe(&in, 2, in_itags);
        if (res != STATUS_OK)
            return res;
        this->swap(&in);

        RT_TRACE_BREAK(this,
            lsp_trace("After cullback");

            lsp_trace("In context is GREEN (%d triangles)", int(this->triangle.size()));
            for (size_t i=0,n=this->triangle.size(); i<n; ++i)
                shared->view->add_triangle_1c(this->triangle.get(i), &C_GREEN);

            if (out != NULL)
            {
                lsp_trace("Out context is RED (%d triangles)", int(out->triangle.size()));
                for (size_t i=0,n=out->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(out->triangle.get(i), &C_RED);
            }
        );

        return STATUS_OK;
    }

    status_t rt_context_t::partition(rt_context_t **out, rt_context_t *on)
    {
        static const ssize_t in_itags[] = { 0, 1 };
        static const ssize_t out_itags[] = { 2, 3 };

        status_t res;

        // Clear all target contexts
        if (out != NULL)
        {
            for (size_t i=0; i<3; ++i)
                if (out[i] != NULL)
                    out[i]->clear();
        }
        if (on != NULL)
            on->clear();

        // Is there data for processing ?
        if (triangle.size() <= 1)
            return STATUS_OK;

        // State of itag for vertex:
        //  0   = vertex lays over the plane
        //  1   = vertex lays on the plane
        //  2   = vertex lays below the plane

        // State of itag for triangle:
        // -1   = triangle has lower priority
        //  0   = triangle is 'in'
        //  1   = triangle is 'in' but needs to be placed at the end of array
        //  2   = triangle is 'out'
        //  2   = triangle is 'out' but needs to be placed at the end of array
        //  4   = triangle is 'on'

        // Find culling triangle
        rt_triangle_t *ct   = NULL; // Current triangle
        for (size_t i=0,n=triangle.size(); i<n; ++i)
        {
            rt_triangle_t *st   = triangle.get(i);
            st->itag            = 0; // 'in' by default

            // Find unprocessed edge in the triangle
            if ((!(st->e[0]->itag & RT_EF_PLANE)) ||
                (!(st->e[1]->itag & RT_EF_PLANE)) ||
                (!(st->e[2]->itag & RT_EF_PLANE)))
            {
                st->itag    = 0;
                ct          = st;
                break;
            }
            else
                st->itag    = -1; // We need to put this triangle to the end of queue because there is nothing to split
        }

        // Nothing to do?
        if (ct == NULL)
            return STATUS_OK;

        RT_TRACE_BREAK(this,
            lsp_trace("Prepare partition with triangle (%d triangles)", int(triangle.size()));

            for (size_t i=0,n=triangle.size(); i<n; ++i)
            {
                rt_triangle_t *t = triangle.get(i);
                shared->view->add_triangle_1c(t, (t == ct) ? &C_ORANGE : &C_YELLOW);
            }
            shared->view->add_plane_sp3p1c(ct->v[2], &view.s, ct->v[0], ct->v[1], &C_RED);
            shared->view->add_plane_sp3p1c(ct->v[0], &view.s, ct->v[1], ct->v[2], &C_GREEN);
            shared->view->add_plane_sp3p1c(ct->v[1], &view.s, ct->v[2], ct->v[0], &C_BLUE);
        );

        // Now perform split for each edge
        rt_context_t tmp(shared);
        vector3d_t     pl;      // Culling plane

        for (size_t i=0; i<3; ++i)
        {
            // Process only edges that have not been processed
            rt_edge_t  *se  = ct->e[i];
            if (se->itag & RT_EF_PLANE)
                continue;

            switch (i)
            {
                case 0: dsp::calc_oriented_plane_p3(&pl, ct->v[2], &view.s, ct->v[0], ct->v[1]); break;
                case 1: dsp::calc_oriented_plane_p3(&pl, ct->v[0], &view.s, ct->v[1], ct->v[2]); break;
                case 2: dsp::calc_oriented_plane_p3(&pl, ct->v[1], &view.s, ct->v[2], ct->v[0]); break;
                default: return STATUS_BAD_STATE;
            }

            // Determine co-location of plane and vertexes
            for (size_t j=0, n=vertex.size(); j<n; ++j)
            {
                rt_vertex_t *v  = vertex.get(j);
                float t         = v->x*pl.dx + v->y*pl.dy + v->z*pl.dz + pl.dw;
                v->itag         = (t < -DSP_3D_TOLERANCE) ? 2 : (t > DSP_3D_TOLERANCE) ? 0 : 1;
            }

            // Reset all temporary flags of edges
            for (size_t j=0, n=edge.size(); j<n; ++j)
                edge.get(j)->itag &= ~RT_EF_TEMP; // Clear temporary flags

            // Mark current edge as processed and laying on the split plane
            se->itag       |= RT_EF_PLANE | RT_EF_PROCESSED;
            se->v[0]->itag  = 1;
            se->v[1]->itag  = 1;

//            RT_TRACE_BREAK(this,
//                lsp_trace("Prepare partition with edge (%d triangles)", int(triangle.size()));
//
//                for (size_t i=0,n=triangle.size(); i<n; ++i)
//                {
//                    rt_triangle_t *t = triangle.get(i);
//                    shared->view->add_triangle_1c(t, (t == ct) ? &C_ORANGE : &C_YELLOW);
//                }
//                shared->view->add_plane_3pn1c(&view.s, se->v[0], se->v[1], &pl, &C_MAGENTA);
//
//                for (size_t i=0,n=edge.size(); i<n; ++i)
//                {
//                    rt_edge_t *e = edge.get(i);
//                    shared->view->add_segment(e,
//                            (e == se) ? &C_MAGENTA :
//                            (e->itag & RT_EF_PLANE) ? &C_GREEN : &C_RED
//                        );
//                }
//            );

            // Perform split of edges
            for (size_t j=0; j<edge.size(); ++j) // The edge array can grow
            {
                rt_edge_t *e    = edge.get(j);
                if (e->itag & RT_EF_PROCESSED) // Skip already splitted eges
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
                    case 0: case 1: case 4: // edge is over the plane, skip
                    case 6: case 9: case 10: // edge is under the plane, skip
                        e->itag    |= RT_EF_PROCESSED;
                        break;
                    case 5: // edge lays on the plane, mark as split edge and skip
                        e->itag    |= RT_EF_PLANE | RT_EF_PROCESSED;
                        break;

                    case 2: // edge is crossing the plane, v0 is over, v1 is under
                    case 8: // edge is crossing the plane, v0 is under, v1 is over
                    {
                        // Allocate split point
                        rt_vertex_t *sp     = vertex.alloc();
                        if (sp == NULL)
                            return STATUS_NO_MEM;

                        dsp::calc_split_point_p2v1(sp, e->v[0], e->v[1], &pl);
                        sp->ve      = NULL;
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
            }

            RT_TRACE(
                if (!validate())
                    return STATUS_CORRUPTED;
            )

            // Toggle state of all triangles
            for (size_t j=0, n=triangle.size(); j<n; ++j)
            {
                rt_triangle_t  *st  = triangle.get(j);
                size_t intag     = (st->itag == -1) ? 0 : 1;
                size_t outtag    = (st->itag == -1) ? 2 : 3;

                // Detect position of triangle: over the plane or under the plane
                if (st->v[0]->itag != 1)
                    st->itag    = (st->v[0]->itag < 1) ? outtag : intag;
                else if (st->v[1]->itag != 1)
                    st->itag    = (st->v[1]->itag < 1) ? outtag : intag;
                else if (st->v[2]->itag != 1)
                    st->itag    = (st->v[2]->itag < 1) ? outtag : intag;
                else
                    st->itag    = 4; // Triangle is on the plane
            }

            // Now we can fetch triangles
            res     = vfetch_triangles_safe(&tmp, 2, in_itags);
            if (res != STATUS_OK)
                return res;
            res     = vfetch_triangles_safe(out[i], 2, out_itags);
            if (res != STATUS_OK)
                return res;
            res     = fetch_triangles_safe(on, 4);
            if (res != STATUS_OK)
                return res;
            tmp.swap(this);
            tmp.clear();

//            RT_TRACE_BREAK(this,
//                lsp_trace("Split space with plane %d", int(i));
//
//                lsp_trace("In context is YELLOW (%d triangles)", int(this->triangle.size()));
//                for (size_t j=0,n= this->triangle.size(); j<n; ++j)
//                {
//                    rt_triangle_t *t =  this->triangle.get(j);
//                    shared->view->add_triangle_1c(t, &C_YELLOW);
//                }
//
//                if (out[i] != NULL)
//                {
//                    lsp_trace("Out context %d is RED (%d triangles)", int(i), int(out[i]->triangle.size()));
//                    for (size_t j=0,n=out[i]->triangle.size(); j<n; ++j)
//                    {
//                        rt_triangle_t *t =  out[i]->triangle.get(j);
//                        shared->view->add_triangle_1c(t, &C_RED);
//                    }
//                }
//
//                if (on != NULL)
//                {
//                    lsp_trace("On context is CYAN (%d triangles)", int(on->triangle.size()));
//                    for (size_t j=0,n=on->triangle.size(); j<n; ++j)
//                    {
//                        rt_triangle_t *t =  on->triangle.get(j);
//                        shared->view->add_triangle_1c(t, &C_CYAN);
//                    }
//                }
//            );


            // Try to obtain new 'current' triangle
            ct          = this->triangle.get(0);
            if (ct == NULL)
                break;
        }

        RT_TRACE_BREAK(this,
            lsp_trace("Partitioned space");

            lsp_trace("In context is YELLOW (%d triangles)", int(this->triangle.size()));
            for (size_t i=0,n= this->triangle.size(); i<n; ++i)
            {
                rt_triangle_t *t =  this->triangle.get(i);
                shared->view->add_triangle_1c(t, &C_YELLOW);
            }

            if (out != NULL)
            {
                if (out[0] != NULL)
                {
                    lsp_trace("Out context 0 is RED (%d triangles)", int(out[0]->triangle.size()));
                    for (size_t i=0,n=out[0]->triangle.size(); i<n; ++i)
                    {
                        rt_triangle_t *t =  out[0]->triangle.get(i);
                        shared->view->add_triangle_1c(t, &C_RED);
                    }
                }
                if (out[1] != NULL)
                {
                    lsp_trace("Out context 1 is GREEN (%d triangles)", int(out[1]->triangle.size()));
                    for (size_t i=0,n=out[1]->triangle.size(); i<n; ++i)
                    {
                        rt_triangle_t *t =  out[1]->triangle.get(i);
                        shared->view->add_triangle_1c(t, &C_GREEN);
                    }
                }
                if (out[2] != NULL)
                {
                    lsp_trace("Out context 2 is BLUE (%d triangles)", int(out[2]->triangle.size()));
                    for (size_t i=0,n=out[2]->triangle.size(); i<n; ++i)
                    {
                        rt_triangle_t *t =  out[2]->triangle.get(i);
                        shared->view->add_triangle_1c(t, &C_BLUE);
                    }
                }
            }

            if (on != NULL)
            {
                lsp_trace("On context is CYAN (%d triangles)", int(on->triangle.size()));
                for (size_t i=0,n=on->triangle.size(); i<n; ++i)
                {
                    rt_triangle_t *t =  on->triangle.get(i);
                    shared->view->add_triangle_1c(t, &C_CYAN);
                }
            }
        );

        return STATUS_OK;
    }

    status_t rt_context_t::partition(rt_context_t **out, rt_context_t *on, rt_context_t *in)
    {
        status_t res = partition(out, on);
        if ((res == STATUS_OK) && (in != NULL))
            in->swap(this);
        return res;
    }

    void rt_context_t::debug(rt_context_t *ctx)
    {
        lsp_trace("Triggered debug on step=%d", int(ctx->shared->step));

        float min_x, min_y, max_x, max_y;

        for (size_t i=0, n=vertex.size(); i<n; ++i)
        {
            rt_vertex_t *v = vertex.get(i);
            if (v->z < -1.5f)
                continue;

            if (i == 0)
            {
                min_x   = v->x;
                min_y   = v->y;
                max_x   = v->x;
                max_y   = v->y;
                continue;
            }
            if (min_x > v->x)
                min_x = v->x;
            if (max_x < v->x)
                max_x = v->x;
            if (min_y > v->y)
                min_y = v->y;
            if (max_y < v->y)
                max_y = v->y;
        }

        lsp_trace("edge dump:");
        printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
        printf("<svg width=\"%d\" height=\"%d\" viewBox=\"0 0 %d %d\" xmlns=\"http://www.w3.org/2000/svg\">\n",
                int((max_x - min_x) * 10000000.0f),
                int((max_y - min_y) * 10000000.0f),
                int((max_x - min_x) * 10000000.0f),
                int((max_y - min_y) * 10000000.0f)
        );
        for (size_t i=0, n=edge.size(); i<n; ++i)
        {
            rt_edge_t *ce = edge.get(i);
            if (ce->v[0]->z < -1.5f)
                continue;

            printf("\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"%s\" stroke-width=\"2\" />\n",
                    (ce->v[0]->x-min_x)*10000000.0f, (ce->v[0]->y-min_y)*10000000.0f,
                    (ce->v[1]->x-min_x)*10000000.0f, (ce->v[1]->y-min_y)*10000000.0f,
                    (ce->itag & RT_EF_PLANE) ? "blue" : "grey"
                    );
//            if (!(ce->itag & RT_EF_PLANE))
//            {
//                printf("%f,%f,%f\n", ce->v[0]->x, ce->v[0]->y, ce->v[0]->z);
//                printf("%f,%f,%f\n", ce->v[1]->x, ce->v[1]->y, ce->v[1]->z);
//            }
        }
        printf("</svg>\n");
        lsp_trace("End of dump");
    }

    status_t rt_context_t::cutoff(rt_context_t *out, rt_context_t *in)
    {
        static const ssize_t in_itags[] = { 0, 1 };

        vector3d_t pl;
        status_t res;

        // Cleanup output contexts
        if (out != NULL)
            out->clear();
        if (in != NULL)
            in->clear();

        // Nothing to split?
        if (triangle.size() <= 1)
        {
            in->swap(this);
            return STATUS_OK;
        }

        // State of itag for vertex:
        //  0   = vertex lays over the plane
        //  1   = vertex lays on the plane
        //  2   = vertex lays below the plane

        // State of itag for triangle:
        // -1   = currently unknown state
        //  0   = triangle is 'in'
        //  1   = triangle is 'in' but lays on the culling plane and needs to be placed at the end of array
        //  2   = triangle is 'out'

        // Get triangle for cut-off
        rt_triangle_t *ct = triangle.get(0);
        if (ct == NULL)
            return STATUS_OK;

        dsp::calc_oriented_plane_p3(&pl, &view.s, ct->v[0], ct->v[1], ct->v[2]);

        RT_TRACE_BREAK(this,
            lsp_trace("Prepare cutoff (%d triangles)", int(triangle.size()));

            for (size_t i=0,n=triangle.size(); i<n; ++i)
            {
                rt_triangle_t *t = triangle.get(i);
                shared->view->add_triangle_1c(t, (t == ct) ? &C_ORANGE : &C_YELLOW);
            }
            shared->view->add_plane_3pn1c(ct->v[0], ct->v[1], ct->v[2], &pl, &C_MAGENTA);
        );
//        RT_CALL_DEBUGGER(this, 182);

        // Estimate co-location of vertexes and plane
        for (size_t i=0, n=vertex.size(); i<n; ++i)
        {
            rt_vertex_t *v  = vertex.get(i);
            float t         = v->x*pl.dx + v->y*pl.dy + v->z*pl.dz + pl.dw;
            v->itag         = (t < -DSP_3D_TOLERANCE) ? 2 : (t > DSP_3D_TOLERANCE) ? 0 : 1;
        }

        // Reset all flags of edges
        for (size_t i=0, n=edge.size(); i<n; ++i)
            edge.get(i)->itag &= ~RT_EF_PROCESSED; // Clear processed flag

        for (size_t i=0,n=triangle.size(); i<n; ++i)
        {
            rt_triangle_t *st = triangle.get(i);
            st->itag        = -1;

            if (st->face == ct->face)
            {
                st->itag        = 1; // 'Face' tag
                st->v[0]->itag  = 1;
                st->v[1]->itag  = 1;
                st->v[2]->itag  = 1;
                st->e[0]->itag  |= RT_EF_PROCESSED;
                st->e[1]->itag  |= RT_EF_PROCESSED;
                st->e[2]->itag  |= RT_EF_PROCESSED;
            }
        }

        // Perform split of edges
        for (size_t i=0; i<edge.size(); ++i) // The edge array can grow
        {
            rt_edge_t *e    = edge.get(i);
            if (e->itag & RT_EF_PROCESSED) // Skip already splitted eges
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
                case 0: case 1: case 4: // edge is over the plane, skip
                case 6: case 9: case 10: // edge is under the plane, skip
                case 5: // edge lays on the plane, skip
                    e->itag    |= RT_EF_PROCESSED;
                    break;

                case 2: // edge is crossing the plane, v0 is over, v1 is under
                case 8: // edge is crossing the plane, v0 is under, v1 is over
                {
                    // Allocate split point
                    rt_vertex_t *sp     = vertex.alloc();
                    if (sp == NULL)
                        return STATUS_NO_MEM;

                    // Compute split point
                    dsp::calc_split_point_p2v1(sp, e->v[0], e->v[1], &pl);
                    sp->ve      = NULL;
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
        }
        RT_TRACE(
            if (!validate())
                return STATUS_CORRUPTED;
        )

        // Toggle state of all triangles
        for (size_t i=0, n=triangle.size(); i<n; ++i)
        {
            rt_triangle_t  *st  = triangle.get(i);
            if (st->itag >= 0)
                continue;

            // Detect position of triangle: over the plane, under the plane or on the plane
            if (st->v[0]->itag != 1)
                st->itag    = (st->v[0]->itag < 1) ? 2 : 0;
            else if (st->v[1]->itag != 1)
                st->itag    = (st->v[1]->itag < 1) ? 2 : 0;
            else if (st->v[2]->itag != 1)
                st->itag    = (st->v[2]->itag < 1) ? 2 : 0;
            else
                st->itag    = 1; // Triangle is on the plane
        }

        res = vfetch_triangles_safe(in, 2, in_itags);
        if (res != STATUS_OK)
            return res;

        res    = fetch_triangles_safe(out, 2);
        if (res != STATUS_OK)
            return res;

        RT_TRACE_BREAK(this,
            lsp_trace("After cutoff");

            if (in != NULL)
            {
                lsp_trace("In context is GREEN (%d triangles)", int(in->triangle.size()));
                for (size_t i=0,n=in->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(in->triangle.get(i), &C_GREEN);
            }
            if (out != NULL)
            {
                lsp_trace("Out context is RED (%d triangles)", int(out->triangle.size()));
                for (size_t i=0,n=out->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(out->triangle.get(i), &C_RED);
            }

            shared->view->add_plane_3pn1c(ct->v[0], ct->v[1], ct->v[2], &pl, &C_YELLOW);
        );

        return STATUS_OK;
    }

    status_t rt_context_t::cutoff(rt_context_t *out)
    {
        rt_context_t tmp(shared);
        status_t res = cutoff(out, &tmp);
        if (res == STATUS_OK)
            tmp.swap(this);

        return res;
    }

    bool rt_context_t::check_crossing(rt_triangle_t *ct, rt_triangle_t *st)
    {
        vector3d_t pl;
        float k[3];

        /**
         Theorem: two projections of triangles are not crossing if all points of first triangle are laying
         over one of the planes built from the second triangle's edges and projection point OR vice verse.
         */

        // Skip triangles laying on the same face because they are not crossing
        if (st->face == ct->face)
            return false;

        // Skip already partitioned edges
        if ((st->e[0]->itag & RT_EF_PARTITIONED) &&
            (st->e[1]->itag & RT_EF_PARTITIONED) &&
            (st->e[2]->itag & RT_EF_PARTITIONED))
            return false;

        // Ensure that ct and st are not crossing (direct checking)
        if (((st->v[0]->itag & 0x3) <= 0x1) && ((st->v[1]->itag & 0x3) <= 0x1) && ((st->v[2]->itag & 0x3) <= 0x1))
            return false;
        if (((st->v[0]->itag & 0xc) <= 0x4) && ((st->v[1]->itag & 0xc) <= 0x4) && ((st->v[2]->itag & 0xc) <= 0x4))
            return false;
        if (((st->v[0]->itag & 0x30) <= 0x10) && ((st->v[1]->itag & 0x30) <= 0x10) && ((st->v[2]->itag & 0x30) <= 0x10))
            return false;

        // Ensure that ct and st are not crossing (reverse checking)
        dsp::calc_oriented_plane_p3(&pl, st->v[2], &view.s, st->v[0], st->v[1]);
        k[0]    = ct->v[0]->x * pl.dx + ct->v[0]->y * pl.dy + ct->v[0]->z * pl.dz + pl.dw;
        k[1]    = ct->v[1]->x * pl.dx + ct->v[1]->y * pl.dy + ct->v[1]->z * pl.dz + pl.dw;
        k[2]    = ct->v[2]->x * pl.dx + ct->v[2]->y * pl.dy + ct->v[2]->z * pl.dz + pl.dw;

        if ((k[0] >= -DSP_3D_TOLERANCE) && (k[1] >= -DSP_3D_TOLERANCE) && (k[2] >= -DSP_3D_TOLERANCE))
            return false;

        dsp::calc_oriented_plane_p3(&pl, st->v[0], &view.s, st->v[1], st->v[2]);
        k[0]    = ct->v[0]->x * pl.dx + ct->v[0]->y * pl.dy + ct->v[0]->z * pl.dz + pl.dw;
        k[1]    = ct->v[1]->x * pl.dx + ct->v[1]->y * pl.dy + ct->v[1]->z * pl.dz + pl.dw;
        k[2]    = ct->v[2]->x * pl.dx + ct->v[2]->y * pl.dy + ct->v[2]->z * pl.dz + pl.dw;

        if ((k[0] >= -DSP_3D_TOLERANCE) && (k[1] >= -DSP_3D_TOLERANCE) && (k[2] >= -DSP_3D_TOLERANCE))
            return false;

        dsp::calc_oriented_plane_p3(&pl, st->v[1], &view.s, st->v[2], st->v[0]);
        k[0]    = ct->v[0]->x * pl.dx + ct->v[0]->y * pl.dy + ct->v[0]->z * pl.dz + pl.dw;
        k[1]    = ct->v[1]->x * pl.dx + ct->v[1]->y * pl.dy + ct->v[1]->z * pl.dz + pl.dw;
        k[2]    = ct->v[2]->x * pl.dx + ct->v[2]->y * pl.dy + ct->v[2]->z * pl.dz + pl.dw;

        if ((k[0] >= -DSP_3D_TOLERANCE) && (k[1] >= -DSP_3D_TOLERANCE) && (k[2] >= -DSP_3D_TOLERANCE))
            return false;

        return true;
    }

    void rt_context_t::calc_partition_itag(rt_triangle_t *ct)
    {
        size_t it[3], xt[3];
        it[0] = ct->v[0]->itag;
        it[1] = ct->v[1]->itag;
        it[2] = ct->v[2]->itag;

        // Currently tag is unknown
        ct->itag    = -1; // Unknown

        // Ensure that triangle lays over one of the planes
        if (((it[0] & 0x3) == 0) && ((it[1] & 0x3) == 0) && ((it[2] & 0x3) == 0))
        {
            ct->itag = 1; // Out context 1
            return;
        }
        else if (((it[0] & 0xc) == 0) && ((it[1] & 0xc) == 0) && ((it[2] & 0xc) == 0))
        {
            ct->itag = 2; // Out context 2
            return;
        }
        else if (((it[0] & 0x30) == 0) && ((it[1] & 0x30) == 0) && ((it[2] & 0x30)  == 0))
        {
            ct->itag = 3; // Out context 3
            return;
        }

        // Check case when points lay on the same plane
        for (size_t mask = 0x03; mask <= 0x30; mask <<= 2)
        {
            if ((it[0] & mask) != 1)
                continue;
            if ((it[1] & mask) != 1)
                continue;
            if ((it[2] & mask) != 1)
                continue;
            ct->itag = 4; // Ignore
            return;
        }

        // Check that points lay inside the triangle
        // 10 10 10   point lays inside of triangle
        // 10 10 01   point lays on edge 0
        // 10 01 10   point lays on edge 1
        // 01 10 10   point lays on edge 2
        // 10 01 01   point is result of crossing of edges 0 and 1
        // 01 10 01   point is result of crossing of edges 0 and 2
        // 01 01 10   point is result of crossing of edges 1 and 2
        for (size_t i=0; i<3; ++i)
        {
            switch (it[i])
            {
                case 0x2a:
                    xt[i]       = 2;
                    break;
                case 0x29: case 0x26: case 0x1a:
                case 0x25: case 0x19: case 0x16:
                    xt[i]       = 1;
                    continue;
                default:
                    xt[i]       = 0;
                    break;
            }
        }

        if ((xt[0] >= 1) && (xt[1] >= 1) && (xt[2] >= 1))
            ct->itag    = 0; // Inside
    }

#if 0
    status_t rt_context_t::partition(rt_context_t **out, rt_context_t *ign, rt_context_t *in)
    {
        vector3d_t pl[3];
        rt_triangle_t *ct;
        status_t res;

        // Clear all target contexts
        for (size_t i=0; i<3; ++i)
            if (out[i] != NULL)
                out[i]->clear();
        if (ign != NULL)
            ign->clear();
        if (in != NULL)
            in->clear();

        // STEP 0
        // Get partitioning triangle
        ct = triangle.get(0);
        if (ct == NULL)
            return STATUS_OK;

        dsp::calc_oriented_plane_p3(&pl[0], ct->v[2], &view.s, ct->v[0], ct->v[1]);
        dsp::calc_oriented_plane_p3(&pl[1], ct->v[0], &view.s, ct->v[1], ct->v[2]);
        dsp::calc_oriented_plane_p3(&pl[2], ct->v[1], &view.s, ct->v[2], ct->v[0]);

        RT_TRACE_BREAK(this,
            lsp_trace("Prepare partition (%d triangles)", int(triangle.size()));

            for (size_t i=0,n=triangle.size(); i<n; ++i)
            {
                rt_triangle_t *t = triangle.get(i);
                shared->view->add_triangle_1c(t, (t == ct) ? &C_ORANGE : &C_YELLOW);
            }
            shared->view->add_plane_3pn1c(&view.s, ct->v[0], ct->v[1], &pl[0], &C_RED);
            shared->view->add_plane_3pn1c(&view.s, ct->v[1], ct->v[2], &pl[1], &C_GREEN);
            shared->view->add_plane_3pn1c(&view.s, ct->v[2], ct->v[0], &pl[2], &C_BLUE);
        );

        // STEP 1
        // Estimate the location of each vertex relative to each triangle edge
        for (size_t i=0, n=vertex.size(); i<n; ++i)
        {
            rt_vertex_t *cv = vertex.get(i);
            cv->itag        = 0;

            // Check co-location of vertexes and triangle planes
            for (size_t j=0; j<3; ++j)
            {
                float k         = cv->x * pl[j].dx + cv->y * pl[j].dy + cv->z * pl[j].dz + pl[j].dw;
                if (k <= -DSP_3D_TOLERANCE)
                    cv->itag       |= 2 << (j << 1);
                else if (k <= DSP_3D_TOLERANCE)
                    cv->itag       |= 1 << (j << 1);
            }
        }
        ct->v[0]->itag      = 0x19; // 01 10 01 - point 0 lays on planes 0 and 2
        ct->v[1]->itag      = 0x25; // 10 01 01 - point 1 lays on planes 0 and 1
        ct->v[2]->itag      = 0x16; // 01 01 10 - point 2 lays on planes 1 and 2

        for (size_t i=0, n=edge.size(); i<n; ++i)
            edge.get(i)->itag &= ~RT_EF_TEMP; // Clear processed flag

        // Pre-process triangles
        for (size_t i=0,n=triangle.size(); i<n; ++i)
            triangle.get(i)->itag   = 0;

        for (size_t i=0; i<3; ++i)
        {
            size_t bit = i << 1;
            for (size_t j=0,n=triangle.size(); j<n; ++j)
            {
                rt_triangle_t *ct = triangle.get(j);
                if (ct->itag > 0)
                    continue;

                if ((((ct->v[0]->itag >> bit) & 0x03) < 1) &&
                    (((ct->v[1]->itag >> bit) & 0x03) < 1) &&
                    (((ct->v[2]->itag >> bit) & 0x03) < 1))
                    ct->itag = i + 1;
            }
        }

        for (size_t j=0,n=triangle.size(); j<n; ++j)
        {
            rt_triangle_t *ct = triangle.get(j);
            if (ct->itag > 0)
            {
                ct->v[0]->itag     |= RT_EF_PARTITIONED;
                ct->v[1]->itag     |= RT_EF_PARTITIONED;
                ct->v[2]->itag     |= RT_EF_PARTITIONED;
            }
        }

        RT_TRACE_BREAK(this,
            lsp_trace("Marked triangles into in (YELLOW), out (RGB)");

            for (size_t i=0,n=triangle.size(); i<n; ++i)
            {
                rt_triangle_t *t = triangle.get(i);
                const color3d_t *xc;

                if (t == ct) xc = &C_ORANGE;
                else if (t->itag == 0) xc = &C_YELLOW;
                else if (t->itag == 1) xc = &C_RED;
                else if (t->itag == 2) xc = &C_GREEN;
                else xc = &C_BLUE;

                shared->view->add_triangle_1c(t, xc);
            }
            shared->view->add_plane_3pn1c(&view.s, ct->v[0], ct->v[1], &pl[0], &C_RED);
            shared->view->add_plane_3pn1c(&view.s, ct->v[1], ct->v[2], &pl[1], &C_GREEN);
            shared->view->add_plane_3pn1c(&view.s, ct->v[2], ct->v[0], &pl[2], &C_BLUE);
        );

        // STEP 2
        // Perform the partitioning
        for (size_t i=0; i<3; ++i)
        {
            // Is there data for partitioning?
            if (this->triangle.size() <= 1)
                break;

            RT_TRACE_BREAK(this,
                lsp_trace("Splitting edges with plane %d", int(i));

                for (size_t j=0,n=triangle.size(); j<n; ++j)
                {
                    rt_triangle_t *t = triangle.get(j);
                    const color3d_t *xc;

                    if (t == ct) xc = &C_ORANGE;
                    else if (t->itag == 0) xc = &C_CYAN;
                    else xc = &C_YELLOW;

                    shared->view->add_triangle_1c(t, xc);
                }
                if (i == 0)
                    shared->view->add_plane_3pn1c(&view.s, ct->v[0], ct->v[1], &pl[0], &C_MAGENTA);
                else if (i == 1)
                    shared->view->add_plane_3pn1c(&view.s, ct->v[1], ct->v[2], &pl[1], &C_MAGENTA);
                else if (i == 2)
                    shared->view->add_plane_3pn1c(&view.s, ct->v[2], ct->v[0], &pl[2], &C_MAGENTA);
            );

            // PREPARE DATA
            // Reset processed flags for all edges
            for (size_t j=0, n=edge.size(); j<n; ++j)
            {
                rt_edge_t *se = edge.get(j);
                if (!(se->itag & RT_EF_PARTITIONED))
                    se->itag &= ~RT_EF_PROCESSED; // Clear processed flag
            }

            size_t bit = i << 1;

            for (size_t j=0; j<edge.size(); ++j)
            {
                rt_edge_t *se   = edge.get(j);
                // Check processed flag
                if (se->itag & RT_EF_PROCESSED)
                    continue;

                // Check that points of current edge are laying on opposite sides of the selected triangle's edge's plane
                ssize_t s       = ((se->v[0]->itag >> bit) & 0x03) | (((se->v[1]->itag >> bit) & 0x03) << 2);
                switch (s)
                {
                    case 0: case 1: case 4: // Edge is outside
                    case 6: case 9: case 10: // Edge is inside
                    case 5: // Edge is laying on the plane
                        se->itag |= RT_EF_PROCESSED;
                        continue;
                    case 2: case 8: // Edge is crossing plane
                        break;
                    default:
                        return STATUS_BAD_STATE;
                }
                if ((s != 0x2) && (s != 0x8))
                    continue;

                // Compute split point coordinates
                vector3d_t d;
                d.dx        = se->v[1]->x - se->v[0]->x;
                d.dy        = se->v[1]->y - se->v[0]->y;
                d.dz        = se->v[1]->z - se->v[0]->z;
                d.dw        = 0.0f;

                float t     = (se->v[0]->x * pl[i].dx + se->v[0]->y * pl[i].dy + se->v[0]->z * pl[i].dz + pl[i].dw) /
                              (pl[i].dx*d.dx + pl[i].dy*d.dy + pl[i].dz*d.dz);

                rt_vertex_t *sp = vertex.alloc();
                if (sp == NULL)
                    return STATUS_NO_MEM;

                sp->x       = se->v[0]->x - d.dx * t;
                sp->y       = se->v[0]->y - d.dy * t;
                sp->z       = se->v[0]->z - d.dz * t;
                sp->w       = 1.0f;

                sp->ve      = NULL;
                sp->ptag    = NULL;

                // Estimate location of split-point relative to other planes
                sp->itag    = 1 << bit;
                for (size_t m = 0; m<3; ++m)
                {
                    if (m == i)
                        continue;
                    float k             = sp->x * pl[m].dx + sp->y * pl[m].dy + sp->z * pl[m].dz + pl[m].dw;
                    if (k <= -DSP_3D_TOLERANCE)
                        sp->itag       |= 2 << (m << 1);
                    else if (k <= DSP_3D_TOLERANCE)
                        sp->itag       |= 1 << (m << 1);
                }

                // Perform edge split
                res    = split_edge(se, sp);
                if (res != STATUS_OK)
                    return res;
            }

            RT_TRACE_BREAK(this,
                lsp_trace("Processed triangles with plane %d: in (YELLOW), out (RGB), ignore(MAGENTA) and unknown (CYAN)", int(i));

                for (size_t i=0,n=triangle.size(); i<n; ++i)
                {
                    rt_triangle_t *t = triangle.get(i);
                    const color3d_t *xc;

                    if (t == ct) xc = &C_ORANGE;
                    else if (t->itag == 0) xc = &C_YELLOW;
                    else if (t->itag == 1) xc = &C_RED;
                    else if (t->itag == 2) xc = &C_GREEN;
                    else xc = &C_BLUE;

                    shared->view->add_triangle_1c(t, xc);
                }
                shared->view->add_plane_3pn1c(&view.s, ct->v[0], ct->v[1], &pl[0], &C_RED);
                shared->view->add_plane_3pn1c(&view.s, ct->v[1], ct->v[2], &pl[1], &C_GREEN);
                shared->view->add_plane_3pn1c(&view.s, ct->v[2], ct->v[0], &pl[2], &C_BLUE);
            );
        }

        // Post-process triangles
        for (size_t i=0; i<3; ++i)
        {
            size_t bit = i << 1;

            for (size_t j=0,n=triangle.size(); j<n; ++j)
            {
                rt_triangle_t *ct = triangle.get(j);

                if (ct->itag > 0)
                    continue;
                else if (((ct->v[0]->itag >> bit) & 0x3) < 1)
                    ct->itag    = i+1;
                else if (((ct->v[1]->itag >> bit) & 0x3) < 1)
                    ct->itag    = i+1;
                else if (((ct->v[2]->itag >> bit) & 0x3) < 1)
                    ct->itag    = i+1;
            }
        }

//        RT_TRACE_BREAK(this,
//            lsp_trace("Processed triangles: in (YELLOW), out (RGB), ignore(MAGENTA) and unknown (CYAN)");
//
//            for (size_t i=0,n=triangle.size(); i<n; ++i)
//            {
//                rt_triangle_t *t = triangle.get(i);
//                const color3d_t *xc;
//
//                if (t->itag == 0) xc = &C_YELLOW;
//                else if (t->itag == 1) xc = &C_RED;
//                else if (t->itag == 2) xc = &C_GREEN;
//                else if (t->itag == 3) xc = &C_BLUE;
//                else if (t->itag == 4) xc = &C_MAGENTA;
//                else xc = &C_CYAN;
//
//                shared->view->add_triangle_1c(t, xc);
//            }
//            shared->view->add_plane_3pn1c(&view.s, ct->v[0], ct->v[1], &pl[0], &C_RED);
//            shared->view->add_plane_3pn1c(&view.s, ct->v[1], ct->v[2], &pl[1], &C_GREEN);
//            shared->view->add_plane_3pn1c(&view.s, ct->v[2], ct->v[0], &pl[2], &C_BLUE);
//        );

        // Store data to set of contexts
        res = fetch_triangles_safe(in, 0);
        if (res != STATUS_OK)
            return res;
        res = fetch_triangles_safe(out[0], 1);
        if (res != STATUS_OK)
            return res;
        res = fetch_triangles_safe(out[1], 2);
        if (res != STATUS_OK)
            return res;
        res = fetch_triangles_safe(out[2], 3);
        if (res != STATUS_OK)
            return res;
        res = fetch_triangles_safe(ign, 4);
        if (res != STATUS_OK)
            return res;

        RT_TRACE_BREAK(this,
            lsp_trace("Partitioned: ");

            if (out[0] != NULL)
            {
                lsp_trace("Out context 0 (%d triangles) is RED", int(out[0]->triangle.size()));
                for (size_t i=0,n=out[0]->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(out[0]->triangle.get(i), &C_RED);
            }
            if (out[1] != NULL)
            {
                lsp_trace("Out context 1 (%d triangles) is GREEN", int(out[1]->triangle.size()));
                for (size_t i=0,n=out[1]->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(out[1]->triangle.get(i), &C_GREEN);
            }
            if (out[2] != NULL)
            {
                lsp_trace("Out context 2 (%d triangles) is BLUE", int(out[2]->triangle.size()));
                for (size_t i=0,n=out[2]->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(out[2]->triangle.get(i), &C_BLUE);
            }
            if (ign != NULL)
            {
                lsp_trace("Ignored context (%d triangles) is MAGENTA", int(ign->triangle.size()));
                for (size_t i=0,n=ign->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(ign->triangle.get(i), &C_MAGENTA);
            }
            if (in != NULL)
            {
                lsp_trace("In context (%d triangles) is YELLOW", int(in->triangle.size()));
                for (size_t i=0,n=in->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(in->triangle.get(i), &C_YELLOW);
            }
        );

        return STATUS_OK;
    }
#endif

    status_t rt_context_t::add_object(Object3D *obj)
    {
        // Reset tags
        obj->scene()->init_tags(NULL, 0);
        if (!obj->scene()->validate())
            return STATUS_CORRUPTED;

        matrix3d_t *m   = obj->matrix();

//        lsp_trace("Processing object \"%s\"", obj->get_name());
        size_t start_t  = triangle.size();
        size_t start_e  = edge.size();
        size_t face_id  = triangle.size();

        // Clone triangles and apply object matrix to vertexes
        for (size_t i=0, n=obj->num_triangles(); i<n; ++i)
        {
            obj_triangle_t *st = obj->triangle(i);
            if (st == NULL)
                return STATUS_BAD_STATE;
            else if (st->ptag != NULL) // Skip already emitted triangle
                continue;

            // Allocate triangle and store pointer
            rt_triangle_t *dt = triangle.alloc();
            if (dt == NULL)
                return STATUS_NO_MEM;

            dt->elnk[0] = NULL;
            dt->elnk[1] = NULL;
            dt->elnk[2] = NULL;
            dt->ptag    = st;
            dt->itag    = 0;
//            dt->face    = st->face;
            dt->face    = face_id++; // Generate face identifier
            st->ptag    = dt;

//            lsp_trace("Link rt_triangle[%p] to obj_triangle[%p]", dt, st);

            dsp::apply_matrix3d_mv2(&dt->n, st->n[0], m);

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

                    dsp::apply_matrix3d_mp2(vx, st->v[j], m);
                    vx->ve          = NULL;
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
                    ex->vlnk[0]     = NULL;
                    ex->vlnk[1]     = NULL;
                    ex->ptag        = st->e[j];
                    ex->itag        = 0;

                    st->e[j]->ptag  = ex;
//                    lsp_trace("Link #%d rt_edge[%p] to obj_edge[%p]", int(j), ex, st->e[j]);
                }

                dt->v[j]        = vx;
                dt->e[j]        = ex;
            }
        }

        // Patch edge structures and link to vertexes
        for (size_t i=start_e, n=edge.size(); i<n; ++i)
        {
            rt_edge_t *de       = edge.get(i);
            obj_edge_t *se      = reinterpret_cast<obj_edge_t *>(de->ptag);

//            lsp_trace("patching rt_edge[%p] with obj_edge[%p]", de, se);
            de->v[0]            = reinterpret_cast<rt_vertex_t *>(se->v[0]->ptag);
            de->v[1]            = reinterpret_cast<rt_vertex_t *>(se->v[1]->ptag);

            de->vlnk[0]         = de->v[0]->ve;
            de->vlnk[1]         = de->v[1]->ve;
            de->v[0]->ve        = de;
            de->v[1]->ve        = de;
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

        if (!obj->scene()->validate())
            return STATUS_CORRUPTED;

        if (!validate())
            return STATUS_CORRUPTED;

        return STATUS_OK;
    }

    bool rt_context_t::validate_list(rt_vertex_t *v)
    {
        rt_edge_t *e = v->ve;
        size_t n = 0;

        while (e != NULL)
        {
            if (!edge.validate(e))
                return false;

            ++n;
            if (e->v[0] == v)
                e   = e->vlnk[0];
            else if (e->v[1] == v)
                e   = e->vlnk[1];
            else
                return false;
        }

        return n > 0; // The vertex should be linked at least to one edge
    }

    ssize_t rt_context_t::linked_count(rt_edge_t *e, rt_vertex_t *v)
    {
        if ((e == NULL) || (v == NULL))
            return -1;

        size_t n = 0;
        for (rt_edge_t *p = v->ve; p != NULL; )
        {
            if (p->v[0] == p->v[1])
                return -1;
            if (p == e)
                ++n;

            if (p->v[0] == v)
                p = p->vlnk[0];
            else if (p->v[1] == v)
                p = p->vlnk[1];
            else
                return -1;
        }

        return n;
    }

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
            if (!validate_list(v))
                return false;
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
                if (!edge.validate(e->vlnk[j]))
                    return false;
                if (linked_count(e, e->v[j]) != 1)
                    return false;
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

        return (shared->ignored.add(&vt)) ? STATUS_OK : STATUS_NO_MEM;
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

        return (shared->matched.add(&vt)) ? STATUS_OK : STATUS_NO_MEM;
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
            dump_edge_list(4, vx->ve);
        }

        printf("Edges (%d items):\n", int(edge.size()));
        for (size_t i=0,n=edge.size(); i<n; ++i)
        {
            rt_edge_t *ex = edge.get(i);
            printf("  [%3d]: %p\n"
                   "    v:  [%d]-[%d]\n"
                   "    l:  [%d]-[%d]\n",
                   int(i), ex,
                   int(vertex.index_of(ex->v[0])),
                   int(vertex.index_of(ex->v[1])),
                   int(edge.index_of(ex->vlnk[0])),
                   int(edge.index_of(ex->vlnk[1]))
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
        dump_edge_list(lvl+2, e->vlnk[0]);
        dump_edge_list(lvl+2, e->vlnk[1]);
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
