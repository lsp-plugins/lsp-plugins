/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 14 мая 2019 г.
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

#ifndef UI_CTL_CTLSOURCE3D_H_
#define UI_CTL_CTLSOURCE3D_H_

namespace lsp
{
    namespace ctl
    {

        class CtlSource3D: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                class LSPSourceColor: public LSPColor
                {
                    protected:
                        CtlSource3D    *pSource;
                        friend class CtlSource3D;

                    protected:
                        virtual void    color_changed();

                    public:
                        inline LSPSourceColor(CtlSource3D *source) { pSource = source; }
                };

            protected:
                LSPSourceColor  sXColor;
                CtlColor        sColor;
                float           fHueShift;

                room_source_config_t sSource;

                bool            bRebuildMesh;

                CtlPort        *pMode;
                CtlPort        *pPosX;
                CtlPort        *pPosY;
                CtlPort        *pPosZ;
                CtlPort        *pYaw;
                CtlPort        *pPitch;
                CtlPort        *pRoll;
                CtlPort        *pSize;
                CtlPort        *pCurvature;
                CtlPort        *pHeight;
                CtlPort        *pAngle;

            protected:
                void        update_source_location();
                void        update_mesh_data();

                static status_t slot_on_draw3d(LSPWidget *sender, void *ptr, void *data);

            public:
                explicit CtlSource3D(CtlRegistry *src, LSPMesh3D *widget);
                virtual ~CtlSource3D();

            public:
                virtual void init();

                virtual void set(widget_attribute_t att, const char *value);

                virtual void notify(CtlPort *port);

                virtual void end();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLSOURCE3D_H_ */
