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
            class CtlFloatPort: public CtlPort
            {
                protected:
                    room_builder_ui    *pUI;
                    const char         *sPattern;

                public:
                    explicit CtlFloatPort(room_builder_ui *ui, const char *pattern, const port_t *meta);
                    virtual ~CtlFloatPort();

                public:
                    virtual float get_value();
                    virtual void set_value(float value);
            };

            class CtlListPort: public CtlPort, KVTListener
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
                    virtual void changed(const char *id, const kvt_param_t *oval, const kvt_param_t *nval);
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
            void notify_kvt_ports();

        protected:
            size_t                  nSelected;
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
