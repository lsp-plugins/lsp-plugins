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
                    char              **pItems;
                    size_t              nCapacity;
                    size_t              nItems;
                    cvector<CtlPort>    vKvtPorts;
                    osc::pattern_t      sOscPattern;

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

        protected:
            ssize_t                 nSelected;

        public:
            explicit room_builder_ui(const plugin_metadata_t *mdata, void *root_widget);
            virtual ~room_builder_ui();

        public:
            virtual status_t    init(IUIWrapper *wrapper, int argc, const char **argv);
    };

} /* namespace lsp */

#endif /* UI_PLUGINS_ROOM_BUILDER_UI_H_ */
