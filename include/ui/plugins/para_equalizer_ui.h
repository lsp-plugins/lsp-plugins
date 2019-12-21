/*
 * para_equalizer_ui.h
 *
 *  Created on: 10 сент. 2019 г.
 *      Author: sadko
 */

#ifndef UI_PLUGINS_PARAEQUALIZERUI_H_
#define UI_PLUGINS_PARAEQUALIZERUI_H_

#include <ui/ui.h>
#include <ui/ctl/ctl.h>

namespace lsp
{
    
    class para_equalizer_ui: public plugin_ui
    {
        protected:
            CtlPort            *pRewPath;
            LSPFileDialog      *pRewImport;
            const char        **fmtStrings;

        protected:
            static status_t slot_start_import_rew_file(LSPWidget *sender, void *ptr, void *data);
            static status_t slot_call_import_rew_file(LSPWidget *sender, void *ptr, void *data);
            static status_t slot_fetch_rew_path(LSPWidget *sender, void *ptr, void *data);
            static status_t slot_commit_rew_path(LSPWidget *sender, void *ptr, void *data);

            status_t    import_rew_file(const LSPString *path);
            void        set_port_value(const char *base, size_t id, float value);

            void        set_filter_mode(size_t id, size_t value);
            void        set_filter_type(size_t id, size_t value);
            void        set_filter_frequency(size_t id, double value);
            void        set_filter_quality(size_t id, double value);
            void        set_filter_gain(size_t id, double value);
            void        set_filter_slope(size_t id, size_t slope);
            void        set_filter_enabled(size_t id, bool enabled);
            void        set_filter_solo(size_t id, bool solo);

        public:
            explicit para_equalizer_ui(const plugin_metadata_t *mdata, void *root_widget);
            virtual ~para_equalizer_ui();

        public:
            virtual status_t    build();
    };

} /* namespace lsp */

#endif /* UI_PLUGINS_PARAEQUALIZERUI_H_ */
