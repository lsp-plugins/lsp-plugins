/*
 * LSPCapture3D.cpp
 *
 *  Created on: 12 мая 2019 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        #define V3(x, y, z) { x, y, z, 1.0f }

        static const point3d_t tk_capture_vertices[] =
        {
            V3(0, 0, 0),
            V3(0.30, 0, 0),
            V3(0.22, 0.06, 0),
            V3(0.22, -0.06, 0),
            V3(0.22, 0, 0.06),
            V3(0.22, 0, -0.06)
        };
//
//        static const point3d_t tk_capture_capsule[] =
//        {
//            V3(0, 0, 1), V3(1, 0, 0), V3(0, 1, 0),
//            V3(0, 0, 1), V3(0, 1, 0), V3(-1, 0, 0),
//            V3(0, 0, 1), V3(-1, 0, 0), V3(0, -1, 0),
//            V3(0, 0, 1), V3(0, -1, 0), V3(1, 0, 0),
//
//            V3(0, 0, -1), V3(0, 1, 0), V3(1, 0, 0),
//            V3(0, 0, -1), V3(-1, 0, 0), V3(0, 1, 0),
//            V3(0, 0, -1), V3(0, -1, 0), V3(-1, 0, 0),
//            V3(0, 0, -1), V3(1, 0, 0), V3(0, -1, 0),
//        };

        static const uint32_t tk_arrow_indexes[] =
        {
            0, 1,
            1, 2,
            1, 3,
            1, 4,
            1, 5
        };

        const w_class_t LSPCapture3D::metadata = { "LSPCapture3D", &LSPObject3D::metadata };

        LSPCapture3D::LSPCapture3D(LSPDisplay *dpy):
            LSPObject3D(dpy),
            sColor(this),
            sAxisColor(this)
        {
            pClass          = &metadata;
        }
        
        LSPCapture3D::~LSPCapture3D()
        {
        }

        status_t LSPCapture3D::init()
        {
            status_t res = LSPObject3D::init();
            if (res != STATUS_OK)
                return res;

            init_color(C_RED, &sColor);
            init_color(C_YELLOW, &sAxisColor);
            return STATUS_OK;
        }

        void LSPCapture3D::destroy()
        {
            LSPObject3D::destroy();
        }

        status_t LSPCapture3D::get_position(point3d_t *dst, size_t id)
        {
            v_capture_t *cap = vItems.get(id);
            if (cap == NULL)
                return STATUS_NOT_FOUND;
            dsp::init_point_xyz(dst, 0.0f, 0.0f, 0.0f);
            dsp::apply_matrix3d_mp1(dst, &cap->pos);
            return STATUS_OK;
        }

        status_t LSPCapture3D::get_direction(vector3d_t *dst, size_t id)
        {
            v_capture_t *cap = vItems.get(id);
            if (cap == NULL)
                return STATUS_NOT_FOUND;
            dsp::init_vector_dxyz(dst, 1.0f, 0.0f, 0.0f);
            dsp::apply_matrix3d_mv1(dst, &cap->pos);
            return STATUS_OK;
        }

        status_t LSPCapture3D::get_location(ray3d_t *dst, size_t id)
        {
            v_capture_t *cap = vItems.get(id);
            if (cap == NULL)
                return STATUS_NOT_FOUND;
            dsp::init_point_xyz(&dst->z, 0.0f, 0.0f, 0.0f);
            dsp::init_vector_dxyz(&dst->v, 1.0f, 0.0f, 0.0f);
            dsp::apply_matrix3d_mp1(&dst->z, &cap->pos);
            dsp::apply_matrix3d_mv1(&dst->v, &cap->pos);
            return STATUS_OK;
        }

        bool LSPCapture3D::enabled(size_t id) const
        {
            LSPCapture3D *_this = const_cast<LSPCapture3D *>(this);
            v_capture_t *cap = _this->vItems.get(id);
            return (cap != NULL) ? cap->bEnabled : false;
        }

        float LSPCapture3D::radius(size_t id) const
        {
            LSPCapture3D *_this = const_cast<LSPCapture3D *>(this);
            v_capture_t *cap = _this->vItems.get(id);
            return (cap != NULL) ? cap->radius : false;
        }

        void LSPCapture3D::clear()
        {
            if (vItems.size() > 0)
            {
                vItems.clear();
                query_draw();
            }
        }

        status_t LSPCapture3D::set_items(size_t items)
        {
            if (vItems.size() == items)
                return STATUS_OK;

            while (vItems.size() < items)
            {
                v_capture_t *cap = vItems.add();
                if (cap == NULL)
                    return STATUS_NO_MEM;

                dsp::init_matrix3d_identity(&cap->pos);
                cap->radius     = 1.0f;
                cap->bEnabled   = false;
            }

            while (vItems.size() > items)
                vItems.remove_last();

            query_draw();
            return STATUS_OK;
        }

        status_t LSPCapture3D::set_transform(size_t id, const matrix3d_t *matrix)
        {
            v_capture_t *cap = vItems.get(id);
            if (cap == NULL)
                return STATUS_NOT_FOUND;
            cap->pos        = *matrix;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPCapture3D::set_enabled(size_t id, bool enabled)
        {
            v_capture_t *cap = vItems.get(id);
            if (cap == NULL)
                return STATUS_NOT_FOUND;
            if (cap->bEnabled == enabled)
                return STATUS_OK;
            cap->bEnabled  = enabled;
            query_draw();
            return STATUS_OK;
        }

        void LSPCapture3D::set_radius(size_t id, float radius)
        {
            v_capture_t *cap = vItems.get(id);
            if ((cap == NULL) || (cap->radius == radius))
                return;
            cap->radius     = radius;
            if (cap->bEnabled)
                query_draw();
        }

        void LSPCapture3D::render(IR3DBackend *r3d)
        {
            if (!is_visible())
                return;

            r3d_buffer_t buf;
            cstorage<raw_triangle_t> mesh;
            cstorage<ray3d_t> vertices;

            // Draw all elements of the capture
            for (size_t id=0, nid=vItems.size(); id < nid; ++id)
            {
                v_capture_t *cap = vItems.get(id);
                if ((cap == NULL) || (!cap->bEnabled))
                    continue;

                // Update mesh data for lines
                for (size_t i=0; i<6; ++i)
                    dsp::apply_matrix3d_mp2(&sLines[i], &tk_capture_vertices[i], &cap->pos);

                // Call draw of lines
                buf.type            = R3D_PRIMITIVE_LINES;
                buf.width           = 2.0f;
                buf.count           = sizeof(tk_arrow_indexes) / (sizeof(uint32_t) * 2);
                buf.flags           = 0;

                buf.vertex.data     = sLines;
                buf.vertex.stride   = sizeof(point3d_t);
                buf.normal.data     = NULL;
                buf.normal.stride   = sizeof(point3d_t);
                buf.color.data      = NULL;
                buf.color.stride    = sizeof(point3d_t);
                buf.color.dfl.r     = sAxisColor.red();
                buf.color.dfl.g     = sAxisColor.green();
                buf.color.dfl.b     = sAxisColor.blue();
                buf.color.dfl.a     = 1.0f;
                buf.index.data      = tk_arrow_indexes;

                r3d->draw_primitives(&buf);

                // Update mesh data for body
                status_t res = rt_gen_capture_mesh(mesh, cap);
                if (res != STATUS_OK)
                    continue;

                vertices.clear();
                if (!vertices.append_n(mesh.size() * 3))
                    continue;

                raw_triangle_t *tv  = mesh.get_array();
                ray3d_t *tr         = vertices.get_array();
                for (size_t i=0, n=mesh.size(); i<n; ++i, tr += 3)
                {
                    dsp::apply_matrix3d_mp2(&tr[0].z, &tv[i].v[0], &cap->pos);
                    dsp::apply_matrix3d_mp2(&tr[1].z, &tv[i].v[1], &cap->pos);
                    dsp::apply_matrix3d_mp2(&tr[2].z, &tv[i].v[2], &cap->pos);

                    dsp::calc_normal3d_p3(&tr[0].v, &tr[0].z, &tr[1].z, &tr[2].z);
                    tr[1].v             = tr[0].v;
                    tr[2].v             = tr[0].v;
                }
                tr                  = vertices.get_array();

                // Call draw of capsule
                buf.type            = R3D_PRIMITIVE_TRIANGLES;
                buf.flags           = R3D_BUFFER_LIGHTING;
                buf.width           = 1.0f;
                buf.count           = mesh.size();
                buf.color.dfl.r     = sColor.red();
                buf.color.dfl.g     = sColor.green();
                buf.color.dfl.b     = sColor.blue();
                buf.color.dfl.a     = 1.0f;

                buf.vertex.data     = &tr[0].z;
                buf.vertex.stride   = sizeof(ray3d_t);
                buf.normal.data     = &tr[0].v;
                buf.normal.stride   = sizeof(ray3d_t);
                buf.index.data      = NULL;

                r3d->draw_primitives(&buf);
            }
        }
    
    } /* namespace tk */
} /* namespace lsp */
