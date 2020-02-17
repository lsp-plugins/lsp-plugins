/*
 * xdnd_proxy.cpp
 *
 *  Created on: 17 февр. 2020 г.
 *      Author: sadko
 */

#include <core/types.h>

#if defined(PLATFORM_LINUX) || defined(PLATFORM_BSD)

#include <test/mtest.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <container/jack/defs.h>

#include <core/stdlib/string.h>
#include <plugins/plugins.h>
#include <core/ipc/Thread.h>

using namespace lsp;

namespace {
    Window  child_wnd = 0;
}

#if defined(LSP_TESTING)
    void patch_xdnd_proxy_window(const void *wparent, const void *wchild)
    {
        if ((wparent == NULL) || (wchild == NULL))
            return;

        union {
            Window wnd;
            const void *handle;
        } child, parent;

        child.handle = wchild;
        parent.handle = wparent;
        Display *dpy = ::XOpenDisplay(NULL);

        // Add DND PROXY attribute to parent window containing ID of the child window
        Atom aDndProxy = XInternAtom(dpy, "XdndProxy", True);

        long prop[1];
        prop[0] = child.wnd;
        XChangeProperty(dpy, parent.wnd, aDndProxy, XA_WINDOW, 32, PropModeReplace, reinterpret_cast<unsigned char *>(prop), 1);

        XWindowAttributes xwa;
        XGetWindowAttributes(dpy, child.wnd, &xwa);
        XResizeWindow(dpy, parent.wnd, xwa.width, xwa.height);

        XFlush(dpy);
        XSync(dpy, True);

        XCloseDisplay(dpy);

        child_wnd = child.wnd;
    }
#endif

MTEST_BEGIN("x11", xdnd_proxy)
    class X11Thread: public ipc::Thread
    {
        private:
            Display    *pDisplay;
            Window      hWindow;
            test::Test *pTest;

        public:
            explicit X11Thread(Display *dpy, Window win, test::Test *test)
            {
                pDisplay = dpy;
                hWindow = win;
                pTest = test;
            }

            virtual status_t run()
            {
                XEvent xev;

                Atom aDeleteWnd     = XInternAtom(pDisplay, "WM_DELETE_WINDOW", False);
                Atom aWmProtocols   = XInternAtom(pDisplay, "WM_PROTOCOLS", False);
                Atom aClose         = XInternAtom(pDisplay, "WM_DELETE_WINDOW", False);
                XSetWMProtocols(pDisplay, hWindow, &aClose, 1);

                if (child_wnd != 0)
                    XSelectInput(pDisplay, child_wnd, StructureNotifyMask);
                XSelectInput(pDisplay, hWindow, SubstructureNotifyMask);
                XFlush(pDisplay);

                do
                {
                    // Next event
                    XNextEvent(pDisplay, &xev);

                    // Decode event
                    switch (xev.type)
                    {
                        case ClientMessage:
                        {
                            XClientMessageEvent *ce = &xev.xclient;
                            Atom type = ce->message_type;

                            if ((type == aWmProtocols) && (ce->data.l[0] == long(aDeleteWnd)) && (ce->window == hWindow))
                            {
                                if (child_wnd != 0)
                                {
                                    XEvent sev;
                                    XClientMessageEvent &send = sev.xclient;
                                    send.type = ClientMessage;
                                    send.serial = 0;
                                    send.send_event = True;
                                    send.display = pDisplay;
                                    send.window = child_wnd;
                                    send.message_type = aWmProtocols;
                                    send.format = 32;
                                    send.data.l[0] = aDeleteWnd;
                                    XSendEvent(pDisplay, child_wnd, True, NoEventMask, &sev);
                                    XFlush(pDisplay);
                                }

                                Thread::cancel();
                            }

                            break;
                        }

                        case ConfigureNotify:
                        {
                            XConfigureEvent *ce = &xev.xconfigure;
                            if (ce->window == child_wnd)
                                XResizeWindow(pDisplay, hWindow, ce->width, ce->height);

                            break;
                        }

                        default:
                            break;
                    }
                } while (!Thread::is_cancelled());

                return STATUS_OK;
            }
    };

    void plugin_not_found(const char *id)
    {
        cvector<const char> plugin_ids;

        // Generate the list of plugins
        #define MOD_PLUGIN(plugin, ui) \
            if (plugin::metadata.ui_resource != NULL) \
                plugin_ids.add(plugin::metadata.lv2_uid);
        #include <metadata/modules.h>

        // Sort the list of plugins
        for (size_t i=0; i<plugin_ids.size()-1; ++i)
            for (size_t j=i+1; j<plugin_ids.size(); ++j)
                if (strcmp(plugin_ids[i], plugin_ids[j]) > 0)
                    plugin_ids.swap(i, j);

        // Output list
        if (id == NULL)
            fprintf(stderr, "\nPlugin identifier required to be passed as first parameter. Available identifiers:\n");
        else
            fprintf(stderr, "\nInvalid plugin identifier '%s'. Available identifiers:\n", id);
        for (size_t i=0; i<plugin_ids.size(); ++i)
            fprintf(stderr, "  %s\n", plugin_ids[i]);

        MTEST_FAIL_SILENT();
    }

    MTEST_MAIN
    {
        XInitThreads();

        if (argc <= 0)
            plugin_not_found(NULL);

        const plugin_metadata_t *m = NULL;
        #define MOD_PLUGIN(plugin, ui) if (!strcmp(argv[0], #plugin)) m = &plugin::metadata;
        #include <metadata/modules.h>
        if (m == NULL)
            plugin_not_found(argv[0]);

        // Open connection and create window
        Display *dpy = ::XOpenDisplay(NULL);
        MTEST_ASSERT_MSG(dpy, "Could not open X display");
        int screen = DefaultScreen(dpy);
        Window root = RootWindow(dpy, screen);
        Window hWnd = ::XCreateSimpleWindow(dpy, root, 0, 0, 600, 400, 0, 0, 0);
        ::XMapWindow(dpy, hWnd);
        XFlush(dpy);
        XSync(dpy, True);

        X11Thread handler(dpy, hWnd, this);
        handler.start();

        char proxy_id[32];
        ::snprintf(proxy_id, sizeof(proxy_id), "%lx", long(hWnd));
        proxy_id[sizeof(proxy_id)-1] = '\0';

        // Prepare command-line arguments
        printf("Preparing to call JACK_MAIN_FUNCION\n");
        const char ** args = reinterpret_cast<const char **>(alloca((argc + 2) * sizeof(const char *)));
        MTEST_ASSERT(args != NULL);

        size_t xargc = 0;
        args[xargc++] = full_name();
        args[xargc++] = "--dnd-proxy";
        args[xargc++] = proxy_id;

        for (ssize_t i=1; i < argc; ++i)
            args[xargc++] = argv[i];

        printf("Calling JACK_MAIN_FUNCTION\n");

        // Call the main function
        int result = JACK_MAIN_FUNCTION(argv[0], xargc, args);
        MTEST_ASSERT(result == 0);

        // Wait for main thread
        handler.join();
    }
MTEST_END

#endif /* PLATFORM_LINUX || PLATFORM_BSD */
