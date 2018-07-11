/*
 * old_plugin_ui.h
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_PLUGIN_UI_H_
#define _UI_PLUGIN_UI_H_

#include <metadata/metadata.h>

#include <data/cvector.h>

#include <ui/ws/ws.h>

namespace lsp
{
    class plugin_ui: public CtlRegistry
    {
        protected:
            const plugin_metadata_t    *pMetadata;
            IUIWrapper                 *pWrapper;
            LSPWindow                  *pRoot;
            void                       *pRootWidget;

            LSPDisplay                  sDisplay;

            cvector<CtlPort>            vPorts;
            cvector<CtlPort>            vSortedPorts;
            cvector<CtlPort>            vConfigPorts;
            cvector<LSPWidget>          vWidgets;
            cvector<CtlSwitchedPort>    vSwitched;
            cvector<CtlPortAlias>       vAliases;

        protected:
            static const port_t         vConfigMetadata[];

        protected:
            size_t          rebuild_sorted_ports();
            CtlWidget      *build_widget(widget_ctl_t w_class);
            FILE           *open_config_file(bool write);
            bool            create_directory(const char *path);
            void            serialize_ports(FILE *fd, cvector<CtlPort> &ports);
            bool            deserialize_ports(FILE *fd, cvector<CtlPort> &ports);
            bool            apply_changes(const char *key, const char *value, cvector<CtlPort> &ports);

        public:
            plugin_ui(const plugin_metadata_t *mdata, void *root_widget);
            virtual ~plugin_ui();

            virtual void destroy();

        public:
            inline const plugin_metadata_t *metadata() const { return pMetadata; };
            inline IUIWrapper *wrapper() { return pWrapper; };

        public:
            CtlWidget      *create_widget(const char *w_ctl);
            CtlWidget      *create_widget(widget_ctl_t w_class);

        public:
            /** Initialize UI
             *
             * @param root_widget root widget to use as a base
             * @param wrapper plugin wrapper
             * @param argc number of arguments
             * @param argv list of arguments
             * @return status of operation
             */
            status_t    init(IUIWrapper *wrapper, int argc, const char **argv);

            /** Add plugin port to UI
             *
             * @param port UI port to communicate with plugin
             * @return status of operation
             */
            status_t add_port(CtlPort *port);

            /** Export settings of the UI to the file
             *
             * @param filename file name
             * @return status of operation
             */
            status_t export_settings(const char *filename);

            /** Import settings of the UI from the file
             *
             * @param filename file name
             * @return status of operation
             */
            status_t import_settings(const char *filename);

            /** Save global configuration file
             *
             * @return status of operation
             */
            status_t save_global_config();

            /** Load global configuration file
             *
             * @return status of operation
             */
            status_t load_global_config();

            /** Get INTERNAL port by name
             *
             * @param name port name
             * @return internal port
             */
            CtlPort *port(const char *name);

            /** Get port count
             *
             * @return number of ports
             */
            inline size_t ports_count()
            {
                return vPorts.size();
            }

            /** Show UI
             *
             */
            inline void show()
            {
                if (pRoot != NULL)
                    pRoot->show();
            }

            /** Hide UI
             *
             */
            inline void hide()
            {
                if (pRoot != NULL)
                    pRoot->hide();
            }

            /** Get width of main plugin window
             *
             * @return width of main plugin window
             */
            inline size_t width()   { return (pRoot != NULL) ? pRoot->width() : 0; }

            /** Get height of main plugin window
             *
             * @return height of main plugin window
             */
            inline size_t height()  { return (pRoot != NULL) ? pRoot->height() : 0; }

            /** Get Display
             *
             * @return display
             */
            inline LSPDisplay *display()    { return &sDisplay; };

            /** Main UI function
             *
             * @return main function
             */
            inline status_t main() { return sDisplay.main(); };

            /** Main iteration
             *
             * @return main iteration
             */
            inline status_t main_iteration() { return sDisplay.main_iteration(); };

            /** Return root window
             *
             * @return root window (if exists)
             */
            inline LSPWindow *root_window() { return pRoot; }

            void set_title(const char *title);
    };

} /* namespace lsp */

#endif /* _UI_PLUGIN_UI_H_ */
