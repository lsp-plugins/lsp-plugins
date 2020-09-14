/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 11 дек. 2016 г.
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

#include <ui/ws/x11/ws.h>

#ifdef USE_X11_DISPLAY
namespace lsp
{
    namespace ws
    {
        namespace x11
        {
            int init_atoms(Display *dpy, x11_atoms_t *atoms)
            {
                #define WM_ATOM(name) \
                    atoms->X11_ ## name = XInternAtom(dpy, #name, False); \
                    /* lsp_trace("  %s = %d", #name, int(atoms->X11_ ## name)); */
    //                if (atoms->X11_ ## name == None)
    //                    return STATUS_NOT_FOUND;

                #define WM_PREDEFINED_ATOM(name) \
                    atoms->X11_ ## name = name; \
                    /*lsp_trace("  %s = %d", #name, int(atoms->X11_ ## name))*/;

                #include <ui/ws/x11/X11AtomList.h>
                #undef WM_PREDEFINED_ATOM
                #undef WM_ATOM

                return STATUS_OK;
            }
        }
    }
}

#endif /* USE_X11_DISPLAY */
