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

using namespace lsp;

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
    }
#endif

MTEST_BEGIN("x11", xdnd_proxy)

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
    }
MTEST_END

#endif /* PLATFORM_LINUX || PLATFORM_BSD */
