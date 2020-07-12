/*
 * multisampler_ui.h
 *
 *  Created on: 12 июл. 2020 г.
 *      Author: sadko
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
            CtlPort            *pHydrogenPath;
            LSPFileDialog      *pHydrogenImport;

        protected:
            static status_t     slot_start_import_hydrogen_file(LSPWidget *sender, void *ptr, void *data);
            static status_t     slot_call_import_hydrogen_file(LSPWidget *sender, void *ptr, void *data);
            static status_t     slot_fetch_hydrogen_path(LSPWidget *sender, void *ptr, void *data);
            static status_t     slot_commit_hydrogen_path(LSPWidget *sender, void *ptr, void *data);

        protected:
            status_t            import_hydrogen_file(const LSPString *path);
            status_t            add_sample(const io::Path *base, int id, int jd, const hydrogen::layer_t *layer);
            status_t            add_instrument(int id, const hydrogen::instrument_t *inst);
            void                set_float_value(float value, const char *fmt...);
            void                set_path_value(const char *path, const char *fmt...);

        public:
            explicit multisampler_ui(const plugin_metadata_t *mdata, void *root_widget);
            virtual ~multisampler_ui();

        public:
            virtual status_t    build();
    };

} /* namespace lsp */

#endif /* UI_PLUGINS_MULTISAMPLER_UI_H_ */
