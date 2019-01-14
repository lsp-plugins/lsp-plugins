/*
 * reflections3d.cpp
 *
 *  Created on: 24 дек. 2018 г.
 *      Author: sadko
 */

#include <test/mtest.h>
#include <test/mtest/3d/common/X11Renderer.h>
#include <core/files/Model3DFile.h>

#include <core/types.h>
#include <core/debug.h>
#include <core/sugar.h>
#include <core/status.h>
#include <stdlib.h>
#include <errno.h>
#include <data/cstorage.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <sys/poll.h>

#if 0

//#define TEST_DEBUG

#ifndef TEST_DEBUG
    #define BREAKPOINT_STEP     -1

    #define INIT_FRONT(front) \
        dsp::init_point_xyz(&front.t.p[0], 0.0f, 1.0f, 0.0f); \
        dsp::init_point_xyz(&front.t.p[1], -1.0f, -0.5f, 0.0f); \
        dsp::init_point_xyz(&front.t.p[2], 1.0f, -0.5f, 0.0f); \
        dsp::init_point_xyz(&front.s, 0.0f, 0.0f, 1.0f);

/*
        dsp::init_point_xyz(&front.p[0], -0.980776, -0.195088, 0.000000); \
        dsp::init_point_xyz(&front.p[1], 0.685477, -0.883233, 0.000000); \
        dsp::init_point_xyz(&front.p[2], 0.295300, 1.078322, 0.000000); \
        dsp::init_point_xyz(&front.s, 0.000000, 0.000000, 1.000000);
*/

#else /* DEBUG */
    #define BREAKPOINT_STEP     0

    #define INIT_FRONT(front) \
        dsp::init_point_xyz(&front.s, 0.0f, 0.0f, -0.75f); \
        dsp::init_point_xyz(&front.p[0], 0.378978, -0.513494, -1.580061); \
        dsp::init_point_xyz(&front.p[1], 0.453656, -0.486792, -1.562277); \
        dsp::init_point_xyz(&front.p[2], 0.352607, -0.494265, -1.620808);
#endif /* DEBUG */

namespace mtest
{
    using namespace lsp;

    static const color3d_t C_RED        = { 1.0f, 0.0f, 0.0f, 0.0f };
    static const color3d_t C_GREEN      = { 0.0f, 1.0f, 0.0f, 0.0f };
    static const color3d_t C_BLUE       = { 0.0f, 0.0f, 1.0f, 0.0f };
    static const color3d_t C_CYAN       = { 0.0f, 1.0f, 1.0f, 0.0f };
    static const color3d_t C_MAGENTA    = { 1.0f, 0.0f, 1.0f, 0.0f };
    static const color3d_t C_YELLOW     = { 1.0f, 1.0f, 0.0f, 0.0f };
    static const color3d_t C_ORANGE     = { 1.0f, 0.5f, 0.0f, 0.0f };
    static const color3d_t C_GRAY       = { 0.75f, 0.75f, 0.75f, 0.0f };

#pragma pack(push, 1)
    typedef struct raw_triangle3d_t
    {
        point3d_t   p[3];
    } raw_triangle3d_t;

    typedef struct rt_triangle3d_t // Raytracing triangle
    {
        point3d_t           p[3];       // Triangle points
        vector3d_t          n;          // Normal
        float               e[3];       // Three edge flags: negative value means that this edge was not analyzed
        float               w;          // Weight (for sorting)
    } rt_triangle3d_t;
#pragma pack(pop)

    typedef struct wfront_t
    {
        raw_triangle3d_t t;     // Raw triangle view
        point3d_t s;            // Source point
    } wfront_t;

    typedef struct object_t
    {
        bound_box3d_t               box;      // Bounding box for each object
        size_t                      nt;     // Number of triangles
        rt_triangle3d_t            *t;      // List of triangles for each object
    } object_t;

    enum context_state_t
    {
        S_SCAN_SCENE,
        S_CULL_FRONT,
        S_CULL_BACK
    };

    typedef struct global_context_t
    {
        cstorage<rt_triangle3d_t>   matched;    // List of matched triangles (for debug)
        cstorage<rt_triangle3d_t>   traced;     // List of traced triangles (for debug)
        cstorage<rt_triangle3d_t>   ignored;    // List of ignored triangles (for debug)
        cvector<object_t>           scene;      // Overall scene
        ssize_t                     step;       // Trace step
        ssize_t                     breakpoint; // Trace breakpoint
        View3D                     *view;
    } global_context_t;

    typedef struct context_t
    {
        wfront_t                    front;      // Wave front
        cstorage<rt_triangle3d_t>   source;     // Triangles for processing
        context_state_t             state;      // Context state
        global_context_t           *global;     // Global state
    } context_t;

    static inline void dump_triangle(const char *label, const rt_triangle3d_t *t)
    {
        lsp_trace("%s: {\n"
                "\t(%f, %f, %f),\n"
                "\t(%f, %f, %f),\n"
                "\t(%f, %f, %f)}\n",
                label,
                t->p[0].x, t->p[0].y, t->p[0].z,
                t->p[1].x, t->p[1].y, t->p[1].z,
                t->p[2].x, t->p[2].y, t->p[2].z
           );
    }

    static inline void dump_triangle(const char *label, const v_triangle3d_t *t)
    {
        lsp_trace("%s: {\n"
                "\t(%f, %f, %f),\n"
                "\t(%f, %f, %f),\n"
                "\t(%f, %f, %f)}\n",
                label,
                t->p[0].x, t->p[0].y, t->p[0].z,
                t->p[1].x, t->p[1].y, t->p[1].z,
                t->p[2].x, t->p[2].y, t->p[2].z
           );
    }

    static inline void add_to_view(context_t *ctx, const rt_triangle3d_t *rt, const color3d_t *c)
    {
        ctx->global->view->add_triangle_pvnc1(rt->p, &rt->n, c);
    }

    static inline void add_to_view_3c(context_t *ctx, const rt_triangle3d_t *rt, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2)
    {
        ctx->global->view->add_triangle_pvnc3(rt->p, &rt->n, c0, c1, c2);
    }

    static inline void dump_context(const char *label, const context_t *ctx)
    {
        const wfront_t &f = ctx->front;
        const raw_triangle3d_t &t = f.t;

        lsp_trace("%s: src_size=%d, front=["
                "s={%f, %f, %f}, "
                "p=[{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}], "
                "pv=[{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}], "
                "init=[\n"
                "dsp::init_point_xyz(&front.p[0], %f, %f, %f); \\\n"
                "dsp::init_point_xyz(&front.p[1], %f, %f, %f); \\\n"
                "dsp::init_point_xyz(&front.p[2], %f, %f, %f); \\\n"
                "dsp::init_point_xyz(&front.s, %f, %f, %f);\n"
                "]",
                label, int(ctx->source.size()),
                f.s.x, f.s.y, f.s.z,

                t.p[0].x, t.p[0].y, t.p[0].z,
                t.p[1].x, t.p[1].y, t.p[1].z,
                t.p[2].x, t.p[2].y, t.p[2].z,

                t.p[1].x-t.p[0].x, t.p[1].y-t.p[0].y, t.p[1].z-t.p[0].z,
                t.p[2].x-t.p[1].x, t.p[2].y-t.p[1].y, t.p[2].z-t.p[1].z,
                t.p[0].x-t.p[2].x, t.p[0].y-t.p[2].y, t.p[0].z-t.p[2].z,


                t.p[0].x, t.p[0].y, t.p[0].z,
                t.p[1].x, t.p[1].y, t.p[1].z,
                t.p[2].x, t.p[2].y, t.p[2].z,
                f.s.x, f.s.y, f.s.z
                );
    }

    static void destroy_tasks(cvector<context_t> &tasks)
    {
        for (size_t i=0, n=tasks.size(); i<n; ++i)
        {
            context_t *ctx = tasks.get(i);
            if (ctx == NULL)
                continue;

            ctx->global     = NULL;
            ctx->source.flush();

            delete ctx;
        }

        tasks.flush();
    }

    static void flip_plane(vector3d_t *v)
    {
        v->dx   = - v->dx;
        v->dy   = - v->dy;
        v->dz   = - v->dz;
        v->dw   = - v->dw;
    }

//    static void init_triangle_pv(v_triangle3d_t *t, const point3d_t *pv)
//    {
//        t->p[0] = pv[0];
//        t->p[1] = pv[1];
//        t->p[2] = pv[2];
//        dsp::calc_normal3d_pv(&t->n[0], pv);
//        t->n[1] = t->n[0];
//        t->n[2] = t->n[0];
//    }

    static void init_triangle_p3(v_triangle3d_t *t, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2, const vector3d_t *n)
    {
        dsp::calc_normal3d_p3(&t->n[0], p0, p1, p2);
        float a = t->n[0].dx * n->dx + t->n[0].dy * n->dy + t->n[0].dz * n->dz;
        if (a < 0.0f)
        {
            t->p[0] = *p0;
            t->p[1] = *p2;
            t->p[2] = *p1;

            t->n[0].dx  = -t->n[0].dx;
            t->n[0].dy  = -t->n[0].dy;
            t->n[0].dz  = -t->n[0].dz;
        }
        else
        {
            t->p[0] = *p0;
            t->p[1] = *p1;
            t->p[2] = *p2;
        }

        t->n[1] = t->n[0];
        t->n[2] = t->n[0];
    }

