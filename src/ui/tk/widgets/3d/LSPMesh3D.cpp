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
        }
        
        LSPMesh3D::~LSPMesh3D()
        {
            do_destroy();
        }

        void LSPMesh3D::do_destroy()
        {
            for (size_t i=0, n=vLayers.size(); i<n; ++i)
            {
                mesh_layer_t *layer = vLayers.get(i);
                if (layer == NULL)
                    continue;

                if (layer->pdata != NULL)
                {
                    free_aligned(layer->pdata);
                    layer->mesh     = NULL;
                    layer->normals  = NULL;
                    layer->vbuffer  = NULL;
                    layer->nbuffer  = NULL;
                }
            }

            vLayers.clear();
        }

        status_t LSPMesh3D::init()
        {
            status_t res = LSPObject3D::init();
            if (res != STATUS_OK)
                return res;

            init_color(C_RED, &sColor);
            init_color(C_YELLOW, &sColor);

            ssize_t id = sSlots.add(LSPSLOT_DRAW3D, slot_draw3d, self());
            return (id >= 0) ? STATUS_OK : -id;
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
            query_draw();
        }

        status_t LSPMesh3D::slot_draw3d(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPArea3D *_this   = widget_ptrcast<LSPArea3D>(ptr);
            return (_this != NULL) ? _this->on_draw3d(static_cast<IR3DBackend *>(data)) : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPMesh3D::on_draw3d(IR3DBackend *r3d)
        {
            return STATUS_OK;
        }

        status_t LSPMesh3D::add_triangles(const point3d_t *mesh, const point3d_t *normals, size_t items)
        {
            if ((mesh == NULL) || (items % 3))
                return STATUS_INVALID_VALUE;

            // Allocate new layer and initialize
            mesh_layer_t layer;

            layer.type          = LT_TRIANGLES;
            layer.mesh          = NULL;
            layer.normals       = NULL;
            layer.vbuffer       = NULL;
            layer.nbuffer       = NULL;
            layer.primitives    = items / 3;
            layer.draw          = 0;
            layer.pdata         = NULL;
            layer.rebuild       = true;

            // Estimate size of buffers
            size_t vbytes       = sizeof(point3d_t) * items;
            size_t nbytes       = sizeof(vector3d_t) * items;

            // Allocate buffers for the layer
            uint8_t *ptr        = alloc_aligned<uint8_t>(layer.pdata, (vbytes+nbytes)*2, DEFAULT_ALIGN);
            if (ptr == NULL)
                return STATUS_NO_MEM;

            layer.mesh          = reinterpret_cast<point3d_t *>(ptr);
            ptr                += vbytes;
            layer.vbuffer       = reinterpret_cast<point3d_t *>(ptr);
            ptr                += vbytes;
            layer.normals       = reinterpret_cast<vector3d_t *>(ptr);
            ptr                += nbytes;
            layer.nbuffer       = reinterpret_cast<vector3d_t *>(ptr);
            ptr                += nbytes;

            // Copy data to the buffer
            ::memcpy(layer.mesh, mesh, sizeof(point3d_t) * items);

            // Copy normal data if present or generate if not
            if (normals != NULL)
                ::memcpy(layer.normals, mesh, sizeof(vector3d_t) * items);
            else
            {
                const point3d_t *p  = mesh;
                vector3d_t *n       = layer.normals;
                for (size_t i=0; i<layer.primitives; ++i, n += 3, p += 3)
                {
                    dsp::calc_normal3d_pv(&n[0], p);
                    n[1]    = n[0];
                    n[2]    = n[0];
                }
            }

            // Try to add layer
            if (!vLayers.add(layer))
            {
                free_aligned(layer.pdata);
                return STATUS_NO_MEM;
            }

            query_draw();
            return STATUS_OK;
        }

        status_t LSPMesh3D::add_lines(const point3d_t *mesh, size_t items)
        {
            if ((mesh == NULL) || (items & 1))
                return STATUS_INVALID_VALUE;

            // Allocate new layer and initialize
            mesh_layer_t layer;

            layer.type          = LT_LINES;
            layer.mesh          = NULL;
            layer.normals       = NULL;
            layer.vbuffer       = NULL;
            layer.nbuffer       = NULL;
            layer.primitives    = items >> 1;
            layer.draw          = 0;
            layer.pdata         = NULL;
            layer.rebuild       = true;

            // Estimate size of buffers
            size_t vbytes       = sizeof(point3d_t) * items;

            // Allocate buffers for the layer
            uint8_t *ptr        = alloc_aligned<uint8_t>(layer.pdata, vbytes*2, DEFAULT_ALIGN);
            if (ptr == NULL)
                return STATUS_NO_MEM;

            layer.mesh          = reinterpret_cast<point3d_t *>(ptr);
            ptr                += vbytes;
            layer.vbuffer       = reinterpret_cast<point3d_t *>(ptr);
            ptr                += vbytes;

            // Copy data to the buffer
            ::memcpy(layer.mesh, mesh, sizeof(point3d_t) * items);

            // Try to add layer
            if (!vLayers.add(layer))
            {
                free_aligned(layer.pdata);
                return STATUS_NO_MEM;
            }

            query_draw();
            return STATUS_OK;
        }

        void LSPMesh3D::mark_for_rebuild()
        {
            for (size_t i=0, n=vLayers.size(); i<n; ++i)
            {
                mesh_layer_t *layer = vLayers.get(i);
                if (layer != NULL)
                    layer->rebuild      = true;
            }
            query_draw();
        }

        void LSPMesh3D::set_transform(const matrix3d_t *matrix)
        {
            sMatrix     = *matrix;
            mark_for_rebuild();
        }

        void LSPMesh3D::set_view_point(const point3d_t *pov)
        {
            LSPObject3D::set_view_point(pov);
            sPov        = *pov;
            mark_for_rebuild();
        }

        void LSPMesh3D::render(IR3DBackend *r3d)
        {
            // Visible?
            if (!is_visible())
                return;

            // Maybe need a sync?
            sSlots.execute(LSPSLOT_DRAW3D, this, r3d);

            // Perform draw
            r3d_buffer_t buf;
            buf.width           = 1.0f;

            buf.vertex.stride   = sizeof(point3d_t);
            buf.normal.stride   = sizeof(vector3d_t);
            buf.color.data      = NULL;
            buf.color.stride    = 0;
            buf.index.data      = NULL;

            // Need to rebuild scene?
            for (size_t i=0, n=vLayers.size(); i<n; ++i)
            {
                mesh_layer_t *layer = vLayers.get(i);
                if (layer == NULL)
                    continue;

                buf.vertex.data     = layer->vbuffer;

                switch (layer->type)
                {
                    case LT_TRIANGLES:
                        rebuild_triangles(layer);

                        buf.count           = layer->draw;
                        buf.type            = R3D_PRIMITIVE_TRIANGLES;
                        buf.flags           = R3D_BUFFER_LIGHTING;
                        buf.normal.data     = layer->nbuffer;

                        buf.color.dfl.r     = sColor.red();
                        buf.color.dfl.g     = sColor.green();
                        buf.color.dfl.b     = sColor.blue();
                        buf.color.dfl.a     = 1.0f;
                        break;

                    case LT_LINES:
                        rebuild_lines(layer);

                        buf.count           = layer->draw;
                        buf.type            = R3D_PRIMITIVE_LINES;
                        buf.flags           = 0;
                        buf.normal.data     = NULL;

                        buf.color.dfl.r     = sLineColor.red();
                        buf.color.dfl.g     = sLineColor.green();
                        buf.color.dfl.b     = sLineColor.blue();
                        buf.color.dfl.a     = 1.0f;
                        break;

                    default:
                        continue;
                }

                r3d->draw_primitives(&buf);
            }
        }

        void LSPMesh3D::rebuild_triangles(mesh_layer_t *layer)
        {
            if (!layer->rebuild)
                return;
            layer->rebuild          = false;

            const point3d_t *sp     = layer->mesh;
            const vector3d_t *sn    = layer->normals;

            point3d_t *dp           = layer->vbuffer;
            vector3d_t *dn          = layer->nbuffer;

            point3d_t p[3];
            vector3d_t n[3], pl;

            // Perform rebuild relative to the point of view
            layer->draw     = 0;
            for (size_t i=0; i<layer->primitives; ++i, sp += 3, sn += 3)
            {
                // Apply transformation to points
                dsp::apply_matrix3d_mp2(&p[0], &sp[0], &sMatrix);
                dsp::apply_matrix3d_mp2(&p[1], &sp[1], &sMatrix);
                dsp::apply_matrix3d_mp2(&p[2], &sp[2], &sMatrix);

                // Apply transformation to normals
                dsp::apply_matrix3d_mv2(&n[0], &sn[0], &sMatrix);
                dsp::apply_matrix3d_mv2(&n[1], &sn[1], &sMatrix);
                dsp::apply_matrix3d_mv2(&n[2], &sn[2], &sMatrix);

                // Compute plane equation and location of POV to the plane
                dsp::calc_plane_pv(&pl, p);
                float d         = sPov.x*pl.dx + sPov.y*pl.dy + sPov.z*pl.dz + pl.dw;

                // Emit the result to the view
                if (d >= 0.0f)
                {
                    // Use regular order of vertices
                    dp[0]       = p[0];
                    dp[1]       = p[1];
                    dp[2]       = p[2];

                    // Use regular order of normals
                    dn[0]       = n[0];
                    dn[1]       = n[1];
                    dn[2]       = n[2];
                }
                else if (d <= -DSP_3D_TOLERANCE)
                {
                    // Reverse order of vertices
                    dp[0]       = p[0];
                    dp[1]       = p[2];
                    dp[2]       = p[1];

                    // Reverse order of normals and flip normals
                    dsp::flip_vector_v2(&dn[0], &n[0]);
                    dsp::flip_vector_v2(&dn[1], &n[2]);
                    dsp::flip_vector_v2(&dn[2], &n[1]);
                }
                else
                    continue;

                // Update pointers and number of primitives
                dn += 3;
                dp += 3;
                ++ layer->draw;
            }
        }

        void LSPMesh3D::rebuild_lines(mesh_layer_t *layer)
        {
            if (!layer->rebuild)
                return;
            layer->rebuild          = false;

            const point3d_t *sp     = layer->mesh;
            point3d_t *dp           = layer->vbuffer;

            for (size_t i=0, n = layer->primitives << 1; i<n; ++i, ++dp, ++sp)
                dsp::apply_matrix3d_mp2(dp, sp, &sMatrix);

            layer->draw             = layer->primitives;
        }

    } /* namespace tk */
} /* namespace lsp */
