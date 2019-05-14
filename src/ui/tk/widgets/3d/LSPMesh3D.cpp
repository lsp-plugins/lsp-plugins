/*
 * LSPSource3D.cpp
 *
 *  Created on: 14 мая 2019 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPMesh3D::metadata     = { "LSPMesh3D", &LSPObject3D::metadata };
        
        LSPMesh3D::LSPMesh3D(LSPDisplay *dpy):
            LSPObject3D(dpy),
            sColor(this)
        {
            pClass          = &metadata;
            
            dsp::init_matrix3d_identity(&sMatrix);
            dsp::init_point_xyz(&sPov, 0.0f, 0.0f, 0.0f);
            pMesh           = NULL;
            pView           = NULL;
            nItems          = 0;
            bRebuild        = false;
        }
        
        LSPMesh3D::~LSPMesh3D()
        {
            do_destroy();
        }

        void LSPMesh3D::do_destroy()
        {
            if (pMesh != NULL)
            {
                ::free(pMesh);
                pMesh   = NULL;
            }
            if (pView != NULL)
            {
                ::free(pView);
                pView   = NULL;
            }
        }

        status_t LSPMesh3D::init()
        {
            status_t res = LSPObject3D::init();
            if (res != STATUS_OK)
                return res;

            init_color(C_RED, &sColor);
            return STATUS_OK;
        }

        void LSPMesh3D::destroy()
        {
            do_destroy();
            LSPObject3D::destroy();
        }

        void LSPMesh3D::get_position(point3d_t *dst)
        {
            dsp::init_point_xyz(dst, 0.0f, 0.0f, 0.0f);
            dsp::apply_matrix3d_mp1(dst, &sMatrix);
        }

        void LSPMesh3D::clear()
        {
            do_destroy();
        }

        status_t LSPMesh3D::set_mesh(const point3d_t *mesh, size_t items)
        {
            point3d_t *pmesh    = NULL;

            if (mesh != NULL)
            {
                if (items % 3)
                    return STATUS_INVALID_VALUE;

                size_t bytes = sizeof(point3d_t) * items;
                pmesh       = reinterpret_cast<point3d_t *>(::malloc(bytes));
                if (pmesh == NULL)
                    return STATUS_NO_MEM;
                ::memcpy(pmesh, mesh, bytes);
            }

            do_destroy();
            pMesh       = pmesh;
            nItems      = items/3;
            bRebuild    = true;

            return STATUS_OK;
        }

        void LSPMesh3D::set_transform(const matrix3d_t *matrix)
        {
            sMatrix     = *matrix;
            bRebuild    = true;
        }

        void LSPMesh3D::set_view_point(const point3d_t *pov)
        {
            LSPObject3D::set_view_point(pov);
            sPov        = *pov;
            bRebuild    = true;
        }

        void LSPMesh3D::render(IR3DBackend *r3d)
        {
            // Visible?
            if (!visible())
                return;

            // Need to rebuild scene?
            if (bRebuild)
            {
                status_t res = rebuild_mesh();
                if (res != STATUS_OK)
                    return;
                bRebuild    = false;
            }

            // Submit a draw call
            r3d_buffer_t buf;

            buf.type            = R3D_PRIMITIVE_TRIANGLES;
            buf.flags           = R3D_BUFFER_LIGHTING;
            buf.width           = 1.0f;
            buf.count           = nItems;

            buf.vertex.data     = &pView[0].z;
            buf.vertex.stride   = sizeof(ray3d_t);
            buf.normal.data     = &pView[0].v;
            buf.normal.stride   = sizeof(ray3d_t);
            buf.color.data      = NULL;
            buf.color.stride    = sizeof(ray3d_t);
            buf.color.dfl.r     = sColor.red();
            buf.color.dfl.g     = sColor.green();
            buf.color.dfl.b     = sColor.blue();
            buf.color.dfl.a     = 1.0f;
            buf.index.data      = NULL;

            r3d->draw_primitives(&buf);
        }

        status_t LSPMesh3D::rebuild_mesh()
        {
            // Try to allocate memory
            if (pView == NULL)
            {
                size_t bytes = sizeof(ray3d_t) * nItems * 3;
                pView       = reinterpret_cast<ray3d_t *>(::malloc(bytes));
                if (pView == NULL)
                    return STATUS_NO_MEM;
            }

            // Perform rebuild relative to the point of view
            const point3d_t *sp     = pMesh;
            ray3d_t *dp             = pView;

            point3d_t p[3];
            vector3d_t n;

            for (size_t i=0; i<nItems; ++i, sp += 3, dp += 3)
            {
                // Apply transformation
                dsp::apply_matrix3d_mp2(&p[0], &sp[0], &sMatrix);
                dsp::apply_matrix3d_mp2(&p[1], &sp[1], &sMatrix);
                dsp::apply_matrix3d_mp2(&p[2], &sp[2], &sMatrix);

                // Compute plane equation and location of POV to the plane
                dsp::calc_plane_pv(&n, sp);
                float d         = sPov.x*n.dx + sPov.y*n.dy + sPov.z*n.dz + n.dw;
                n.dw            = 0.0f;

                // Emit the result to the view
                if (d >= 0.0f)
                {
                    // Store triangle with normal vertices
                    dp[0].z     = p[0];
                    dp[0].v     = n;
                    dp[1].z     = p[1];
                    dp[1].v     = n;
                    dp[2].z     = p[2];
                    dp[2].v     = n;
                }
                else
                {
                    // Flip normal
                    n.dx        = -n.dx;
                    n.dy        = -n.dy;
                    n.dz        = -n.dz;

                    // Store triangle with reversed vertices
                    dp[0].z     = p[0];
                    dp[0].v     = n;
                    dp[1].z     = p[2];
                    dp[1].v     = n;
                    dp[2].z     = p[1];
                    dp[2].v     = n;
                }
            }

            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */
