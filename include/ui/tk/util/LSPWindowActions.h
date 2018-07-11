/*
 * LSPWindowActions.h
 *
 *  Created on: 12 окт. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_UTIL_LSPWINDOWACTIONS_H_
#define UI_TK_UTIL_LSPWINDOWACTIONS_H_

namespace lsp
{
    namespace tk
    {
        class LSPWindow;
        
        class LSPWindowActions
        {
            protected:
                size_t          nFlags;
                LSPWindow      *pWindow;

            public:
                LSPWindowActions(LSPWindow *wnd);
                ~LSPWindowActions();

            public:
                status_t        init();

            public:
                inline bool     allowed(window_action_t wa) const   { return nFlags & wa;               }
                inline bool     denied(window_action_t wa) const    { return !(nFlags & wa);            }
                inline bool     get(window_action_t wa) const       { return nFlags & wa;               }
                inline size_t   actions() const                     { return nFlags;                    }
                inline bool     movable() const                     { return nFlags & WA_MOVE;          }
                inline bool     resizable() const                   { return nFlags & WA_RESIZE;        }
                inline bool     minimizable() const                 { return nFlags & WA_MINIMIZE;      }
                inline bool     maximizable() const                 { return nFlags & WA_MAXIMIZE;      }
                inline bool     closeable() const                   { return nFlags & WA_CLOSE;         }
                inline bool     stickable() const                   { return nFlags & WA_STICK;         }
                inline bool     shadeable() const                   { return nFlags & WA_SHADE;         }
                inline bool     fullscreenable() const              { return nFlags & WA_FULLSCREEN;    }
                inline bool     transportable() const               { return nFlags & WA_CHANGE_DESK;   }

            public:
                status_t        set_actions(size_t flags);
                status_t        toggle_actions(size_t flags);
                status_t        allow_actions(size_t flags);
                status_t        deny_actions(size_t flags);

                inline status_t set(window_action_t wa, bool set = true) { return set_actions((set) ? (nFlags | wa) : (nFlags & (~wa)));}
                inline status_t toggle(window_action_t wa)          { return set_actions(nFlags ^ wa); }
                inline status_t allow(window_action_t wa)           { return set_actions(nFlags | wa); }
                inline status_t deny(window_action_t wa)            { return set_actions(nFlags & (~wa)); }

                inline status_t set_movable()                       { return allow(WA_MOVE); }
                inline status_t set_movable(bool value)             { return set(WA_MOVE, value); }

                inline status_t set_resizable()                     { return allow(WA_RESIZE); }
                inline status_t set_resizable(bool value)           { return set(WA_RESIZE, value); }

                inline status_t set_minimizable()                   { return allow(WA_MINIMIZE); }
                inline status_t set_minimizable(bool value)         { return set(WA_MINIMIZE, value); }

                inline status_t set_maximizable()                   { return allow(WA_MAXIMIZE); }
                inline status_t set_maximizable(bool value)         { return set(WA_MAXIMIZE, value); }

                inline status_t set_closeable()                     { return allow(WA_CLOSE); }
                inline status_t set_closeable(bool value)           { return set(WA_CLOSE, value); }

                inline status_t set_stickable()                     { return allow(WA_STICK); }
                inline status_t set_stickable(bool value)           { return set(WA_STICK, value); }

                inline status_t set_shadeable()                     { return allow(WA_SHADE); }
                inline status_t set_shadeable(bool value)           { return set(WA_SHADE, value); }

                inline status_t set_fullscreenable()                { return allow(WA_FULLSCREEN); }
                inline status_t set_fullscreenable(bool value)      { return set(WA_FULLSCREEN, value); }

                inline status_t set_transportable()                 { return allow(WA_CHANGE_DESK); }
                inline status_t set_transportable(bool value)       { return set(WA_CHANGE_DESK, value); }

                inline status_t allow_all()                         { return set_actions(WA_ALL); }
                inline status_t deny_all()                          { return set_actions(WA_NONE); }
                inline status_t toggle_all()                        { return set_actions(nFlags ^ WA_ALL); }
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPWINDOWACTIONS_H_ */
