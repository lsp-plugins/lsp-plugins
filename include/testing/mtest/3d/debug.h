/*
 * debug.h
 *
 *  Created on: 31 мар. 2017 г.
 *      Author: sadko
 */

#ifndef TEST_3D_DEBUG_H_
#define TEST_3D_DEBUG_H_

namespace geometry3d_test
{
    void dump_triangle(const triangle3d_t *t)
    {
        printf(
            "triangle{"
                "p={"
                    "{%.3f, %.3f, %.3f, %.3f}, "
                    "{%.3f, %.3f, %.3f, %.3f}, "
                    "{%.3f, %.3f, %.3f, %.3f}"
                "}, n="
                    "{%.3f, %.3f, %.3f, %.3f}"
            "}",
            t->p[0].x, t->p[0].y, t->p[0].z, t->p[0].w,
            t->p[1].x, t->p[1].y, t->p[1].z, t->p[1].w,
            t->p[2].x, t->p[2].y, t->p[2].z, t->p[2].w,
            t->n.dx, t->n.dy, t->n.dz, t->n.dw
        );
    }

    void dump_ray(const ray3d_t *l)
    {
        printf(
            "ray{"
                "z={%.3f, %.3f, %.3f, %.3f}"
                ", v={%.3f, %.3f, %.3f, %.3f}"
            "}",
            l->z.x, l->z.y, l->z.z, l->z.w,
            l->v.dx, l->v.dy, l->v.dz, l->v.dw
        );
    }

    void dump_point(const point3d_t *p)
    {
        printf(
            "point{%.3f, %.3f, %.3f, %.3f}",
            p->x, p->y, p->z, p->w
        );
    }
}

#endif /* TEST_3D_DEBUG_H_ */
