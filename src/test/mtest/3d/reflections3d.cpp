/*
 * reflections3d.cpp
 *
 *  Created on: 24 дек. 2018 г.
 *      Author: sadko
 */

#include <test/mtest.h>
#include <test/mtest/3d/common/X11Renderer.h>
#include <core/files/Model3DFile.h>
#include <core/3d/rt_context.h>

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

//#define TEST_DEBUG

#ifndef TEST_DEBUG
    #define BREAKPOINT_STEP     0

    #define INIT_FRONT(front) \
        dsp::init_point_xyz(&front.p[0], 0.0f, 1.0f, 0.0f); \
        dsp::init_point_xyz(&front.p[1], -1.0f, -0.5f, 0.0f); \
        dsp::init_point_xyz(&front.p[2], 1.0f, -0.5f, 0.0f); \
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

#if 0
#if 0
#pragma pack(push, 1)


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
        S_SCAN_OBJECTS,
        S_CULL_VIEW,
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

//    static void init_triangle_pv(v_triangle3d_t *t, const point3d_t *pv)
//    {
//        t->p[0] = pv[0];
//        t->p[1] = pv[1];
//        t->p[2] = pv[2];
//        dsp::calc_normal3d_pv(&t->n[0], pv);
//        t->n[1] = t->n[0];
//        t->n[2] = t->n[0];
//    }


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


