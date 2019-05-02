/*
 * CtlViewer3D.cpp
 *
 *  Created on: 25 апр. 2019 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>
#include <core/3d/common.h>
#include <core/3d/bsp_context.h>
#include <core/files/Model3DFile.h>

namespace lsp
{
    namespace ctl
    {
        static const color3d_t *colors[] =
        {
            &C3D_RED,
            &C3D_GREEN,
            &C3D_BLUE,
            &C3D_CYAN,
            &C3D_MAGENTA,
            &C3D_YELLOW
        };
        
        CtlViewer3D::CtlViewer3D(CtlRegistry *src, LSPArea3D *widget):
            CtlWidget(src, widget)
        {
            widget->slots()->bind(LSPSLOT_DRAW3D, slot_on_draw3d, this);
            widget->slots()->bind(LSPSLOT_RESIZE, slot_resize, this);

            pPathID         = NULL;
            pPath           = NULL;
            bViewChanged    = true;

            dsp::init_point_xyz(&sPov, 0.0f, -6.0f, 0.0f);
            dsp::init_vector_dxyz(&sTop, 0.0f, 0.0f, -1.0f);
            dsp::init_vector_dxyz(&sDir, 0.0f, -1.0f, 0.0f);
            dsp::init_vector_dxyz(&sSide, -1.0f, 0.0f, 0.0f);

            update_camera_state();
        }
        
        CtlViewer3D::~CtlViewer3D()
        {
            if (pPathID != NULL)
            {
                free(pPathID);
                pPathID = NULL;
            }
        }

        status_t CtlViewer3D::slot_on_draw3d(LSPWidget *sender, void *ptr, void *data)
        {
            CtlViewer3D *_this      = static_cast<CtlViewer3D *>(ptr);
            if (data == NULL)
                return STATUS_BAD_ARGUMENTS;
            return (_this != NULL) ? _this->on_draw3d(reinterpret_cast<IR3DBackend *>(data)) : STATUS_OK;
        }

        status_t CtlViewer3D::slot_resize(LSPWidget *sender, void *ptr, void *data)
        {
            CtlViewer3D *_this      = static_cast<CtlViewer3D *>(ptr);
            if (data == NULL)
                return STATUS_BAD_ARGUMENTS;

            _this->update_frustum();
            return STATUS_OK;
        }

        void CtlViewer3D::update_frustum()
        {
            LSPArea3D   *r3d        = widget_cast<LSPArea3D>(pWidget);
            if (r3d == NULL)
                return;

            // Recompute the frustum matrix
            matrix3d_t projection;
            ssize_t width   = r3d->context_width(), height = r3d->context_height();

            float fovY      = 90.0f;
            float aspect    = float(width)/float(height);
            float zNear     = 0.1f;
            float zFar      = 100.0f;

            float fH        = tan( fovY * M_PI / 360.0f) * zNear;
            float fW        = fH * aspect;
            dsp::init_matrix3d_frustum(&projection, -fW, fW, -fH, fH, zNear, zFar);

            // Update projection matrix
            r3d->set_projection_matrix(&projection);
        }

        status_t CtlViewer3D::redraw_area(timestamp_t ts, void *arg)
        {
            if (arg == NULL)
                return STATUS_OK;
            CtlViewer3D *_this = static_cast<CtlViewer3D *>(arg);

            if ((_this->pWidget != NULL) && (_this->pWidget->visible()))
                _this->pWidget->query_draw();
            return STATUS_OK;
        }

        void CtlViewer3D::init()
        {
            CtlWidget::init();
            if (pWidget == NULL)
                return;

            LSPArea3D *r3d    = widget_cast<LSPArea3D>(pWidget);

            // Initialize color controllers
            sColor.init_hsl2(pRegistry, r3d, r3d->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
            sBgColor.init_basic2(pRegistry, r3d, r3d->bg_color(), A_BG_COLOR);
            sPadding.init(r3d->padding());

//            sTimer.bind(r3d->display());
//            sTimer.set_handler(redraw_area, this);
//            sTimer.launch(-1, 250); // Schedule at 4 hz rate
        }

        void CtlViewer3D::update_camera_state()
        {
            LSPArea3D *r3d    = widget_cast<LSPArea3D>(pWidget);
            if (r3d == NULL)
                return;

            // Compute rotation matrix
            matrix3d_t delta, tmp;
            dsp::init_matrix3d_rotate_z(&delta, sAngles.fYaw);
            dsp::init_matrix3d_rotate_x(&tmp, sAngles.fPitch);
            dsp::apply_matrix3d_mm1(&delta, &tmp);

            // Compute camera direction vector
            dsp::init_vector_dxyz(&sDir, 0.0f, -1.0f, 0.0f);
            dsp::init_vector_dxyz(&sSide, -1.0f, 0.0f, 0.0f);
            dsp::apply_matrix3d_mv1(&sDir, &delta);
            dsp::apply_matrix3d_mv1(&sSide, &delta);

            // Initialize camera look
            matrix3d_t view;
            dsp::init_matrix3d_lookat_p1v2(&view, &sPov, &sDir, &sTop);

            // Set-up the view matrix
            bViewChanged    = true;
            r3d->set_view_matrix(&view);
        }

        void CtlViewer3D::set(widget_attribute_t att, const char *value)
        {
            LSPArea3D *r3d  = (pWidget != NULL) ? widget_cast<LSPArea3D>(pWidget) : NULL;

            switch (att)
            {
                case A_WIDTH:
                    if (r3d != NULL)
                        PARSE_INT(value, r3d->set_min_width(__));
                    break;
                case A_HEIGHT:
                    if (r3d != NULL)
                        PARSE_INT(value, r3d->set_min_height(__));
                    break;
                case A_BORDER:
                    if (r3d != NULL)
                        PARSE_INT(value, r3d->set_border(__));
                    break;
                case A_SPACING:
                    if (r3d != NULL)
                        PARSE_INT(value, r3d->set_radius(__));
                    break;
                case A_PATH_ID:
                    if (pPathID != NULL)
                        free(pPathID);
                    pPathID = (value != NULL) ? strdup(value) : NULL;
                    break;
                default:
                {
                    bool set = sColor.set(att, value);
                    set |= sBgColor.set(att, value);
                    set |= sPadding.set(att, value);

                    if (!set)
                        CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlViewer3D::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            // Changed file name?
            if (port == pPath)
            {
                sScene.clear();
                const char *spath   = pPath->get_buffer<char>();
                if (spath == NULL)
                    return;

                status_t res = Model3DFile::load(&sScene, spath, false);
                if (res != STATUS_OK)
                    return;

                // Mark that view has changed and query for redraw
                bViewChanged    = true;
                pWidget->query_draw();
            }
        }

        void CtlViewer3D::end()
        {
            if (pPathID != NULL)
            {
                BIND_PORT(pRegistry, pPath, pPathID);
                notify(pPath);
            }
            else
            {
                Model3DFile::load(&sScene, "res/test/3d/devel-room.obj", true);
                bViewChanged    = true;
                pWidget->query_draw();
            }

            CtlWidget::end();
        }

        void CtlViewer3D::commit_view(IR3DBackend *r3d)
        {
            if (!bViewChanged)
                return;

            // Update the scene mesh
            status_t res;
            bsp_context_t ctx;

            vVertexes.clear();

            // Add all visible objects to BSP context
            for (size_t i=0, n=sScene.num_objects(); i<n; ++i)
            {
                // Check object visibility
                Object3D *o = sScene.object(i);
                if (!o->is_visible())
                    continue;

                color3d_t c = *(colors[i % 6]);
                c.a         = 0.5f; // Update alpha value

                res = ctx.add_object(o, i, &c);
                if (res != STATUS_OK)
                    return;
            }

            // Build BSP tree and commit the BSP tree to list of drawn vertexes
            res = ctx.build_tree();
            if (res == STATUS_OK)
                res = ctx.build_mesh(&vVertexes, &sPov);
        }

        status_t CtlViewer3D::on_draw3d(IR3DBackend *r3d)
        {
            // Need to update vertex list for the scene?
            commit_view(r3d);

            r3d_buffer_t buf;

            // Draw simple triangle
            static const v_point3d_t points[] =
            {
                { { -1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
                { { 1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
                { { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
            };

            buf.type    = R3D_PRIMITIVE_TRIANGLES;
            buf.count   = 1;
            buf.width   = 1.0f;

            buf.vertex.data     = &points[0].p;
            buf.vertex.stride   = sizeof(v_point3d_t);
            buf.normal.data     = NULL;
            buf.color.data      = &points[0].c;
            buf.color.stride    = sizeof(v_point3d_t);
            buf.index.data      = NULL;

            r3d->draw_primitives(&buf);

            // Draw scene primitives
            v_vertex3d_t *vv    = vVertexes.get_array();
            size_t nvertex      = vVertexes.size();

            // Fill buffer
            buf.type            = R3D_PRIMITIVE_TRIANGLES; //(bWireframe) ? R3D_PRIMITIVE_WIREFRAME_TRIANGLES : R3D_PRIMITIVE_TRIANGLES;
            buf.width           = 1.0f;
            buf.count           = nvertex / 3;
            buf.flags           = R3D_BUFFER_BLENDING;
//            if (bLight)
//                buffer.flags       |= R3D_BUFFER_LIGHTING;

            buf.vertex.data     = &vv->p;
            buf.vertex.stride   = sizeof(v_vertex3d_t);
            buf.normal.data     = &vv->n;
            buf.normal.stride   = sizeof(v_vertex3d_t);
            buf.color.data      = &vv->c;
            buf.color.stride    = sizeof(v_vertex3d_t);
            buf.index.data      = NULL;

            // Draw call
            r3d->draw_primitives(&buf);

            return STATUS_OK;
        }
    
    } /* namespace ctl */
} /* namespace lsp */
