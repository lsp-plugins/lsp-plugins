/*
 * ui_test_plugin.h
 *
 *  Created on: 5 мая 2019 г.
 *      Author: sadko
 */

#ifndef UI_PLUGINS_TEST_PLUGIN_UI_H_
#define UI_PLUGINS_TEST_PLUGIN_UI_H_

#ifndef LSP_NO_EXPERIMENTAL

namespace lsp
{
    class test_plugin_ui: public plugin_ui
    {
        public:
            explicit test_plugin_ui(const plugin_metadata_t *mdata, void *root_widget);
            virtual ~test_plugin_ui();

        public:
            virtual status_t    init(IUIWrapper *wrapper, int argc, const char **argv);
    };

} /* namespace lsp */

#endif

#endif /* UI_PLUGINS_UI_TEST_PLUGIN_UI_H_ */
