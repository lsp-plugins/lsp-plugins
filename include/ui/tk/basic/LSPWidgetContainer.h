/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 16 июн. 2017 г.
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
