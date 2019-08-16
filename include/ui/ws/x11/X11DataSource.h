/*
 * X11DataSource.h
 *
 *  Created on: 16 авг. 2019 г.
 *      Author: sadko
 */

#ifndef UI_WS_X11_X11DATASOURCE_H_
#define UI_WS_X11_X11DATASOURCE_H_

namespace lsp
{
    namespace ws
    {
        namespace x11
        {
            class X11Display;
        
            class X11DataSource: public IDataSource
            {
                protected:
                    X11Display     *pDisplay;       // Display
                    Window          hSelOwner;      // Window that owns selection atom
                    Atom            hSelAtom;       // Atom that identifies selection
                    char          **vMimeTypes;     // List of supported MIME types
                    size_t          nMimeTypes;

                public:
                    explicit X11DataSource(X11Display *dpy, Window owner, Atom atom);
                    virtual ~X11DataSource();

                public:
                    void                set_mime_types(char **types, size_t count);

                    virtual size_t      mime_types();

                    virtual const char *mime_type(size_t id);
            };

        }
    } /* namespace ws */
} /* namespace lsp */

#endif /* UI_WS_X11_X11DATASOURCE_H_ */
