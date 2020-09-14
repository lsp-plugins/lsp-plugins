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

#ifndef LSP_NO_EXPERIMENTAL

#include <ui/ui.h>

namespace lsp
{
    test_plugin_ui::test_plugin_ui(const plugin_metadata_t *mdata, void *root_widget):
        plugin_ui(mdata, root_widget)
    {
    }
    
    test_plugin_ui::~test_plugin_ui()
    {
    }

    status_t test_plugin_ui::slot_on_submit(LSPWidget *sender, void *ptr, void *data)
    {
        lsp_trace("Menu item %p has been selected by widget %p", ptr, sender);
        return STATUS_OK;
    }

    status_t test_plugin_ui::build()
    {
        // Call parent class for build
        status_t res = plugin_ui::build();
        if (res != STATUS_OK)
            return res;

        // Add subwidgets
        LSPMenu *menu       = widget_cast<LSPMenu>(resolve(WUID_MAIN_MENU));
        if (menu != NULL)
        {
            // Get display
            LSPDisplay *dpy     = menu->display();

            // Create separator
            LSPMenuItem *item   = new LSPMenuItem(dpy);
            item->init();
            vWidgets.add(item);
            item->set_separator(true);
            menu->add(item);

            // Create submenu item
            item                = new LSPMenuItem(dpy);
            item->init();
            vWidgets.add(item);
            item->text()->set_raw("Test Submenu");
            menu->add(item);

            // Create submenu
            LSPMenu *menu       = new LSPMenu(dpy);
            menu->init();
            vWidgets.add(menu);
            item->set_submenu(menu);

            // Add subitems
            for (size_t i=0; i<5; ++i)
            {
                LSPString str;
                str.fmt_ascii("Item %d", int(i+1));

                item                = new LSPMenuItem(dpy);
                item->init();
                item->slots()->bind(LSPSLOT_SUBMIT, slot_on_submit, (void *)(i+1));
                vWidgets.add(item);
                item->text()->set_raw(&str);
                menu->add(item);
            }
        }

        return STATUS_OK;
    }

} /* namespace lsp */

#endif
