/*
 * X11Atoms.cpp
 *
 *  Created on: 11 дек. 2016 г.
 *      Author: sadko
 */

#include <ui/x11/ui.h>
#include <core/status.h>
#include <core/debug.h>

namespace lsp
{
    namespace x11ui
    {
        int init_atoms(Display *dpy, x11_atoms_t *atoms)
        {
            #define WM_ATOM(name) \
                atoms->X11_ ## name = XInternAtom(dpy, #name, False); \
                lsp_trace("  %s = %d", #name, int(atoms->X11_ ## name));
//                if (atoms->X11_ ## name == None)
//                    return STATUS_NOT_FOUND;

            #define WM_PREDEFINED_ATOM(name) \
                atoms->X11_ ## name = name; \
                lsp_trace("  %s = %d", #name, int(atoms->X11_ ## name));

            #include <ui/x11/X11AtomList.h>
            #undef WM_PREDEFINED_ATOM
            #undef WM_ATOM

            return STATUS_OK;
        }
    }
}