    static void calc_plane_vector_p3(vector3d_t *v, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
    {
        // Calculate edge parameters
        vector3d_t d[2];
        d[0].dx     = p1->x - p0->x;
        d[0].dy     = p1->y - p0->y;
        d[0].dz     = p1->z - p0->z;
        d[0].dw     = p1->w - p0->w;

        d[1].dx     = p2->x - p1->x;
        d[1].dy     = p2->y - p1->y;
        d[1].dz     = p2->z - p1->z;
        d[1].dw     = p2->w - p1->w;

        // Do vector multiplication to calculate the normal vector
        v->dx       = d[0].dy*d[1].dz - d[0].dz*d[1].dy;
        v->dy       = d[0].dz*d[1].dx - d[0].dx*d[1].dz;
        v->dz       = d[0].dx*d[1].dy - d[0].dy*d[1].dx;
        v->dw       = 0.0f;

        dsp::normalize_vector(v);

        v->dw       = - ( v->dx * p0->x + v->dy * p0->y + v->dz * p0->z); // Parameter for the plane equation
    }

    static void calc_plane_vector_pv(vector3d_t *v, const point3d_t *p)
    {
        // Calculate edge parameters
        vector3d_t d[2];
        d[0].dx     = p[1].x - p[0].x;
        d[0].dy     = p[1].y - p[0].y;
        d[0].dz     = p[1].z - p[0].z;
        d[0].dw     = p[1].w - p[0].w;

        d[1].dx     = p[2].x - p[1].x;
        d[1].dy     = p[2].y - p[1].y;
        d[1].dz     = p[2].z - p[1].z;
        d[1].dw     = p[2].w - p[1].w;

        // Do vector multiplication to calculate the normal vector
        v->dx       = d[0].dy*d[1].dz - d[0].dz*d[1].dy;
        v->dy       = d[0].dz*d[1].dx - d[0].dx*d[1].dz;
        v->dz       = d[0].dx*d[1].dy - d[0].dy*d[1].dx;
        v->dw       = 0.0f;

        dsp::normalize_vector(v);

        v->dw       = - ( v->dx * p[0].x + v->dy * p[0].y + v->dz * p[0].z); // Parameter for the plane equation
    }

    static bool check_triangle(const rt_triangle3d_t *t)
    {
        vector3d_t d[3];
        d[0].dx     = t->p[1].x - t->p[0].x;
        d[0].dy     = t->p[1].y - t->p[0].y;
        d[0].dz     = t->p[1].z - t->p[0].z;

        d[1].dx     = t->p[2].x - t->p[0].x;
        d[1].dy     = t->p[2].y - t->p[0].y;
        d[1].dz     = t->p[2].z - t->p[0].z;

        d[2].dx     = d[0].dy*d[1].dz - d[0].dz*d[1].dy;
        d[2].dy     = d[0].dz*d[1].dx - d[0].dx*d[1].dz;
        d[2].dz     = d[0].dx*d[1].dy - d[0].dy*d[1].dx;

        float w     = d[2].dx*d[2].dx + d[2].dy*d[2].dy + d[2].dz*d[2].dz;

        return w > DSP_3D_TOLERANCE;
    }

//    static float check_triangle_left_order_pv(const vector3d_t *pov, const point3d_t *p)
//    {
//        vector3d_t d[3];
//        d[0].dx     = p[1].x - p[0].x;
//        d[0].dy     = p[1].y - p[0].y;
//        d[0].dz     = p[1].z - p[0].z;
//
//        d[1].dx     = p[2].x - p[0].x;
//        d[1].dy     = p[2].y - p[0].y;
//        d[1].dz     = p[2].z - p[0].z;
//
//        d[2].dx     = d[0].dy*d[1].dz - d[0].dz*d[1].dy;
//        d[2].dy     = d[0].dz*d[1].dx - d[0].dx*d[1].dz;
//        d[2].dz     = d[0].dx*d[1].dy - d[0].dy*d[1].dx;
//
//        return pov->dx * d[2].dx + pov->dy * d[2].dy + pov->dz * d[2].dz;
//    }

    /**
     * Split raw triangle with plane, generates output set of triangles into out (triangles above split plane)
     * and in (triangles below split plane). For every triangle, points 1 and 2 are the points that
     * lay on the split plane, the first triangle ALWAYS has 2 common points with plane (1 and 2)
     *
     * @param out array of vertexes above plane
     * @param n_out counter of vertexes above plane (multiple of 3), should be initialized
     * @param in array of vertexes below plane
     * @param n_in counter of vertexes below plane (multiple of 3), should be initialized
     * @param pl plane equation
     * @param pv triangle to perform the split
     */
    static void split_triangle_raw(
            raw_triangle3d_t *out,
            size_t *n_out,
            raw_triangle3d_t *in,
            size_t *n_in,
            const vector3d_t *pl,
            const raw_triangle3d_t *pv
        )
    {
        point3d_t sp[2];    // Split point
        vector3d_t d[2];    // Delta vector
        point3d_t p[3];     // Triangle sources
        float k[3];         // Co-location of points
        float t[2];

        in     += *n_in;
        out    += *n_out;

        p[0]    = pv->p[0];
        p[1]    = pv->p[1];
        p[2]    = pv->p[2];

        k[0]    = pl->dx*p[0].x + pl->dy*p[0].y + pl->dz*p[0].z + pl->dw;
        k[1]    = pl->dx*p[1].x + pl->dy*p[1].y + pl->dz*p[1].z + pl->dw;
        k[2]    = pl->dx*p[2].x + pl->dy*p[2].y + pl->dz*p[2].z + pl->dw;

        // Check that the whole triangle lies above the plane or below the plane
        if (k[0] < 0.0f)
        {
            if ((k[1] <= 0.0f) && (k[2] <= 0.0f))
            {
                in->p[0]        = p[0];
                in->p[1]        = p[1];
                in->p[2]        = p[2];
                ++*n_in;
                return;
            }
        }
        else if (k[0] > 0.0f)
        {
            if ((k[1] >= 0.0f) && (k[2] >= 0.0f))
            {
                out->p[0]       = p[0];
                out->p[1]       = p[1];
                out->p[2]       = p[2];
                ++*n_out;
                return;
            }
        }
        else // (k[0] == 0)
        {
            if ((k[1] >= 0.0f) && (k[2] >= 0.0f))
            {
                out->p[0]       = p[0];
                out->p[1]       = p[1];
                out->p[2]       = p[2];
                ++*n_out;
                return;
            }
            else if ((k[1] <= 0.0f) && (k[2] <= 0.0f))
            {
                in->p[0]        = p[0];
                in->p[1]        = p[1];
                in->p[2]        = p[2];
                ++*n_in;
                return;
            }
        }

        // There is an intersection with plane, we need to analyze it
        // Rotate triangle until vertex 0 is above the split plane
        while (k[0] <= 0.0f)
        {
            t[0]    = k[0];
            sp[0]   = p[0];

            k[0]    = k[1];
            p[0]    = p[1];
            k[1]    = k[2];
            p[1]    = p[2];
            k[2]    = t[0];
            p[2]    = sp[0];
        }

        // Now we have p[0] guaranteed to be above plane, analyze p[1] and p[2]
        if (k[1] < 0.0f) // k[1] < 0
        {
            d[0].dx = p[0].x - p[1].x;
            d[0].dy = p[0].y - p[1].y;
            d[0].dz = p[0].z - p[1].z;

            t[0]    = -k[0] / (pl->dx*d[0].dx + pl->dy*d[0].dy + pl->dz*d[0].dz);

            sp[0].x = p[0].x + d[0].dx * t[0];
            sp[0].y = p[0].y + d[0].dy * t[0];
            sp[0].z = p[0].z + d[0].dz * t[0];
            sp[0].w = 1.0f;

            if (k[2] < 0.0f) // (k[1] < 0) && (k[2] < 0)
            {
                d[1].dx = p[0].x - p[2].x;
                d[1].dy = p[0].y - p[2].y;
                d[1].dz = p[0].z - p[2].z;

                t[1]    = -k[0] / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);

                sp[1].x = p[0].x + d[1].dx * t[1];
                sp[1].y = p[0].y + d[1].dy * t[1];
                sp[1].z = p[0].z + d[1].dz * t[1];
                sp[1].w = 1.0f;

                // 1 triangle above plane, 2 below
                out->p[0]       = p[0];
                out->p[1]       = sp[0];
                out->p[2]       = sp[1];
                ++*n_out;
                ++out;

                in->p[0]        = p[1];
                in->p[1]        = sp[1];
                in->p[2]        = sp[0];
                ++*n_in;
                ++in;

                in->p[0]        = p[2];
                in->p[1]        = sp[1];
                in->p[2]        = p[1];
                ++*n_in;
            }
            else if (k[2] > 0.0f) // (k[1] < 0) && (k[2] > 0)
            {
                d[1].dx = p[2].x - p[1].x;
                d[1].dy = p[2].y - p[1].y;
                d[1].dz = p[2].z - p[1].z;

                t[1]    = -k[2] / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);

                sp[1].x = p[2].x + d[1].dx * t[1];
                sp[1].y = p[2].y + d[1].dy * t[1];
                sp[1].z = p[2].z + d[1].dz * t[1];
                sp[1].w = 1.0f;

                // 2 triangles above plane, 1 below
                out->p[0]       = p[2];
                out->p[1]       = sp[0];
                out->p[2]       = sp[1];
                ++*n_out;
                ++out;

                out->p[0]       = p[0];
                out->p[1]       = sp[0];
                out->p[2]       = p[2];
                ++*n_out;

                in->p[0]        = p[1];
                in->p[1]        = sp[1];
                in->p[2]        = sp[0];
                ++*n_in;
            }
            else // (k[1] < 0) && (k[2] == 0)
            {
                // 1 triangle above plane, 1 below
                out->p[0]       = p[0];
                out->p[1]       = sp[0];
                out->p[2]       = p[2];
                ++*n_out;

                in->p[0]        = p[1];
                in->p[1]        = p[2];
                in->p[2]        = sp[0];
                ++*n_in;
            }
        }
        else // (k[1] >= 0) && (k[2] < 0)
        {
            d[0].dx = p[0].x - p[2].x;
            d[0].dy = p[0].y - p[2].y;
            d[0].dz = p[0].z - p[2].z;

            t[0]    = -k[0] / (pl->dx*d[0].dx + pl->dy*d[0].dy + pl->dz*d[0].dz);

            sp[0].x = p[0].x + d[0].dx * t[0];
            sp[0].y = p[0].y + d[0].dy * t[0];
            sp[0].z = p[0].z + d[0].dz * t[0];
            sp[0].w = 1.0f;

            if (k[1] > 0.0f) // (k[1] > 0) && (k[2] < 0)
            {
                d[1].dx = p[1].x - p[2].x;
                d[1].dy = p[1].y - p[2].y;
                d[1].dz = p[1].z - p[2].z;

                t[1]    = -k[1] / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);

                sp[1].x = p[1].x + d[1].dx * t[1];
                sp[1].y = p[1].y + d[1].dy * t[1];
                sp[1].z = p[1].z + d[1].dz * t[1];
                sp[1].w = 1.0f;

                // 2 triangles above plane, 1 below
                out->p[0]       = p[0];
                out->p[1]       = sp[1];
                out->p[2]       = sp[0];
                ++*n_out;
                ++out;

                out->p[0]       = p[1];
                out->p[1]       = sp[1];
                out->p[2]       = p[0];
                ++*n_out;

                in->p[0]        = p[2];
                in->p[1]        = sp[0];
                in->p[2]        = sp[1];
                ++*n_in;
            }
            else // (k[1] == 0) && (k[2] < 0)
            {
                // 1 triangle above plane, 1 triangle below plane
                out->p[0]       = p[0];
                out->p[1]       = p[1];
                out->p[2]       = sp[0];
                ++*n_out;

                in->p[0]        = p[2];
                in->p[1]        = sp[0];
                in->p[2]        = p[1];
                ++*n_in;
            }
        }
    }

    /**
     * Split raytracing triangle with plane, generates output set of triangles into out (triangles above split plane)
     * and in (triangles below split plane). For every triangle, points 1 and 2 are the points that
     * lay on the split plane, the first triangle ALWAYS has 2 common points with plane (1 and 2)
     *
     * Edges left from original triangle inherit their flags from original triangle,
     * New edges that lay on the split plane are marked with positive values (+1),
     * New edges that do not lay on the split plane, are marked with negative values (-1)
     *
     * @param out array of vertexes above plane
     * @param n_out counter of vertexes above plane (multiple of 3), should be initialized
     * @param in array of vertexes below plane
     * @param n_in counter of vertexes below plane (multiple of 3), should be iniitialized
     * @param pl plane equation
     * @param pv triangle to perform the split
     */
    static void split_triangle_rt(
            rt_triangle3d_t *out,
            size_t *n_out,
            rt_triangle3d_t *in,
            size_t *n_in,
            const vector3d_t *pl,
            const rt_triangle3d_t *pv
        )
    {
        point3d_t   sp[2];      // Split points
        vector3d_t  d[2];       // Delta vector
        point3d_t   p[3];       // Triangle
        float       e[3], w;    // Edge flags
        vector3d_t  n;          // Normal
        float       k[3];       // Co-location of points
        float       t[2];

        // Copy data
        p[0]    = pv->p[0];
        p[1]    = pv->p[1];
        p[2]    = pv->p[2];
        e[0]    = pv->e[0];
        e[1]    = pv->e[1];
        e[2]    = pv->e[2];
        w       = pv->w;
        n       = pv->n;

        // Update pointers
        in     += *n_in;
        out    += *n_out;

        // Estimate all point positions
        k[0]    = pl->dx*p[0].x + pl->dy*p[0].y + pl->dz*p[0].z + pl->dw;
        k[1]    = pl->dx*p[1].x + pl->dy*p[1].y + pl->dz*p[1].z + pl->dw;
        k[2]    = pl->dx*p[2].x + pl->dy*p[2].y + pl->dz*p[2].z + pl->dw;

        // Patch location of points relative to the split plane
        if ((k[0] >= -DSP_3D_TOLERANCE) && (k[0] <= DSP_3D_TOLERANCE))
            k[0]    = 0.0f;
        if ((k[1] >= -DSP_3D_TOLERANCE) && (k[1] <= DSP_3D_TOLERANCE))
            k[1]    = 0.0f;
        if ((k[2] >= -DSP_3D_TOLERANCE) && (k[2] <= DSP_3D_TOLERANCE))
            k[2]    = 0.0f;

        // Check that the whole triangle lies above the plane or below the plane
        if (k[0] < 0.0f)
        {
            if ((k[1] <= 0.0f) && (k[2] <= 0.0f))
            {
                *in             = *pv;
                ++*n_in;
                return;
            }
        }
        else if (k[0] > 0.0f)
        {
            if ((k[1] >= 0.0f) && (k[2] >= 0.0f))
            {
                *out            = *pv;
                ++*n_out;
                return;
            }
        }
        else // (k[0] == 0)
        {
            if ((k[1] >= 0.0f) && (k[2] >= 0.0f))
            {
                *out            = *pv;
                ++*n_out;
                return;
            }
            else if ((k[1] <= 0.0f) && (k[2] <= 0.0f))
            {
                *in             = *pv;
                ++*n_in;
                return;
            }
        }

        // There is an intersection with plane, we need to analyze it
        // Rotate triangle until vertex 0 is above the split plane
        while (k[0] <= 0.0f)
        {
            // Rotate points
            sp[0]   = p[0];
            p[0]    = p[1];
            p[1]    = p[2];
            p[2]    = sp[0];

            // Rotate edge flags
            t[0]    = e[0];
            e[0]    = e[1];
            e[1]    = e[2];
            e[2]    = t[0];

            // Rotate point locations
            t[0]    = k[0];
            k[0]    = k[1];
            k[1]    = k[2];
            k[2]    = t[0];
        }

        // Now we have p[0] guaranteed to be above plane, analyze p[1] and p[2]
        if (k[1] < 0.0f) // k[1] < 0
        {
            d[0].dx = p[0].x - p[1].x;
            d[0].dy = p[0].y - p[1].y;
            d[0].dz = p[0].z - p[1].z;

            t[0]    = -k[0] / (pl->dx*d[0].dx + pl->dy*d[0].dy + pl->dz*d[0].dz);

            sp[0].x = p[0].x + d[0].dx * t[0];
            sp[0].y = p[0].y + d[0].dy * t[0];
            sp[0].z = p[0].z + d[0].dz * t[0];
            sp[0].w = 1.0f;

            if (k[2] < 0.0f) // (k[1] < 0) && (k[2] < 0)
            {
                d[1].dx = p[0].x - p[2].x;
                d[1].dy = p[0].y - p[2].y;
                d[1].dz = p[0].z - p[2].z;

                t[1]    = -k[0] / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);

                sp[1].x = p[0].x + d[1].dx * t[1];
                sp[1].y = p[0].y + d[1].dy * t[1];
                sp[1].z = p[0].z + d[1].dz * t[1];
                sp[1].w = 1.0f;

                // 1 triangle above plane, 2 below
                out->p[0]       = p[0];
                out->p[1]       = sp[0];
                out->p[2]       = sp[1];
                out->n          = n;
                out->e[0]       = e[0];
                out->e[1]       = 1.0f;
                out->e[2]       = e[2];
                out->w          = w;
                if (check_triangle(out))
                {
                    ++*n_out;
                    ++out;
                }

                in->p[0]        = p[1];
                in->p[1]        = sp[1];
                in->p[2]        = sp[0];
                in->n           = n;
                in->e[0]        = -1.0f;
                in->e[1]        = 1.0f;
                in->e[2]        = e[0];
                in->w           = w;
                if (check_triangle(in))
                {
                    ++*n_in;
                    ++in;
                }

                in->p[0]        = p[2];
                in->p[1]        = sp[1];
                in->p[2]        = p[1];
                in->n           = n;
                in->e[0]        = e[2];
                in->e[1]        = -1.0f;
                in->e[2]        = e[1];
                in->w           = w;
                if (check_triangle(in))
                    ++*n_in;
            }
            else if (k[2] > 0.0f) // (k[1] < 0) && (k[2] > 0)
            {
                d[1].dx = p[2].x - p[1].x;
                d[1].dy = p[2].y - p[1].y;
                d[1].dz = p[2].z - p[1].z;

                t[1]    = -k[2] / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);

                sp[1].x = p[2].x + d[1].dx * t[1];
                sp[1].y = p[2].y + d[1].dy * t[1];
                sp[1].z = p[2].z + d[1].dz * t[1];
                sp[1].w = 1.0f;

                // 2 triangles above plane, 1 below
                out->p[0]       = p[2];
                out->p[1]       = sp[0];
                out->p[2]       = sp[1];
                out->n          = n;
                out->e[0]       = -1.0f;
                out->e[1]       = 1.0f;
                out->e[2]       = e[1];
                out->w          = w;
                if (check_triangle(out))
                {
                    ++*n_out;
                    ++out;
                }

                out->p[0]       = p[0];
                out->p[1]       = sp[0];
                out->p[2]       = p[2];
                out->n          = n;
                out->e[0]       = e[0];
                out->e[1]       = -1.0f;
                out->e[2]       = e[2];
                out->w          = w;
                if (check_triangle(out))
                    ++*n_out;

                in->p[0]        = p[1];
                in->p[1]        = sp[1];
                in->p[2]        = sp[0];
                in->n           = n;
                in->e[0]        = e[1];
                in->e[1]        = 1.0f;
                in->e[2]        = e[0];
                in->w           = w;
                if (check_triangle(in))
                    ++*n_in;
            }
            else // (k[1] < 0) && (k[2] == 0)
            {
                // 1 triangle above plane, 1 below
                out->p[0]       = p[0];
                out->p[1]       = sp[0];
                out->p[2]       = p[2];
                out->n          = n;
                out->e[0]       = e[0];
                out->e[1]       = 1.0f;
                out->e[2]       = e[2];
                out->w          = w;
                if (check_triangle(out))
                    ++*n_out;

                in->p[0]        = p[1];
                in->p[1]        = p[2];
                in->p[2]        = sp[0];
                in->n           = n;
                in->e[0]        = e[1];
                in->e[1]        = 1.0f;
                in->e[2]        = e[0];
                in->w           = w;
                if (check_triangle(in))
                    ++*n_in;
            }
        }
        else // (k[1] >= 0) && (k[2] < 0)
        {
            d[0].dx = p[0].x - p[2].x;
            d[0].dy = p[0].y - p[2].y;
            d[0].dz = p[0].z - p[2].z;

            t[0]    = -k[0] / (pl->dx*d[0].dx + pl->dy*d[0].dy + pl->dz*d[0].dz);

            sp[0].x = p[0].x + d[0].dx * t[0];
            sp[0].y = p[0].y + d[0].dy * t[0];
            sp[0].z = p[0].z + d[0].dz * t[0];
            sp[0].w = 1.0f;

            if (k[1] > 0.0f) // (k[1] > 0) && (k[2] < 0)
            {
                d[1].dx = p[1].x - p[2].x;
                d[1].dy = p[1].y - p[2].y;
                d[1].dz = p[1].z - p[2].z;

                t[1]    = -k[1] / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);

                sp[1].x = p[1].x + d[1].dx * t[1];
                sp[1].y = p[1].y + d[1].dy * t[1];
                sp[1].z = p[1].z + d[1].dz * t[1];
                sp[1].w = 1.0f;

                // 2 triangles above plane, 1 below
                out->p[0]       = p[0];
                out->p[1]       = sp[1];
                out->p[2]       = sp[0];
                out->n          = n;
                out->e[0]       = -1.0f;
                out->e[1]       = 1.0f;
                out->e[2]       = e[2];
                out->w          = w;
                if (check_triangle(out))
                {
                    ++*n_out;
                    ++out;
                }

                out->p[0]       = p[1];
                out->p[1]       = sp[1];
                out->p[2]       = p[0];
                out->n          = n;
                out->e[0]       = e[1];
                out->e[1]       = -1.0f;
                out->e[2]       = e[0];
                out->w          = w;
                if (check_triangle(out))
                    ++*n_out;

                in->p[0]        = p[2];
                in->p[1]        = sp[0];
                in->p[2]        = sp[1];
                in->n           = n;
                in->e[0]        = e[2];
                in->e[1]        = 1.0f;
                in->e[2]        = e[1];
                in->w           = w;
                if (check_triangle(in))
                    ++*n_in;
            }
            else // (k[1] == 0) && (k[2] < 0)
            {
                // 1 triangle above plane, 1 triangle below plane
                out->p[0]       = p[0];
                out->p[1]       = p[1];
                out->p[2]       = sp[0];
                out->n          = n;
                out->e[0]       = e[0];
                out->e[1]       = 1.0f;
                out->e[2]       = e[2];
                out->w          = w;
                if (check_triangle(out))
                    ++*n_out;

                in->p[0]        = p[2];
                in->p[1]        = sp[0];
                in->p[2]        = p[1];
                in->n           = n;
                in->e[0]        = e[2];
                in->e[1]        = 1.0f;
                in->e[2]        = e[1];
                in->w           = w;
                if (check_triangle(in))
                    ++*n_in;
            }
        }
    }

    /**
     * Split triangle with plane, generates output set of triangles into out (triangles above split plane)
     * and in (triangles below split plane). For every triangle, points 1 and 2 are the points that
     * lay on the split plane, the first triangle ALWAYS has 2 common points with plane (1 and 2)
     *
     * The relaxed mode enables triangle bounds checking to prevent small triangles from being
     * infinitely splitted
     *
     * @param out array of vertexes above plane
     * @param n_out counter of vertexes above plane (multiple of 3), should be initialized
     * @param in array of vertexes below plane
     * @param n_in counter of vertexes below plane (multiple of 3), should be iniitialized
     * @param pl plane equation
     * @param pv triangle to perform the split
     */
