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
        class CtlViewer3D: public CtlWidget, public CtlKvtListener
        {
            public:
                static const ctl_class_t metadata;

            protected:
                typedef struct pov_angles_t
                {
                    float                   fYaw;
                    float                   fPitch;
                    float                   fRoll;
                } pov_angles_t;

            protected:
                CtlColor        sColor;
                CtlColor        sBaseColor;
                CtlPadding      sPadding;

                CtlPort        *pFile;
                CtlPort        *pStatus;

                CtlPort        *pPosX;
                CtlPort        *pPosY;
                CtlPort        *pPosZ;
                CtlPort        *pYaw;
                CtlPort        *pPitch;
                CtlPort        *pScaleX;
                CtlPort        *pScaleY;
                CtlPort        *pScaleZ;
                CtlPort        *pOrientation;

                bool            bViewChanged;
                float           fOpacity;
                float           fFov;
                matrix3d_t      sOrientation;

                Scene3D         sScene;
                LSPString       sKvtRoot;
                cstorage<v_vertex3d_t>      vVertexes;  // Vertexes of the scene

                // Camera position
                point3d_t       sPov;           // Point-of-view for the camera
                point3d_t       sOldPov;        // Old point of view
                vector3d_t      sScale;         // Scene scaling
                pov_angles_t    sAngles;        // Yaw, pitch, roll
                pov_angles_t    sOldAngles;     // Old angles
                vector3d_t      sTop;           // Top-of-view for the camera
                vector3d_t      sXTop;          // Updated top-of-view for the camera
                vector3d_t      sDir;           // Direction-of-view for the camera
                vector3d_t      sSide;          // Side-of-view for the camera

                // Mouse events
                size_t          nBMask;         // Button mask
                ssize_t         nMouseX;        // Mouse X position
                ssize_t         nMouseY;        // Mouse Y position

            protected:
                static status_t slot_on_draw3d(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_resize(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_mouse_down(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_mouse_up(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_mouse_move(LSPWidget *sender, void *ptr, void *data);

            protected:
                static status_t redraw_area(timestamp_t ts, void *arg);

                void    commit_view(IR3DBackend *r3d);
                void    update_camera_state();
                void    update_frustum();
                void    rotate_camera(ssize_t dx, ssize_t dy);
                void    move_camera(ssize_t dx, ssize_t dy, ssize_t dz);

                static  float get_delta(CtlPort *p, float dfl);
                static  float get_adelta(CtlPort *p, float dfl);
                void    submit_pov_change(float *vold, float vnew, CtlPort *port);
                void    submit_angle_change(float *vold, float vnew, CtlPort *port);
                void    sync_pov_change(float *dst, CtlPort *port, CtlPort *psrc);
                void    sync_scale_change(float *dst, CtlPort *port, CtlPort *psrc);
                void    sync_angle_change(float *dst, CtlPort *port, CtlPort *psrc);

            public:
                explicit CtlViewer3D(CtlRegistry *src, LSPArea3D *widget);
                virtual ~CtlViewer3D();

                virtual void init();

            public:
                virtual status_t    on_draw3d(IR3DBackend *r3d);

                virtual void set(widget_attribute_t att, const char *value);

                virtual void notify(CtlPort *port);

                virtual status_t add(CtlWidget *child);

                virtual bool changed(KVTStorage *kvt, const char *id, const kvt_param_t *value);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLVIEWER3D_H_ */
