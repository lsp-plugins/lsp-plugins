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
            if (src != NULL)
                construct(src->sText, src->fValue);
            else
                construct(NULL, 0.0f);
        }

        LSPItem::LSPItem(float value)
        {
            construct(NULL, value);
        }

        LSPItem::LSPItem(const char *text, float value)
        {
            construct(text, value);
        }

        LSPItem::~LSPItem()
        {
            if (sText != NULL)
            {
                lsp_free(sText);
                sText = NULL;
            }
        }

        void LSPItem::construct(const char *text, float value)
        {
            sText       = (text != NULL) ? strdup(text) : NULL;
            fValue      = value;
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
            // Prevent from setting the same value
            if (sText == text)
                return STATUS_OK;
            else if (text != NULL)
            {
                if ((sText != NULL) && (!strcmp(sText, text)))
                    return STATUS_OK;
            }

            // Duplicate text
            char *t = NULL;
            if (text != NULL)
            {
                t = strdup(text);
                if (t == NULL)
                    return STATUS_NO_MEM;
            }

            // Drop previously used text
            if (sText != NULL)
                free(sText);
            sText       = t;
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
            char *old_text  = sText;
            float old_value = fValue;

            status_t res = set_text(text);
            if (res == STATUS_OK)
                set_value(value);

            if ((old_text != sText) || (old_value != fValue))
                on_change();
            return res;
        }

        status_t LSPItem::set(const LSPItem *src)
        {
            if (src == NULL)
                return set(NULL, 0.0f);

            char *old_text  = sText;
            float old_value = fValue;

            status_t res = set_text(src->sText);
            if (res == STATUS_OK)
                set_value(src->fValue);

            if ((old_text != sText) || (old_value != fValue))
                on_change();
            return res;
        }

    } /* namespace tk */
} /* namespace lsp */
