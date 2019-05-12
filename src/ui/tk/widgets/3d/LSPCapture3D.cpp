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
            V3(0.015, 0, 0),
            V3(0.011, 0.003, 0),
            V3(0.011, -0.003, 0),
            V3(0.011, 0, 0.003),
            V3(0.011, 0, -0.003),

            V3(0, 0, 1),
            V3(0, 0, -1),
            V3(1, 0, 0),
            V3(0, 1, 0),
            V3(-1, 0, 0),
            V3(0, -1, 0)
        };

        static const uint32_t tk_arrow_indexes[] =
        {
            0, 1,
            1, 2,
            1, 3,
            1, 4,
            1, 5
        };

        static const uint32_t tk_box_indexes[] =
        {
            6, 8, 9,
            6, 9, 10,
            6, 10, 11,
            6, 11, 8,
            7, 9, 8,
            7, 10, 9,
            7, 11, 10,
            7, 8, 11
        };

        const w_class_t LSPCapture3D::metadata = { "LSPCapture3D", &LSPObject3D::metadata };

        LSPCapture3D::LSPCapture3D(LSPDisplay *dpy):
            LSPObject3D(dpy),
            sColor(this),
            sAxisColor(this)
        {
            pClass          = &metadata;
            fRadius         = 1.0f;
        }
        
        LSPCapture3D::~LSPCapture3D()
        {
        }

        status_t LSPCapture3D::init()
        {
            status_t res = LSPWidget::init();
            if (res != STATUS_OK)
                return res;

            init_color(C_RED, &sColor);
            init_color(C_YELLOW, &sAxisColor);
            return STATUS_OK;
        }

        void LSPCapture3D::destroy()
        {
            LSPWidget::destroy();
        }

        const point3d_t *LSPCapture3D::position(size_t id)
        {
            v_capture_t *cap = vItems.get(id);
            return (cap != NULL) ? &cap->sPosition.z : NULL;
        }

        const vector3d_t *LSPCapture3D::direction(size_t id)
        {
            v_capture_t *cap = vItems.get(id);
            return (cap != NULL) ? &cap->sPosition.v : NULL;
        }

        const ray3d_t *LSPCapture3D::location(size_t id)
        {
            v_capture_t *cap = vItems.get(id);
            return (cap != NULL) ? &cap->sPosition : NULL;
        }

        bool LSPCapture3D::enabled(size_t id)
        {
            v_capture_t *cap = vItems.get(id);
            return (cap != NULL) ? &cap->bEnabled: false;
        }

        void LSPCapture3D::clear()
        {
            vItems.clear();
        }

        status_t LSPCapture3D::set_items(size_t items)
        {
            while (vItems.size() < items)
            {
                v_capture_t *cap = vItems.add();
                if (cap == NULL)
                    return STATUS_NO_MEM;

                dsp::init_point_xyz(&cap->sPosition.z, 0.0f, 0.0f, 0.0f);
                dsp::init_vector_dxyz(&cap->sPosition.v, 1.0f, 0.0f, 0.0f);
                cap->bEnabled   = false;
            }

            while (vItems.size() > items)
                vItems.remove_last();

            return STATUS_OK;
        }

        status_t LSPCapture3D::set_position(size_t id, const point3d_t *pos)
        {
            v_capture_t *cap = vItems.get(id);
            if (cap == NULL)
                return STATUS_NOT_FOUND;
            cap->sPosition.z = *pos;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPCapture3D::set_direction(size_t id, const vector3d_t *dir)
        {
            v_capture_t *cap = vItems.get(id);
            if (cap == NULL)
                return STATUS_NOT_FOUND;
            cap->sPosition.v = *dir;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPCapture3D::set_location(size_t id, const ray3d_t *loc)
        {
            v_capture_t *cap = vItems.get(id);
            if (cap == NULL)
                return STATUS_NOT_FOUND;
            cap->sPosition  = *loc;
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

        void LSPCapture3D::set_radius(float radius)
        {
            if (fRadius == radius)
                return;
            fRadius     = radius;
            query_draw();
        }

        void LSPCapture3D::render(IR3DBackend *r3d)
        {
            matrix3d_t m;
            r3d_buffer_t buf;
            float k = fRadius * 0.001f;

            // Draw all elements of the capture
            for (size_t id=0, nid=vItems.size(); id < nid; ++id)
            {
                v_capture_t *cap = vItems.get(id);
                if ((cap == NULL) || (!cap->bEnabled))
                    continue;

                // Update mesh data
                dsp::calc_matrix3d_transform_r1(&m, &cap->sPosition);
                for (size_t i=0; i<6; ++i)
                    dsp::apply_matrix3d_mp2(&sPoints[i], &tk_capture_vertices[i], &m);

                for (size_t i=6; i<12; ++i)
                {
                    sPoints[i].x    = tk_capture_vertices[i].x * k;
                    sPoints[i].y    = tk_capture_vertices[i].y * k;
                    sPoints[i].z    = tk_capture_vertices[i].z * k;
                    sPoints[i].w    = 1.0f;
                    dsp::apply_matrix3d_mp2(&sPoints[i], &tk_capture_vertices[i], &m);
                }

                // Call draw of lines
                buf.type            = R3D_PRIMITIVE_LINES;
                buf.width           = 2.0f;
                buf.count           = sizeof(tk_arrow_indexes) / (sizeof(uint32_t) * 2);
                buf.flags           = 0;

                buf.vertex.data     = sPoints;
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

                // Call draw of capsule
                buf.type            = R3D_PRIMITIVE_TRIANGLES;
                buf.width           = 1.0f;
                buf.count           = sizeof(tk_box_indexes) / (sizeof(uint32_t) * 2);
                buf.color.dfl.r     = sColor.red();
                buf.color.dfl.g     = sColor.green();
                buf.color.dfl.b     = sColor.blue();
                buf.color.dfl.a     = 1.0f;
                buf.index.data      = tk_box_indexes;

                r3d->draw_primitives(&buf);
            }
        }
    
    } /* namespace tk */
} /* namespace lsp */
