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
            class CtlFloatPort: public CtlPort, public CtlKvtListener
            {
                protected:
                    room_builder_ui    *pUI;
                    const char         *sPattern;
                    osc::pattern_t      sOscPattern;
                    float               fValue;

                public:
                    explicit CtlFloatPort(room_builder_ui *ui, const char *pattern, const port_t *meta);
                    virtual ~CtlFloatPort();

                public:
                    virtual float get_value();
                    virtual void set_value(float value);
                    virtual bool changed(KVTStorage *storage, const char *id, const kvt_param_t *value);
                    virtual bool match(const char *id);
                    virtual const char *name();
            };

            class CtlListPort: public CtlPort, public CtlKvtListener
            {
                protected:
                    room_builder_ui    *pUI;
                    port_t              sMetadata;
                    port_item_t        *pItems;
                    size_t              nCapacity;
                    size_t              nItems;
                    cvector<CtlPort>    vKvtPorts;
                    osc::pattern_t      sOscPattern;
                    ssize_t             nSelectedReq;

                protected:
                    void set_list_item(size_t id, const char *value);

                public:
                    explicit CtlListPort(room_builder_ui *ui, const port_t *meta);
                    virtual ~CtlListPort();

                public:
                    virtual float get_value();
                    virtual void set_value(float value);
                    virtual bool changed(KVTStorage *storage, const char *id, const kvt_param_t *value);
                    virtual bool match(const char *id);
                    virtual const char *name();

                    void add_port(CtlPort *port);
            };

            class CtlMaterialPreset: public CtlPortListener
            {
                protected:
                    room_builder_ui    *pUI;
                    LSPComboBox        *pCBox;
                    ui_handler_id_t     hHandler;
                    CtlPort            *pSpeed;
                    CtlPort            *pAbsorption;
                    CtlPort            *pSelected;

                public:
                    explicit CtlMaterialPreset(room_builder_ui *ui);
                    virtual ~CtlMaterialPreset();

                    void init(const char *preset, const char *selected, const char *speed, const char *absorption);

                public:
                    virtual void notify(CtlPort *port);

                    static status_t slot_change(LSPWidget *sender, void *ptr, void *data);
            };

            class CtlKnobBinding: public CtlPortListener
            {
                protected:
                    room_builder_ui    *pUI;
                    CtlPort            *pOuter;
                    CtlPort            *pInner;
                    CtlPort            *pLink;
                    bool                bReverse;

                public:
                    explicit CtlKnobBinding(room_builder_ui *ui, bool reverse);
                    virtual ~CtlKnobBinding();

                    void init(const char *outer, const char *inner, const char *link);

                public:
                    virtual void notify(CtlPort *port);
            };

        protected:
            ssize_t                 nSelected;
            CtlMaterialPreset       sPresets;
            CtlKnobBinding          sAbsorption;
            CtlKnobBinding          sTransparency;
            CtlKnobBinding          sDispersion;
            CtlKnobBinding          sDiffuse;

        public:
            explicit room_builder_ui(const plugin_metadata_t *mdata, void *root_widget);
            virtual ~room_builder_ui();

        public:
            virtual status_t    init(IUIWrapper *wrapper, int argc, const char **argv);

            virtual status_t    build();
    };

} /* namespace lsp */

#endif /* UI_PLUGINS_ROOM_BUILDER_UI_H_ */