/*    static void split_triangle_relaxed(
            v_triangle3d_t *out,
            size_t *n_out,
            v_triangle3d_t *in,
            size_t *n_in,
            const vector3d_t *pl,
            const v_triangle3d_t *pv
        )
    {
        point3d_t sp[2];    // Split point
        vector3d_t d[2];    // Delta vector
        point3d_t p[3];     // Triangle sources
        float k[3];         // Co-location of points
        float t[2];

        in     += *n_in;
        out    += *n_out;

        p[0]    = pv->p[0];
        p[1]    = pv->p[1];
        p[2]    = pv->p[2];

        k[0]    = pl->dx*p[0].x + pl->dy*p[0].y + pl->dz*p[0].z + pl->dw;
        k[1]    = pl->dx*p[1].x + pl->dy*p[1].y + pl->dz*p[1].z + pl->dw;
        k[2]    = pl->dx*p[2].x + pl->dy*p[2].y + pl->dz*p[2].z + pl->dw;

        // Patch location of points relative to the split plane
        if ((k[0] >= -DSP_3D_TOLERANCE) && (k[0] <= DSP_3D_TOLERANCE))
            k[0]    = 0.0f;
        if ((k[1] >= -DSP_3D_TOLERANCE) && (k[1] <= DSP_3D_TOLERANCE))
            k[1]    = 0.0f;
        if ((k[2] >= -DSP_3D_TOLERANCE) && (k[2] <= DSP_3D_TOLERANCE))
            k[2]    = 0.0f;

        // Check that the whole triangle lies above the plane or below the plane
        if (k[0] < 0.0f)
        {
            if ((k[1] <= 0.0f) && (k[2] <= 0.0f))
            {
                in->p[0]        = p[0];
                in->p[1]        = p[1];
                in->p[2]        = p[2];
                in->n[0]        = pv->n[0];
                in->n[1]        = pv->n[1];
                in->n[2]        = pv->n[2];
                ++*n_in;
                return;
            }
        }
        else if (k[0] > 0.0f)
        {
            if ((k[1] >= 0.0f) && (k[2] >= 0.0f))
            {
                out->p[0]       = p[0];
                out->p[1]       = p[1];
                out->p[2]       = p[2];
                out->n[0]       = pv->n[0];
                out->n[1]       = pv->n[1];
                out->n[2]       = pv->n[2];
                ++*n_out;
                return;
            }
        }
        else // (k[0] == 0)
        {
            if ((k[1] >= 0.0f) && (k[2] >= 0.0f))
            {
                out->p[0]       = p[0];
                out->p[1]       = p[1];
                out->p[2]       = p[2];
                out->n[0]       = pv->n[0];
                out->n[1]       = pv->n[1];
                out->n[2]       = pv->n[2];
                ++*n_out;
                return;
            }
            else if ((k[1] <= 0.0f) && (k[2] <= 0.0f))
            {
                in->p[0]        = p[0];
                in->p[1]        = p[1];
                in->p[2]        = p[2];
                in->n[0]        = pv->n[0];
                in->n[1]        = pv->n[1];
                in->n[2]        = pv->n[2];
                ++*n_in;
                return;
            }
        }

        // There is an intersection with plane, we need to analyze it
        // Rotate triangle until vertex 0 is above the split plane
        while (k[0] <= 0.0f)
        {
            t[0]    = k[0];
            sp[0]   = p[0];

            k[0]    = k[1];
            p[0]    = p[1];
            k[1]    = k[2];
            p[1]    = p[2];
            k[2]    = t[0];
            p[2]    = sp[0];
        }

//        lsp_trace("split triangle: {\n"
//                "\t(%f, %f, %f)\n"
//                "\t(%f, %f, %f)\n"
//                "\t(%f, %f, %f)\n"
//                "}:",
//                p[0].x, p[0].y, p[0].z,
//                p[1].x, p[1].y, p[1].z,
//                p[2].x, p[2].y, p[2].z
//            );

        // Now we have p[0] guaranteed to be above plane, analyze p[1] and p[2]
        if (k[1] < 0.0f) // k[1] < 0
        {
            d[0].dx = p[0].x - p[1].x;
            d[0].dy = p[0].y - p[1].y;
            d[0].dz = p[0].z - p[1].z;

            t[0]    = -k[0] / (pl->dx*d[0].dx + pl->dy*d[0].dy + pl->dz*d[0].dz);

            sp[0].x = p[0].x + d[0].dx * t[0];
            sp[0].y = p[0].y + d[0].dy * t[0];
            sp[0].z = p[0].z + d[0].dz * t[0];
            sp[0].w = 1.0f;

//            lsp_trace("sp[0]: (%f, %f, %f)",
//                    sp[0].x, sp[0].y, sp[0].z
//                );

            if (k[2] < 0.0f) // (k[1] < 0) && (k[2] < 0)
            {
                d[1].dx = p[0].x - p[2].x;
                d[1].dy = p[0].y - p[2].y;
                d[1].dz = p[0].z - p[2].z;

                t[1]    = -k[0] / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);

                sp[1].x = p[0].x + d[1].dx * t[1];
                sp[1].y = p[0].y + d[1].dy * t[1];
                sp[1].z = p[0].z + d[1].dz * t[1];
                sp[1].w = 1.0f;

//                lsp_trace("sp[1]: (%f, %f, %f)",
//                        sp[1].x, sp[1].y, sp[1].z
//                    );

                // 1 triangle above plane, 2 below
                out->p[0]       = p[0];
                out->p[1]       = sp[0];
                out->p[2]       = sp[1];
                out->n[0]       = pv->n[0];
                out->n[1]       = pv->n[1];
                out->n[2]       = pv->n[2];

                if (check_triangle(out))
                {
                    ++*n_out;
                    ++out;
                }

                in->p[0]        = p[1];
                in->p[1]        = sp[1];
                in->p[2]        = sp[0];
                in->n[0]        = pv->n[0];
                in->n[1]        = pv->n[1];
                in->n[2]        = pv->n[2];

                if (check_triangle(in))
                {
                    ++*n_in;
                    ++in;
                }

                in->p[0]        = p[2];
                in->p[1]        = sp[1];
                in->p[2]        = p[1];
                in->n[0]        = pv->n[0];
                in->n[1]        = pv->n[1];
                in->n[2]        = pv->n[2];

                if (check_triangle(in))
                    ++*n_in;
            }
            else if (k[2] > 0.0f) // (k[1] < 0) && (k[2] > 0)
            {
                d[1].dx = p[2].x - p[1].x;
                d[1].dy = p[2].y - p[1].y;
                d[1].dz = p[2].z - p[1].z;

                t[1]    = -k[2] / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);

                sp[1].x = p[2].x + d[1].dx * t[1];
                sp[1].y = p[2].y + d[1].dy * t[1];
                sp[1].z = p[2].z + d[1].dz * t[1];
                sp[1].w = 1.0f;

//                lsp_trace("sp[1]: (%f, %f, %f)",
//                        sp[1].x, sp[1].y, sp[1].z
//                    );

                // 2 triangles above plane, 1 below
                out->p[0]       = p[2];
                out->p[1]       = sp[0];
                out->p[2]       = sp[1];
                out->n[0]       = pv->n[0];
                out->n[1]       = pv->n[1];
                out->n[2]       = pv->n[2];

                if (check_triangle(out))
                {
                    ++*n_out;
                    ++out;
                }

                out->p[0]       = p[0];
                out->p[1]       = sp[0];
                out->p[2]       = p[2];
                out->n[0]       = pv->n[0];
                out->n[1]       = pv->n[1];
                out->n[2]       = pv->n[2];

                if (check_triangle(out))
                    ++*n_out;

                in->p[0]        = p[1];
                in->p[1]        = sp[1];
                in->p[2]        = sp[0];
                in->n[0]        = pv->n[0];
                in->n[1]        = pv->n[1];
                in->n[2]        = pv->n[2];

                if (check_triangle(in))
                    ++*n_in;
            }
            else // (k[1] < 0) && (k[2] == 0)
            {
                // 1 triangle above plane, 1 below
                out->p[0]       = p[0];
                out->p[1]       = sp[0];
                out->p[2]       = p[2];
                out->n[0]       = pv->n[0];
                out->n[1]       = pv->n[1];
                out->n[2]       = pv->n[2];

                if (check_triangle(out))
                    ++*n_out;

                in->p[0]        = p[1];
                in->p[1]        = p[2];
                in->p[2]        = sp[0];
                in->n[0]        = pv->n[0];
                in->n[1]        = pv->n[1];
                in->n[2]        = pv->n[2];

                if (check_triangle(in))
                    ++*n_in;
            }
        }
        else // (k[1] >= 0) && (k[2] < 0)
        {
            d[0].dx = p[0].x - p[2].x;
            d[0].dy = p[0].y - p[2].y;
            d[0].dz = p[0].z - p[2].z;

            t[0]    = -k[0] / (pl->dx*d[0].dx + pl->dy*d[0].dy + pl->dz*d[0].dz);

            sp[0].x = p[0].x + d[0].dx * t[0];
            sp[0].y = p[0].y + d[0].dy * t[0];
            sp[0].z = p[0].z + d[0].dz * t[0];
            sp[0].w = 1.0f;

//            lsp_trace("sp[0]: (%f, %f, %f)",
//                    sp[0].x, sp[0].y, sp[0].z
//                );

            if (k[1] > 0.0f) // (k[1] > 0) && (k[2] < 0)
            {
                d[1].dx = p[1].x - p[2].x;
                d[1].dy = p[1].y - p[2].y;
                d[1].dz = p[1].z - p[2].z;

                t[1]    = -k[1] / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);

                sp[1].x = p[1].x + d[1].dx * t[1];
                sp[1].y = p[1].y + d[1].dy * t[1];
                sp[1].z = p[1].z + d[1].dz * t[1];
                sp[1].w = 1.0f;

//                lsp_trace("sp[1]: (%f, %f, %f)",
//                        sp[1].x, sp[1].y, sp[1].z
//                    );

                // 2 triangles above plane, 1 below
                out->p[0]       = p[0];
                out->p[1]       = sp[1];
                out->p[2]       = sp[0];
                out->n[0]       = pv->n[0];
                out->n[1]       = pv->n[1];
                out->n[2]       = pv->n[2];

                if (check_triangle(out))
                {
                    ++*n_out;
                    ++out;
                }

                out->p[0]       = p[1];
                out->p[1]       = sp[1];
                out->p[2]       = p[0];
                out->n[0]       = pv->n[0];
                out->n[1]       = pv->n[1];
                out->n[2]       = pv->n[2];

                if (check_triangle(out))
                    ++*n_out;

                in->p[0]        = p[2];
                in->p[1]        = sp[0];
                in->p[2]        = sp[1];
                in->n[0]        = pv->n[0];
                in->n[1]        = pv->n[1];
                in->n[2]        = pv->n[2];

                if (check_triangle(in))
                    ++*n_in;
            }
            else // (k[1] == 0) && (k[2] < 0)
            {
                // 1 triangle above plane, 1 triangle below plane
                out->p[0]       = p[0];
                out->p[1]       = p[1];
                out->p[2]       = sp[0];
                out->n[0]       = pv->n[0];
                out->n[1]       = pv->n[1];
                out->n[2]       = pv->n[2];

                if (check_triangle(out))
                    ++*n_out;

                in->p[0]        = p[2];
                in->p[1]        = sp[0];
                in->p[2]        = p[1];
                in->n[0]        = pv->n[0];
                in->n[1]        = pv->n[1];
                in->n[2]        = pv->n[2];

                if (check_triangle(in))
                    ++*n_in;
            }
        }
    }
*/

