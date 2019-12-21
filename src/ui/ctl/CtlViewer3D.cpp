/*
 * CtlViewer3D.cpp
 *
 *  Created on: 25 апр. 2019 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <core/3d/common.h>
#include <core/3d/bsp_context.h>
#include <core/files/Model3DFile.h>
#include <plugins/room_builder.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlViewer3D::metadata = { "CtlViewer3D", &CtlWidget::metadata };

        static const v_point3d_t axis_lines[] =
        {
            // X axis (red)
            { { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { { 0.25f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            // Y axis (green)
            { { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
            { { 0.0f, 0.25f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
            // Z axis (blue)
            { { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
            { { 0.0f, 0.0f, 0.25f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
        };

        CtlViewer3D::CtlViewer3D(CtlRegistry *src, LSPArea3D *widget):
            CtlWidget(src, widget)
        {
            pClass          = &metadata;

            widget->slots()->bind(LSPSLOT_DRAW3D, slot_on_draw3d, this);
            widget->slots()->bind(LSPSLOT_RESIZE, slot_resize, this);
            widget->slots()->bind(LSPSLOT_MOUSE_DOWN, slot_mouse_down, this);
            widget->slots()->bind(LSPSLOT_MOUSE_UP, slot_mouse_up, this);
            widget->slots()->bind(LSPSLOT_MOUSE_MOVE, slot_mouse_move, this);

            pFile           = NULL;
            pStatus         = NULL;
            pPosX           = NULL;
            pPosY           = NULL;
            pPosZ           = NULL;
            pYaw            = NULL;
            pPitch          = NULL;
            pScaleX         = NULL;
            pScaleY         = NULL;
            pScaleZ         = NULL;
            pOrientation    = NULL;

            bViewChanged    = true;

            fOpacity        = 0.25f;
            fFov            = 70.0f;

            sAngles.fYaw    = 0.0f;
            sAngles.fPitch  = 0.0f;
            sAngles.fRoll   = 0.0f;
            sOldAngles      = sAngles;

            nBMask          = 0;
            nMouseX         = 0;
            nMouseY         = 0;

            dsp::init_point_xyz(&sPov, 0.0f, -6.0f, 0.0f);
            dsp::init_point_xyz(&sOldPov, 0.0f, -6.0f, 0.0f);
            dsp::init_vector_dxyz(&sScale, 1.0f, 1.0f, 1.0f);
            dsp::init_vector_dxyz(&sTop, 0.0f, 0.0f, -1.0f);
            dsp::init_vector_dxyz(&sXTop, 0.0f, 0.0f, -1.0f);
            dsp::init_vector_dxyz(&sDir, 0.0f, -1.0f, 0.0f);
            dsp::init_vector_dxyz(&sSide, -1.0f, 0.0f, 0.0f);
            dsp::init_matrix3d_identity(&sOrientation);

            update_camera_state();
        }
        
        CtlViewer3D::~CtlViewer3D()
        {
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

        status_t CtlViewer3D::slot_mouse_down(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            CtlViewer3D *_this  = static_cast<CtlViewer3D *>(ptr);
            ws_event_t *ev  = static_cast<ws_event_t *>(data);

            if (_this->nBMask == 0)
            {
                _this->nMouseX      = ev->nLeft;
                _this->nMouseY      = ev->nTop;
                _this->sOldAngles   = _this->sAngles;
                _this->sOldPov      = _this->sPov;
            }

            _this->nBMask |= (1 << ev->nCode);

            return STATUS_OK;
        }

        status_t CtlViewer3D::slot_mouse_up(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            CtlViewer3D *_this  = static_cast<CtlViewer3D *>(ptr);
            ws_event_t *ev  = static_cast<ws_event_t *>(data);

            if (_this->nBMask == 0)
                return STATUS_OK;

            _this->nBMask &= ~(1 << ev->nCode);
            if (_this->nBMask == 0)
            {
                if (ev->nCode == MCB_MIDDLE)
                    _this->rotate_camera(ev->nLeft - _this->nMouseX, ev->nTop - _this->nMouseY);
                else if (ev->nCode == MCB_RIGHT)
                    _this->move_camera(ev->nLeft - _this->nMouseX, ev->nTop - _this->nMouseY, 0);
                else if (ev->nCode == MCB_LEFT)
                    _this->move_camera(ev->nLeft - _this->nMouseX, 0, _this->nMouseY - ev->nTop);
            }

            return STATUS_OK;
        }

        status_t CtlViewer3D::slot_mouse_move(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            CtlViewer3D *_this  = static_cast<CtlViewer3D *>(ptr);
            ws_event_t *ev  = static_cast<ws_event_t *>(data);

            if (_this->nBMask == (1 << MCB_MIDDLE))
                _this->rotate_camera(ev->nLeft - _this->nMouseX, ev->nTop - _this->nMouseY);
            else if (_this->nBMask == (1 << MCB_RIGHT))
                _this->move_camera(ev->nLeft - _this->nMouseX, ev->nTop - _this->nMouseY, 0);
            else if (_this->nBMask == (1 << MCB_LEFT))
                _this->move_camera(ev->nLeft - _this->nMouseX, 0, _this->nMouseY - ev->nTop);

            return STATUS_OK;
        }

        float CtlViewer3D::get_adelta(CtlPort *p, float dfl)
        {
            const port_t *meta = (p != NULL) ? p->metadata() : NULL;
            if ((meta != NULL) && (meta->flags & F_STEP))
                return is_degree_unit(meta->unit) ? meta->step * 5.0f * M_PI / 180.0f : meta->step;
            return dfl;
        }

        void CtlViewer3D::rotate_camera(ssize_t dx, ssize_t dy)
        {
            float dyaw      = get_adelta(pYaw, M_PI * 2e-3f);
            float dpitch    = get_adelta(pPitch, M_PI * 2e-3f);

            float yaw       = sOldAngles.fYaw - (dx * dyaw);
            float pitch     = sOldAngles.fPitch - (dy * dpitch);

            if (pPitch == NULL)
            {
                if (pitch >= (89.0f * M_PI / 360.0f))
                    pitch       = (89.0f * M_PI / 360.0f);
                else if (pitch <= (-89.0f * M_PI / 360.0f))
                    pitch       = (-89.0f * M_PI / 360.0f);
            }

            submit_angle_change(&sAngles.fYaw, yaw, pYaw);
            submit_angle_change(&sAngles.fPitch, pitch, pPitch);
        }

        void CtlViewer3D::submit_angle_change(float *vold, float vnew, CtlPort *port)
        {
            if (*vold == vnew)
                return;

            const port_t *meta = (port != NULL) ? port->metadata() : NULL;
            if (meta != NULL)
            {
                if (is_degree_unit(meta->unit))
                    vnew    = vnew * 180.0f / M_PI;
                port->set_value(vnew);
                port->notify_all();
            }
            else
            {
                *vold           = vnew;
                bViewChanged    = true;
                update_camera_state();
                pWidget->query_draw();
            }
        }

        float CtlViewer3D::get_delta(CtlPort *p, float dfl)
        {
            const port_t *meta = (p != NULL) ? p->metadata() : NULL;
            if ((meta != NULL) && (meta->flags & F_STEP))
                return meta->step;
            return dfl;
        }

        void CtlViewer3D::submit_pov_change(float *vold, float vnew, CtlPort *port)
        {
            if (*vold == vnew)
                return;

            if (port != NULL)
            {
                port->set_value(vnew);
                port->notify_all();
            }
            else
            {
                *vold           = vnew;
                bViewChanged    = true;
                update_camera_state();
                pWidget->query_draw();
            }
        }

        void CtlViewer3D::move_camera(ssize_t dx, ssize_t dy, ssize_t dz)
        {
            point3d_t pov;
            float mdx       = dx * get_delta(pPosX, 0.01f) * 5.0f;
            float mdy       = dy * get_delta(pPosY, 0.01f) * 5.0f;
            float mdz       = dz * get_delta(pPosZ, 0.01f) * 5.0f;

            pov.x           = sOldPov.x + sSide.dx * mdx + sDir.dx * mdy + sXTop.dx * mdz;
            pov.y           = sOldPov.y + sSide.dy * mdx + sDir.dy * mdy + sXTop.dy * mdz;
            pov.z           = sOldPov.z + sSide.dz * mdx + sDir.dz * mdy + sXTop.dz * mdz;

            submit_pov_change(&sPov.x, pov.x, pPosX);
            submit_pov_change(&sPov.y, pov.y, pPosY);
            submit_pov_change(&sPov.z, pov.z, pPosZ);
        }

        void CtlViewer3D::update_frustum()
        {
            LSPArea3D   *r3d        = widget_cast<LSPArea3D>(pWidget);
            if (r3d == NULL)
                return;

            // Recompute the frustum matrix
            matrix3d_t projection;
            ssize_t width   = r3d->context_width(), height = r3d->context_height();

            float aspect    = float(width)/float(height);
            float zNear     = 0.1f;
            float zFar      = 1000.0f;

            float fH        = tanf( fFov * M_PI / 360.0f) * zNear;
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
            sColor.init_hsl(pRegistry, r3d, r3d->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
            sPadding.init(r3d->padding());
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
            dsp::init_vector_dxyz(&sXTop, 0.0f, 0.0f, -1.0f);
            dsp::apply_matrix3d_mv1(&sDir, &delta);
            dsp::apply_matrix3d_mv1(&sSide, &delta);
            dsp::apply_matrix3d_mv1(&sXTop, &delta);

            // Initialize camera look
            matrix3d_t view;
            dsp::init_matrix3d_lookat_p1v2(&view, &sPov, &sDir, &sTop);

            // Set-up the view matrix
            bViewChanged    = true;
            r3d->set_view_point(&sPov);
            r3d->set_view_matrix(&view);
        }

        void CtlViewer3D::set(widget_attribute_t att, const char *value)
        {
            LSPArea3D *r3d  = widget_cast<LSPArea3D>(pWidget);

            switch (att)
            {
                case A_ID:
                    BIND_PORT(pRegistry, pFile, value);
                    break;
                case A_WIDTH:
                    if (r3d != NULL)
                        PARSE_INT(value, r3d->set_min_width(__));
                    break;
                case A_HEIGHT:;
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
                case A_STATUS_ID:
                    BIND_PORT(pRegistry, pStatus, value);
                    break;
                case A_XPOS_ID:
                    BIND_PORT(pRegistry, pPosX, value);
                    break;
                case A_YPOS_ID:
                    BIND_PORT(pRegistry, pPosY, value);
                    break;
                case A_ZPOS_ID:
                    BIND_PORT(pRegistry, pPosZ, value);
                    break;
                case A_XSCALE_ID:
                    BIND_PORT(pRegistry, pScaleX, value);
                    break;
                case A_YSCALE_ID:
                    BIND_PORT(pRegistry, pScaleY, value);
                    break;
                case A_ZSCALE_ID:
                    BIND_PORT(pRegistry, pScaleZ, value);
                    break;
                case A_YAW_ID:
                    BIND_PORT(pRegistry, pYaw, value);
                    break;
                case A_PITCH_ID:
                    BIND_PORT(pRegistry, pPitch, value);
                    break;
                case A_ORIENTATION_ID:
                    BIND_PORT(pRegistry, pOrientation, value);
                    break;
                case A_OPACITY:
                    PARSE_FLOAT(value, fOpacity = __);
                    break;
                case A_TRANSPARENCY:
                    PARSE_FLOAT(value, fOpacity = 1.0f - __);
                    break;
                case A_KVT_ROOT:
                    sKvtRoot.set_utf8(value);
                    pRegistry->add_kvt_listener(this);
                    break;
                default:
                {
                    sColor.set(att, value);
                    sPadding.set(att, value);
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlViewer3D::sync_pov_change(float *dst, CtlPort *port, CtlPort *psrc)
        {
            if ((psrc != port) || (port == NULL))
                return;
            *dst    = psrc->get_value();
            update_camera_state();
            bViewChanged    = true;
            pWidget->query_draw();
        }

        void CtlViewer3D::sync_scale_change(float *dst, CtlPort *port, CtlPort *psrc)
        {
            if ((psrc != port) || (port == NULL))
                return;
            float v = psrc->get_value() * 0.01f;
            if (*dst == v)
                return;

            *dst            = v;
            bViewChanged    = true;
            pWidget->query_draw();
        }

        status_t CtlViewer3D::add(CtlWidget *child)
        {
            LSPArea3D *r3d  = widget_cast<LSPArea3D>(pWidget);
            LSPWidget *w = child->widget();
            return (r3d != NULL) ? r3d->add(w) : STATUS_NOT_IMPLEMENTED;
        }

        void CtlViewer3D::sync_angle_change(float *dst, CtlPort *port, CtlPort *psrc)
        {
            if ((psrc != port) || (port == NULL))
                return;
            const port_t *meta = port->metadata();
            if (meta == NULL)
                return;

            float value = psrc->get_value();
            if (is_degree_unit(meta->unit))
                value       = value * M_PI / 180.0f;
            *dst    = value;

            update_camera_state();
            bViewChanged    = true;
            pWidget->query_draw();
        }

        void CtlViewer3D::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            // Changed file name?
            if ((port == pFile) || (port == pStatus))
            {
                // Clear scene state
                sScene.clear();

                // Load scene only if status is not defined or valid
                if ((pStatus == NULL) || (status_t(pStatus->get_value()) == STATUS_OK))
                {
                    const char *spath   = pFile->get_buffer<char>();
                    if (spath != NULL)
                    {
                        // Try to load
                        status_t res = Model3DFile::load(&sScene, spath, false);
                        if (res != STATUS_OK)
                            sScene.clear();
                    }
                }

                // Mark that view has changed and query for redraw
                bViewChanged    = true;
                pWidget->query_draw();
            }

            if (port == pOrientation)
            {
                dsp::init_matrix3d_orientation(&sOrientation, axis_orientation_t(pOrientation->get_value()));
                bViewChanged    = true;
                pWidget->query_draw();
            }

            sync_pov_change(&sPov.x, pPosX, port);
            sync_pov_change(&sPov.y, pPosY, port);
            sync_pov_change(&sPov.z, pPosZ, port);
            sync_angle_change(&sAngles.fYaw, pYaw, port);
            sync_angle_change(&sAngles.fPitch, pPitch, port);
            sync_scale_change(&sScale.dx, pScaleX, port);
            sync_scale_change(&sScale.dy, pScaleY, port);
            sync_scale_change(&sScale.dz, pScaleZ, port);
        }

        void CtlViewer3D::commit_view(IR3DBackend *r3d)
        {
            if (!bViewChanged)
                return;

            // Update the scene mesh
            status_t res;
            bsp_context_t ctx;

            vVertexes.clear();

            matrix3d_t m, scale;
            Color col;
            col.set_rgba(1.0f, 0.0f, 0.0f, 0.0f);

            dsp::init_matrix3d_scale(&scale, sScale.dx, sScale.dy, sScale.dz);

            // Add all visible objects to BSP context
            for (size_t i=0, n=sScene.num_objects(); i<n; ++i)
            {
                // Check object visibility
                Object3D *o = sScene.object(i);
                if (o == NULL)
                    continue;

                Color xc(col);
                color3d_t c;
                xc.hue(float(i) / float(n));

                // Apply changes
                matrix3d_t om = *(o->matrix());
                if (!sKvtRoot.is_empty())
                {
                    KVTStorage *kvt = pRegistry->kvt_lock();
                    if (kvt)
                    {
                        room_builder_base::obj_props_t props;
                        LSPString base;
                        bool res = base.set(&sKvtRoot);
                        if (res) res = base.fmt_append_ascii("%d", int(i));

                        if (res)
                        {
//                            matrix3d_t tmp;
                            room_builder_base::read_object_properties(&props, base.get_utf8(), kvt);
                            o->set_visible(props.bEnabled);
                            room_builder_base::build_object_matrix(&om, &props, &scale);
//                            dsp::apply_matrix3d_mm2(&om, &tmp, &om);
//                            dsp::apply_matrix3d_mm2(&om, &scale, &om);
//                            dsp::apply_matrix3d_mm2(&om, &tmp, &scale);
                            xc.hue(props.fHue);
                        }

                        pRegistry->kvt_release();
                    }
                }

                // Object is invisible?
                if (!o->is_visible())
                    continue;

                c.r         = xc.red();
                c.g         = xc.green();
                c.b         = xc.blue();
                c.a         = fOpacity; // Update alpha value

                dsp::apply_matrix3d_mm2(&m, &sOrientation, &om);
                res = ctx.add_object(o, i, &m, &c);
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
            LSPArea3D *area     = widget_cast<LSPArea3D>(pWidget);

            // Need to update vertex list for the scene?
            commit_view(r3d);

            // Set Light parameters
            r3d_light_t light;

            light.type          = R3D_LIGHT_POINT; //R3D_LIGHT_DIRECTIONAL;
            light.position      = sPov;
            light.direction.dx  = -sDir.dx;
            light.direction.dy  = -sDir.dy;
            light.direction.dz  = -sDir.dz;
            light.direction.dw  = 0.0f;

            light.ambient.r     = 0.0f;
            light.ambient.g     = 0.0f;
            light.ambient.b     = 0.0f;
            light.ambient.a     = 1.0f;

            light.diffuse.r     = 1.0f;
            light.diffuse.g     = 1.0f;
            light.diffuse.b     = 1.0f;
            light.diffuse.a     = 1.0f;

            light.specular.r    = 1.0f;
            light.specular.g    = 1.0f;
            light.specular.b    = 1.0f;
            light.specular.a    = 1.0f;

            light.constant      = 1.0f;
            light.linear        = 0.0f;
            light.quadratic     = 0.0f;
            light.cutoff        = 180.0f;

            // Enable/disable lighting
            r3d->set_lights(&light, 1);

            r3d_buffer_t buf;

            // Draw axes
            buf.type            = R3D_PRIMITIVE_LINES;
            buf.width           = 2.0f;
            buf.count           = sizeof(axis_lines) / (sizeof(v_point3d_t) * 2);
            buf.flags           = 0;

            buf.vertex.data     = &axis_lines[0].p;
            buf.vertex.stride   = sizeof(v_point3d_t);
            buf.normal.data     = NULL;
            buf.normal.stride   = sizeof(v_point3d_t);
            buf.color.data      = &axis_lines[0].c;
            buf.color.stride    = sizeof(v_point3d_t);
            buf.index.data      = NULL;

            // Draw call
            r3d->draw_primitives(&buf);

            // Render supplementary objects
            for (size_t i=0, n=area->num_objects3d(); i<n; ++i)
            {
                LSPObject3D *obj = area->object3d(i);
                if ((obj != NULL) && (obj->visible()))
                    obj->render(r3d);
            }

            // Draw scene primitives
            v_vertex3d_t *vv    = vVertexes.get_array();
            size_t nvertex      = vVertexes.size();

            // Fill buffer
            buf.type            = R3D_PRIMITIVE_TRIANGLES; //(bWireframe) ? R3D_PRIMITIVE_WIREFRAME_TRIANGLES : R3D_PRIMITIVE_TRIANGLES;
            buf.width           = 1.0f;
            buf.count           = nvertex / 3;
            buf.flags           = R3D_BUFFER_BLENDING | R3D_BUFFER_LIGHTING;

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

        bool CtlViewer3D::changed(KVTStorage *kvt, const char *id, const kvt_param_t *value)
        {
            if (::strstr(id, sKvtRoot.get_utf8()) != id)
                return false;

            bViewChanged    = true;
            pWidget->query_draw();
            return true;
        }
    
    } /* namespace ctl */
} /* namespace lsp */
