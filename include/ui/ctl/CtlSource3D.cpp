/*
 * CtlSource3D.cpp
 *
 *  Created on: 14 мая 2019 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        
        CtlSource3D::CtlSource3D(CtlRegistry *src, LSPMesh3D *widget): CtlWidget(src, widget)
        {
            pMode       = NULL;
            pPosX       = NULL;
            pPosY       = NULL;
            pPosZ       = NULL;
            pYaw        = NULL;
            pPitch      = NULL;
            pRoll       = NULL;
            pRadius     = NULL;
            pExtRadius  = NULL;
            pHeight     = NULL;
            pAngle      = NULL;
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
                case A_RADIUS_ID:
                    BIND_PORT(pRegistry, pRadius, value);
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

        void CtlSource3D::notify(CtlPort *port)
        {

        }

    } /* namespace ctl */
} /* namespace lsp */