#if 0
    /**
     * Project triangle to the plane
     * @param pv array of 3 points to store projected points
     * @param fp focus point, the point where all projective lines do intersect
     * @param pl plane equation vector
     * @param tv triangle points
     */
    static void project_triangle(
        point3d_t *pv,
        const point3d_t *fp,
        const vector3d_t *pl,
        const point3d_t *tv
    )
    {
        vector3d_t d[3];
        float k, t[3];

        d[0].dx     = fp->x - tv[0].x;
        d[0].dy     = fp->y - tv[0].y;
        d[0].dz     = fp->z - tv[0].z;
        d[0].dw     = 0.0f;

        d[1].dx     = fp->x - tv[1].x;
        d[1].dy     = fp->y - tv[1].y;
        d[1].dz     = fp->z - tv[1].z;
        d[1].dw     = 0.0f;

        d[2].dx     = fp->x - tv[2].x;
        d[2].dy     = fp->y - tv[2].y;
        d[2].dz     = fp->z - tv[2].z;
        d[2].dw     = 0.0f;

        k           = - (pl->dx*fp->x + pl->dy*fp->y + pl->dz*fp->z + pl->dw);
        t[0]        = k / (pl->dx*d[0].dx + pl->dy*d[0].dy + pl->dz*d[0].dz);
        t[1]        = k / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);
        t[2]        = k / (pl->dx*d[2].dx + pl->dy*d[2].dy + pl->dz*d[2].dz);

        pv[0].x     = fp->x + t[0] * d[0].dx;
        pv[0].y     = fp->y + t[0] * d[0].dy;
        pv[0].z     = fp->z + t[0] * d[0].dz;
        pv[0].w     = 1.0f;

        pv[1].x     = fp->x + t[1] * d[1].dx;
        pv[1].y     = fp->y + t[1] * d[1].dy;
        pv[1].z     = fp->z + t[1] * d[1].dz;
        pv[1].w     = 1.0f;

        pv[2].x     = fp->x + t[2] * d[2].dx;
        pv[2].y     = fp->y + t[2] * d[2].dy;
        pv[2].z     = fp->z + t[2] * d[2].dz;
        pv[2].w     = 1.0f;
    }
