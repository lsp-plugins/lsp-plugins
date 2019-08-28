/*
 * ui.h
 *
 *  Created on: 10 окт. 2016 г.
 *      Author: sadko
 */

#ifndef UI_X11_WS_H_
#define UI_X11_WS_H_

// Include common windowing system interface
#include <ui/ws/ws.h>

#ifdef USE_X11_DISPLAY

    // Include common libraries
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <X11/Xatom.h>
    #include <X11/keysymdef.h>
    #include <X11/cursorfont.h>

    #include <cairo.h>
    #include <cairo-xlib.h>

    namespace lsp
    {
        namespace ws
        {
            namespace x11
            {
                // Merge with ::lsp::ws namespace
                using namespace ::lsp::ws;
            }
        }
    }

    // Include specific libraries
    #define UI_X11_WS_H_INCL_
        #include <ui/ws/x11/decode.h>
        #include <ui/ws/x11/X11Atoms.h>
        #include <ui/ws/x11/X11CairoGradient.h>
        #include <ui/ws/x11/X11CairoSurface.h>
        #include <ui/ws/x11/X11Display.h>
        #include <ui/ws/x11/X11Window.h>
    #undef UI_X11_WS_H_INCL_

#endif /* USE_X11_DISPLAY */

#endif /* UI_X11_UI_H_ */
