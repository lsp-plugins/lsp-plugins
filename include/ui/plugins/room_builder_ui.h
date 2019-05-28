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
                float                   fEnabled;   // Enabled flag
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
                    size_t              nOffset;

                public:
                    explicit CtlFloatPort(room_builder_ui *ui, size_t offset, const char *pattern, const port_t *meta);
                    virtual ~CtlFloatPort();

                public:
                    virtual float get_value();
                    virtual void set_value(float value);

                    void                process_message(osc::parse_frame_t *message, size_t id, const char *path);
                    void                init_default(size_t id, obj_props_t *props);
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
                    virtual void set_value(float value);
                    void    sync_size();
                    void    sync_list();
            };

            class CtlOscListener: public CtlPortListener
            {
                protected:
                    room_builder_ui    *pUI;

                public:
                    explicit CtlOscListener(room_builder_ui *ui);
                    virtual ~CtlOscListener();

                public:
                    virtual void notify(CtlPort *port);
            };

        protected:
            void process_osc_message(const char *address, osc::parser_frame_t *message);
            void resize_properties(size_t count);
            void notify_osc_all();

        protected:
            cvector<CtlFloatPort>   vOscPorts;
            cstorage<obj_props_t>   vObjectProps;
            size_t                  nSelected;
            CtlPort                *pOscIn;
            CtlPort                *pOscOut;
            CtlListPort            *pListPort;
            CtlOscListener          sOscListener;

        public:
            explicit room_builder_ui(const plugin_metadata_t *mdata, void *root_widget);
            virtual ~room_builder_ui();

            virtual void destroy();

        public:
            virtual status_t    init(IUIWrapper *wrapper, int argc, const char **argv);
    };

} /* namespace lsp */

#endif /* UI_PLUGINS_ROOM_BUILDER_UI_H_ */