#endif

    static const size_t bbox_map[] =
    {
        0, 1, 2,
        0, 2, 3,
        6, 5, 4,
        6, 4, 7,
        1, 0, 4,
        1, 4, 5,
        3, 2, 6,
        3, 6, 7,
        1, 5, 2,
        2, 5, 6,
        0, 3, 4,
        3, 7, 4
    };

    static void destroy_scene(cvector<object_t> &list)
    {
        for (size_t i=0, n=list.size(); i<n; ++i)
        {
            object_t *obj = list.get(i);
            if (obj != NULL)
                free(obj);
        }
        list.flush();
    }

    static status_t prepare_scene(cvector<object_t> &scene, Scene3D *s)
    {
        status_t res        = STATUS_OK;
        size_t hsize        = ALIGN_SIZE(sizeof(object_t), DEFAULT_ALIGN);

        cvector<object_t> ol;

        for (size_t i=0, n=s->num_objects(); i<n; ++i)
        {
            // Get scene object
            Object3D *obj   = s->get_object(i);
            if ((obj == NULL) || (!obj->is_visible()))
                continue;

            // Add bounding box
            bound_box3d_t *bbox = s->get_bound_box(i);
            if (bbox == NULL)
                continue;

            // Initialize pointers
            matrix3d_t *om      = obj->get_matrix();
            point3d_t *tr       = obj->get_vertexes();
            vector3d_t *tn      = obj->get_normals();
            vertex_index_t *vvx = obj->get_vertex_indexes();
            vertex_index_t *vnx = obj->get_normal_indexes();

            // Allocate object descriptor
            size_t nt           = obj->get_triangles_count();
            size_t osize        = hsize + ALIGN_SIZE(sizeof(rt_triangle3d_t) * nt, DEFAULT_ALIGN);
            object_t *o         = reinterpret_cast<object_t *>(malloc(osize));
            if (o == NULL)
            {
                res             = STATUS_NO_MEM;
                break;
            }
            if (!ol.add(o))
            {
                free(o);
                res             = STATUS_NO_MEM;
                break;
            }

            o->t                = reinterpret_cast<rt_triangle3d_t *>(reinterpret_cast<uint8_t *>(o) + hsize);
            o->box              = *bbox;
            o->nt               = nt;

            // Apply object matrix to vertexes and produce final array
            rt_triangle3d_t *t  = o->t;

            for (size_t j=0; j < nt; ++j, ++t)
            {
                dsp::apply_matrix3d_mp2(&t->p[0], &tr[*(vvx++)], om);
                dsp::apply_matrix3d_mp2(&t->p[1], &tr[*(vvx++)], om);
                dsp::apply_matrix3d_mp2(&t->p[2], &tr[*(vvx++)], om);

                dsp::apply_matrix3d_mv2(&t->n, &tn[*(vnx++)], om);

                t->e[0]     = -1.0f;
                t->e[1]     = -1.0f;
                t->e[2]     = -1.0f;
                t->w        = 0.0f;
            }
        }

        if (res == STATUS_OK)
            ol.swap_data(&scene);

        destroy_scene(ol);
        return res;
    }

    /**
     * Scan scene for triangles laying inside the viewing area of wave front
     * @param ctx wave front context
     * @return statuf of operation
     */
    static status_t scan_scene(context_t *ctx)
    {
        // Check crossing with bounding box
        vector3d_t pl[4];
        calc_plane_vector_p3(&pl[0], &ctx->front.s, &ctx->front.t.p[0], &ctx->front.t.p[1]);
        calc_plane_vector_p3(&pl[1], &ctx->front.s, &ctx->front.t.p[1], &ctx->front.t.p[2]);
        calc_plane_vector_p3(&pl[2], &ctx->front.s, &ctx->front.t.p[2], &ctx->front.t.p[0]);
        calc_plane_vector_pv(&pl[3], ctx->front.t.p);

        rt_triangle3d_t out[16], buf1[16], buf2[16], *q, *in, *tmp;
        size_t n_out, n_buf1, n_buf2, *n_q, *n_in, *n_tmp;

        // Check for crossing with all bounding boxes
        for (size_t i=0, n=ctx->global->scene.size(); i<n; ++i)
        {
            object_t *obj = ctx->global->scene.get(i);
            if (obj->nt < 16)
            {
                if (obj->nt == 0)
                    continue;

                if (!ctx->source.append(obj->t, obj->nt))
                    return STATUS_NO_MEM;
                continue;
            }

            // Cull each triangle of bounding box with four scissor planes
            for (size_t j=0, m = sizeof(bbox_map)/sizeof(size_t); j < m; )
            {
                // Initialize input and queue buffer
                q = buf1, in = buf2;
                n_q = &n_buf1, n_in = &n_buf2;

                // Put to queue with updated matrix
                *n_q        = 1;
                n_out       = 0;
                q->p[0]     = obj->box.p[bbox_map[j++]];
                q->p[1]     = obj->box.p[bbox_map[j++]];
                q->p[2]     = obj->box.p[bbox_map[j++]];

                // Cull triangle with planes
                for (size_t k=0; ; )
                {
                    // Reset counters
                    *n_in   = 0;

                    // Split all triangles:
                    // Put all triangles above the plane to out
                    // Put all triangles below the plane to in
                    for (size_t l=0; l < *n_q; ++l)
                    {
                        split_triangle_rt(out, &n_out, in, n_in, &pl[k], &q[l]);
                        if ((n_out > 16) || ((*n_in) > 16))
                            lsp_trace("split overflow: n_out=%d, n_in=%d", int(n_out), int(*n_in));
                    }

                    // Interrupt cycle if there is no data to process
                    if ((*n_in <= 0) || ((++k) >= 4))
                       break;

                    // Swap buffers buf0 <-> buf1
                    n_tmp = n_in, tmp = in;
                    n_in = n_q, in = q;
                    n_q = n_tmp, q = tmp;
                }

                if (*n_in > 0) // Is there intersection with bounding box?
                    break;
            }
            if (*n_in > 0)
            {
                if (!ctx->source.append(obj->t, obj->nt))
                    return STATUS_NO_MEM;
            }
            else if (!ctx->global->ignored.append(obj->t, obj->nt))
                return STATUS_NO_MEM;
        }

        return STATUS_OK;
    }

    static status_t cull_front(context_t *ctx)
    {
#ifndef TEST_DEBUG
        rt_triangle3d_t out[16], buf1[16], buf2[16], *q, *in, *tmp;
        size_t n_out, n_buf1, n_buf2, *n_q, *n_in, *n_tmp;
        vector3d_t pl[4];

        cstorage<rt_triangle3d_t> source;
        source.swap(&ctx->source);

        calc_plane_vector_p3(&pl[0], &ctx->front.s, &ctx->front.t.p[0], &ctx->front.t.p[1]);
        calc_plane_vector_p3(&pl[1], &ctx->front.s, &ctx->front.t.p[1], &ctx->front.t.p[2]);
        calc_plane_vector_p3(&pl[2], &ctx->front.s, &ctx->front.t.p[2], &ctx->front.t.p[0]);
        calc_plane_vector_pv(&pl[3], ctx->front.t.p);

        // Cull each triangle with four scissor planes
        for (ssize_t i=0, n=source.size(); i < n; ++i)
        {
            // Initialize input and queue buffer
            q = buf1, in = buf2;
            n_q = &n_buf1, n_in = &n_buf2;

            // Put triangle to queue
            *n_q        = 1;
            n_out       = 0;
            *q          = *(source.at(i)); // Get next triangle for processing

            // Cull triangle with planes
            for (size_t k=0; ; )
            {
                // Reset counters
                *n_in   = 0;
                // Split all triangles:
                // Put all triangles above the plane to out
                // Put all triangles below the plane to in
                if (*n_q >= 8)
                    lsp_trace("split overflow: n_q=%d", int(*n_q));

                for (size_t l=0; l < *n_q; ++l)
                {
                    split_triangle_rt(out, &n_out, in, n_in, &pl[k], &q[l]);
                    if ((n_out > 16) || ((*n_in) > 16))
                        lsp_trace("split overflow: n_out=%d, n_in=%d", int(n_out), int(*n_in));
                }

                // Interrupt cycle if there is no data to process
                if ((*n_in <= 0) || ((++k) >= 4))
                   break;

                // Swap buffers buf0 <-> buf1
                n_tmp = n_in, tmp = in;
                n_in = n_q, in = q;
                n_q = n_tmp, q = tmp;
            }

            // Emit all triangles above the plane (outside vision) as ignored
            for (size_t l=0; l < n_out; ++l)
            {
                if (!ctx->global->ignored.add(&out[l]))
                    return STATUS_NO_MEM;
            }

            // The final set of triangles inside vision is in 'q' buffer, put them as visible
            for (size_t l=0; l < *n_in; ++l)
            {
                if (!ctx->source.add(&in[l]))
                    return STATUS_NO_MEM;
            }
        }
#else
        /*

         src_size=1, front=[s={0.000000, 0.000000, -0.750000},
         p=[
             {0.378978, -0.513494, -1.580061},
             {0.453656, -0.486792, -1.562277},
             {0.352607, -0.494265, -1.620808}
         ], pv=[{0.074678, 0.026702, 0.017784}, {-0.101049, -0.007473, -0.058531}, {0.026372, -0.019229, 0.040747}]]

        Planes:
            Details:{{x = 0, y = 0, z = -0.75, w = 1}, {x = 0.506148219, y = -0.709491611, z = -2, w = 1}, {x = 0.652818084, y = -0.757707298, z = -2, w = 1}}
            Details:{{x = 0, y = 0, z = -0.75, w = 1}, {x = 0.652818084, y = -0.757707298, z = -2, w = 1}, {x = 0.698123634, y = -0.749116242, z = -2, w = 1}}
            Details:{{x = 0, y = 0, z = -0.75, w = 1}, {x = 0.698123634, y = -0.749116242, z = -2, w = 1}, {x = 0.506148219, y = -0.709491611, z = -2, w = 1}}
            Details:{{x = 0.506148219, y = -0.709491611, z = -2, w = 1}, {x = 0.652818084, y = -0.757707298, z = -2, w = 1}, {x = 0.698123634, y = -0.749116242, z = -2, w = 1}}
        Triangle:
            Details:{{x = 0.506148219, y = -0.709491611, z = -2, w = 1}, {x = 0.570708156, y = -0.773277402, z = -2, w = 1}, {x = 0.652818084, y = -0.757707298, z = -2, w = 1}}
         */

        ctx->source.clear();

        rt_triangle3d_t t;
        dsp::init_point_xyz(&t.p[0], 0.506148219, -0.709491611, -2.0);
        dsp::init_point_xyz(&t.p[1], 0.652818084, -0.757707298, -2.0);
        dsp::init_point_xyz(&t.p[2], 0.698123634, -0.749116242, -2.0);
        dsp::calc_normal3d_pv(&t.n[0], t.p);
        dsp::calc_normal3d_pv(&t.n[1], t.p);
        dsp::calc_normal3d_pv(&t.n[2], t.p);
        ctx->source.add(&t);

        dsp::init_point_xyz(&t.p[0], 0.506148219, -0.709491611, -2.0);
        dsp::init_point_xyz(&t.p[1], 0.570708156, -0.773277402, -2.0);
        dsp::init_point_xyz(&t.p[2], 0.652818084, -0.757707298, -2.0);
        dsp::calc_normal3d_pv(&t.n[0], t.p);
        dsp::calc_normal3d_pv(&t.n[1], t.p);
        dsp::calc_normal3d_pv(&t.n[2], t.p);
        ctx->source.add(&t);
#endif

        return STATUS_OK;
    }

#define TRACE_BREAK(ctx, action) \
    if ((ctx->global->breakpoint >= 0) && ((ctx->global->step++) == ctx->global->breakpoint)) { \
        lsp_trace("Triggered breakpoint %d\n", int(ctx->global->breakpoint)); \
        action; \
        return STATUS_OK; \
    }

