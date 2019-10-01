/*
 * LSPStyle.h
 *
 *  Created on: 1 окт. 2019 г.
 *      Author: sadko
 */

#ifndef UI_TK_SYS_LSPSTYLE_H_
#define UI_TK_SYS_LSPSTYLE_H_

namespace lsp
{
    namespace tk
    {
        class LSPWidget;
        
        /**
         * Some widget style. Allows nesting
         */
        class LSPStyle
        {
            private:
                LSPStyle           *pParent;
                LSPWidget          *pWidget;
                cvector<LSPStyle>   vChildren;

            public:
                explicit LSPStyle(LSPWidget *widget);
                virtual ~LSPStyle();

                status_t            init();
                void                destroy();

            protected:
                void                do_destroy();
                void                notify_all();

            public:
                /**
                 * Add child style
                 * @param child child style
                 * @return status of operation
                 */
                status_t            add(LSPStyle *child);

                /** Remove child style
                 *
                 * @param child child style to remove
                 * @return status of operation
                 */
                status_t            remove(LSPStyle *child);

                /**
                 * Set parent style
                 * @param parent parent style
                 * @return statys of operation
                 */
                status_t            set_parent(LSPStyle *parent);

                /**
                 * Get parent style
                 * @return parent style
                 */
                inline LSPStyle    *parent()        { return pParent; };

                /**
                 * Get the associated widget
                 * @return associated widget
                 */
                inline LSPWidget   *widget()        { return pWidget; }

                /**
                 * Get root style
                 * @return root style
                 */
                LSPStyle           *root();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_SYS_LSPSTYLE_H_ */
