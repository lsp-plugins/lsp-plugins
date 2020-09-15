/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 23 окт. 2017 г.
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

#ifndef UI_TK_WIDGETS_LSPHYPERLINK_H_
#define UI_TK_WIDGETS_LSPHYPERLINK_H_

namespace lsp
{
    namespace tk
    {
        class LSPHyperlink: public LSPLabel
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum state_t
                {
                    F_MOUSE_IN      = 1 << 0,
                    F_MOUSE_DOWN    = 1 << 1,
                    F_MOUSE_IGN     = 1 << 2,
                };

            protected:
                LSPColor        sHoverColor;
                LSPString       sUrl;
                size_t          nMFlags;
                bool            bFollow;
                size_t          nState;
                LSPMenu         sStdMenu;
                LSPMenuItem    *vStdItems[2];
                LSPMenu        *pPopup;

            protected:
                static status_t         slot_on_submit(LSPWidget *sender, void *ptr, void *data);
                static status_t         slot_copy_link_action(LSPWidget *sender, void *ptr, void *data);

            public:
                explicit LSPHyperlink(LSPDisplay *dpy);
                virtual ~LSPHyperlink();

                virtual status_t init();

                virtual void destroy();

            public:
                inline const char      *url() const         { return sUrl.get_native(); }
                inline status_t         get_url(LSPString *dst) const { return (dst->set(&sUrl)) ? STATUS_OK : STATUS_NO_MEM; };
                inline LSPColor        *hover()             { return &sHoverColor; }
                inline bool             follow() const      { return bFollow;       }
                LSPMenu                *popup()             { return pPopup;        }

            public:
                status_t set_url(const char *url);

                status_t set_url(const LSPString *url);

                void set_follow(bool follow = true);

                status_t follow_url();

                status_t copy_url(clipboard_id_t cb);

                inline void set_popup(LSPMenu *popup)       { pPopup = popup; }

            public:
                virtual void draw(ISurface *s);

                virtual status_t on_mouse_in(const ws_event_t *e);

                virtual status_t on_mouse_out(const ws_event_t *e);

                virtual status_t on_mouse_move(const ws_event_t *e);

                virtual status_t on_mouse_down(const ws_event_t *e);

                virtual status_t on_mouse_up(const ws_event_t *e);

                virtual status_t on_submit();

        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPHYPERLINK_H_ */