#define TRACE_SKIP(ctx, action) \
    if (ctx->global->breakpoint >= 0) { \
        if (ctx->global->step > ctx->global->breakpoint) { \
            action; \
            return STATUS_OK; \
        } \
    }

    /**
     * Perform binary split of the space
     * @param tout list of context tasks outside the space
     * @param tin list of context tasks inside the space
     * @param ctx context to apply split
     * @param pl plane to use splitting
     * @return status of operation
     */
    static status_t split_binary(
            cvector<context_t> &tout,
            cvector<context_t> &tin,
            context_t *ctx,
            const vector3d_t *pl
        )
    {
        raw_triangle3d_t out[2], in[2];
        rt_triangle3d_t sout[2], sin[2];
        size_t n_out, n_in, n_sout, n_sin;
        vector3d_t npl;
        v_triangle3d_t npt[2];
        context_t *sctx1, *sctx2;

        // STEP
        TRACE_SKIP(ctx,
            delete ctx;
        );

        // First, split front triangle into sub-triangles
        n_out = 0, n_in = 0;
        split_triangle_raw(out, &n_out, in, &n_in, pl, &ctx->front.t);
        if ((n_out > 2) || (n_in > 2))
            lsp_trace("split overflow: n_out=%d, n_in=%d", int(n_out), int(n_in));

        TRACE_BREAK(ctx,
            lsp_trace("split performed into %d in triangles, %d out triangles", int(n_in), int(n_out));
            for (size_t i=0; i<n_out; ++i)
                ctx->global->view->add_triangle_pv1c(out[i].p, (i) ? &C_RED : &C_MAGENTA);
            for (size_t i=0; i<n_in; ++i)
                ctx->global->view->add_triangle_pv1c(in[i].p, (i) ? &C_BLUE : &C_CYAN);
        );
        // END STEP

        TRACE_SKIP(ctx, )

        // Analyze result: out and in may be in range of [0..2]
        if (n_out <= 0) // There are no triangles outside
        {
            TRACE_BREAK(ctx,
                lsp_trace("All triangles are IN, moving context to IN TASKS");
                ctx->global->view->add_triangle_pv1c(ctx->front.t.p, &C_RED);
                for (size_t i=0, n=ctx->source.size(); i<n; ++i)
                    add_to_view(ctx, ctx->source.at(i), &C_RED);
            );

            // There's nothing to do, return back to the 'in' queue
            if (tin.add(ctx))
            {
                dump_context("added context to 'in'", ctx);
                return STATUS_OK;
            }

            delete ctx;
            return STATUS_NO_MEM;
        }
        else if (n_in <= 0) // There are no triangles inside
        {
            TRACE_BREAK(ctx,
                lsp_trace("All triangles are OUT, moving context to OUT TASKS");
                ctx->global->view->add_triangle_pv1c(ctx->front.t.p, &C_RED);
                for (size_t i=0, n=ctx->source.size(); i<n; ++i)
                    add_to_view(ctx, ctx->source.at(i), &C_RED);
            );

            // There's nothing to do, return back to the 'out' queue
            if (tout.add(ctx))
            {
                dump_context("added context to 'out'", ctx);
                return STATUS_OK;
            }

            delete ctx;
            return STATUS_NO_MEM;
        }

        TRACE_SKIP(ctx, )

        // Perform space split
        cstorage<rt_triangle3d_t> source, clipped;
        source.swap(&ctx->source);

        for (size_t i=0, nt=source.size(); i<nt; ++i)
        {
            n_sin = 0, n_sout = 0;
            split_triangle_rt(sout, &n_sout, sin, &n_sin, pl, source.at(i));
            if ((n_sout > 2) || ((n_sin) > 2))
                lsp_trace("split overflow: n_sout=%d, n_sin=%d", int(n_sout), int(n_sin));

            // Add generated triangles to target buffers
            for (size_t l=0; l < n_sout; ++l)
            {
                if (!clipped.add(&sout[l]))
                    return STATUS_NO_MEM;
            }
            for (size_t l=0; l < n_sin; ++l)
            {
                if (!ctx->source.add(&sin[l]))
                    return STATUS_NO_MEM;
            }
        }
        source.flush();

        // Add source context to queue
        if (!tin.add(ctx))
        {
            delete ctx;
            return STATUS_NO_MEM;
        }

        // STEP
        sctx1 = NULL;
        sctx2 = NULL;

        // Create at least one additional context
        sctx1               = new context_t;
        if (sctx1 == NULL)
            return STATUS_NO_MEM;
        else if (!tout.add(sctx1))
        {
            delete sctx1;
            return STATUS_NO_MEM;
        }

        // Create additional context if needed
        if ((n_in == 2) || (n_out == 2))
        {
            sctx2               = new context_t;
            if (sctx2 == NULL)
                return STATUS_NO_MEM;
            cvector<context_t> &cvec = (n_in == 2) ? tin : tout;
            if (!cvec.add(sctx2))
            {
                delete sctx2;
                return STATUS_NO_MEM;
            }
        }

        // Now there are possble variants:
        // {in=1, out=1}, {in=1, out=2}, {in=2, out=1}
        ctx->front.t.p[0]   = in[0].p[0];
        ctx->front.t.p[1]   = in[0].p[1];
        ctx->front.t.p[2]   = in[0].p[2];
        ctx->state          = S_CULL_BACK;

        if (n_in == 1)
        {
            sctx1->front.s      = ctx->front.s;
            sctx1->front.t.p[0] = out[0].p[0];
            sctx1->front.t.p[1] = out[0].p[1];
            sctx1->front.t.p[2] = out[0].p[2];
            sctx1->global       = ctx->global;
            sctx1->state        = S_CULL_BACK;

            init_triangle_p3(&npt[0], &ctx->front.s, &in[0].p[1], &in[0].p[2], pl);

            if (n_out == 2) // in=1, out=2
            {
                // Need to split 'clipped' into two sub-spaces
                // Prepare cutting plane
                calc_plane_vector_p3(&npl, &ctx->front.s, &out[1].p[1], &out[1].p[2]);

                float a = out[1].p[0].x * npl.dx + out[1].p[0].y * npl.dy + out[1].p[0].z * npl.dz + npl.dw;
                if (a < 0.0f)
                    flip_plane(&npl);
                init_triangle_p3(&npt[1], &ctx->front.s, &out[1].p[1], &out[1].p[2], &npl);

                sctx2->front.s      = ctx->front.s;
                sctx2->front.t.p[0] = out[1].p[0];
                sctx2->front.t.p[1] = out[1].p[1];
                sctx2->front.t.p[2] = out[1].p[2];
                sctx2->global       = ctx->global;
                sctx2->state        = S_CULL_BACK;

                TRACE_BREAK(ctx,
                    lsp_trace("ctx is IN(RED), sctx1 and sctx2 are OUT(BLUE)");
                    ctx->global->view->add_triangle_pv1c(ctx->front.t.p, &C_RED);
                    ctx->global->view->add_triangle_pv1c(sctx1->front.t.p, &C_BLUE);
                    ctx->global->view->add_triangle_pv1c(sctx2->front.t.p, &C_BLUE);

                    for (size_t i=0, n=ctx->source.size(); i<n; ++i)
                        add_to_view(ctx, ctx->source.at(i), &C_RED);
                    for (size_t i=0, n=clipped.size(); i<n; ++i)
                        add_to_view(ctx, clipped.at(i), &C_BLUE);

                    ctx->global->view->add_plane_pv1c(npt[0].p, &C_CYAN);
                    ctx->global->view->add_plane_pv1c(npt[1].p, &C_CYAN);
                )

                for (size_t i=0, nt=clipped.size(); i<nt; ++i)
                {
                    n_sin = 0, n_sout = 0;
                    split_triangle_rt(sout, &n_sout, sin, &n_sin, &npl, clipped.at(i));
                    if ((n_sout > 2) || ((n_sin) > 2))
                        lsp_trace("split overflow: n_sout=%d, n_sin=%d", int(n_sout), int(n_sin));

                    // Add generated triangles to target buffers
                    for (size_t l=0; l < n_sin; ++l)
                    {
                        if (!sctx1->source.add(&sin[l]))
                            return STATUS_NO_MEM;
                    }
                    for (size_t l=0; l < n_sout; ++l)
                    {
                        if (!sctx2->source.add(&sout[l]))
                            return STATUS_NO_MEM;
                    }
                }
            }
            else // in=1, out=1
            {
                TRACE_BREAK(ctx,
                    lsp_trace("ctx is IN(RED), sctx1 is OUT(BLUE)");
                    ctx->global->view->add_triangle_pv1c(ctx->front.t.p, &C_RED);
                    ctx->global->view->add_triangle_pv1c(sctx1->front.t.p, &C_BLUE);

                    ctx->global->view->add_plane_pv1c(npt[0].p, &C_MAGENTA);

                    for (size_t i=0, n=ctx->source.size(); i<n; ++i)
                        add_to_view(ctx, ctx->source.at(i), &C_RED);
                    for (size_t i=0, n=clipped.size(); i<n; ++i)
                        add_to_view(ctx, clipped.at(i), &C_BLUE);
                )

                sctx1->source.swap(&clipped);
            }
        }
        else // in=2, out=1
        {
            init_triangle_p3(&npt[0], &ctx->front.s, &in[0].p[1], &in[0].p[2], pl);

            sctx1->front.s      = ctx->front.s;
            sctx1->front.t.p[0] = out[0].p[0];
            sctx1->front.t.p[1] = out[0].p[1];
            sctx1->front.t.p[2] = out[0].p[2];
            sctx1->global       = ctx->global;
            sctx1->state        = S_CULL_BACK;
            sctx1->source.swap(&clipped);

            // Need to split 'source' into two sub-spaces
            // Prepare cutting plane
            calc_plane_vector_p3(&npl, &ctx->front.s, &in[1].p[1], &in[1].p[2]);
            float a = in[1].p[0].x * npl.dx + in[1].p[0].y * npl.dy + in[1].p[0].z * npl.dz + npl.dw;
            if (a < 0.0f)
                flip_plane(&npl);
            init_triangle_p3(&npt[1], &ctx->front.s, &in[1].p[1], &in[1].p[2], &npl);

            sctx2->front.s      = ctx->front.s;
            sctx2->front.t.p[0] = in[1].p[0];
            sctx2->front.t.p[1] = in[1].p[1];
            sctx2->front.t.p[2] = in[1].p[2];
            sctx2->global       = ctx->global;
            sctx2->state        = S_CULL_BACK;

            TRACE_BREAK(ctx,
                lsp_trace("ctx is IN(RED), sctx2 is IN(MAGENTA), sctx1 is OUT(BLUE)");
                ctx->global->view->add_triangle_pv1c(ctx->front.t.p, &C_RED);
                ctx->global->view->add_triangle_pv1c(sctx1->front.t.p, &C_BLUE);
                ctx->global->view->add_triangle_pv1c(sctx2->front.t.p, &C_MAGENTA);

                for (size_t i=0, n=ctx->source.size(); i<n; ++i)
                    add_to_view(ctx, ctx->source.at(i), &C_RED);
                for (size_t i=0, n=sctx1->source.size(); i<n; ++i)
                    add_to_view(ctx, sctx1->source.at(i), &C_BLUE);

                ctx->global->view->add_plane_pv1c(npt[0].p, &C_YELLOW);
                ctx->global->view->add_plane_pv1c(npt[1].p, &C_YELLOW);
            )

            // Split triangles inside
            source.swap(&ctx->source);
            for (size_t i=0, nt=source.size(); i<nt; ++i)
            {
                n_sin = 0, n_sout = 0;
                split_triangle_rt(sout, &n_sout, sin, &n_sin, &npl, source.at(i));
                if ((n_sout > 2) || ((n_sin) > 2))
                    lsp_trace("split overflow: n_sout=%d, n_sin=%d", int(n_sout), int(n_sin));

                // Add generated triangles to target buffers
                for (size_t l=0; l < n_sin; ++l)
                {
                    if (!ctx->source.add(&sin[l]))
                        return STATUS_NO_MEM;
                }
                for (size_t l=0; l < n_sout; ++l)
                {
                    if (!sctx2->source.add(&sout[l]))
                        return STATUS_NO_MEM;
                }
            }
        }

//        dump_context("added context to 'in'", ctx);
//        dump_context("added context to 'out'", sctx1);
//        if (n_in == 2)
//            dump_context("added context to 'in'", sctx2);
//        else if (n_out == 2)
//            dump_context("added context to 'out'", sctx2);

        TRACE_BREAK(ctx,
            lsp_trace("End of view split, ctx is RED, sctx1 is GREEN, sctx2 is BLUE");
            ctx->global->view->add_triangle_pv1c(ctx->front.t.p, &C_RED);
            for (size_t i=0, n=ctx->source.size(); i<n; ++i)
                add_to_view(ctx, ctx->source.at(i), &C_RED);
            if (sctx1 != NULL)
            {
                sctx1->global->view->add_triangle_pv1c(sctx1->front.t.p, &C_GREEN);
                for (size_t i=0, n=sctx1->source.size(); i<n; ++i)
                    add_to_view(ctx, sctx1->source.at(i), &C_GREEN);
            }
            if (sctx2 != NULL)
            {
                sctx2->global->view->add_triangle_pv1c(sctx2->front.t.p, &C_BLUE);
                for (size_t i=0, n=sctx2->source.size(); i<n; ++i)
                    add_to_view(ctx, sctx2->source.at(i), &C_BLUE);
            }
        );

        return STATUS_OK;
    }

    /**
     * Perform binary space culling and return culled triangle to it's queue
     * @param tasks list of tasks to save contexts
     * @param ctx context to process
     * @param ct triangle to split and return to queue
     * @param pl culling plane
     * @return status of operation
     */
    static status_t cull_binary(
            cvector<context_t> &tasks,
            context_t *ctx,
            const rt_triangle3d_t *ct,
            const vector3d_t *pl
        )
    {
        rt_triangle3d_t out[16], buf1[16], buf2[16], *q, *in, *tmp;
        size_t n_out, n_buf1, n_buf2, *n_q, *n_in, *n_tmp;
        vector3d_t spl[3];
        v_triangle3d_t spt[3];
        rt_triangle3d_t t;

        // STEP
        TRACE_SKIP(ctx,
            delete ctx;
        );

        cstorage<rt_triangle3d_t> source;
        source.swap(&ctx->source);

        // Perform culling of all triangles inside the space
        for (size_t i=0, n=source.size(); i<n; ++i)
        {
            // Reset counters
            n_buf1 = 0, n_out = 0;
            t   = *(source.at(i));

            // Split all triangles:
            split_triangle_rt(out, &n_out, buf1, &n_buf1, pl, &t);
            if ((n_out > 16) || ((n_buf1) > 16))
                lsp_trace("split overflow: n_out=%d, n_buf1=%d", int(n_out), int(n_buf1));

            for (size_t j=0; j<n_out; ++j)
                ctx->source.add(&out[j]);
            for (size_t j=0; j<n_buf1; ++j)
                ctx->global->ignored.add(&buf1[j]);
        }

        // Now we need to split source triangle and add parts back to context queue
        calc_plane_vector_p3(&spl[0], &ctx->front.s, &ctx->front.t.p[0], &ctx->front.t.p[1]);
        calc_plane_vector_p3(&spl[1], &ctx->front.s, &ctx->front.t.p[1], &ctx->front.t.p[2]);
        calc_plane_vector_p3(&spl[2], &ctx->front.s, &ctx->front.t.p[2], &ctx->front.t.p[0]);

        init_triangle_p3(&spt[0], &ctx->front.s, &ctx->front.t.p[0], &ctx->front.t.p[1], &spl[0]);
        init_triangle_p3(&spt[1], &ctx->front.s, &ctx->front.t.p[1], &ctx->front.t.p[2], &spl[1]);
        init_triangle_p3(&spt[2], &ctx->front.s, &ctx->front.t.p[2], &ctx->front.t.p[0], &spl[2]);

        TRACE_BREAK(ctx,
            for (size_t i=0, n=ctx->source.size(); i<n; ++i)
                add_to_view(ctx, ctx->source.at(i), &C_RED);

            ctx->global->view->add_plane_pv1c(spt[0].p, &C_RED);
            ctx->global->view->add_plane_pv1c(spt[1].p, &C_GREEN);
            ctx->global->view->add_plane_pv1c(spt[2].p, &C_BLUE);
            delete ctx;
        );

        // STEP
        TRACE_SKIP(ctx,
            delete ctx;
        );

        // Initialize input and queue buffer
        q = buf1, in = buf2;
        n_q = &n_buf1, n_in = &n_buf2;

        // Put triangle to queue
        *n_q        = 1;
        n_out       = 0;
        *q          = *ct;

        // Cull triangle with planes
        for (size_t k=0; ; )
        {
            // Reset counters
            *n_in   = 0;
            // Split all triangles:
            // Put all triangles above the plane to out
            // Put all triangles below the plane to in
            for (size_t l=0; l < *n_q; ++l)
            {
                split_triangle_rt(out, &n_out, in, n_in, &spl[k], &q[l]);
                if ((n_out > 16) || (*n_in > 16))
                    lsp_trace("split overflow: n_out=%d, n_in=%d", int(n_out), int(*n_in));
            }

            // Interrupt cycle if there is no data to process
            if ((*n_in <= 0) || ((++k) >= 3))
               break;

            // Swap buffers buf0 <-> buf1
            n_tmp = n_in, tmp = in;
            n_in = n_q, in = q;
            n_q = n_tmp, q = tmp;
        }

        // The final set of triangles inside vision is in 'q' buffer, put them as visible
        for (size_t l=0; l < *n_in; ++l)
        {
            if (!ctx->source.add(&in[l]))
                return STATUS_NO_MEM;
        }

        TRACE_BREAK(ctx,
            for (size_t i=0, n=ctx->source.size(); i<n; ++i)
                add_to_view_3c(ctx, ctx->source.at(i), &C_CYAN, &C_MAGENTA, &C_YELLOW);

            ctx->global->view->add_plane_pv1c(spt[0].p, &C_RED);
            ctx->global->view->add_plane_pv1c(spt[1].p, &C_GREEN);
            ctx->global->view->add_plane_pv1c(spt[2].p, &C_BLUE);
            delete ctx;
        );

        // Analyze context state
        if (ctx->source.size() <= 1)
        {
            if (ctx->source.size() > 0)
            {
                if (!ctx->global->matched.add(ctx->source.get(0)))
                    return STATUS_NO_MEM;
            }
            delete ctx;
        }
        else if (!tasks.add(ctx))
            return STATUS_NO_MEM;

        return STATUS_OK;
    }

    static status_t cull_back(cvector<context_t> &tasks, context_t *ctx)
    {
        v_triangle3d_t pt[4];
        vector3d_t pl[4];
        rt_triangle3d_t t;

        // STEP
        TRACE_SKIP(ctx,
            delete ctx;
        );

        TRACE_BREAK(ctx,
            dump_context("Current context: ", ctx);
            lsp_trace("Source triangles (%d):", int(ctx->source.size()));
            for (size_t i=0, n=ctx->source.size(); i<n; ++i)
            {
                char s[80];
                sprintf(s, "[%d]", int(i));
                dump_triangle(s, ctx->source.at(i));
                add_to_view(ctx, ctx->source.at(i), &C_YELLOW);
            }
            delete ctx;
        );

        // Select triangle from queue that will be used for culling
        while (true)
        {
            // Try to fetch new triangle from context
            if (ctx->source.size() <= 1)
            {
                if (ctx->source.size() > 0)
                {
                    if (!ctx->global->matched.add(ctx->source.get(0)))
                        return STATUS_NO_MEM;
                }
                delete ctx;
                return STATUS_OK;
            }
            else if (!ctx->source.remove(0, &t))
                return STATUS_UNKNOWN_ERR;

//            // Skip very small triangles
//            if (!check_triangle(&t))
//            {
//                if (!ctx->global->ignored.add(&t))
//                        return STATUS_NO_MEM;
//                dump_triangle("Skipping triangle (too small)", &t);
//                continue;
//            }

            // Analyze position
            calc_plane_vector_p3(&pl[3], &t.p[0], &t.p[1], &t.p[2]); // Compute culling plane
            float a = (pl[3].dx * ctx->front.s.x + pl[3].dy * ctx->front.s.y + pl[3].dz * ctx->front.s.z + pl[3].dw);
            if (a == 0.0f)
            {
                dump_triangle("Skipping triangle (perpendicular to point-of-view)", &t);
                if (!ctx->global->ignored.add(&t))
                    return STATUS_NO_MEM;
                continue;
            }

            // Normals of projection plane and culling plane have same directions
            // patch plane equations if needed
            if (a < 0.0f)
            {
                calc_plane_vector_p3(&pl[0], &ctx->front.s, &t.p[1], &t.p[0]);
                calc_plane_vector_p3(&pl[1], &ctx->front.s, &t.p[2], &t.p[1]);
                calc_plane_vector_p3(&pl[2], &ctx->front.s, &t.p[0], &t.p[2]);
                flip_plane(&pl[3]);
            }
            else
            {
                calc_plane_vector_p3(&pl[0], &ctx->front.s, &t.p[0], &t.p[1]);
                calc_plane_vector_p3(&pl[1], &ctx->front.s, &t.p[1], &t.p[2]);
                calc_plane_vector_p3(&pl[2], &ctx->front.s, &t.p[2], &t.p[0]);
            }

            break;
        }

        init_triangle_p3(&pt[0], &ctx->front.s, &t.p[0], &t.p[1], &pl[0]);
        init_triangle_p3(&pt[1], &ctx->front.s, &t.p[1], &t.p[2], &pl[1]);
        init_triangle_p3(&pt[2], &ctx->front.s, &t.p[2], &t.p[0], &pl[2]);
        init_triangle_p3(&pt[3], &t.p[0], &t.p[1], &t.p[2], &pl[3]);

        TRACE_BREAK(ctx,
            dump_context("Selected context", ctx);
            dump_triangle("Selected triangle", &t);
            add_to_view(ctx, &t, &C_MAGENTA);
            ctx->global->view->add_plane_pv1c(pt[0].p, (t.e[0] > 0.0f) ? &C_GRAY : &C_RED);
            dump_triangle("Culling plane #0", &pt[0]);
            ctx->global->view->add_plane_pv1c(pt[1].p, (t.e[1] > 0.0f) ? &C_GRAY : &C_GREEN);
            dump_triangle("Culling plane #1", &pt[1]);
            ctx->global->view->add_plane_pv1c(pt[2].p, (t.e[2] > 0.0f) ? &C_GRAY : &C_BLUE);
            dump_triangle("Culling plane #2", &pt[2]);
            ctx->global->view->add_plane_pv1c(pt[3].p, &C_YELLOW);
            dump_triangle("Culling plane #3", &pt[3]);
            delete ctx;
        );
        // END OF STEP

        // Now we have 3 planes for culling and one plane for back-culling
        cvector<context_t> inside, queue; // Store all triangle contexts that are inside the culling space

        // Put context to queue
        if (!inside.add(ctx))
        {
            delete ctx;
            return STATUS_NO_MEM;
        }

        // Operate with each plane
        size_t count;

        for (size_t i=0; i<3; ++i)
        {
            // Check if there is still data for processing
            count = inside.size();
            if (count <= 0)
            {
                TRACE_BREAK(ctx,
                    lsp_trace("No more context 'inside' for splitting\n");
                );
                return STATUS_OK;
            }
            else if (t.e[i] > 0.0f)
            {
                TRACE_BREAK(ctx,
                    lsp_trace("Edge has been processed previously, skipping\n");
                    ctx->global->view->add_plane_pv1c(pt[i].p, &C_GRAY);
                    add_to_view(ctx, &t, &C_GREEN);
                    ctx->global->view->add_triangle_pv1c(ctx->front.t.p, &C_MAGENTA);
                );
                continue;
            }

            // Obtain number of contexts in queue
            for (size_t j=0; j < count; ++j)
            {
                // Fetch new context
                ctx = inside.at(j);
                inside.set(j, NULL);

                TRACE_BREAK(ctx,
                    char s[80];
                    dump_triangle("Selected triangle", &t);
                    dump_triangle("Culling plane #0", &pt[0]);
                    dump_triangle("Culling plane #1", &pt[1]);
                    dump_triangle("Culling plane #2", &pt[2]);
                    dump_triangle("Culling plane #3", &pt[3]);

                    sprintf(s, "Culling with plane #%d", int(i));
                    dump_triangle(s, &pt[i]);

                    ctx->global->view->add_plane_pv1c(pt[i].p, &C_YELLOW);
                    add_to_view(ctx, &t, &C_GREEN);
                    ctx->global->view->add_triangle_pv1c(ctx->front.t.p, &C_MAGENTA);
                    destroy_tasks(inside);
                    delete ctx;
                );
                // END STEP

                // Perform binary space split
                status_t res = split_binary(tasks, queue, ctx, &pl[i]);
                if (res != STATUS_OK)
                {
                    delete ctx;
                    destroy_tasks(inside);
                    destroy_tasks(queue);
                    return res;
                }
            }

            // Now we have all pending items in 'queue' array, take them for the next iteration
            inside.swap_data(&queue);
            destroy_tasks(queue);
        }

        // Cut all triangles behind the space, partition the cutting triangle and put triangle back to the queue
        count = inside.size();

        for (size_t i=0; i<count; ++i)
        {
            // Fetch new context
            ctx = inside.at(i);
            inside.set(i, NULL);

            TRACE_BREAK(ctx,
                dump_triangle("Selected triangle", &t);
                lsp_trace("Culling with plane #3");
                ctx->global->view->add_plane_pv1c(pt[3].p, &C_YELLOW);
                add_to_view(ctx, &t, &C_GREEN);
                delete ctx;
                destroy_tasks(inside);
            );
            // END STEP

            status_t res = cull_binary(tasks, ctx, &t, &pl[3]);
            if (res != STATUS_OK)
            {
                delete ctx;
                destroy_tasks(inside);
                return res;
            }
        }
        destroy_tasks(inside);

        return STATUS_OK;
    }

    static status_t perform_raytrace(
            cvector<context_t> &tasks
        )
    {
        context_t *ctx = NULL;
        status_t res = STATUS_OK;

        while (tasks.size() > 0)
        {
            // Get next context from queue
            if (!tasks.pop(&ctx))
                return STATUS_CORRUPTED;

            TRACE_SKIP(ctx,
                delete ctx;
                destroy_tasks(tasks);
            );

            // Check that we need to perform a scan
            switch (ctx->state)
            {
                case S_SCAN_SCENE:
                    // Scan scene for intersections with objects
                    res = scan_scene(ctx);
                    if (res != STATUS_OK)
                        break;

                    // Change state and put to queue
                    ctx->state = S_CULL_FRONT;
                    if (!tasks.push(ctx))
                        res = STATUS_NO_MEM;
                    else
                        ctx = NULL;
                    break;

                case S_CULL_FRONT:
                    res = cull_front(ctx);
                    if (res != STATUS_OK)
                        break;

                    TRACE_BREAK(ctx,
                        lsp_trace("State after culled data");
                        for (size_t i=0, n=ctx->source.size(); i<n; ++i)
                            ctx->global->matched.add(ctx->source.get(i));
                        delete ctx;
                        ctx = NULL;
                    );

                    if (ctx->source.size() <= 1)
                    {
                        bool success = (ctx->source.size() > 0) ?
                                (ctx->global->matched.add(ctx->source.get(0)) != NULL) : true;
                        if (!success)
                        {
                            res = STATUS_NO_MEM;
                            break;
                        }
                    }
                    else
                    {
                        // Update state and return task to queue
                        ctx->state = S_CULL_BACK;

                        if (!tasks.push(ctx))
                            res = STATUS_NO_MEM;
                        else
                            ctx = NULL;
                    }
                    break;

                case S_CULL_BACK:
                    // This function will automatically manage context instance
                    // when execution result is successful
                    res = cull_back(tasks, ctx);
                    ctx = NULL;
                    break;
            }

            // Delete context if present
            if (ctx != NULL)
                delete ctx;

            // Analyze status
            if (res != STATUS_OK)
                break;
        }

        return res;
    }
} // Namespace mtest

