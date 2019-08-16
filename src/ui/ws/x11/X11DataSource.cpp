/*
 * X11DataSource.cpp
 *
 *  Created on: 16 авг. 2019 г.
 *      Author: sadko
 */

#include <ui/ws/x11/ws.h>

namespace lsp
{
    namespace ws
    {
        namespace x11
        {
            X11DataSource::X11DataSource(X11Display *dpy, Window owner, Atom atom)
            {
                pDisplay    = dpy;
                hSelOwner   = owner;
                hSelAtom    = atom;
                vMimeTypes  = NULL;
                nMimeTypes  = 0;
            }
            
            X11DataSource::~X11DataSource()
            {
                if (vMimeTypes != NULL)
                {
                    for (size_t i=0; i<nMimeTypes; ++i)
                    {
                        if (vMimeTypes[i] != NULL)
                        {
                            ::free(vMimeTypes[i]);
                            vMimeTypes[i]   = NULL;
                        }
                    }
                    ::free(vMimeTypes);
                    vMimeTypes = NULL;
                }
                nMimeTypes      = 0;
            }

            size_t X11DataSource::mime_types()
            {
                return nMimeTypes;
            }

            const char *X11DataSource::mime_type(size_t id)
            {
                return (id < nMimeTypes) ? vMimeTypes[id] : NULL;
            }
        }
    
    } /* namespace ws */
} /* namespace lsp */
