/*
 * TraceCapture3D.cpp
 *
 *  Created on: 12 мая 2017 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/3d/TraceCapture3D.h>

namespace lsp
{
    const point3d_t TraceCapture3D::tc_vp[] =
    {
        { 1.0f, 0.0f, 0.0f, 1.0f },     // 0
        { 0.0f, 1.0f, 0.0f, 1.0f },     // 1
        { -1.0f, 0.0f, 0.0f, 1.0f },    // 2
        { 0.0f, -1.0f, 0.0f, 1.0f },    // 3
        { 0.0f, 0.0f, 1.0f, 1.0f },     // 4
        { 0.0f, 0.0f, -1.0f, 1.0f }     // 5
    };

    const TraceCapture3D::triangle_t TraceCapture3D::tc_vt[] =
    {
        { 0, 1, 4 },
        { 1, 2, 4 },
        { 2, 3, 4 },
        { 3, 0, 4 },
        { 1, 0, 5 },
        { 2, 1, 5 },
        { 3, 2, 5 },
        { 0, 3, 5 }
    };

    TraceCapture3D::TraceCapture3D()
    {
        pSample         = NULL;
        nChannel        = -1;
        nSampleRate     = -1;
        fRadius         = 1.0f;
        enType          = TC3DT_OMNI;
    }
    
    TraceCapture3D::~TraceCapture3D()
    {
        pSample         = NULL;
        nChannel        = -1;
        nSampleRate     = -1;
    }

    void TraceCapture3D::init(Sample *s, size_t channel)
    {
        pSample         = s;
        nChannel        = channel;
    }

    void TraceCapture3D::set_sample_rate(size_t sr)
    {
        nSampleRate     = sr;
    }

    void TraceCapture3D::capture(const vector3d_t *v, float amplitude, float delay)
    {
        if ((pSample == NULL) || (nSampleRate == size_t(-1)))
            return;
        if (nChannel >= pSample->channels())
            return;

        // Update the gain according to the direction type
        switch (enType)
        {
            case TC3DT_OMNI:
            default:
                break;
        }

        // Resize sample if possible
        size_t index    = seconds_to_samples(nSampleRate, delay);
        if (pSample->extend(index+1) <= index)
            return;

        // Update sample in buffer
        float *buf      = pSample->getBuffer(nChannel);
        buf[index]     += amplitude;
    }

    Object3D *TraceCapture3D::build_surface(size_t rank)
    {
        //---------------------------------------
        // Build tetrasphere
        point3d_t  *pp[3], *xp;
        ssize_t     spi;

        // Allocate object
        Object3D *obj   = new Object3D();
        if (obj == NULL)
            return NULL;

        // Add initial set of vertexes
        size_t ntc_vp   = sizeof(tc_vp)/sizeof(point3d_t);
        xp              = obj->create_vertex(ntc_vp);
        if (xp == NULL)
        {
            obj->destroy();
            delete obj;
            return NULL;
        }

        for (size_t i=0; i<ntc_vp; ++i)
            dsp::scale_point2(xp++, &tc_vp[i], fRadius);

        // Allocate set of triangles
        if (rank > 8)
            rank = 8;
        ntc_vp = 8 << (rank << 1);
        triangle_t *vt  = lsp_tmalloc(triangle_t, ntc_vp);
        if (vt == NULL)
        {
            obj->destroy();
            delete obj;
            return NULL;
        }
        size_t n        = 8;
        ::memcpy(vt, tc_vt, n * sizeof(triangle_t));

        // Now we need to do additional loops
        for (size_t k=0; k<rank; ++k)
        {
            size_t limit = n;
            for (size_t i=0; i<limit; ++i)
            {
                triangle_t *tr  = &vt[i];

                // Allocate three new points
                spi         = obj->get_vertex_count();
                xp          = obj->create_vertex(3);
                if (xp == NULL)
                {
                    lsp_free(vt);
                    obj->destroy();
                    delete obj;
                    return NULL;
                }

                // Get vertexes of the triangle that should be divided
                pp[0]       = obj->get_vertex(tr->ip[0]);
                pp[1]       = obj->get_vertex(tr->ip[1]);
                pp[2]       = obj->get_vertex(tr->ip[2]);

                // Calculate middle-points
                dsp::move_point3d_p2(&xp[0], pp[0], pp[1], 0.5f);
                dsp::move_point3d_p2(&xp[1], pp[1], pp[2], 0.5f);
                dsp::move_point3d_p2(&xp[2], pp[2], pp[0], 0.5f);

                // Normalize points
                dsp::scale_point1(&xp[0], fRadius);
                dsp::scale_point1(&xp[1], fRadius);
                dsp::scale_point1(&xp[2], fRadius);

                // Allocate and (re)build triangles
                triangle_t *tt  = &vt[n];
                n              += 3;

                tt->ip[0]       = spi;
                tt->ip[1]       = tr->ip[1];
                tt->ip[2]       = spi + 1;
                tt++;

                tt->ip[0]       = spi + 1;
                tt->ip[1]       = tr->ip[2];
                tt->ip[2]       = spi + 2;
                tt++;

                tt->ip[0]       = spi;
                tt->ip[1]       = spi + 1;
                tt->ip[2]       = spi + 2;

                tr->ip[1]       = spi;
                tr->ip[2]       = spi + 2;
            }
        }

        // Now we have array of triangles, just add them to object
        triangle_t *tt  = vt;
        for (size_t i=0; i<n; ++i)
        {
            status_t result = obj->add_triangle(tt->ip[0], tt->ip[1], tt->ip[2]);
            if (result != STATUS_OK)
            {
                lsp_free(vt);
                obj->destroy();
                delete obj;
                return NULL;
            }
            tt++;
        }

        // Free temporary data
        lsp_free(vt);

        return obj;
    }

    void TraceCapture3D::reset()
    {
        // TODO: reset state of the capture
    }

} /* namespace lsp */
