/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 12 окт. 2017 г.
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

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        LSPWindowActions::LSPWindowActions(LSPWindow *wnd)
        {
            nFlags      = 0;
            pWindow     = wnd;
        }
        
        LSPWindowActions::~LSPWindowActions()
        {
        }

        status_t LSPWindowActions::init()
        {
            return pWindow->pWindow->get_window_actions(&nFlags);
        }

        status_t LSPWindowActions::set_actions(size_t flags)
        {
            size_t old = nFlags;
            nFlags = flags;
            return (flags != old) ? pWindow->pWindow->set_window_actions(nFlags) : STATUS_OK;
        }

        status_t LSPWindowActions::toggle_actions(size_t flags)
        {
            size_t old = nFlags;
            nFlags ^= flags;
            return (nFlags != old) ? pWindow->pWindow->set_window_actions(nFlags) : STATUS_OK;
        }

        status_t LSPWindowActions::allow_actions(size_t flags)
        {
            size_t old = nFlags;
            nFlags |= flags;
            return (nFlags != old) ? pWindow->pWindow->set_window_actions(nFlags) : STATUS_OK;
        }

        status_t LSPWindowActions::deny_actions(size_t flags)
        {
            size_t old = nFlags;
            nFlags &= ~flags;
            return (nFlags != old) ? pWindow->pWindow->set_window_actions(nFlags) : STATUS_OK;
        }
    
    } /* namespace tk */
} /* namespace lsp */
