/*
 * LSPWindowActions.cpp
 *
 *  Created on: 12 окт. 2017 г.
 *      Author: sadko
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
