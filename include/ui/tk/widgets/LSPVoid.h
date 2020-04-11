/*
 * LSPVoid.h
 *
 *  Created on: 11 апр. 2020 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_LSPVOID_H_
#define UI_TK_WIDGETS_LSPVOID_H_

namespace lsp
{
    namespace tk
    {
        
        class LSPVoid: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                LSPSizeConstraints  sConstraints;

            public:
                explicit LSPVoid(LSPDisplay *dpy);
                virtual ~LSPVoid();

            public:
                inline LSPSizeConstraints  *constraints()   { return &sConstraints; }

            public:
                virtual void        render(ISurface *s, bool force);

                virtual void        size_request(size_request_t *r);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPVOID_H_ */
