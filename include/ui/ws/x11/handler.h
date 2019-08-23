/*
 * handler.h
 *
 *  Created on: 23 авг. 2019 г.
 *      Author: sadko
 */

#ifndef UI_WS_X11_HANDLER_H_
#define UI_WS_X11_HANDLER_H_

/**
 * This header is created as a workaround to fucking X11 implementation of error handlers
 * that forces use of global states and global variables
 */
namespace lsp
{
    namespace ws
    {
        namespace x11
        {
            class X11Display;

            typedef int (*x11_error_handler_t) (X11Display *display, Display *dpy, XErrorEvent *ev);

            /**
             * Allocate handler in proper way that will link X11Display to handler that will be called
             * @param x11dpy X11Display that should be linked
             * @param handler handler routine
             * @return pointer to generated handler routine with binding to X11Display
             */
            XErrorHandler alloc_x11_error_handler(X11Display *x11dpy, x11_error_handler_t handler);

            /**
             * Release the handler in proper way
             * @param handler handler to release
             */
            void free_x11_error_handler(XErrorHandler handler);
        }
    }
}


#endif /* UI_WS_X11_HANDLER_H_ */