#endif

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

    static void flip_plane(vector3d_t *v)
    {
        v->dx   = - v->dx;
        v->dy   = - v->dy;
        v->dz   = - v->dz;
        v->dw   = - v->dw;
    }

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
            raw_triangle_t *out,
            size_t *n_out,
            raw_triangle_t *in,
            size_t *n_in,
            const vector3d_t *pl,
            const raw_triangle_t *pv
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

    static void destroy_tasks(cvector<rt_context_t> &tasks)
    {
        for (size_t i=0, n=tasks.size(); i<n; ++i)
        {
            rt_context_t *ctx = tasks.get(i);
            if (ctx != NULL)
                delete ctx;
        }

        tasks.flush();
    }

    static bool check_bound_box(const bound_box3d_t *bbox, const rt_view_t *view)
    {
        vector3d_t pl[4];

        calc_plane_vector_p3(&pl[0], &view->s, &view->p[0], &view->p[1]);
        calc_plane_vector_p3(&pl[1], &view->s, &view->p[1], &view->p[2]);
        calc_plane_vector_p3(&pl[2], &view->s, &view->p[2], &view->p[0]);
        calc_plane_vector_p3(&pl[3], &view->p[0], &view->p[1], &view->p[2]);

        raw_triangle_t out[16], buf1[16], buf2[16], *q, *in, *tmp;
        size_t n_out, n_buf1, n_buf2, *n_q, *n_in, *n_tmp;

        // Cull each triangle of bounding box with four scissor planes
        for (size_t j=0, m = sizeof(bbox_map)/sizeof(size_t); j < m; )
        {
            // Initialize input and queue buffer
            q = buf1, in = buf2;
            n_q = &n_buf1, n_in = &n_buf2;

            // Put to queue with updated matrix
            *n_q        = 1;
            n_out       = 0;
            q->p[0]     = bbox->p[bbox_map[j++]];
            q->p[1]     = bbox->p[bbox_map[j++]];
            q->p[2]     = bbox->p[bbox_map[j++]];

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
                    split_triangle_raw(out, &n_out, in, n_in, &pl[k], &q[l]);
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

        return (*n_in) > 0;
    }

    /**
     * Scan scene for triangles laying inside the viewing area of wave front
     * @param ctx wave front context
     * @return status of operation
     */
    static status_t scan_objects(cvector<rt_context_t> &tasks, rt_context_t *ctx)
    {
        status_t res = STATUS_OK;

        RT_TRACE_BREAK(ctx,
            lsp_trace("Scanning objects...");

            for (size_t i=0, n=ctx->shared->scene->num_objects(); i<n; ++i)
            {
                Object3D *obj = ctx->shared->scene->object(i);
                if ((obj == NULL) || (!obj->is_visible()))
                    continue;
                for (size_t j=0,m=obj->num_triangles(); j<m; ++j)
                    ctx->shared->view->add_triangle_3c(obj->triangle(j), &C_RED, &C_GREEN, &C_BLUE);
            }
        )

        // Check for crossing with all bounding boxes
        for (size_t i=0, n=ctx->shared->scene->num_objects(); i<n; ++i)
        {
            Object3D *obj = ctx->shared->scene->object(i);
            if (obj == NULL)
                return STATUS_BAD_STATE;
            else if (!obj->is_visible()) // Skip invisible objects
                continue;

            // Ensure that we need to add the object to queue
            if (obj->num_triangles() >= 16)
            {
                matrix3d_t *m = obj->matrix();
                bound_box3d_t box = *(obj->bound_box());
                for (size_t j=0; j<8; ++j)
                    dsp::apply_matrix3d_mp1(&box.p[i], m);

                // Skip object if view is not crossing bounding-box
                RT_TRACE_BREAK(ctx,
                    lsp_trace("Testing bound box");

                    v_vertex3d_t v[3];
                    for (size_t j=0, m = sizeof(bbox_map)/sizeof(size_t); j < m; )
                    {
                        v[0].p      = box.p[bbox_map[j++]];
                        v[0].c      = C_YELLOW;
                        v[1].p      = box.p[bbox_map[j++]];
                        v[1].c      = C_YELLOW;
                        v[2].p      = box.p[bbox_map[j++]];
                        v[2].c      = C_YELLOW;

                        dsp::calc_normal3d_p3(&v[0].n, &v[0].p, &v[1].p, &v[2].p);
                        v[1].n      = v[0].n;
                        v[2].n      = v[0].n;

                        ctx->shared->view->add_triangle(v);
                    }
                )

                if (!check_bound_box(obj->bound_box(), &ctx->view))
                {
                    RT_TRACE(
                        matrix3d_t *mx = obj->matrix();

                        for (size_t j=0,m=obj->num_triangles(); j<m; ++j)
                        {
                            obj_triangle_t *st = obj->triangle(j);

                            v_triangle3d_t t;
                            dsp::apply_matrix3d_mp2(&t.p[0], st->v[0], mx);
                            dsp::apply_matrix3d_mp2(&t.p[1], st->v[1], mx);
                            dsp::apply_matrix3d_mp2(&t.p[2], st->v[2], mx);

                            dsp::apply_matrix3d_mv2(&t.n[0], st->n[0], mx);
                            dsp::apply_matrix3d_mv2(&t.n[1], st->n[1], mx);
                            dsp::apply_matrix3d_mv2(&t.n[2], st->n[2], mx);

                            ctx->shared->ignored.add(&t);
                        }
                    );

                    continue;
                }
            }

            // Add object to context
            res = ctx->add_object(obj);
            if (res != STATUS_OK)
                break;
#ifdef LSP_DEBUG
            if (!ctx->validate())
                return STATUS_BAD_STATE;
#endif /* LSP_DEBUG */
        }

#ifdef LSP_DEBUG
        if (!ctx->shared->scene->validate())
            return STATUS_CORRUPTED;
#endif /* LSP_DEBUG */

        // Update state
        ctx->current    = NULL;
        ctx->state      = S_CULL_VIEW;
        return (tasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
    }

    static status_t cull_view(cvector<rt_context_t> &tasks, rt_context_t *ctx)
    {
        status_t res;
        vector3d_t pl[4]; // Split plane
        RT_TRACE(v_triangle3d_t npt[4]); // Split plane presentation

        RT_TRACE(
            // Split edges
            if (!ctx->validate())
                return STATUS_BAD_STATE;
            if (!ctx->shared->scene->validate())
                return STATUS_CORRUPTED;
        )

        calc_plane_vector_p3(&pl[0], &ctx->view.p[0], &ctx->view.p[1], &ctx->view.p[2]);
        calc_plane_vector_p3(&pl[1], &ctx->view.s, &ctx->view.p[0], &ctx->view.p[1]);
        calc_plane_vector_p3(&pl[2], &ctx->view.s, &ctx->view.p[1], &ctx->view.p[2]);
        calc_plane_vector_p3(&pl[3], &ctx->view.s, &ctx->view.p[2], &ctx->view.p[0]);

        RT_TRACE(
            init_triangle_p3(&npt[0], &ctx->view.p[0], &ctx->view.p[1], &ctx->view.p[2], &pl[0]);
            init_triangle_p3(&npt[1], &ctx->view.s, &ctx->view.p[0], &ctx->view.p[1], &pl[1]);
            init_triangle_p3(&npt[2], &ctx->view.s, &ctx->view.p[1], &ctx->view.p[2], &pl[2]);
            init_triangle_p3(&npt[3], &ctx->view.s, &ctx->view.p[2], &ctx->view.p[0], &pl[3]);
        );

        for (size_t pi=0; pi<4; ++pi)
        {
            rt_context_t in(ctx->shared);
            RT_TRACE(
                rt_context_t out(ctx->shared);
            )

            RT_TRACE_BREAK(ctx,
                lsp_trace("Culling space with view plane #%d", int(pi));

                for (size_t j=0, n=ctx->triangle.size(); j<n; ++j)
                   ctx->shared->view->add_triangle_1c(ctx->triangle.get(j), &C_DARKGREEN);

                ctx->shared->view->add_triangle_pv1c(ctx->view.p, &C_MAGENTA);
                ctx->shared->view->add_plane_pv1c(npt[pi].p, &C_YELLOW);
            )

#ifdef LSP_DEBUG
            res = ctx->split(&out, &in, &pl[pi]);
#else
            res = ctx->split(NULL, &in, &pl[pi]);
#endif /* LSP_DEBUG */
            if (res != STATUS_OK)
                return res;

            RT_TRACE(
                if (!ctx->shared->scene->validate())
                    return STATUS_CORRUPTED;
                if (!ctx->validate())
                    return STATUS_BAD_STATE;
                if (!out.validate())
                    return STATUS_BAD_STATE;
                if (!in.validate())
                    return STATUS_BAD_STATE;

                // Add set of triangles to ignored
                for (size_t j=0,n=out.triangle.size(); j<n; ++j)
                    ctx->ignore(out.triangle.get(j));
            );

            RT_TRACE_BREAK(ctx,
                lsp_trace("Data after culling (%d triangles)", int(in.triangle.size()));
                for (size_t j=0,n=in.triangle.size(); j<n; ++j)
                    ctx->shared->view->add_triangle_3c(in.triangle.get(j), &C_CYAN, &C_MAGENTA, &C_YELLOW);
            );

            // Check that there is data for processing and take it for next iteration
            if (in.triangle.size() <= 0)
                break;
            else
                ctx->swap(&in);
        }

        // Change state and submit to queue
        if (ctx->triangle.size() <= 1)
        {
            if (ctx->triangle.size() == 0)
            {
                delete ctx;
                return STATUS_OK;
            }
            ctx->current    = NULL;
            ctx->state      = S_REFLECT;
        }
        else
            ctx->state      = S_PARTITION;

        return (tasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
    }

    static status_t partition_view(cvector<rt_context_t> &tasks, rt_context_t *ctx)
    {
        rt_edge_t *se;
        rt_vertex_t *sv;
        rt_triangle_t *st;
        float a;
        vector3d_t pl;
        RT_TRACE(v_triangle3d_t npt);
        raw_triangle_t vout[2], vin[2], vsrc;
        size_t n_out, n_in;
        status_t res;

        // Check that context is in final state
        if (ctx->triangle.size() <= 1)
        {
            if (ctx->triangle.size() == 0)
            {
                delete ctx;
                return STATUS_OK;
            }

            ctx->current    = NULL;
            ctx->state      = S_REFLECT;
            return (tasks.add(ctx)) ? STATUS_OK : STATUS_NO_MEM;
        }

        RT_TRACE_BREAK(ctx,
            lsp_trace("Partitioning view");
            ctx->shared->view->add_triangle_pv1c(ctx->view.p, &C_MAGENTA);
            for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
            {
                rt_triangle_t *t = ctx->triangle.get(i);
                ctx->shared->view->add_triangle_1c(t, (t == ctx->current) ? &C_ORANGE : &C_YELLOW);
            }
            for (size_t i=0,n=ctx->edge.size(); i<n; ++i)
            {
                rt_edge_t *e = ctx->edge.get(i);
                ctx->shared->view->add_segment(e, (e->itag & RT_EF_PLANE) ? &C_GREEN : &C_CYAN);
            }
        )

        rt_context_t out(ctx->shared), in(ctx->shared);

        // Select current triangle
        if (ctx->current == NULL)
            ctx->current    = ctx->triangle.get(0);
        st  = ctx->current;

        // Try to split with edge
        for (size_t ei=0; ei<3; ++ei)
        {
            // Check if we need to apply this side of triangle to culling
            se  = st->e[ei];
            if (se->itag & RT_EF_PLANE)
                continue;

            // Select opposite to the edge point
            sv = st->v[0];
            if ((sv == se->v[0]) || (sv == se->v[1]))
            {
                sv = st->v[1];
                if ((sv == se->v[0]) || (sv == se->v[1]))
                {
                    sv = st->v[2];
                    if ((sv == se->v[0]) || (sv == se->v[1]))
                        return STATUS_CORRUPTED;
                }
            }

            // Build split plane
            calc_plane_vector_p3(&pl, &ctx->view.s, se->v[0], se->v[1]);
            float a = (sv->x * pl.dx + sv->y * pl.dy + sv->z * pl.dz + pl.dw);
            if (a > 0.0f)
                flip_plane(&pl);

            RT_TRACE_BREAK(ctx,
                lsp_trace("Applying edge %d", int(ei));
                init_triangle_p3(&npt, &ctx->view.s, se->v[0], se->v[1], &pl);

                ctx->shared->view->add_triangle_pv1c(ctx->view.p, &C_MAGENTA);
                for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
                {
                    rt_triangle_t *t = ctx->triangle.get(i);
                    ctx->shared->view->add_triangle_1c(t, (t == ctx->current) ? &C_ORANGE : &C_YELLOW);
                }
                for (size_t i=0,n=ctx->edge.size(); i<n; ++i)
                {
                    rt_edge_t *e = ctx->edge.get(i);
                    ctx->shared->view->add_segment(e,
                            (e == se) ? &C_RED :
                            (e->itag & RT_EF_PLANE) ? &C_GREEN : &C_CYAN
                    );
                }
                ctx->shared->view->add_plane_pv1c(npt.p, &C_RED);
            )

            se->itag    |= RT_EF_PLANE;     // Mark edge as processed

            // Split context into 'OUT' and 'IN' domain
            res = ctx->split(&out, &in, &pl);
            if (res != STATUS_OK)
                return res;

            // Now split view
            n_out       = 0;
            n_in        = 0;
            vsrc.p[0]   = ctx->view.p[0];
            vsrc.p[1]   = ctx->view.p[1];
            vsrc.p[2]   = ctx->view.p[2];

            split_triangle_raw(vout, &n_out, vin, &n_in, &pl, &vsrc);

            RT_TRACE_BREAK(ctx,
                lsp_trace("After applying edge %d", int(ei));

                if (n_out >= 1)
                    ctx->shared->view->add_triangle_pv1c(vout[0].p, &C_RED);
                if (n_in >= 1)
                    ctx->shared->view->add_triangle_pv1c(vin[0].p, &C_GREEN);
                if (n_out >= 2)
                    ctx->shared->view->add_triangle_pv1c(vout[1].p, &C_MAGENTA);
                if (n_in >= 2)
                    ctx->shared->view->add_triangle_pv1c(vin[1].p, &C_MAGENTA);

                for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
                {
                    rt_triangle_t *t = ctx->triangle.get(i);
                    ctx->shared->view->add_triangle_1c(t, (t == ctx->current) ? &C_ORANGE : &C_YELLOW);
                }
                for (size_t i=0,n=ctx->edge.size(); i<n; ++i)
                {
                    rt_edge_t *e = ctx->edge.get(i);
                    ctx->shared->view->add_segment(e,
                            (e == se) ? &C_RED :
                            (e->itag & RT_EF_PLANE) ? &C_GREEN : &C_CYAN
                    );
                }
                ctx->shared->view->add_plane_pv1c(npt.p, &C_RED);
            )

            // Possible variants
            // n_in = 0, n_out = 1 - ignore triangle, add all 'out' triangles to context and update view to 'out'
            // n_in = 1, n_out = 0 - move to next step
            // n_in = 1, n_out = 1 - create additional context and put to queue
            // n_in = 1, n_out = 2 - create two additional contexts and put to queue
            // n_in = 2, n_out = 1 - create two additional contexts and put to queue

            // Analyze result
            if (n_in == 0) // All triangles outside?
            {
                RT_TRACE(
                    for (size_t i=0, n=in.triangle.size(); i<n; ++i)
                        ctx->ignore(in.triangle.get(i));
                    for (size_t i=0, n=out.triangle.size(); i<n; ++i)
                        ctx->ignore(out.triangle.get(i));
                );

                if (n_out == 0) // Impossible, but keep this code for sure
                {
                    delete ctx;
                    return STATUS_OK;
                }

                // Update context state and add to tasks
                ctx->swap(&out);
                ctx->current        = NULL;
                ctx->view.p[0]      = vout[0].p[0];
                ctx->view.p[1]      = vout[0].p[1];
                ctx->view.p[2]      = vout[0].p[2];

                return (tasks.add(ctx)) ? STATUS_OK : STATUS_NO_MEM;
            }
            else if (n_out == 0) // No triangles outside?
            {
                if (n_in == 1)
                    continue;

                delete ctx;
                return STATUS_OK;
            }

            // Create at least one context
            rt_context_t *nctx1  = new rt_context_t(ctx->shared);
            if (nctx1 == NULL)
                return STATUS_NO_MEM;
            else if (!tasks.add(nctx1))
            {
                delete nctx1;
                return STATUS_NO_MEM;
            }

            if (n_in == 1) // Only one triangle inside?
            {
                RT_TRACE(
                    for (size_t i=0, n=out.triangle.size(); i<n; ++i)
                        ctx->ignore(out.triangle.get(i));
                );

                // Swap content, update view and continue
                if (n_out == 2)
                {
                    // Perform additional split of 'out' context into nctx1 and nctx2
                    rt_context_t *nctx2 = new rt_context_t(ctx->shared);
                    if (nctx2 == NULL)
                        return STATUS_NO_MEM;
                    else if (!tasks.add(nctx2))
                    {
                        delete nctx2;
                        return STATUS_NO_MEM;
                    }

                    // Perform additional split
                    calc_plane_vector_p3(&pl, &ctx->view.s, &vout[1].p[2], &vout[1].p[1]);

                    RT_TRACE_BREAK(ctx,
                        lsp_trace("Additional split 1");
                        init_triangle_p3(&npt, &ctx->view.s, &vout[1].p[2], &vout[1].p[1], &pl);

                        ctx->shared->view->add_triangle_pv1c(vin[0].p, &C_GREEN);
                        ctx->shared->view->add_triangle_pv1c(vout[0].p, &C_RED);
                        ctx->shared->view->add_triangle_pv1c(vout[1].p, &C_MAGENTA);

                        for (size_t i=0,n=in.triangle.size(); i<n; ++i)
                            ctx->shared->view->add_triangle_1c(in.triangle.get(i), &C_GREEN);
                        for (size_t i=0,n=nctx1->triangle.size(); i<n; ++i)
                            ctx->shared->view->add_triangle_1c(nctx1->triangle.get(i), &C_RED);
                        for (size_t i=0,n=nctx2->triangle.size(); i<n; ++i)
                            ctx->shared->view->add_triangle_1c(nctx2->triangle.get(i), &C_MAGENTA);

                        ctx->shared->view->add_plane_pv1c(npt.p, &C_YELLOW);
                    )

                    res                 = out.split(nctx2, nctx1, &pl);
                    if (res != STATUS_OK)
                        return res;

                    nctx2->view.s       = ctx->view.s;
                    nctx2->view.p[0]    = vout[1].p[0];
                    nctx2->view.p[1]    = vout[1].p[1];
                    nctx2->view.p[2]    = vout[1].p[2];
                    nctx1->state        = ctx->state;
                }
                else // Just copy data of 'out' context to nctx1
                    nctx1->swap(&out);

                nctx1->view.s       = ctx->view.s;
                nctx1->view.p[0]    = vout[0].p[0];
                nctx1->view.p[1]    = vout[0].p[1];
                nctx1->view.p[2]    = vout[0].p[2];
                nctx1->state        = ctx->state;

                ctx->swap(&in);
                ctx->view.p[0]      = vin[0].p[0];
                ctx->view.p[1]      = vin[0].p[1];
                ctx->view.p[2]      = vin[0].p[2];

                continue;
            }
            else // n_in == 2, n_out == 1
            {
                // Perform additional split of 'in' context into nctx1 and nctx2
                rt_context_t *nctx2 = new rt_context_t(ctx->shared);
                if (nctx2 == NULL)
                    return STATUS_NO_MEM;
                else if (!tasks.add(nctx2))
                {
                    delete nctx2;
                    return STATUS_NO_MEM;
                }

                // Perform additional split
                calc_plane_vector_p3(&pl, &ctx->view.s, &vin[1].p[2], &vin[1].p[1]);
                res                 = in.split(nctx2, nctx1, &pl);
                if (res != STATUS_OK)
                    return res;

                RT_TRACE_BREAK(ctx,
                    lsp_trace("Additional split 2");
                    init_triangle_p3(&npt, &ctx->view.s, &vin[1].p[2], &vin[1].p[1], &pl);

                    ctx->shared->view->add_triangle_pv1c(vin[0].p, &C_GREEN);
                    ctx->shared->view->add_triangle_pv1c(vin[1].p, &C_BLUE);
                    ctx->shared->view->add_triangle_pv1c(vout[0].p, &C_RED);

                    for (size_t i=0,n=nctx1->triangle.size(); i<n; ++i)
                        ctx->shared->view->add_triangle_1c(nctx1->triangle.get(i), &C_GREEN);
                    for (size_t i=0,n=nctx2->triangle.size(); i<n; ++i)
                        ctx->shared->view->add_triangle_1c(nctx2->triangle.get(i), &C_BLUE);
                    for (size_t i=0,n=out.triangle.size(); i<n; ++i)
                        ctx->shared->view->add_triangle_1c(out.triangle.get(i), &C_RED);

                    ctx->shared->view->add_plane_pv1c(npt.p, &C_YELLOW);
                )

                // Swap content, update view and continue
                nctx1->view.s       = ctx->view.s;
                nctx1->view.p[0]    = vin[0].p[0];
                nctx1->view.p[1]    = vin[0].p[1];
                nctx1->view.p[2]    = vin[0].p[2];
                nctx1->state        = ctx->state;

                nctx2->view.s       = ctx->view.s;
                nctx2->view.p[0]    = vin[1].p[0];
                nctx2->view.p[1]    = vin[1].p[1];
                nctx2->view.p[2]    = vin[1].p[2];
                nctx2->state        = ctx->state;

                ctx->swap(&out);
                ctx->view.p[0]      = vout[0].p[0];
                ctx->view.p[1]      = vout[0].p[1];
                ctx->view.p[2]      = vout[0].p[2];

                return (tasks.add(ctx)) ? STATUS_OK : STATUS_NO_MEM;
            }
        }

        // Now perform cull-back
        calc_plane_vector_p3(&pl, st->v[0], st->v[1], st->v[2]);
        a   = ctx->view.s.x * pl.dx + ctx->view.s.y * pl.dy + ctx->view.s.z * pl.dz + ctx->view.s.w;
        if (a > 0.0f)
            flip_plane(&pl);

        RT_TRACE_BREAK(ctx,
            lsp_trace("Applying back-split");
            init_triangle_p3(&npt, st->v[0], st->v[1], st->v[2], &pl);

            ctx->shared->view->add_triangle_pv1c(ctx->view.p, &C_MAGENTA);
            for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
            {
                rt_triangle_t *t = ctx->triangle.get(i);
                ctx->shared->view->add_triangle_1c(t, (t == ctx->current) ? &C_ORANGE : &C_YELLOW);
            }
            for (size_t i=0,n=ctx->edge.size(); i<n; ++i)
            {
                rt_edge_t *e = ctx->edge.get(i);
                ctx->shared->view->add_segment(e,
                        (e == se) ? &C_RED :
                        (e->itag & RT_EF_PLANE) ? &C_CYAN : &C_GREEN
                );
            }
            ctx->shared->view->add_plane_pv1c(npt.p, &C_RED);
        )

#ifdef LSP_DEBUG
        res = ctx->split(&out, &in, &pl);
#else
        res = ctx->split(NULL, &in, &pl);
#endif /* LSP_DEBUG */
        if (res != STATUS_OK)
            return res;

        RT_TRACE_BREAK(ctx,
            lsp_trace("After back-split: in is GREEN, out is RED");
            ctx->shared->view->add_triangle_pv1c(ctx->view.p, &C_MAGENTA);
            for (size_t i=0,n=in.triangle.size(); i<n; ++i)
                ctx->shared->view->add_triangle_1c(in.triangle.get(i), &C_GREEN);
            for (size_t i=0,n=out.triangle.size(); i<n; ++i)
                ctx->shared->view->add_triangle_1c(out.triangle.get(i), &C_RED);
        )

        RT_TRACE(
            for (size_t i=0,n=out.triangle.size(); i<n; ++i)
                ctx->ignore(out.triangle.get(i));
        );

        // Update context state and push to queue if OK
        ctx->swap(&in);
        ctx->current        = NULL;

        if (ctx->triangle.size() <= 1)
        {
            if (ctx->triangle.size() == 0)
            {
                delete ctx;
                return STATUS_OK;
            }
            ctx->state      = S_REFLECT;
        }
        else
            ctx->state          = S_PARTITION;

        return (tasks.add(ctx)) ? STATUS_OK : STATUS_NO_MEM;
    }

    static status_t reflect_view(cvector<rt_context_t> &tasks, rt_context_t *ctx)
    {
        // DEBUG
        for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
            ctx->match(ctx->triangle.get(i));
        for (size_t i=0,n=ctx->edge.size(); i<n; ++i)
        {
            rt_edge_t *e = ctx->edge.get(i);
            if (e->itag & RT_EF_PLANE)
                ctx->shared->view->add_segment(e, &C_YELLOW);
        }

        delete ctx;
        return STATUS_OK;
    }

    static status_t perform_raytrace(cvector<rt_context_t> &tasks)
    {
        rt_context_t *ctx = NULL;
        status_t res = STATUS_OK;

        while (tasks.size() > 0)
        {
            // Get next context from queue
            if (!tasks.pop(&ctx))
                return STATUS_CORRUPTED;

            // Check that we need to perform a scan
            switch (ctx->state)
            {
                case S_SCAN_OBJECTS:
                    res = scan_objects(tasks, ctx);
                    break;
                case S_CULL_VIEW:
                    res = cull_view(tasks, ctx);
                    break;
                case S_PARTITION:
                    res = partition_view(tasks, ctx);
                    break;
                case S_REFLECT:
                    res = reflect_view(tasks, ctx);
                    break;
                default:
                    res = STATUS_BAD_STATE;
                    break;
            }

            // Analyze status
            if (res != STATUS_OK)
            {
                delete ctx;
                break;
            }
        }

        destroy_tasks(tasks);
        return res;
    }
} // Namespace mtest

MTEST_BEGIN("3d", reflections)

    class Renderer: public X11Renderer
    {
        private:
            Scene3D        *pScene;
            rt_view_t       sFront;
            ssize_t         nTrace;
            bool            bBoundBoxes;

        public:
            explicit Renderer(Scene3D *scene, View3D *view): X11Renderer(view)
            {
                pScene = scene;
                bBoundBoxes = false;
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
                        sFront.p[0].x += incr;
                        sFront.p[1].x += incr;
                        sFront.p[2].x += incr;
                        sFront.s.x += incr;
                        update_view();
                        break;
                    }

                    case XK_F2:
                    {
                        float incr = (ev.state & ShiftMask) ? 0.25f : -0.25f;
                        sFront.p[0].y += incr;
                        sFront.p[1].y += incr;
                        sFront.p[2].y += incr;
                        sFront.s.y += incr;
                        update_view();
                        break;
                    }

                    case XK_F3:
                    {
                        float incr = (ev.state & ShiftMask) ? 0.25f : -0.25f;
                        sFront.p[0].z += incr;
                        sFront.p[1].z += incr;
                        sFront.p[2].z += incr;
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
                            sFront.p[i].x -= sFront.s.x;
                            sFront.p[i].y -= sFront.s.y;
                            sFront.p[i].z -= sFront.s.z;
                        }
                        if (key == XK_F4)
                            dsp::init_matrix3d_rotate_x(&m, incr);
                        else if (key == XK_F5)
                            dsp::init_matrix3d_rotate_y(&m, incr);
                        else
                            dsp::init_matrix3d_rotate_z(&m, incr);
                        for (size_t i=0; i<3; ++i)
                            dsp::apply_matrix3d_mp1(&sFront.p[i], &m);
                        for (size_t i=0; i<3; ++i)
                        {
                            sFront.p[i].x += sFront.s.x;
                            sFront.p[i].y += sFront.s.y;
                            sFront.p[i].z += sFront.s.z;
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
                status_t res = STATUS_OK;

                if (!pScene->validate())
                    return STATUS_BAD_STATE;

                // Clear view state
                pView->clear_all();

                // List of ignored and matched triangles
                rt_shared_t global;
                global.breakpoint   = nTrace;
                global.step         = 0;
                global.scene        = pScene;
                global.view         = pView;

                cvector<rt_context_t> tasks;

                // Create initial context
                rt_context_t *ctx = new rt_context_t(&global);
                if (ctx == NULL)
                    return STATUS_NO_MEM;

                ctx->state          = S_SCAN_OBJECTS;
                ctx->view           = sFront;

                // Add context to tasks
                if (!tasks.add(ctx))
                {
                    delete ctx;
                    return STATUS_NO_MEM;
                }

                // Render bounding boxes of the scene
                if (bBoundBoxes)
                {
                    s.c[0] = C_ORANGE;
                    s.c[1] = C_ORANGE;
                    for (size_t i=0, n=global.scene->num_objects(); i<n; ++i)
                    {
                        Object3D *o = global.scene->object(i);
                        matrix3d_t *m = o->matrix();
                        bound_box3d_t *pmbox = o->bound_box();
                        bound_box3d_t bbox;

                        for (size_t i=0; i<8; ++i)
                            dsp::apply_matrix3d_mp2(&bbox.p[i], &pmbox->p[i], m);

                        for (size_t i=0; i<4; ++i)
                        {
                            s.p[0] = bbox.p[i];
                            s.p[1] = bbox.p[(i+1)%4];
                            pView->add_segment(&s);
                            s.p[0] = bbox.p[i];
                            s.p[1] = bbox.p[i+4];
                            pView->add_segment(&s);
                            s.p[0] = bbox.p[i+4];
                            s.p[1] = bbox.p[(i+1)%4 + 4];
                            pView->add_segment(&s);
                        }
                    }
                }

                // Clear allocated resources, tasks and ctx should be already deleted
                res = perform_raytrace(tasks);
                if (res == STATUS_BREAKPOINT) // This status is used for immediately returning from traced code
                    res = STATUS_OK;

                if (!pScene->validate())
                    return STATUS_BAD_STATE;

                // Build final scene from matched and ignored items
                for (size_t i=0, m=global.ignored.size(); i < m; ++i)
                    pView->add_triangle_1c(global.ignored.at(i), &C_GRAY);

                for (size_t i=0, m=global.matched.size(); i < m; ++i)
                {
                    v_triangle3d_t *t = global.matched.at(i);
                    v[0].p     = t->p[0];
                    v[0].n     = t->n[0];
                    v[0].c     = C_RED;

                    v[1].p     = t->p[1];
                    v[1].n     = t->n[1];
                    v[1].c     = C_GREEN;

                    v[2].p     = t->p[2];
                    v[2].n     = t->n[2];
                    v[2].c     = C_BLUE;

                    pView->add_triangle(v);
                }

                global.ignored.flush();
                global.matched.flush();

                // Calc scissor planes' normals
                vector3d_t pl[4];
                calc_plane_vector_p3(&pl[0], &sFront.s, &sFront.p[0], &sFront.p[1]);
                calc_plane_vector_p3(&pl[1], &sFront.s, &sFront.p[1], &sFront.p[2]);
                calc_plane_vector_p3(&pl[2], &sFront.s, &sFront.p[2], &sFront.p[0]);
                calc_plane_vector_p3(&pl[3], &sFront.p[0], &sFront.p[1], &sFront.p[2]);

                // Draw front
                v_ray3d_t r;
                s.c[0] = C_MAGENTA;
                s.c[1] = C_MAGENTA;

                for (size_t i=0; i<3; ++i)
                {
                    // State
                    r.p = sFront.p[i];
                    dsp::init_vector_p2(&r.v, &sFront.s, &r.p);
                    r.c = C_MAGENTA;
                    pView->add_ray(&r);

                    s.p[0] = sFront.s;
                    s.p[1] = sFront.p[i];
                    pView->add_segment(&s);

                    s.p[0] = sFront.p[(i+1)%3];
                    pView->add_segment(&s);
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

