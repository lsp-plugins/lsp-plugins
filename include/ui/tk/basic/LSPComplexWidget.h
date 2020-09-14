/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 10 авг. 2017 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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
