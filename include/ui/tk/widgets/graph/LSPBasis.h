/*
 * LSPBasis.h
 *
 *  Created on: 19 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPBASIS_H_
#define UI_TK_LSPBASIS_H_

namespace lsp
{
    namespace tk
    {
        class LSPBasis: public LSPGraphItem
        {
            public:
                static const w_class_t    metadata;

            protected:
                ssize_t     nID;

            public:
                explicit LSPBasis(LSPDisplay *dpy);
                virtual ~LSPBasis();

            public:
                inline ssize_t get_id() const { return nID; }
                inline void set_id(ssize_t id) { nID = id; }
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPBASIS_H_ */
