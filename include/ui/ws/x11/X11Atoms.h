/*
 * X11Atoms.h
 *
 *  Created on: 11 дек. 2016 г.
 *      Author: sadko
 */

#ifndef UI_X11_X11ATOMS_H_
#define UI_X11_X11ATOMS_H_

#ifndef UI_X11_WS_H_INCL_
    #error "This header should not be included directly"
#endif /* UI_X11_WS_H_INCL_ */

// Flags
#define MWM_HINTS_FUNCTIONS     (1L << 0)
#define MWM_HINTS_DECORATIONS   (1L << 1)
#define MWM_HINTS_INPUT_MODE    (1L << 2)
#define MWM_HINTS_STATUS        (1L << 3)

// Functions
#define MWM_FUNC_ALL            (1L << 0)
#define MWM_FUNC_RESIZE         (1L << 1)
#define MWM_FUNC_MOVE           (1L << 2)
#define MWM_FUNC_MINIMIZE       (1L << 3)
#define MWM_FUNC_MAXIMIZE       (1L << 4)
#define MWM_FUNC_CLOSE          (1L << 5)

// Decorations
#define MWM_DECOR_ALL           (1L << 0)
#define MWM_DECOR_BORDER        (1L << 1)
#define MWM_DECOR_RESIZEH       (1L << 2)
#define MWM_DECOR_TITLE         (1L << 3)
#define MWM_DECOR_MENU          (1L << 4)
#define MWM_DECOR_MINIMIZE      (1L << 5)
#define MWM_DECOR_MAXIMIZE      (1L << 6)

// Input mode
#define MWM_INPUT_MODELESS                      0
#define MWM_INPUT_PRIMARY_APPLICATION_MODAL     1
#define MWM_INPUT_SYSTEM_MODAL                  2
#define MWM_INPUT_FULL_APPLICATION_MODAL        3
#define MWM_INPUT_APPLICATION_MODAL             MWM_INPUT_PRIMARY_APPLICATION_MODAL

// Status
#define MWM_TEAROFF_WINDOW      (1L<<0)

namespace lsp
{
    namespace ws
    {
        namespace x11
        {
            typedef struct x11_atoms_t
            {
                #define WM_PREDEFINED_ATOM(name) Atom X11_ ## name;
                #define WM_ATOM(name) Atom X11_ ## name;
                #include <ui/ws/x11/X11AtomList.h>
                #undef WM_ATOM
                #undef WM_PREDEFINED_ATOM
            } x11_atoms_t;

            #pragma pack(push, 1)
            typedef struct motif_hints_t
            {
                unsigned long   flags;
                unsigned long   functions;
                unsigned long   decorations;
                long            input_mode;
                unsigned long   status;
            } motif_hints_t;
            #pragma pack(pop)

            int init_atoms(Display *dpy, x11_atoms_t *atoms);

            template <class T>
                int get_array_property(Display *dpy, ::Window wnd, Atom atom, Atom type, int *nreturn, T **result)
                {
                    T *buf = NULL;
                    int total = 0;

                    Atom ret_type;
                    int fmt;
                    unsigned long curr = 0, count = 0, left = 0;
                    unsigned char *props = NULL;

                    do
                    {
                        // Try to fetch
                        if (XGetWindowProperty(
                            dpy, wnd, atom,
                            curr, 32, False, type,
                            &ret_type, &fmt, &count, &left, &props) != Success)
                        {
                            if (buf != NULL)
                                delete[] buf;
                            return STATUS_UNKNOWN_ERR;
                        }

                        // Empty value ?
                        if (ret_type == None)
                        {
                            if (props != NULL)
                                XFree(props);
                            if (buf != NULL)
                                delete[] buf;
                            *nreturn = 0;
                            *result  = NULL;
                            return STATUS_OK;
                        }

                        // Create buffer
                        if (buf == NULL)
                        {
                            buf = new T[count + left];
                            if (buf == NULL)
                            {
                                if (props != NULL)
                                    XFree(props);
                                return STATUS_NO_MEM;
                            }
                        }

                        // Copy atoms
                        total += count;
                        T *src = (T *)props;
                        while ((count--) > 0)
                            buf[curr++] = *(src++);

                        // Free properties
                        XFree(props);
                    }
                    while (left > 0);

                    // Return data
                    *result     = buf;
                    *nreturn    = total;
                    return STATUS_OK;
                }
        }
    }
}


#endif /* UI_X11_X11ATOMS_H_ */
