/*
 * LSPLocalString.cpp
 *
 *  Created on: 2 мар. 2020 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        
        LSPLocalString::LSPLocalString()
        {
            nFlags      = 0;
            pOwner      = NULL;
        }
        
        LSPLocalString::LSPLocalString(LSPWidget *owner)
        {
            nFlags      = 0;
            pOwner      = owner;
        }

        LSPLocalString::~LSPLocalString()
        {
            nFlags      = 0;
            pOwner      = NULL;
        }
    
        status_t LSPLocalString::set_raw(const LSPString *value)
        {
            if (value == NULL)
                sText.truncate();
            else if (!sText.set(value))
                return STATUS_NO_MEM;

            nFlags      = 0; //F_DIRTY;
            sParams.clear();

            if (pOwner != NULL)
                pOwner->query_resize();

            return STATUS_OK;
        }

        status_t LSPLocalString::set_raw(const char *value)
        {
            if (value == NULL)
                sText.truncate();
            else if (!sText.set_utf8(value))
                return STATUS_NO_MEM;

            nFlags      = 0; //F_DIRTY;
            sParams.clear();

            if (pOwner != NULL)
                pOwner->query_resize();

            return STATUS_OK;
        }

        status_t LSPLocalString::set(const LSPString *key, const calc::Parameters *params)
        {
            if (key == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString ts;
            calc::Parameters tp;

            // Create copies
            if (!ts.set(key))
                return STATUS_NO_MEM;
            status_t res = tp.set(params);
            if (res != STATUS_OK)
                return res;

            // Apply
            sText.swap(&ts);
            sParams.swap(&tp);
            nFlags      = /* F_DIRTY | */ F_LOCALIZED;

            if (pOwner != NULL)
                pOwner->query_resize();

            return STATUS_OK;
        }

        status_t LSPLocalString::set(const char *key, const calc::Parameters *params)
        {
            if (key == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString ts;
            calc::Parameters tp;

            // Create copies
            if (!ts.set_utf8(key))
                return STATUS_NO_MEM;
            status_t res = tp.set(params);
            if (res != STATUS_OK)
                return res;

            // Apply
            sText.swap(&ts);
            sParams.swap(&tp);
            nFlags      = /* F_DIRTY | */ F_LOCALIZED;

            if (pOwner != NULL)
                pOwner->query_resize();

            return STATUS_OK;
        }

        status_t LSPLocalString::set(const LSPLocalString *value)
        {
            if (value == NULL)
            {
                clear();
                return STATUS_OK;
            }

            LSPString ts;
            calc::Parameters tp;

            // Create copies
            if (!ts.set(&value->sText))
                return STATUS_NO_MEM;
            status_t res = tp.set(&value->sParams);
            if (res != STATUS_OK)
                return res;

            // Apply
            sText.swap(&ts);
            sParams.swap(&tp);
            nFlags      = value->nFlags;

            if (pOwner != NULL)
                pOwner->query_resize();

            return STATUS_OK;
        }

        void LSPLocalString::clear()
        {
            sText.truncate();
            sParams.clear();
            nFlags      = 0; //F_DIRTY;

            if (pOwner != NULL)
                pOwner->query_resize();
        }

    } /* namespace tk */
} /* namespace lsp */
