/*
 * xwindow.h
 *
 *  Created on: 10 окт. 2016 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <ui/x11/ui.h>

namespace xwindow_test
{
    using namespace lsp::x11ui;

    int test(int argc, const char **argv)
    {
        X11Core ui;
        if (ui.init(argc, argv) < 0)
        {
            lsp_error("Error while initializing UI core");
            return -1;
        }

        // Create window
        X11Window *wnd = ui.createWindow(200, 200);
        if (wnd->init() < 0)
        {
            lsp_error("Error while initializing UI window");
            return -1;
        }

        // Show window
        wnd->show();

        // Call Main cycle
        ui.main();

        return 0;
    }
}
