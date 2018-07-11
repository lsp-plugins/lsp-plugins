/*
 * X11Atoms.h
 *
 *  Created on: 11 дек. 2016 г.
 *      Author: sadko
 */

#ifndef UI_X11_X11ATOMS_H_
#define UI_X11_X11ATOMS_H_

namespace lsp
{
    namespace x11ui
    {
        typedef struct x11_atoms_t
        {
            #define WM_PREDEFINED_ATOM(name) Atom X11_ ## name;
            #define WM_ATOM(name) Atom X11_ ## name;
            #include <ui/x11/X11AtomList.h>
            #undef WM_ATOM
            #undef WM_PREDEFINED_ATOM
        } x11_atoms_t;

        int init_atoms(Display *dpy, x11_atoms_t *atoms);
    }
}


#endif /* UI_X11_X11ATOMS_H_ */
