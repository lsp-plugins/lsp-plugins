/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 12 июл. 2020 г.
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

#ifndef UI_PLUGINS_MULTISAMPLER_UI_H_
#define UI_PLUGINS_MULTISAMPLER_UI_H_

#include <ui/ui.h>
#include <ui/ctl/ctl.h>
#include <core/files/Hydrogen.h>

namespace lsp
{

    class multisampler_ui: public plugin_ui
    {
        protected:
            typedef struct h2drumkit_t
            {
                LSPString           sName;      // Name of the drumkit
                io::Path            sPath;      // Location of the drumkit
                bool                bSystem;    // System directory
                LSPMenuItem        *pMenu;      // Corresponding menu item
            } h2drumkit_t;

        protected:
            CtlPort            *pHydrogenPath;
            LSPFileDialog      *pHydrogenImport;
            cvector<h2drumkit_t> vDrumkits;

        protected:
            static status_t     slot_start_import_hydrogen_file(LSPWidget *sender, void *ptr, void *data);
            static status_t     slot_import_hydrogen_file(LSPWidget *sender, void *ptr, void *data);
            static status_t     slot_call_import_hydrogen_file(LSPWidget *sender, void *ptr, void *data);
            static status_t     slot_fetch_hydrogen_path(LSPWidget *sender, void *ptr, void *data);
            static status_t     slot_commit_hydrogen_path(LSPWidget *sender, void *ptr, void *data);

        protected:
            status_t            import_hydrogen_file(const LSPString *path);
            status_t            add_sample(const io::Path *base, int id, int jd, const hydrogen::layer_t *layer);
            status_t            add_instrument(int id, const hydrogen::instrument_t *inst);
            void                set_float_value(float value, const char *fmt...);
            void                set_path_value(const char *path, const char *fmt...);

            void                lookup_hydrogen_files();
            void                sort_hydrogen_files();
            void                add_hydrogen_files_to_menu(LSPMenu *menu);
            status_t            scan_hydrogen_directory(const io::Path *path, bool system);
            status_t            add_drumkit(const io::Path *path, const hydrogen::drumkit_t *dk, bool system);

        public:
            explicit multisampler_ui(const plugin_metadata_t *mdata, void *root_widget);
            virtual ~multisampler_ui();

        public:
            virtual status_t    build();
    };

} /* namespace lsp */

#endif /* UI_PLUGINS_MULTISAMPLER_UI_H_ */
