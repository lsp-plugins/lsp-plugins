/*
 * LSPGraphItem.h
 *
 *  Created on: 18 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPGRAPHITEM_H_
#define UI_TK_LSPGRAPHITEM_H_

namespace lsp
{
    namespace tk
    {
        class LSPGraph;
        
        class LSPGraphItem: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                bool        bSmooth;

            public:
                explicit LSPGraphItem(LSPDisplay *dpy);
                virtual ~LSPGraphItem();

            public:
                LSPGraph        *graph();

            public:
                inline bool is_smooth() const   { return bSmooth;               };

            public:
                void        set_smooth(bool value = true);

                inline void set_sharp(bool value = true)    { set_smooth(!value); }

            public:
                virtual bool inside(ssize_t x, ssize_t y);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPGRAPHITEM_H_ */
