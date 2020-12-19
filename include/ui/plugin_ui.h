/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 20 окт. 2015 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _UI_PLUGIN_UI_H_
#define _UI_PLUGIN_UI_H_

#include <metadata/metadata.h>

#include <data/cvector.h>

#include <ui/ws/ws.h>
#include <core/io/IInStream.h>
#include <core/files/config.h>
#include <core/port_data.h>

namespace lsp
{
    class plugin_ui: public CtlRegistry
    {
        private:
            plugin_ui &operator = (const plugin_ui &);

        protected:
            typedef struct preset_t
            {
                char       *name;
                char       *path;
                LSPWidget  *item;
            } preset_t;

            class ConfigHandler: public config::IConfigHandler
            {
                private:
                    plugin_ui          *pUI;
                    cvector<CtlPort>   &hPorts;
                    KVTStorage         *pKVT;
                    cvector<char>       vNotify;
                    bool                bPreset;
                    io::Path           *pBasePath;

                protected:
                    void add_notification(const char *id);

                public:
                    explicit ConfigHandler(plugin_ui *ui, cvector<CtlPort> &ports,
                            KVTStorage *kvt, bool preset, io::Path *base
                    ):
                        pUI(ui), hPorts(ports), pKVT(kvt), bPreset(preset), pBasePath(base) {}
                    virtual ~ConfigHandler();

                public:
                    virtual status_t handle_parameter(const char *name, const char *value, size_t flags);

                    virtual status_t handle_kvt_parameter(const char *name, const kvt_param_t *param, size_t flags);

                    void notify_all();
            };

            class ConfigSource: public config::IConfigSource
            {
                private:
                    plugin_ui      *pUI;
                    cvector<CtlPort> &hPorts;
                    LSPString      *pComment;
                    KVTIterator    *pIter;
                    size_t          nPortID;
                    io::Path       *pBasePath;

                public:
                    explicit ConfigSource(plugin_ui *ui, cvector<CtlPort> &ports,
                            KVTStorage *kvt, LSPString *comment, io::Path *base
                        ):
                        pUI(ui), hPorts(ports), pComment(comment), nPortID(0) {
                        pIter       = (kvt != NULL) ? kvt->enum_all() : NULL;
                        pBasePath   = base;
                    }

                public:
                    virtual status_t get_head_comment(LSPString *comment);

                    virtual status_t get_parameter(LSPString *name, LSPString *value, LSPString *comment, int *flags);
            };

            class ConfigSink: public LSPTextDataSink
            {
                private:
                    plugin_ui      *pUI;

                public:
                    explicit ConfigSink(plugin_ui *ui): pUI(ui) {}
                    virtual ~ConfigSink();

                    void unbind();

                public:
                    virtual status_t    on_complete(status_t code, const LSPString *data);
            };

        protected:
            const plugin_metadata_t    *pMetadata;
            IUIWrapper                 *pWrapper;
            LSPWindow                  *pRoot;
            CtlPluginWindow            *pRootCtl;
            void                       *pRootWidget;

            LSPDisplay                  sDisplay;

            cvector<CtlPort>            vPorts;
            cvector<CtlPort>            vCustomPorts;
            cvector<CtlPort>            vSortedPorts;
            cvector<CtlPort>            vConfigPorts;
            cvector<CtlValuePort>       vTimePorts;
            cvector<LSPWidget>          vWidgets;
            cvector<CtlSwitchedPort>    vSwitched;
            cvector<CtlPortAlias>       vAliases;
            cvector<CtlKvtListener>     vKvtListeners;

            cstorage<preset_t>          vPresets;
            ConfigSink                 *pConfigSink;

        protected:
            static const port_t         vConfigMetadata[];
            static const port_t         vTimeMetadata[];

        protected:
            size_t          rebuild_sorted_ports();
            CtlWidget      *build_widget(widget_ctl_t w_class);
            io::File       *open_config_file(bool write);
            bool            apply_changes(const char *key, const char *value, cvector<CtlPort> &ports, bool preset, const io::Path *base);
            status_t        scan_presets();
            void            build_config_header(LSPString &c);
            void            destroy_presets();
            status_t        paste_from_clipboard(const LSPString *data);

            static status_t slot_preset_select(LSPWidget *sender, void *ptr, void *data);

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
            virtual status_t    init(IUIWrapper *wrapper, int argc, const char **argv);

            /**
             * Build UI from the XML schema
             * @return status of operation
             */
            virtual status_t    build();

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

            /** Add custom port to UI
             *
             * @param port custom UI port
             * @return status of operation
             */
            status_t add_custom_port(CtlPort *port);

            /** Export settings of the UI to the file
             *
             * @param filename file name
             * @param relative use relative paths
             * @return status of operation
             */
            status_t export_settings(const char *filename, bool relative);

            /** Import settings of the UI from the file
             *
             * @param filename file name
             * @param preset indicator that the source is preset
             * @return status of operation
             */
            status_t import_settings(const char *filename, bool preset);

            /**
             * Export settings to clipboard
             * @return status of operation
             */
            status_t export_settings_to_clipboard();

            /**
             * Import settings from clipboard
             * @return status of operation
             */
            status_t import_settings_from_clipboard();

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

            /**
             * Get port by index
             * @param idx index of the port
             * @return internal port by index
             */
            CtlPort *port_by_index(size_t idx);

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
            inline status_t main()          { return sDisplay.main(); };

            /** Main iteration
             *
             * @return main iteration
             */
            inline status_t main_iteration() { return sDisplay.main_iteration(); }

            /**
             * Synchronize state of meta ports
             */
            void sync_meta_ports();

            /** Return root window
             *
             * @return root window (if exists)
             */
            inline LSPWindow *root_window() { return pRoot; }

            /**
             * Set title of the main window
             * @param title title of the main window
             */
            void set_title(const char *title);

            /**
             * Notify the write of the KVT parameter
             * @param storage KVT storage
             * @param id kvt parameter identifier
             * @param value KVT parameter value
             */
            virtual void kvt_write(KVTStorage *storage, const char *id, const kvt_param_t *value);

            /**
             * Lock the KVT storage
             * @return pointer to KVT storage or NULL
             */
            virtual KVTStorage *kvt_lock();

            /**
             * Try to lock the KVT storage
             * @return pointer to KVT storage or NULL if not locked/not supported
             */
            virtual KVTStorage *kvt_trylock();

            /**
             * Release the KVT storage
             */
            virtual void kvt_release();

            /**
             * Add KVT listener
             * @param listener listener to add
             * @return status of operation
             */
            virtual status_t add_kvt_listener(CtlKvtListener *listener);

            /**
             * Request state dump
             */
            void request_state_dump();
    };

} /* namespace lsp */

#endif /* _UI_PLUGIN_UI_H_ */
