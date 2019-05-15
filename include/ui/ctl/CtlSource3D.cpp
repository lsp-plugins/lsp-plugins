/*
 * CtlSource3D.cpp
 *
 *  Created on: 14 мая 2019 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>
#include <plugins/room_builder.h>

namespace lsp
{
    namespace ctl
    {
        
        CtlSource3D::CtlSource3D(CtlRegistry *src, LSPMesh3D *widget): CtlWidget(src, widget)
        {
            fPosX       = 0.0f;
            fPosY       = 0.0f;
            fPosZ       = 0.0f;
            fYaw        = 0.0f;
            fPitch      = 0.0f;
            fRoll       = 0.0f;

            pMode       = NULL;
            pPosX       = NULL;
            pPosY       = NULL;
            pPosZ       = NULL;
            pYaw        = NULL;
            pPitch      = NULL;
            pRoll       = NULL;
            pCurvature  = NULL;
            pHeight     = NULL;
            pAngle      = NULL;

            dsp::init_matrix3d_identity(&sSource.pos);
            sSource.type    = RT_AS_ICO;
            sSource.size    = 1.0f;
            sSource.height  = 1.0f;
            sSource.angle   = 90.0f;
        }
        
        CtlSource3D::~CtlSource3D()
        {
        }
    
        void CtlSource3D::init()
        {
            CtlWidget::init();

            LSPMesh3D *mesh = widget_cast<LSPMesh3D>(pWidget);
            if (mesh != NULL)
                sColor.init_hsl2(pRegistry, pWidget, mesh->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
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
                case A_HEIGHT_ID:
                    BIND_PORT(pRegistry, pHeight, value);
                    break;

                default:
                    bool set = sColor.set(att, value);
                    if (!set)
                        CtlWidget::set(att, value);
                    break;
            }
        }

        void CtlSource3D::update_source_location()
        {
            LSPMesh3D *mesh = widget_cast<LSPMesh3D>(pWidget);
            if (mesh == NULL)
                return;

            matrix3d_t delta, m;

            // Compute rotation matrix
            dsp::init_matrix3d_translate(&delta, fPosX, fPosY, fPosZ);

            dsp::init_matrix3d_rotate_z(&m, fYaw * M_PI / 180.0f);
            dsp::apply_matrix3d_mm1(&delta, &m);

            dsp::init_matrix3d_rotate_y(&m, fPitch * M_PI / 180.0f);
            dsp::apply_matrix3d_mm1(&delta, &m);

            dsp::init_matrix3d_rotate_x(&m, fRoll * M_PI / 180.0f);
            dsp::apply_matrix3d_mm1(&delta, &m);

            // Commit matrix to mesh
            mesh->set_transform(&delta);
        }

        void CtlSource3D::update_mesh_data()
        {

        }

        void CtlSource3D::notify(CtlPort *port)
        {
            bool sync       = false;
            bool rebuild    = false;
            if (port == pPosX)
            {
                fPosX       = port->get_value();
                sync    = true;
            }
            if (port == pPosY)
            {
                fPosY       = port->get_value();
                sync    = true;
            }
            if (port == pPosZ)
            {
                fPosZ       = port->get_value();
                sync    = true;
            }
            if (port == pYaw)
            {
                fYaw        = port->get_value();
                sync    = true;
            }
            if (port == pPitch)
            {
                fPitch      = port->get_value();
                sync    = true;
            }
            if (port == pRoll)
            {
                fRoll       = port->get_value();
                sync    = true;
            }

            if (port == pMode)
            {
                sSource.type    = room_builder_base::decode_source_type(port->get_value());
                rebuild         = true;
            }
            if (port == pHeight)
            {
                sSource.height  = port->get_value();
                rebuild         = true;
            }
            if (port == pAngle)
            {
                sSource.angle   = port->get_value();
                rebuild         = true;
            }
            if (port == pCurvature)
            {
                sSource.curvature   = port->get_value();
                rebuild             = true;
            }

            if (sync)
                update_source_location();
            if (rebuild)
                update_mesh_data();
        }

    } /* namespace ctl */
} /* namespace lsp */
