/*
 * ui_test_plugin.cpp
 *
 *  Created on: 5 мая 2019 г.
 *      Author: sadko
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

    status_t test_plugin_ui::init(IUIWrapper *wrapper, int argc, const char **argv)
    {
        status_t res = plugin_ui::init(wrapper, argc, argv);
        if (res != STATUS_OK)
            return res;

        // Add subwidgets
        LSPMenu *menu       = widget_cast<LSPMenu>(resolve(WUID_MAIN_MENU));
        if (menu != NULL)
        {
            // Get display
            LSPDisplay *dpy     = menu->display();

            // Create menu item
            LSPMenuItem *item   = new LSPMenuItem(dpy);
            item->init();
            vWidgets.add(item);
            item->set_text("Test Submenu");

            // Add child to the menu
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
                vWidgets.add(item);
                item->set_text(&str);
                menu->add(item);
            }
        }

        return STATUS_OK;
    }

} /* namespace lsp */

#endif
