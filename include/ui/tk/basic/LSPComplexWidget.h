/*
 * LSPComplexWidget.h
 *
 *  Created on: 10 авг. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_BASIC_LSPCOMPLEXWIDGET_H_
#define UI_TK_BASIC_LSPCOMPLEXWIDGET_H_

namespace lsp
{
    namespace tk
    {
        
        class LSPComplexWidget: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                ssize_t             nMouse;
                ssize_t             nKey;
                LSPWidget          *pKey;
                LSPWidget          *pMouse;

            protected:
                LSPWidget          *acquire_mouse_handler(const ws_event_t *e);
                void                release_mouse_handler(const ws_event_t *e);

            //---------------------------------------------------------------------------------
            // Manipulation
            protected:
                virtual LSPWidget      *find_widget(ssize_t x, ssize_t y);

                virtual status_t        handle_event_internal(const ws_event_t *e);

            //---------------------------------------------------------------------------------
            // Construction and destruction
            public:
                explicit LSPComplexWidget(LSPDisplay *dpy);

                virtual ~LSPComplexWidget();

            //---------------------------------------------------------------------------------
            // Manipulation
            public:
                /** Handle event from window system
                 *
                 * @param e event to handle
                 * @return status of operation
                 */
                virtual status_t    handle_event(const ws_event_t *e);

        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_BASIC_LSPCOMPLEXWIDGET_H_ */
