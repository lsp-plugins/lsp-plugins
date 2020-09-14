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

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPMenuItem::metadata = { "LSPMenuItem", &LSPWidget::metadata };

        LSPMenuItem::LSPMenuItem(LSPDisplay *dpy):
            LSPWidget(dpy),
            sText(this)
        {
            pSubmenu    = NULL;
            bSeparator  = false;
            pClass      = &metadata;
        }
        
        LSPMenuItem::~LSPMenuItem()
        {
        }

        status_t LSPMenuItem::slot_on_submit(LSPWidget *sender, void *ptr, void *data)
        {
            LSPMenuItem *_this = widget_ptrcast<LSPMenuItem>(ptr);
            return (ptr != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPMenuItem::init()
        {
            ui_handler_id_t id = sSlots.add(LSPSLOT_SUBMIT, slot_on_submit, self());
            sText.bind();

            return (id >= 0) ? STATUS_OK : -id;
        }

        status_t LSPMenuItem::set_submenu(LSPMenu *submenu)
        {
            if (pSubmenu == submenu)
                return STATUS_OK;

            pSubmenu = submenu;
            query_draw();

            return STATUS_OK;
        }

        status_t LSPMenuItem::set_separator(bool value)
        {
            if (bSeparator == value)
                return STATUS_OK;

            bSeparator = value;
            query_resize();

            return STATUS_OK;
        }

        status_t LSPMenuItem::on_submit()
        {
            return STATUS_OK;
        }
    
    } /* namespace tk */
} /* namespace lsp */
