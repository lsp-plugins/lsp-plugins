/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 18 сент. 2017 г.
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

#ifndef UI_TK_WIDGETS_LSPMENUITEM_H_
#define UI_TK_WIDGETS_LSPMENUITEM_H_

namespace lsp
{
    namespace tk
    {
        class LSPMenu;

        class LSPMenuItem: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                friend class LSPMenu;

            protected:
                LSPLocalString  sText;
                LSPMenu        *pSubmenu;
                bool            bSeparator;

            protected:
                static status_t        slot_on_submit(LSPWidget *sender, void *ptr, void *data);

            public:
                explicit LSPMenuItem(LSPDisplay *dpy);
                virtual ~LSPMenuItem();

                virtual status_t init();

            public:
                inline LSPLocalString  *text()                  { return &sText; }
                inline const LSPLocalString  *text() const      { return &sText; }
                LSPMenu                *submenu()               { return pSubmenu; }
                inline bool             is_separator() const    { return bSeparator; }
                inline bool             has_submenu() const     { return pSubmenu != NULL; }

            public:
                status_t            set_submenu(LSPMenu *submenu);
                status_t            set_separator(bool value);

            public:
                virtual status_t    on_submit();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPMENUITEM_H_ */
