/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 29 авг. 2018 г.
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

#include <core/types.h>

#if defined(PLATFORM_LINUX) || defined(PLATFORM_BSD)

#include <test/mtest.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

MTEST_BEGIN("x11", selection)

    void show_targets(Display *dpy, Window w, Atom p)
    {
        Atom type, *targets;
        int di;
        unsigned long i, nitems, dul;
        unsigned char *prop_ret = NULL;
        char *an = NULL;

        /* Read the first 1024 atoms from this list of atoms. We don't
         * expect the selection owner to be able to convert to more than
         * 1024 different targets. :-) */
        XGetWindowProperty(dpy, w, p, 0, 1024 * sizeof (Atom), False, XA_ATOM,
                           &type, &di, &nitems, &dul, &prop_ret);

        printf("Targets (actual type = %d %s):\n", int(type), XGetAtomName(dpy, type));
        targets = (Atom *)prop_ret;
        for (i = 0; i < nitems; i++)
        {
            printf("    id = %d\n", int(targets[i]));
            an = XGetAtomName(dpy, targets[i]);
            printf("    name '%s'\n", an);
            if (an)
                XFree(an);
        }
        XFree(prop_ret);

        XDeleteProperty(dpy, w, p);
    }

    MTEST_MAIN
    {
        Display *dpy;
        Window target_window, root;
        int screen;
        Atom sel, targets, target_property;
        XEvent ev;
        XSelectionEvent *sev;

        dpy = XOpenDisplay(NULL);
        MTEST_ASSERT_MSG(dpy, "Could not open X display");

        screen = DefaultScreen(dpy);
        root = RootWindow(dpy, screen);

        sel = XInternAtom(dpy, "CLIPBOARD", False);
        targets = XInternAtom(dpy, "TARGETS", False);
        target_property = XInternAtom(dpy, "PENGUIN", False);

        target_window = XCreateSimpleWindow(dpy, root, -10, -10, 1, 1, 0, 0, 0);
        XSelectInput(dpy, target_window, SelectionNotify);

        XConvertSelection(dpy, sel, targets, target_property, target_window, CurrentTime);

        while (true)
        {
            XNextEvent(dpy, &ev);
            switch (ev.type)
            {
                case SelectionNotify:
                    sev = (XSelectionEvent*)&ev.xselection;
                    MTEST_ASSERT_MSG(sev->property != None, "Conversion could not be performed.");
                    show_targets(dpy, target_window, target_property);
                    return;
            }
        }
    }
MTEST_END

#endif /* PLATFORM_LINUX || PLATFORM_BSD */
