/*
 * room_builder_ui.h
 *
 *  Created on: 28 мая 2019 г.
 *      Author: sadko
 */

#ifndef UI_PLUGINS_ROOM_BUILDER_UI_H_
#define UI_PLUGINS_ROOM_BUILDER_UI_H_

#include <ui/ui.h>
#include <ui/ctl/ctl.h>

namespace lsp
{
    
    class room_builder_ui: public plugin_ui
    {
        protected:
            typedef struct obj_props_t
            {
                char                   *sName;      // UTF-8 object name
                point3d_t               sPos;       // Object relative position
                float                   fYaw;       // Yaw
                float                   fPitch;     // Pitch
                float                   fRoll;      // Roll
                float                   fSizeX;     // Size of object (X)
                float                   fSizeY;     // Size of object (Y)
                float                   fSizeZ;     // Size of object (Z)
                float                   fHue;       // Hue color
                float                   fAbsorption[2];
                float                   fDispersion[2];
                float                   fDissipation[2];
                float                   fTransparency[2];
                float                   fSndSpeed;
            } obj_props_t;

            class CtlFloatPort: public CtlPort
            {
                protected:
                    room_builder_ui    *pUI;
                    const char         *sPattern;
                    float               fOldValue;
                    size_t              nOffset;

                public:
                    explicit CtlFloatPort(room_builder_ui *ui, size_t offset, const char *pattern, const port_t *meta);
                    virtual ~CtlFloatPort();

                public:
                    virtual float get_value();
                    virtual void set_value(float value);

                    void                osc_sync();
            };

            class CtlListPort: public CtlPort
            {
                protected:
                    room_builder_ui    *pUI;
                    port_t              sMetadata;
                    char              **pItems;
                    size_t              nCapacity;

                public:
                    explicit CtlListPort(room_builder_ui *ui, const port_t *meta);
                    virtual ~CtlListPort();

                public:
                    virtual float get_value();
                    void    sync_size();
                    void    sync_list();
            };

        protected:
            cvector<CtlPort>        vOscPorts;
            cstorage<obj_props_t>   vObjectProps;
            size_t                  nSelected;
            CtlPort                *pOscOut;

        public:
            explicit room_builder_ui(const plugin_metadata_t *mdata, void *root_widget);
            virtual ~room_builder_ui();

        public:
            virtual status_t    init(IUIWrapper *wrapper, int argc, const char **argv);
    };

} /* namespace lsp */

#endif /* UI_PLUGINS_ROOM_BUILDER_UI_H_ */
