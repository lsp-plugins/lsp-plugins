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

#include <ui/ws/ws.h>
#include <core/files/config.h>
#include <core/io/IInStream.h>

namespace lsp
{
    class plugin_ui: public CtlRegistry
    {
        protected:
            class ConfigHandler: public config::IConfigHandler
            {
                private:
                    plugin_ui   *pUI;
                    cvector<CtlPort> &hPorts;

                public:
                    ConfigHandler(plugin_ui *ui, cvector<CtlPort> &ports): pUI(ui), hPorts(ports) {}

                public:
                    virtual status_t handle_parameter(const char *name, const char *value);
            };

            class ConfigSource: public config::IConfigSource
            {
                private:
                    plugin_ui   *pUI;
                    cvector<CtlPort> &hPorts;
                    LSPString   *pComment;
                    size_t       nPortID;

                public:
                    ConfigSource(plugin_ui *ui, cvector<CtlPort> &ports, LSPString *comment):
                        pUI(ui), hPorts(ports), pComment(comment), nPortID(0) {}

                public:
                    virtual status_t get_head_comment(LSPString *comment);

                    virtual status_t get_parameter(LSPString *name, LSPString *value, LSPString *comment, int *flags);
            };

        protected:
            const plugin_metadata_t    *pMetadata;
            IUIWrapper                 *pWrapper;
            LSPWindow                  *pRoot;
            void                       *pRootWidget;

            LSPDisplay                  sDisplay;

            cvector<CtlPort>            vPorts;
            cvector<CtlPort>            vSortedPorts;
            cvector<CtlPort>            vConfigPorts;
            cvector<CtlValuePort>       vTimePorts;
            cvector<LSPWidget>          vWidgets;
            cvector<CtlSwitchedPort>    vSwitched;
            cvector<CtlPortAlias>       vAliases;

        protected:
            static const port_t         vConfigMetadata[];
            static const port_t         vTimeMetadata[];

        protected:
            size_t          rebuild_sorted_ports();
            CtlWidget      *build_widget(widget_ctl_t w_class);
            io::File       *open_config_file(bool write);
            bool            apply_changes(const char *key, const char *value, cvector<CtlPort> &ports);

        public:
            explicit plugin_ui(const plugin_metadata_t *mdata, void *root_widget);
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

            /** Method executed when the time position of plugin was updated
             *
             */
            void position_updated(const position_t *pos);

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
            inline status_t main_iteration() { return sDisplay.main_iteration(); }

            void sync_meta_ports();

            /** Return root window
             *
             * @return root window (if exists)
             */
            inline LSPWindow *root_window() { return pRoot; }

            void set_title(const char *title);
    };

} /* namespace lsp */

#endif /* _UI_PLUGIN_UI_H_ */
