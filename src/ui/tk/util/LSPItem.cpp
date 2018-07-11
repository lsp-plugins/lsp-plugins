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
        LSPItem::LSPItem(const LSPItem *src)
        {
            sText.set(&src->sText);
            fValue      = src->fValue;
        }

        LSPItem::LSPItem(float value)
        {
            sText.set_native("");
            fValue      = value;
        }

        LSPItem::LSPItem(const char *text, float value)
        {
            sText.set_native((text != NULL) ? text : "");
            fValue      = value;
        }

        LSPItem::LSPItem(const LSPString *text, float value)
        {
            sText.set(text);
            fValue      = value;
        }

        LSPItem::~LSPItem()
        {
        }

        status_t LSPItem::get_text(LSPString *text) const
        {
            return text->set(&sText);
        }

        void LSPItem::on_text_change()
        {
        }

        void LSPItem::on_value_change()
        {
        }

        void LSPItem::on_change()
        {
        }

        status_t LSPItem::set_text(const char *text)
        {
            LSPString s;
            status_t res = s.set_native((text == NULL) ? "" : text);
            if (res != STATUS_OK)
                return res;
            else if (s.equals(&sText))
                return STATUS_OK;

            sText.swap(&s);
            on_text_change();

            return STATUS_OK;
        }

        status_t LSPItem::set_text(const LSPString *text)
        {
            if (sText.equals(text))
                return STATUS_OK;

            status_t res = sText.set(text);
            if (res != STATUS_OK)
                return res;

            on_text_change();
            return STATUS_OK;
        }

        void LSPItem::set_value(float value)
        {
            if (fValue == value)
                return;
            fValue      = value;
            on_value_change();
        }

        status_t LSPItem::set(const char *text, float value)
        {
            LSPString s;
            s.set_native((text != NULL) ? text : "");

            if (sText.equals(&s))
                return STATUS_OK;

            s.swap(&sText);
            on_change();
            return STATUS_OK;
        }

        status_t LSPItem::set(const LSPItem *src)
        {
            if (src == NULL)
                return set(NULL, 0.0f);

            if ((sText.equals(&src->sText)) && (fValue == src->fValue))
                return STATUS_OK;

            status_t res    = sText.set(&src->sText);
            if (res != STATUS_OK)
                return res;

            set_value(src->fValue);
            on_change();
            return res;
        }

    } /* namespace tk */
} /* namespace lsp */
