/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 9 авг. 2017 г.
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
        void LSPItem::LocalString::sync()
        {
            if (pItem != NULL)
                pItem->on_change();
        }

        LSPItem::LSPItem():
            sText(this)
        {
            fValue      = 0.0f;
        }

        LSPItem::LSPItem(const LSPItem *src):
            sText(NULL)
        {
            sText.set(&src->sText);
            fValue      = src->fValue;
            sText.pItem = this;
        }

        LSPItem::~LSPItem()
        {
        }

        void LSPItem::on_change()
        {
        }

        void LSPItem::set_value(float value)
        {
            if (fValue == value)
                return;
            fValue      = value;
            on_change();
        }

        status_t LSPItem::set(const LSPItem *src)
        {
            status_t res    = sText.set(&src->sText);
            if (res != STATUS_OK)
                return res;

            float back = fValue;
            fValue = src->fValue;
            if ((res = sText.set(&src->sText)) != STATUS_OK)
                fValue = back;

            return res;
        }

    } /* namespace tk */
} /* namespace lsp */
