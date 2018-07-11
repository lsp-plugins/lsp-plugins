/*
 * plugin_ui.h
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_PLUGIN_UI_H_
#define _UI_PLUGIN_UI_H_

#include <metadata/metadata.h>

#include <data/cvector.h>

#include <ui/IUIWrapper.h>

namespace lsp
{
    class UISwitchedPort;

    class plugin_ui
    {
        private:
            cvector<IWidget>            vWidgets;
            cvector<IUIPort>            vPorts;
            cvector<IUIPort>            vSortedPorts;
            cvector<UISwitchedPort>     vSwitched;
            cvector<PortAlias>          vAliases;
            cvector<IWidget>            vRedraw[2];
            Theme                       sTheme;
            size_t                      nRedrawFrame;

        protected:
            const plugin_metadata_t    *pMetadata;
            IWidgetFactory             *pFactory;
            IUIWrapper                 *pWrapper;

        protected:
            bool apply_changes(const char *key, const char *value);
            size_t rebuild_sorted_ports();

        public:
            plugin_ui(const plugin_metadata_t *mdata, IWidgetFactory *factory);

            virtual ~plugin_ui();

        private:
            IWidget *addWidget(IWidget *widget);

        public:
            IWidget *createWidget(const char *w_class);
            IWidget *createWidget(widget_t w_class);
            inline const plugin_metadata_t *metadata() const { return pMetadata; };
            inline Theme &theme() { return sTheme; };
            inline IUIWrapper *getWrapper() { return pWrapper; };

        public:
            /** Initialize UI
             *
             * @param wrapper UI wrapper
             */
            void init(IUIWrapper *wrapper);

            /** Build UI from XML file
             *
             */
            void build();

            /** Destroy UI
             *
             */
            void destroy();

            /** Add plugin port to UI
             *
             * @param port UI port to communicate with plugin
             * @return status of operation
             */
            bool add_port(IUIPort *port);

            /** Export settings of the UI to the file
             *
             * @param filename file name
             * @return status of operation
             */
            bool export_settings(const char *filename);

            /** Import settings of the UI from the file
             *
             * @param filename file name
             * @return status of operation
             */
            bool import_settings(const char *filename);

            /** Get INTERNAL port by name
             *
             * @param name port name
             * @return internal port
             */
            IUIPort *port(const char *name);

            /** Redraw pending UI widgets
             *
             */
            void redraw();

            /** Queue widget for redraw
             *
             * @param widget widget to redraw
             */
            bool queue_redraw(IWidget *widget);

            /** Get port count
             *
             * @return number of ports
             */
            inline size_t ports_count()
            {
                return vPorts.size();
            }
    };

} /* namespace lsp */

#endif /* _UI_PLUGIN_UI_H_ */
