#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <math.h>

#include <core/types.h>
#include <core/dsp.h>


namespace solutions3d_test
{
    using namespace lsp;

    static inline bool is_zero(float x)
    {
        return fabs(x) < DSP_3D_TOLERANCE;
    }

    /* Problem solution (1):
        Segment L:
             x - xL     y - yL    z - zL
            -------- = ------- = --------
              dxL        dyL       dzL

        First two equations for Segment 1 and 2:
             x - x1     y - y1
            -------- = --------
              dx1        dy1

             x - x2     y - y2
            -------- = --------
              dx2        dy2

        Then:
             (x - x1)*dy1 + y1*dx1     (x - x2)*dy2 + y2*dx2
            ----------------------- = -----------------------
                      dx1                       dx2

        Without denominator:
            x*dx2*dy1 - x1*dx2*dy1 + y1*dx1*dx2 = x*dy2*dx1 - x2*dy2*dx1 + y2*dx2*dx1

        Now x is:
                 x1*dx2*dy1 - y1*dx1*dx2 - x2*dy2*dx1 + y2*dx2*dx1
            x = --------------------------------------------------
                                 dx2*dy1 - dy2*dx1

        OR:
                 dx1 * | x2 dx2 | - dx2 * | x1 dx1 |
                       | y2 dy2 |         | y1 dy1 |
            x = -------------------------------------
                           | dx1 dx2 |
                           | dy1 dy2 |

        Not good for projected surfaces
     */
#if 0
    bool find_intersection(point3d_t *p, const segment3d_t *s1, const segment3d_t *s2)
    {
        vector3d_t d1 = {
            s1->p[0].x - s1->p[1].x,
            s1->p[0].y - s1->p[1].y,
            s1->p[0].z - s1->p[1].z,
            s1->p[0].w - s1->p[1].w
        };

        vector3d_t d2 = {
            s2->p[0].x - s2->p[1].x,
            s2->p[0].y - s2->p[1].y,
            s2->p[0].z - s2->p[1].z,
            s2->p[0].w - s2->p[1].w
        };

        // Calculate row determinants
        float det_xy = d1.dx * d2.dy - d1.dy * d2.dx; // y -> x
        float det_yz = d1.dy * d2.dz - d1.dz * d2.dy; // z -> y
        float det_zx = d1.dz * d2.dx - d1.dx * d2.dz; // x -> z

        // Validate determinants
//        if (is_zero(det_xy) && is_zero(det_yz))
//            return false;
//        if (is_zero(det_xy) && is_zero(det_zx))
//            return false;
//        if (is_zero(det_yz) && is_zero(det_zx))
//            return false;

        const point3d_t *p1 = &s1->p[0];
        const point3d_t *p2 = &s2->p[0];

        p->x = (fabs(det_xy) > fabs(det_zx)) ?
                (d1.dx * (p2->x * d2.dy - p2->y * d2.dx) - d2.dx * (p1->x * d1.dy - p1->y * d1.dx)) / det_xy :
                (d1.dx * (p2->x * d2.dz - p2->z * d2.dx) - d2.dx * (p1->x * d1.dz - p1->z * d1.dx)) / (-det_zx);

        p->y = (fabs(det_yz) > fabs(det_xy)) ?
                (d1.dy * (p2->y * d2.dz - p2->z * d2.dy) - d2.dy * (p1->y * d1.dz - p1->z * d1.dy)) / det_yz :
                (d1.dy * (p2->y * d2.dx - p2->x * d2.dy) - d2.dy * (p1->y * d1.dx - p1->x * d1.dy)) / (-det_xy);

        p->z = (fabs(det_zx) > fabs(det_yz)) ?
                (d1.dz * (p2->z * d2.dx - p2->x * d2.dz) - d2.dz * (p1->z * d1.dx - p1->x * d1.dz)) / det_zx :
                (d1.dz * (p2->z * d2.dy - p2->y * d2.dz) - d2.dz * (p1->z * d1.dy - p1->y * d1.dz)) / (-det_yz);

        return true;
    }
#endif

    /*
        Problem solution (2):

            x = x1 + dx1*t1 = x2 + dx2*t2
            y = y1 + dy1*t1 = y2 + dy2*t2
            z = z1 + dz1*t1 = z2 + dz2*t2

        Equation system:

            dx1*t1 - dx2*t2 + x1 - x2 = 0
            dy1*t1 - dy2*t2 + y1 - y2 = 0
            dz1*t1 - dz2*t2 + z1 - z2 = 0
     */
    bool find_intersection(point3d_t *p, const segment3d_t *s1, const segment3d_t *s2)
    {
        point3d_t p0 = {
            s1->p[1].x - s1->p[0].x,
            s2->p[0].x - s2->p[1].x,
            s1->p[0].x - s2->p[0].x,
            0.0f
        };

        point3d_t p1 = {
            s1->p[1].y - s1->p[0].y,
            s2->p[0].y - s2->p[1].y,
            s1->p[0].y - s2->p[0].y,
            0.0f
        };

        point3d_t p2 = {
            s1->p[1].z - s1->p[0].z,
            s2->p[0].z - s2->p[1].z,
            s1->p[0].z - s2->p[0].z,
            0.0f
        };

        point3d_t xp;

        // Swap data
        if (fabs(p0.x) < fabs(p1.x))
            {   xp = p0; p0 = p1; p1 = xp;  }
        if (fabs(p0.x) < fabs(p2.x))
            {   xp = p0; p0 = p2; p2 = xp;  }
        if (is_zero(p0.x))
            return false;

        // Divide matrix
        if (!is_zero(p1.x))
        {
            float d = p0.x / p1.x;
            p1.x    = 0.0f;
            p1.y   -= p0.y * d;
            p1.z   -= p0.z * d;
        }
        if (!is_zero(p2.x))
        {
            float d = p0.x / p2.x;
            p2.x    = 0.0f;
            p2.y   -= p0.y * d;
            p2.z   -= p0.z * d;
        }

        // Swap again
        if (fabs(p1.y) < fabs(p2.y))
            {   xp = p0; p0 = p1; p1 = xp;  }
        if (is_zero(p1.y))
            return false;

        float t2    = - p1.z / p1.y;
        p->x        = s2->p[0].x + t2 * (s2->p[1].x - s2->p[0].x);
        p->y        = s2->p[0].y + t2 * (s2->p[1].y - s2->p[0].y);
        p->z        = s2->p[0].z + t2 * (s2->p[1].z - s2->p[0].z);
        p->w        = 0;

        return true;
    }





    int test(int argc, const char **argv)
    {
        dsp::init();

        segment3d_t s1, s2;
        s1.p[0] = { 2, 1, 0, 0 };
        s1.p[1] = { 7, 6, 0, 0 };
        s2.p[0] = { 2, 6, 0, 0 };
        s2.p[1] = { 8, 1, 0, 0 };
        point3d_t p = { 0, 0, 0, 0 };

        bool found = find_intersection(&p, &s1, &s2);

        lsp_trace("intersection = (%f, %f, %f), found=%s", p.x, p.y, p.z, (found) ? "true" : "false");

        return 0;
    }
    
}

#undef ADDM_BUF_SIZE
