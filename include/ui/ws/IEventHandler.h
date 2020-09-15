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

#ifndef UI_WS_IEVENTHANDLER_H_
#define UI_WS_IEVENTHANDLER_H_

namespace lsp
{
    namespace ws
    {
        /** Windowing system's event handler
         *
         */
        class IEventHandler
        {
            public:
                IEventHandler();
                virtual ~IEventHandler();

            public:
                virtual status_t handle_event(const ws_event_t *ev);
        };
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_WS_IEVENTHANDLER_H_ */
