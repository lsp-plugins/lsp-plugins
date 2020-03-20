/*
 * LSPItem.cpp
 *
 *  Created on: 9 авг. 2017 г.
 *      Author: sadko
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
