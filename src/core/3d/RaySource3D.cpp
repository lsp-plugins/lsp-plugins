/*
 * RaySource3D.cpp
 *
 *  Created on: 20 апр. 2017 г.
 *      Author: sadko
 */

#include <core/3d/RaySource3D.h>

namespace lsp
{
    RaySource3D::RaySource3D()
    {
        dsp::init_matrix3d_identity(&sMatrix);
        enType = RS3DT_RANDOM;
        sRandomizer.init();

        fRadius1    = 0.1f;
        fRadius2    = 0.1f;
        fHeight     = 0.1f;
        bEnabled    = true;
        nSeed       = 0;
    }

    RaySource3D::~RaySource3D()
    {
    }

    void RaySource3D::init(uint32_t seed)
    {
        nSeed           = seed;
        sRandomizer.init(seed);
    }

    void RaySource3D::reset()
    {
        sRandomizer.init(nSeed);
    }

    void RaySource3D::generate(RayTrace3D *rt, size_t count)
    {
        switch (enType)
        {
            case RS3DT_CONIC:
                build_conic_rays(rt, count);
                break;

            case RS3DT_CYLINDRIC:
                build_cylindric_rays(rt, count);
                break;

            case RS3DT_OMNI:
                build_omni_rays(rt, count);
                break;

            case RS3DT_SPHERIC:
                build_spheric_rays(rt, count);
                break;

            default:
                build_random_rays(rt, count);
                break;
        }
    }

    void RaySource3D::build_conic_rays(RayTrace3D *rt, size_t count)
    {
        ray3d_t r;
        float rdiff = fRadius2 - fRadius1;

        while (count--)
        {
            float dr    = sRandomizer.random(RND_LINEAR);
            float da    = sRandomizer.random(RND_LINEAR) * 2.0f * M_PI;
            float cc    = cosf(da) * dr;
            float cs    = sinf(da) * dr;

            r.z.x       = 0.0f;
            r.z.y       = fRadius1 * cc;
            r.z.z       = fRadius1 * cs;
            r.z.w       = 1.0f;

            r.v.dx      = fHeight;
            r.v.dy      = rdiff * cc;
            r.v.dz      = rdiff * cs;
            r.v.dw      = 0.0f;

            dsp::apply_matrix3d_mp1(&r.z, &sMatrix);
            dsp::apply_matrix3d_mv1(&r.v, &sMatrix);

            r.v.dw      = 0.0f;
            r.z.w       = 1.0f;

            rt->push(&r);
        }
    }

    void RaySource3D::build_cylindric_rays(RayTrace3D *rt, size_t count)
    {
        ray3d_t r;
        float dz = fRadius1 - fRadius2;

        while (count--)
        {
            float gh    = sRandomizer.random(RND_LINEAR);
            float da    = sRandomizer.random(RND_LINEAR) * 2.0f * M_PI;
            float cc    = cosf(da);
            float cs    = sinf(da);
            float cr    = fRadius1 - dz*gh;

            r.z.w       = 1.0f;
            r.z.x       = cr * cc;
            r.z.y       = cr * cs;
            r.z.z       = fHeight * gh;

            r.v.dx      = fHeight * cc;
            r.v.dy      = fHeight * cs;
            r.v.dz      = dz;
            r.v.dw      = 0.0f;

            dsp::apply_matrix3d_mp1(&r.z, &sMatrix);
            dsp::apply_matrix3d_mv1(&r.v, &sMatrix);

            r.v.dw      = 0.0f;
            r.z.w       = 1.0f;

            rt->push(&r);
        }
    }

    void RaySource3D::build_omni_rays(RayTrace3D *rt, size_t count)
    {
        ray3d_t r;

        while (count--)
        {
            float gh    = sRandomizer.random(RND_LINEAR) - 0.5f;
            float da    = sRandomizer.random(RND_LINEAR) * 2.0f * M_PI;
            float cc    = cosf(da);
            float cs    = sinf(da);

            r.v.dx      = fRadius2 * cc;
            r.v.dy      = fRadius1 * cs;
            r.v.dz      = fHeight * gh;
            r.v.dw      = 0.0f;

            r.z.x       = r.v.dx;
            r.z.y       = r.v.dy;
            r.z.z       = r.v.dz;
            r.z.w       = 1.0f;

            dsp::apply_matrix3d_mp1(&r.z, &sMatrix);
            dsp::apply_matrix3d_mv1(&r.v, &sMatrix);

            r.v.dw      = 0.0f;
            r.z.w       = 1.0f;

            rt->push(&r);
        }
    }

    void RaySource3D::build_spheric_rays(RayTrace3D *rt, size_t count)
    {
        ray3d_t r;

        while (count--)
        {
            float gh    = sRandomizer.random(RND_LINEAR) * 2.0f * M_PI;
            float da    = sRandomizer.random(RND_LINEAR) * 2.0f * M_PI;

            float hz    = sinf(gh);
            float radxy = cosf(gh);
            float cc    = cosf(da) * radxy;
            float cs    = sinf(da) * radxy;

            r.v.dx      = fRadius1 * cc;
            r.v.dy      = fRadius2 * cs;
            r.v.dz      = fHeight * hz;
            r.v.dw      = 0.0f;

            r.z.x       = r.v.dx;
            r.z.y       = r.v.dy;
            r.z.z       = r.v.dz;
            r.z.w       = 1.0f;

            dsp::apply_matrix3d_mp1(&r.z, &sMatrix);
            dsp::apply_matrix3d_mv1(&r.v, &sMatrix);

            r.v.dw      = 0.0f;
            r.z.w       = 1.0f;

            rt->push(&r);
        }
    }

    void RaySource3D::build_random_rays(RayTrace3D *rt, size_t count)
    {
        ray3d_t r;

        while (count--)
        {
            r.z.x       = 0.0f;
            r.z.y       = 0.0f;
            r.z.z       = 0.0f;
            r.z.w       = 1.0f;

            r.v.dx      = sRandomizer.random(RND_LINEAR) - 0.5f;
            r.v.dy      = sRandomizer.random(RND_LINEAR) - 0.5f;
            r.v.dz      = sRandomizer.random(RND_LINEAR) - 0.5f;
            r.v.dw      = 0.0f;

            dsp::apply_matrix3d_mp1(&r.z, &sMatrix);
            dsp::apply_matrix3d_mv1(&r.v, &sMatrix);

            r.z.w       = 1.0f;
            r.v.dw      = 0.0f;

            rt->push(&r);
        }
    }

} /* namespace lsp */
