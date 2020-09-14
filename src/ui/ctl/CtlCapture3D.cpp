/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 12 мая 2019 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#include <ui/ui.h>
#include <plugins/room_builder.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlCapture3D::metadata = { "CtlCapture3D", &CtlWidget::metadata };
        
        void CtlCapture3D::LSPCaptureColor::color_changed()
        {
            LSPCapture3D *cap = widget_cast<LSPCapture3D>(pCapture->pWidget);
            if (cap == NULL)
                return;

            LSPColor c;
            c.copy(color());
            cap->color()->copy(&c);

            c.hue(fmodf(c.hue() + pCapture->fHueShift, 1.0f));
            cap->axis_color()->copy(&c);
        }

        CtlCapture3D::CtlCapture3D(CtlRegistry *src, LSPCapture3D *widget):
            CtlWidget(src, widget),
            sXColor(this)
        {
            pClass          = &metadata;
            fHueShift       = 0.25f;
            sXColor.set_rgb(1.0f, 0.0f, 0.0f);

            dsp::init_point_xyz(&sCapture.sPos, 0.0f, 0.0f, 0.0f);
            sCapture.fYaw       = 0.0f;
            sCapture.fPitch     = 0.0f;
            sCapture.fRoll      = 0.0f;
            sCapture.fCapsule   = 0.015f;
            sCapture.sConfig    = RT_CC_MONO;
            sCapture.fAngle     = 0.0f;
            sCapture.fDistance  = 0.0f;
            sCapture.enDirection= RT_AC_OMNI;
            sCapture.enSide     = RT_AC_OMNI;

            pPosX           = NULL;
            pPosY           = NULL;
            pPosZ           = NULL;
            pYaw            = NULL;
            pPitch          = NULL;
            pRoll           = NULL;
            pSize           = NULL;
            pMode           = NULL;
            pAngle          = NULL;
            pDistance       = NULL;
        }
        
        CtlCapture3D::~CtlCapture3D()
        {
        }

        void CtlCapture3D::init()
        {
            CtlWidget::init();

            LSPCapture3D *cap = widget_cast<LSPCapture3D>(pWidget);
            if (cap != NULL)
                sXColor.copy(cap->color());
            sColor.init_hsl(pRegistry, pWidget, &sXColor, A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
        }

        void CtlCapture3D::set(widget_attribute_t att, const char *value)
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
                case A_SIZE_ID:
                    BIND_PORT(pRegistry, pSize, value);
                    break;
                case A_MODE_ID:
                    BIND_PORT(pRegistry, pMode, value);
                    break;
                case A_ANGLE_ID:
                    BIND_PORT(pRegistry, pAngle, value);
                    break;
                case A_DISTANCE_ID:
                    BIND_PORT(pRegistry, pDistance, value);
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

        void CtlCapture3D::end()
        {
            sXColor.color_changed();
            CtlWidget::end();
        }

        void CtlCapture3D::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            bool sync = false;
            if (port == pPosX)
            {
                sCapture.sPos.x     = port->get_value();
                sync    = true;
            }
            if (port == pPosY)
            {
                sCapture.sPos.y     = port->get_value();
                sync    = true;
            }
            if (port == pPosZ)
            {
                sCapture.sPos.z     = port->get_value();
                sync    = true;
            }
            if (port == pYaw)
            {
                sCapture.fYaw       = port->get_value();
                sync    = true;
            }
            if (port == pPitch)
            {
                sCapture.fPitch     = port->get_value();
                sync    = true;
            }
            if (port == pRoll)
            {
                sCapture.fRoll      = port->get_value();
                sync    = true;
            }
            if (port == pSize)
            {
                sCapture.fCapsule   = port->get_value() * 0.5f;
                sync    = true;
            }
            if (port == pMode)
            {
                sCapture.sConfig    = room_builder_base::decode_config(port->get_value());
                sync    = true;
            }
            if (port == pAngle)
            {
                sCapture.fAngle     = port->get_value();
                sync    = true;
            }
            if (port == pDistance)
            {
                sCapture.fDistance  = port->get_value();
                sync    = true;
            }

            if (sync)
                sync_capture_state();
        }

        void CtlCapture3D::sync_capture_state()
        {
            // Get widget
            LSPCapture3D *cap = widget_cast<LSPCapture3D>(pWidget);
            if (cap == NULL)
                return;

            // Configure capture settings
            size_t n = 0;
            rt_capture_settings_t cset[2];
            status_t res = rt_configure_capture(&n, cset, &sCapture);
            if (res != STATUS_OK)
                return;

            // Commit capture settings
            res = cap->set_items(2);
            if (res != STATUS_OK)
                return;

            for (size_t i=0; i<2; ++i)
            {
                cap->set_radius(i, cset[i].radius);
                cap->set_transform(i, &cset[i].pos);
                cap->set_enabled(i, i < n);
            }
        }
    
    } /* namespace ctl */
} /* namespace lsp */
