/*
 * IWidgetContainer.h
 *
 *  Created on: 16 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPWIDGETCONTAINER_H_
#define UI_TK_LSPWIDGETCONTAINER_H_

namespace lsp
{
    namespace tk
    {
        /** This is a basic widget class for any widget that contains sub-widgets
         *
         */
        class LSPWidgetContainer: public LSPComplexWidget
        {
            public:
                static const w_class_t    metadata;

            //---------------------------------------------------------------------------------
            // Construction and destruction
            public:
                explicit LSPWidgetContainer(LSPDisplay *dpy);

                virtual ~LSPWidgetContainer();

            //---------------------------------------------------------------------------------
            // Manipulation
            public:
                /** Add child to widget container
                 *
                 * @param child child widget to add
                 * @return status of operation
                 */
                virtual status_t    add(LSPWidget *child);

                /** Remove child from widget container
                 *
                 * @param child child widget to remove
                 * @return status of operation
                 */
                virtual status_t    remove(LSPWidget *child);

                /** Remove all widgets
                 *
                 * @return status of operation
                 */
                virtual status_t    remove_all();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPWIDGETCONTAINER_H_ */
