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
        this->shared    = shared;
        this->index     = 0;
    }
    
    rt_context_t::~rt_context_t()
    {
        shared          = NULL;

        vertex.destroy();
        edge.destroy();
        triangle.destroy();
    }

    void rt_context_t::swap(rt_context_t *src)
    {
        // Do swap
        vertex.swap(&src->vertex);
        edge.swap(&src->edge);
        triangle.swap(&src->triangle);
    }

    void rt_context_t::unlink_edge(rt_edge_t *e, rt_vertex_t *v)
    {
        for (rt_edge_t **pcurr = &v->ve; *pcurr != NULL; )
        {
            rt_edge_t *curr = *pcurr;
            rt_edge_t **pnext = (curr->v[0] == v) ? &curr->vlnk[0] : &curr->vlnk[1];
            if (curr == e)
            {
                *pcurr = *pnext;
                return;
            }
            pcurr = pnext;
        }
    }

    void rt_context_t::unlink_triangle(rt_triangle_t *t, rt_edge_t *e)
    {
        for (rt_triangle_t **pcurr = &e->vt; *pcurr != NULL; )
        {
            rt_triangle_t *curr = *pcurr;
            rt_triangle_t **pnext = (curr->e[0] == e) ? &curr->elnk[0] :
                                    (curr->e[1] == e) ? &curr->elnk[1] :
                                    &curr->elnk[2];
            if (curr == t)
            {
                *pcurr = *pnext;
                return;
            }
            pcurr = pnext;
        }
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
        ne->v[1]        = ct->v[1];
        ne->vt          = NULL;
        ne->vlnk[0]     = NULL;
        ne->vlnk[1]     = NULL;
        ne->split[0]    = NULL;
        ne->split[1]    = NULL;
        ne->ptag        = NULL;
        ne->itag        = e->itag | RT_EF_SPLIT;

        ne->vlnk[0]     = ne->v[0]->ve;
        ne->vlnk[1]     = ne->v[1]->ve;
        ne->v[0]->ve    = ne;
        ne->v[1]->ve    = ne;

        if ((ne->v[0] == NULL) || (ne->v[1] == NULL))
            return STATUS_CORRUPTED;

        // Re-link current edge
        unlink_edge(e, e->v[0]);
        unlink_edge(e, e->v[1]);

        e->itag        |= RT_EF_SPLIT;

        // Process all triangles
        while (true)
        {
            // Save pointer to triangle to move forward
            pt              = ct->elnk[0];  // Save pointer to pending triangle, splitting edge is always rearranged to have index 0

            // Allocate triangle
            nt              = triangle.alloc();
            if (nt == NULL)
                return STATUS_NO_MEM;

            // Allocate splitting edge and link
            se              = edge.alloc();

            se->v[0]        = sp;
            se->v[1]        = ct->v[2];
            se->vt          = NULL;
            se->split[0]    = NULL;
            se->split[1]    = NULL;
            se->ptag        = NULL;
            se->itag        = 0;

            se->vlnk[0]     = se->v[0]->ve;
            se->vlnk[1]     = se->v[1]->ve;
            se->v[0]->ve    = se;
            se->v[1]->ve    = se;

            if ((se->v[0] == NULL) || (se->v[1] == NULL))
                return STATUS_CORRUPTED;

            unlink_triangle(ct, ct->e[0]);
            unlink_triangle(ct, ct->e[1]);
            unlink_triangle(ct, ct->e[2]);

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
                ct->e[2]        = ne;
              //ct->n           = ct->n;
                ct->itag        = ct->itag + 1; // Increment number of modified edges

                RT_TRACE_BREAK(this,
                    lsp_trace("Drawing new triangles");
                    shared->view->add_triangle_1c(ct, &C_CYAN);
                    shared->view->add_triangle_1c(nt, &C_MAGENTA);
                    shared->view->add_segment(e[0].v[0], sp, &C_RED);
                    shared->view->add_segment(ne, &C_GREEN);
                    shared->view->add_segment(se, &C_BLUE);
                );
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

            // Move to next triangle
            if ((ct = pt) == NULL)
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

            // Re-arrange next triangle and edges
            res             = arrange_triangle(ct, e);
            if (res != STATUS_OK)
                return res;
        }

        // Now the edge 'e' is stored in context but not linked to any primitive
        return STATUS_OK;
    }

    status_t rt_context_t::split_edges(const vector3d_t *pl)
    {
        float t;
        size_t s;
        vector3d_t d;
        rt_vertex_t *sp;

        // Try to split all edges (including new ones)
        for (size_t i=0; i< edge.size(); ++i)
        {
            rt_edge_t *e    = edge.get(i);

            RT_TRACE_BREAK(this,
                lsp_trace("Testing edge %d", int(i));
                for (size_t i=0,n=triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(triangle.get(i), &C_RED);
                for (size_t j=0,n=edge.size(); j<n; ++j)
                {
                    rt_edge_t *xe    = edge.get(j);
                    if (xe == e)
                        continue;
                    shared->view->add_segment(xe,
                            (xe->itag & RT_EF_SPLIT) ? &C_GREEN : &C_CYAN
                    );
                }
                shared->view->add_segment(e, &C_YELLOW);
            );

            if (e->itag & RT_EF_SPLIT) // Skip already splitted eges
                continue;

            /*
             State of each vertex:
                 0 if point is over the plane
                 1 if point is on the plane
                 2 if point is above the plane

             The chart of egge state 's':
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
                    break;
                case 5: case 7: case 8: // edge is under the plane, skip
                    break;
                case 4: // edge lays on the plane, mark as split edge and skip
                    e->itag     = RT_EF_SPLIT;
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
                    sp->split[0]= NULL;
                    sp->split[1]= NULL;
                    sp->ptag    = NULL;
                    sp->itag    = 1;

                    split_edge(e, sp);
                    break;

                default:
                    return STATUS_BAD_STATE;
            }
        }

        RT_TRACE_BREAK(this,
            lsp_trace("Triangle edges have been split, out=RED, in=GREEN");
            for (size_t i=0, n=triangle.size(); i<n; ++i)
            {
                rt_triangle_t *t = triangle.get(i);
                bool out         = (t->v[0]->itag != 1) ? (t->v[0]->itag <= 1) :
                                   (t->v[1]->itag != 1) ? (t->v[1]->itag <= 1) :
                                   (t->v[2]->itag <= 1);

                shared->view->add_triangle_1c(t, (out) ? &C_RED : &C_GREEN);
            }
        );

        return STATUS_OK;
    }

    status_t rt_context_t::split_triangles(rt_context_t *out, rt_context_t *in)
    {
        rt_context_t *dst;
        rt_triangle_t *st, *tx;
        rt_vertex_t *sv, *vx;
        rt_edge_t *se, *ex;
        ssize_t k;

        // Create data structures in out and in contexts
        for (size_t i=0, n=triangle.size(); i<n; ++i)
        {
            st          = triangle.get(i);
            bool xout   = (st->v[0]->itag != 1) ? (st->v[0]->itag <= 1) :
                          (st->v[1]->itag != 1) ? (st->v[1]->itag <= 1) :
                          (st->v[2]->itag <= 1);

            // Get destination to store
            dst     = (xout) ? out : in;
            if (dst == NULL)
                continue;
            k       = (xout) ? 1 : 0;

            // Allocate triangle in destination context
            tx = dst->triangle.alloc();
            if (tx == NULL)
                return STATUS_NO_MEM;

            tx->n               = st->n;
            tx->ptag            = st;
            tx->itag            = 0;

            st->ptag            = tx;

            // Process each element in triangle
            for (size_t j=0; j<3; ++j)
            {
                // Allocate vertex if required
                sv      = st->v[j];
                vx      = sv->split[k];
                if (vx == NULL)
                {
                    vx              = dst->vertex.alloc();
                    if (vx == NULL)
                        return STATUS_NO_MEM;

                    vx->x           = sv->x;
                    vx->y           = sv->y;
                    vx->z           = sv->z;
                    vx->w           = sv->w;
                    vx->ve          = NULL;
                    vx->ptag        = NULL;
                    vx->itag        = 0;
                    vx->split[0]    = NULL;
                    vx->split[1]    = NULL;

                    sv->split[k]    = vx;
                }

                // Allocate edge if required
                se      = st->e[j];
                ex      = se->split[k];
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
                    ex->split[0]    = NULL;
                    ex->split[1]    = NULL;
                    ex->ptag        = NULL;
                    ex->itag        = se->itag & ~RT_EF_TEMP;

                    se->split[k]    = ex;
                }

                // Store pointers
                tx->v[j]        = vx;
                tx->e[j]        = ex;
                tx->elnk[j]     = NULL;
            }
        } // for

        // Patch edge structures and link to vertexes
        for (size_t i=0, n=edge.size(); i<n; ++i)
        {
            se          = edge.get(i);

            for (size_t j=0; j<2; ++j)
            {
                ex          = se->split[j];
                if (ex == NULL)
                    continue;

                // Perform link
                ex->v[0]            = se->v[0]->split[j];
                if (ex->v[0] != NULL)
                {
                    ex->vlnk[0]         = ex->v[0]->ve;
                    ex->v[0]->ve        = ex;
                }

                ex->v[1]            = se->v[1]->split[j];
                if (ex->v[1] != NULL)
                {
                    ex->vlnk[1]         = ex->v[1]->ve;
                    ex->v[1]->ve        = ex;
                }
            }
        }

        // Link triangle structures to edges
        for (size_t i=0, n=triangle.size(); i<n; ++i)
        {
            st                  = triangle.get(i);
            tx                  = reinterpret_cast<rt_triangle_t *>(st->ptag);
            if (tx == NULL)
                continue;

            // Link triangle to the edge
            tx->elnk[0]         = tx->e[0]->vt;
            tx->elnk[1]         = tx->e[1]->vt;
            tx->elnk[2]         = tx->e[2]->vt;

            tx->e[0]->vt        = tx;
            tx->e[1]->vt        = tx;
            tx->e[2]->vt        = tx;
        }

        RT_TRACE_BREAK(this,
            lsp_trace("Split original context into in (GREEN) and out (RED)");
            if (out != NULL)
            {
                for (size_t i=0,n=out->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(out->triangle.get(i), &C_RED);
            }
            if (in != NULL)
            {
                for (size_t i=0,n=in->triangle.size(); i<n; ++i)
                    shared->view->add_triangle_1c(in->triangle.get(i), &C_GREEN);
            }
        );

        return STATUS_OK;
    }

    status_t rt_context_t::split(rt_context_t *out, rt_context_t *in, const vector3d_t *pl)
    {
        // Initialize state
        // Compute state of all vertexes
        for (size_t i=0, n=vertex.size(); i<n; ++i)
        {
            rt_vertex_t *v  = vertex.get(i);
            float t         = v->x*pl->dx + v->y*pl->dy + v->z*pl->dz + pl->dw;
            v->ptag         = NULL;
            v->itag         = (t < -DSP_3D_TOLERANCE) ? 2 : (t > DSP_3D_TOLERANCE) ? 0 : 1;
            v->split[0]     = NULL;
            v->split[1]     = NULL;

            /*
            The chart of 'itag' (s) state:
               s=0 s=1 s=2
              | o |   |   |
              |   |   |   |
            ==|===|=o=|===|== <- Splitting plane
              |   |   |   |
              |   |   | o |
            */
        }

        // Reset all flags of edges
        for (size_t i=0, n=edge.size(); i<n; ++i)
        {
            rt_edge_t *e    = edge.get(i);
            e->itag        &= ~RT_EF_SPLIT; // Clear split flag
            e->ptag         = NULL;
            e->split[0]     = NULL;
            e->split[1]     = NULL;
        }

        // Mark all triangles as non-modified
        for (size_t i=0, n=triangle.size(); i<n; ++i)
        {
            rt_triangle_t *t= triangle.get(i);
            t->ptag         = NULL;
            t->itag         = 0; // Number of modified edges
        }

        // First step: split edges
        status_t res = split_edges(pl);
        if (res != STATUS_OK)
            return res;

        // Now we can move triangles
        return split_triangles(out, in);
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
                    vx->split[0]    = NULL;
                    vx->split[1]    = NULL;

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
                    ex->split[0]    = NULL;
                    ex->split[1]    = NULL;
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
                if (!vertex.validate(e->v[j]))
                    return false;
                if (!edge.validate(e->vlnk[j]))
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
                if (!vertex.validate(t->v[j]))
                    return false;
                if (!edge.validate(t->e[j]))
                    return false;
                if (!triangle.validate(t->elnk[j]))
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

} /* namespace mtest */
