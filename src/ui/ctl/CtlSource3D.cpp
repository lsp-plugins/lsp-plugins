/*
 * CtlSource3D.cpp
 *
 *  Created on: 14 мая 2019 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>
#include <core/3d/raytrace.h>
#include <plugins/room_builder.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlSource3D::metadata = { "CtlSource3D", &CtlWidget::metadata };

        void CtlSource3D::LSPSourceColor::color_changed()
        {
            LSPMesh3D *mesh = widget_cast<LSPMesh3D>(pSource->pWidget);
            if (mesh == NULL)
                return;

            LSPColor c;
            c.copy(color());
            mesh->color()->copy(&c);

            c.hue(fmodf(c.hue() + pSource->fHueShift, 1.0f));
            mesh->line_color()->copy(&c);
        }
        
        CtlSource3D::CtlSource3D(CtlRegistry *src, LSPMesh3D *widget):
            CtlWidget(src, widget),
            sXColor(this)
        {
            pClass          = &metadata;
            fHueShift       = 0.1f;
            sXColor.set_rgb(1.0f, 0.0f, 0.0f);

            bRebuildMesh= true;

            pMode           = NULL;
            pPosX           = NULL;
            pPosY           = NULL;
            pPosZ           = NULL;
            pYaw            = NULL;
            pPitch          = NULL;
            pSize           = NULL;
            pRoll           = NULL;
            pCurvature      = NULL;
            pHeight         = NULL;
            pAngle          = NULL;

            dsp::init_point_xyz(&sSource.sPos, 0.0f, 0.0f, 0.0f);
            sSource.fYaw        = 0.0f;
            sSource.fPitch      = 0.0f;
            sSource.fRoll       = 0.0f;
            sSource.enType      = RT_AS_ICO;
            sSource.fSize       = 1.0f;
            sSource.fHeight     = 1.0f;
            sSource.fAngle      = 90.0f;
            sSource.fCurvature  = 100.0f;
            sSource.fAmplitude  = 1.0f;
        }
        
        CtlSource3D::~CtlSource3D()
        {
        }
    
        void CtlSource3D::init()
        {
            CtlWidget::init();

            LSPMesh3D *mesh = widget_cast<LSPMesh3D>(pWidget);
            if (mesh != NULL)
                sColor.init_hsl(pRegistry, pWidget, &sXColor, A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);

            mesh->slots()->bind(LSPSLOT_DRAW3D, slot_on_draw3d, this);
        }

        void CtlSource3D::set(widget_attribute_t att, const char *value)
        {
            switch (att)
            {
                case A_XPOS_ID:
                    BIND_PORT(pRegistry, pPosX, value);
                    break;
                case A_YPOS_ID:
                    BIND_PORT(pRegistry, pPosY, value);
                    break;
                case A_ZPOS_ID:
                    BIND_PORT(pRegistry, pPosZ, value);
                    break;
                case A_YAW_ID:
                    BIND_PORT(pRegistry, pYaw, value);
                    break;
                case A_PITCH_ID:
                    BIND_PORT(pRegistry, pPitch, value);
                    break;
                case A_ROLL_ID:
                    BIND_PORT(pRegistry, pRoll, value);
                    break;
                case A_CURVATURE_ID:
                    BIND_PORT(pRegistry, pCurvature, value);
                    break;
                case A_MODE_ID:
                    BIND_PORT(pRegistry, pMode, value);
                    break;
                case A_ANGLE_ID:
                    BIND_PORT(pRegistry, pAngle, value);
                    break;
                case A_SIZE_ID:
                    BIND_PORT(pRegistry, pSize, value);
                    break;
                case A_HEIGHT_ID:
                    BIND_PORT(pRegistry, pHeight, value);
                    break;
                case A_HUE_SHIFT:
                    PARSE_FLOAT(value, fHueShift = __);
                    break;

                default:
                    bool set = sColor.set(att, value);
                    if (!set)
                        CtlWidget::set(att, value);
                    break;
            }
        }

        void CtlSource3D::end()
        {
            sXColor.color_changed();
            CtlWidget::end();
        }

        void CtlSource3D::update_source_location()
        {
            LSPMesh3D *mesh = widget_cast<LSPMesh3D>(pWidget);
            if (mesh == NULL)
                return;

            rt_source_settings_t settings;
            if (rt_configure_source(&settings, &sSource) != STATUS_OK)
                return;

            // Commit matrix to mesh
            mesh->set_transform(&settings.pos);
            mesh->query_draw();
        }

        status_t CtlSource3D::slot_on_draw3d(LSPWidget *sender, void *ptr, void *data)
        {
            CtlSource3D *_this = static_cast<CtlSource3D *>(ptr);
            if (_this == NULL)
                return STATUS_BAD_STATE;

            _this->update_mesh_data();
            return STATUS_OK;
        }

        void CtlSource3D::update_mesh_data()
        {
            if (!bRebuildMesh)
                return;

            LSPMesh3D *mesh = widget_cast<LSPMesh3D>(pWidget);
            if (mesh == NULL)
                return;

            rt_source_settings_t settings;
            if (rt_configure_source(&settings, &sSource) != STATUS_OK)
                return;

            // Generate source mesh depending on current configuration
            cstorage<rt_group_t> groups;
            status_t res    = rt_gen_source_mesh(groups, &settings);
            if (res != STATUS_OK)
                return;

            // Now we need to process the mesh and submit it to the mesh
            cstorage<point3d_t> vp;
            cstorage<point3d_t> vn;

            // Allocate space for triangles
            size_t  nt      = groups.size();
            point3d_t *dp   = vp.append_n(nt * 3); // 1 triangle x 3 vertices
            if (dp == NULL)
                return;
            point3d_t *dl   = vn.append_n(nt * 6); // 3 lines x 2 vertices
            if (dl == NULL)
                return;

            const rt_group_t *grp   = groups.get_array();

            // Generate the final data
            vector3d_t dv[3];
            for (size_t i=0; i<nt; ++i, ++grp, dp += 3, dl += 6)
            {
                dp[0]   = grp->p[0];
                dp[1]   = grp->p[1];
                dp[2]   = grp->p[2];

                dl[0]   = grp->p[0];
                dl[2]   = grp->p[1];
                dl[4]   = grp->p[2];

                dsp::init_vector_p2(&dv[0], &grp->s, &grp->p[0]);
                dsp::init_vector_p2(&dv[1], &grp->s, &grp->p[1]);
                dsp::init_vector_p2(&dv[2], &grp->s, &grp->p[2]);

                dsp::normalize_vector(&dv[0]);
                dsp::normalize_vector(&dv[1]);
                dsp::normalize_vector(&dv[2]);

                dsp::add_vector_pvk2(&dl[1], &grp->p[0], &dv[0], 0.25f);
                dsp::add_vector_pvk2(&dl[3], &grp->p[1], &dv[1], 0.25f);
                dsp::add_vector_pvk2(&dl[5], &grp->p[2], &dv[2], 0.25f);
            }

            // Apply this data as layers
            mesh->clear();
            res = mesh->add_triangles(vp.get_array(), NULL, vp.size());
            if (res != STATUS_OK)
                return;

            res = mesh->add_lines(vn.get_array(), vn.size());
            if (res != STATUS_OK)
                return;

            // Cleanup rebuild flag
            bRebuildMesh    = false;
        }

        void CtlSource3D::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            bool sync       = false;
            bool rebuild    = false;

            if (port == pPosX)
            {
                sSource.sPos.x  = port->get_value();
                sync            = true;
            }
            if (port == pPosY)
            {
                sSource.sPos.y  = port->get_value();
                sync            = true;
            }
            if (port == pPosZ)
            {
                sSource.sPos.z  = port->get_value();
                sync            = true;
            }
            if (port == pYaw)
            {
                sSource.fYaw    = port->get_value();
                sync            = true;
            }
            if (port == pPitch)
            {
                sSource.fPitch  = port->get_value();
                sync            = true;
            }
            if (port == pRoll)
            {
                sSource.fRoll   = port->get_value();
                sync            = true;
            }

            if (port == pMode)
            {
                sSource.enType  = room_builder_base::decode_source_type(port->get_value());
                rebuild         = true;
            }
            if (port == pHeight)
            {
                sSource.fHeight = port->get_value() * 0.01f;    // cm -> m
                rebuild         = true;
            }
            if (port == pAngle)
            {
                sSource.fAngle  = port->get_value();
                rebuild         = true;
            }
            if (port == pCurvature)
            {
                sSource.fCurvature  = port->get_value();
                rebuild             = true;
            }
            if (port == pSize)
            {
                sSource.fSize       = port->get_value() * 0.01 * 0.5f;    // cm -> m, diameter -> radius
                rebuild             = true;
            }

            if (sync)
                update_source_location();
            if ((rebuild) && (!bRebuildMesh))
            {
                bRebuildMesh    = true;
                pWidget->query_draw();
            }
        }

    } /* namespace ctl */
} /* namespace lsp */
