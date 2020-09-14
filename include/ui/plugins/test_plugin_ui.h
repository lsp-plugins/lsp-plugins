/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 5 мая 2019 г.
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

        protected:
            static status_t slot_on_submit(LSPWidget *sender, void *ptr, void *data);

        public:
            virtual status_t    build();
    };

} /* namespace lsp */

#endif

#endif /* UI_PLUGINS_UI_TEST_PLUGIN_UI_H_ */
