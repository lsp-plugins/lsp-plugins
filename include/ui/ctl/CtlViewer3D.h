/*
 * CtlViewer3D.h
 *
 *  Created on: 25 апр. 2019 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLVIEWER3D_H_
#define UI_CTL_CTLVIEWER3D_H_

#include <core/3d/common.h>
#include <core/3d/Scene3D.h>
#include <data/cstorage.h>

namespace lsp
{
    namespace ctl
    {
        /**
         * 3D Model viewer
         */
        class CtlViewer3D: public CtlWidget
        {
            protected:
                typedef struct pov_angles_t
                {
                    float                   fYaw;
                    float                   fPitch;
                    float                   fRoll;
                } pov_angles_t;

            protected:
                CtlColor        sColor;
                CtlColor        sBgColor;
                CtlPadding      sPadding;
//                LSPTimer        sTimer;

                CtlPort        *pPath;

                bool            bViewChanged;

                char           *pPathID;
                Scene3D         sScene;
                cstorage<v_vertex3d_t>      vVertexes;  // Vertexes of the scene

                // Camera position
                point3d_t       sPov;           // Point-of-view for the camera
                pov_angles_t    sAngles;        // Yaw, pitch, roll
                pov_angles_t    sOldAngles;     // Old angles
                vector3d_t      sTop;           // Top-of-view for the camera
                vector3d_t      sDir;           // Direction-of-view for the camera
                vector3d_t      sSide;          // Side-of-view for the camera

            protected:
                static status_t slot_on_draw3d(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_resize(LSPWidget *sender, void *ptr, void *data);

                static status_t redraw_area(timestamp_t ts, void *arg);

                void    commit_view(IR3DBackend *r3d);
                void    update_camera_state();
                void    update_frustum();

            public:
                explicit CtlViewer3D(CtlRegistry *src, LSPArea3D *widget);
                virtual ~CtlViewer3D();

            public:
                virtual void init();

                virtual status_t    on_draw3d(IR3DBackend *r3d);

                virtual void set(widget_attribute_t att, const char *value);

                virtual void notify(CtlPort *port);

                virtual void end();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLVIEWER3D_H_ */
