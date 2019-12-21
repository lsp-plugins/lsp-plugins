/*
 * CtlSource3D.h
 *
 *  Created on: 14 мая 2019 г.
 *      Author: sadko
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