MTEST_BEGIN("3d", reflections)

    class Renderer: public X11Renderer
    {
        private:
            Scene3D        *pScene;
            wfront_t        sFront;
            ssize_t         nTrace;
            bool            bBoundBoxes;

        public:
            explicit Renderer(Scene3D *scene, View3D *view): X11Renderer(view)
            {
                pScene = scene;
                bBoundBoxes = true;
                nTrace = BREAKPOINT_STEP;

                INIT_FRONT(sFront);

                update_view();
            }

            virtual ~Renderer()
            {
            }

        public:
            virtual void on_key_press(const XKeyEvent &ev, KeySym key)
            {
                switch (key)
                {
                    case XK_F1:
                    {
                        float incr = (ev.state & ShiftMask) ? 0.25f : -0.25f;
                        sFront.t.p[0].x += incr;
                        sFront.t.p[1].x += incr;
                        sFront.t.p[2].x += incr;
                        sFront.s.x += incr;
                        update_view();
                        break;
                    }

                    case XK_F2:
                    {
                        float incr = (ev.state & ShiftMask) ? 0.25f : -0.25f;
                        sFront.t.p[0].y += incr;
                        sFront.t.p[1].y += incr;
                        sFront.t.p[2].y += incr;
                        sFront.s.y += incr;
                        update_view();
                        break;
                    }

                    case XK_F3:
                    {
                        float incr = (ev.state & ShiftMask) ? 0.25f : -0.25f;
                        sFront.t.p[0].z += incr;
                        sFront.t.p[1].z += incr;
                        sFront.t.p[2].z += incr;
                        sFront.s.z += incr;
                        update_view();
                        break;
                    }

                    case XK_F4:
                    case XK_F5:
                    case XK_F6:
                    {
                        matrix3d_t m;
                        float incr = (ev.state & ShiftMask) ? M_PI/16.0f : -M_PI/16.0f;

                        for (size_t i=0; i<3; ++i)
                        {
                            sFront.t.p[i].x -= sFront.s.x;
                            sFront.t.p[i].y -= sFront.s.y;
                            sFront.t.p[i].z -= sFront.s.z;
                        }
                        if (key == XK_F4)
                            dsp::init_matrix3d_rotate_x(&m, incr);
                        else if (key == XK_F5)
                            dsp::init_matrix3d_rotate_y(&m, incr);
                        else
                            dsp::init_matrix3d_rotate_z(&m, incr);
                        for (size_t i=0; i<3; ++i)
                            dsp::apply_matrix3d_mp1(&sFront.t.p[i], &m);
                        for (size_t i=0; i<3; ++i)
                        {
                            sFront.t.p[i].x += sFront.s.x;
                            sFront.t.p[i].y += sFront.s.y;
                            sFront.t.p[i].z += sFront.s.z;
                        }
                        update_view();
                        break;
                    }

                    case XK_Up:
                        nTrace++;
                        lsp_trace("Set trace breakpoint to %d", int(nTrace));
                        update_view();
                        break;
                    case XK_Down:
                        if (nTrace >= 0)
                        {
                            nTrace--;
                            lsp_trace("Set trace breakpoint to %d", int(nTrace));
                            update_view();
                        }
                        break;

                    case 'b':
                    {
                        bBoundBoxes = ! bBoundBoxes;
                        update_view();
                        break;
                    }

                    case '0': case '1': case '2': case '3': case '4':
                    case '5': case '6': case '7': case '8': case '9':
                    {
                        Object3D *obj = pScene->get_object(key - '0');
                        if (obj != NULL)
                        {
                            obj->set_visible(!obj->is_visible());
                            update_view();
                        }
                        break;
                    }
                    default:
                        X11Renderer::on_key_press(ev, key);
                        break;
                }
            }

        protected:
            status_t    update_view()
            {
                v_segment3d_t s;
                v_vertex3d_t v[3];

                // Clear view state
                pView->clear_all();

                // List of ignored and matched triangles
                global_context_t global;
                cvector<context_t> tasks;

                // Create initial context
                context_t *ctx = new context_t;
                if (ctx == NULL)
                    return STATUS_NO_MEM;

                global.breakpoint   = nTrace;
                global.step         = 0;
                global.view         = pView;

                ctx->state      = S_SCAN_SCENE;
                ctx->front      = sFront;
                ctx->global     = &global;

                // Add context to tasks
                if (!tasks.add(ctx))
                {
                    delete ctx;
                    return STATUS_NO_MEM;
                }

                // Prepare scene for analysis
                status_t res = prepare_scene(global.scene, pScene);
                if (res != STATUS_OK)
                {
                    tasks.flush();
                    delete ctx;
                    return res;
                }

                // Render bounding boxes of the scene
                s.c = C_ORANGE;
                for (size_t i=0, n=global.scene.size(); i<n; ++i)
                {
                    object_t *o = global.scene.at(i);
                    bound_box3d_t *bbox = &o->box;

                    if (bBoundBoxes)
                    {
                        for (size_t i=0; i<4; ++i)
                        {
                            s.p[0] = bbox->p[i];
                            s.p[1] = bbox->p[(i+1)%4];
                            pView->add_segment(&s);
                            s.p[0] = bbox->p[i];
                            s.p[1] = bbox->p[i+4];
                            pView->add_segment(&s);
                            s.p[0] = bbox->p[i+4];
                            s.p[1] = bbox->p[(i+1)%4 + 4];
                            pView->add_segment(&s);
                        }
                    }
                }

                // Clear allocated resources, tasks and ctx should be already deleted
                res = perform_raytrace(tasks);

                destroy_tasks(tasks);
                destroy_scene(global.scene);

                // Build final scene from matched and ignored items
                for (size_t i=0, m=global.ignored.size(); i < m; ++i)
                {
                    rt_triangle3d_t *t = global.ignored.at(i);
                    v[0].p     = t->p[0];
                    v[0].n     = t->n;
                    v[0].c     = C_GRAY;

                    v[1].p     = t->p[1];
                    v[1].n     = t->n;
                    v[1].c     = C_GRAY;

                    v[2].p     = t->p[2];
                    v[2].n     = t->n;
                    v[2].c     = C_GRAY;

                    pView->add_triangle(v);
                }
                global.ignored.flush();

                for (size_t i=0, m=global.traced.size(); i < m; ++i)
                {
                    rt_triangle3d_t *t = global.traced.at(i);
                    v[0].p     = t->p[0];
                    v[0].n     = t->n;
                    v[0].c     = C_CYAN;

                    v[1].p     = t->p[1];
                    v[1].n     = t->n;
                    v[1].c     = C_MAGENTA;

                    v[2].p     = t->p[2];
                    v[2].n     = t->n;
                    v[2].c     = C_YELLOW;

                    pView->add_triangle(v);
                }

                global.traced.flush();

                for (size_t i=0, m=global.matched.size(); i < m; ++i)
                {
                    rt_triangle3d_t *t = global.matched.at(i);
                    v[0].p     = t->p[0];
                    v[0].n     = t->n;
                    v[0].c     = C_RED;

                    v[1].p     = t->p[1];
                    v[1].n     = t->n;
                    v[1].c     = C_GREEN;

                    v[2].p     = t->p[2];
                    v[2].n     = t->n;
                    v[2].c     = C_BLUE;

                    pView->add_triangle(v);
                }

                global.matched.flush();

                // Calc scissor planes' normals
                vector3d_t pl[4];
                calc_plane_vector_p3(&pl[0], &sFront.s, &sFront.t.p[0], &sFront.t.p[1]);
                calc_plane_vector_p3(&pl[1], &sFront.s, &sFront.t.p[1], &sFront.t.p[2]);
                calc_plane_vector_p3(&pl[2], &sFront.s, &sFront.t.p[2], &sFront.t.p[0]);
                calc_plane_vector_pv(&pl[3], sFront.t.p);

                // Draw front
                v_ray3d_t r;
                s.c = C_MAGENTA;

                for (size_t i=0; i<3; ++i)
                {
                    // State
                    r.p = sFront.t.p[i];
                    dsp::init_vector_p2(&r.v, &sFront.s, &r.p);
                    r.c = C_MAGENTA;
                    pView->add_ray(&r);

                    s.p[0] = sFront.s;
                    s.p[1] = sFront.t.p[i];
                    pView->add_segment(&s);

                    s.p[0] = sFront.t.p[(i+1)%3];
                    pView->add_segment(&s);

                    // Normals
/*                    r.p = sFront.p[i];
                    r.v = pl[i];
                    r.v.dw = 0.0f;
                    r.c = C_YELLOW;
                    pView->add_ray(&r);

                    r.p = sFront.p[(i+1)%3];
                    pView->add_ray(&r);

                    r.v = pl[3];
                    r.v.dw = 0.0f;
                    pView->add_ray(&r);*/
                }

                return res;
            }
    };

    MTEST_MAIN
    {
        const char *scene_file = (argc < 1) ? "res/test/3d/cross.obj" : argv[0];

        // Load scene
        Scene3D s;
        View3D v;
        status_t res = Model3DFile::load(&s, scene_file, true);
        MTEST_ASSERT_MSG(res == STATUS_OK, "Error loading scene from file %s", scene_file);

        // Initialize renderer
        Renderer r(&s, &v);
        MTEST_ASSERT_MSG(r.init() == STATUS_OK, "Error initializing renderer");
        r.run();
        r.destroy();

        // Destroy scene
        s.destroy();
    }

MTEST_END


#endif
