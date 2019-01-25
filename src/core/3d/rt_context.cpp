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
        else if (linked_count(e, e->v[0]) != 0)
            return STATUS_CORRUPTED;
        if (!unlink_edge(e, e->v[1]))
            return STATUS_CORRUPTED;
        else if (linked_count(e, e->v[1]) != 0)
            return STATUS_CORRUPTED;

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
                nt->itag        = 1; // 1 modified edge

                // Update current triangle
              //ct->v[0]        = ct->v[0];
                ct->v[1]        = sp;
              //ct->v[2]        = ct->v[2];
              //ct->e[0]        = e;
                ct->e[1]        = se;
              //ct->e[2]        = ct->e[2];
              //ct->n           = ct->n;
                ct->itag        = ct->itag + 1; // Increment number of modified edges
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
                nt->itag        = 1; // 1 modified edge

                // Update current triangle
                ct->v[0]        = sp;
              //ct->v[1]        = ct->v[1];
              //ct->v[2]        = ct->v[2];
              //ct->e[0]        = e;
              //ct->e[1]        = ct->e[1];
                ct->e[2]        = se;
              //ct->n           = ct->n;
                ct->itag        = ct->itag + 1; // Increment number of modified edges
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
                 s=0   s=1   s=2   s=3   s=4   s=5   s=6   s=7   s=8   Normal
               | 0 1 | 0   | 0   |   1 |     |     |   1 |     |     |  ^
               |     |     |     |     |     |     |     |     |     |  |
             ==|=====|===1=|=====|=0===|=0=1=|=0===|=====|===1=|=====|==== <- Splitting plane
               |     |     |     |     |     |     |     |     |     |
               |     |     |   1 |     |     |   1 | 0   | 0   | 0 1 |
             */
            s               = e->v[0]->itag*3 + e->v[1]->itag;

            // Analyze ks[0] and ks[1]
            switch (s)
            {
                case 0: case 1: case 3: // edge is over the plane, skip
                    e->itag    |= RT_EF_PROCESSED;
                    break;
                case 5: case 7: case 8: // edge is under the plane, skip
                    e->itag    |= RT_EF_PROCESSED;
                    break;
                case 4: // edge lays on the plane, mark as split edge and skip
                    e->itag    |= RT_EF_PLANE | RT_EF_PROCESSED;
                    break;

                case 2: // edge is crossing the plane, v0 is over, v1 is under
                case 6: // edge is crossing the plane, v0 is under, v1 is over
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
        tx->ptag    = st->ptag;
        tx->itag    = st->itag;

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

    void rt_context_t::complete_fetch(rt_context_t *dst)
    {
        rt_edge_t *se, *ex;
        rt_triangle_t *tx;

        // Patch edge structures and link to vertexes
        for (size_t i=0, n=dst->edge.size(); i<n; ++i)
        {
            ex              = dst->edge.get(i);
            se              = reinterpret_cast<rt_edge_t *>(ex->ptag);

            // Patch vertex pointers
            ex->v[0]        = reinterpret_cast<rt_vertex_t *>(se->v[0]->ptag);
            ex->v[1]        = reinterpret_cast<rt_vertex_t *>(se->v[1]->ptag);

            // Link to verexes
            ex->vlnk[0]     = ex->v[0]->ve;
            ex->vlnk[1]     = ex->v[1]->ve;
            ex->v[0]->ve    = ex;
            ex->v[1]->ve    = ex;
        }

        // Link triangle structures to edges
        for (size_t i=0, n=dst->triangle.size(); i<n; ++i)
        {
            tx                  = dst->triangle.get(i);

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

    status_t rt_context_t::split(rt_context_t *out, rt_context_t *in, const vector3d_t *pl)
    {
        // Always clear target context before proceeding
        if (out != NULL)
            out->clear();
        if (in != NULL)
            in->clear();

        // Compute state of all vertexes
        /*
        The chart of 'itag' (s) state:
           s=0 s=1 s=2
          | o |   |   |
          |   |   |   |
        ==|===|=o=|===|== <- Splitting plane
          |   |   |   |
          |   |   | o |
        */
        for (size_t i=0, n=vertex.size(); i<n; ++i)
        {
            rt_vertex_t *v  = vertex.get(i);
            float t         = v->x*pl->dx + v->y*pl->dy + v->z*pl->dz + pl->dw;
            v->itag         = (t < -DSP_3D_TOLERANCE) ? 2 : (t > DSP_3D_TOLERANCE) ? 0 : 1;
        }

        // Reset all flags of edges
        for (size_t i=0, n=edge.size(); i<n; ++i)
            edge.get(i)->itag &= ~RT_EF_PROCESSED; // Clear processed flag

        // First step: split edges
        status_t res = split_edges(pl);
        if (res != STATUS_OK)
            return res;
        RT_TRACE(
            if (!validate())
                return STATUS_CORRUPTED;
        )

        // Toggle state of all triangles
        for (size_t i=0, n=triangle.size(); i<n; ++i)
        {
            rt_triangle_t  *st  = triangle.get(i);

            // Determine target context
            st->itag    = (st->v[0]->itag != 1) ? (st->v[0]->itag <= 1) :
                          (st->v[1]->itag != 1) ? (st->v[1]->itag <= 1) :
                          (st->v[2]->itag <= 1);
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

    status_t rt_context_t::partition(rt_context_t *out, rt_context_t *in)
    {
        vector3d_t pl;
        rt_triangle_t *ct, *st;

        // Always clear target context before proceeding
        if (out != NULL)
            out->clear();
        if (in != NULL)
            in->clear();

        ct = triangle.get(0);
        if (ct == NULL)
            return STATUS_OK;

        dsp::calc_oriented_plane_p3(&pl, &view.s, ct->v[0], ct->v[1], ct->v[2]);

        RT_TRACE_BREAK(this,
            lsp_trace("Partitioning space (%d triangles)", int(triangle.size()));

            for (size_t i=0,n=triangle.size(); i<n; ++i)
            {
                rt_triangle_t *t = triangle.get(i);
                shared->view->add_triangle_1c(t, (t == ct) ? &C_ORANGE : &C_YELLOW);
            }

            for (size_t i=0,n=edge.size(); i<n; ++i)
                shared->view->add_segment(edge.get(i), &C_GREEN);
            shared->view->add_plane_3pn1c(ct->v[0], ct->v[1], ct->v[2], &pl, &C_RED);
        );

        // Compute state of all vertexes
        /*
        The chart of 'itag' (s) state:
           s=0 s=1 s=2
          | o |   |   |
          |   |   |   |
        ==|===|=o=|===|== <- Splitting plane
          |   |   |   |
          |   |   | o |
        */
        for (size_t i=0, n=vertex.size(); i<n; ++i)
        {
            rt_vertex_t *v  = vertex.get(i);
            float t         = v->x*pl.dx + v->y*pl.dy + v->z*pl.dz + pl.dw;
            v->itag         = (t < -DSP_3D_TOLERANCE) ? 2 : (t > DSP_3D_TOLERANCE) ? 0 : 1;
        }
        ct->v[0]->itag      = 1;
        ct->v[1]->itag      = 1;
        ct->v[2]->itag      = 1;

        // First step: split edges
        status_t res = split_edges(&pl);
        if (res != STATUS_OK)
            return res;
        RT_TRACE(
            if (!validate())
                return STATUS_CORRUPTED;
        )

        // Toggle state of all triangles
        for (size_t i=0, n=triangle.size(); i<n; ++i)
        {
            st  = triangle.get(i);

            // Determine target context
            st->itag    = (st->v[0]->itag != 1) ? (st->v[0]->itag <= 1) :
                          (st->v[1]->itag != 1) ? (st->v[1]->itag <= 1) :
                          (st->v[2]->itag <= 1);
        }
        ct->itag    = 0;

        // Now we can fetch triangles
        res    = fetch_triangles_safe(in, 0);
        if (res != STATUS_OK)
            return res;

        res    = fetch_triangles_safe(out, 1);
        if (res != STATUS_OK)
            return res;

        RT_TRACE_BREAK(this,
            lsp_trace("Partitioned space, in=GREEN, out=RED");

            if (in != NULL)
                for (size_t i=0,n=in->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(in->triangle.get(i), &C_GREEN);

            if (out != NULL)
                for (size_t i=0,n=out->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(out->triangle.get(i), &C_RED);
        );

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

    status_t rt_context_t::add_object(Object3D *obj)
    {
        // Reset tags
        obj->scene()->init_tags(NULL, 0);
        if (!obj->scene()->validate())
            return STATUS_CORRUPTED;

        matrix3d_t *m   = obj->matrix();

//        lsp_trace("Processing object \"%s\"", obj->get_name());
        size_t start_t = triangle.size();
        size_t start_e = edge.size();

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
