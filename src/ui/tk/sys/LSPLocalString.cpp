/*
 * LSPLocalString.cpp
 *
 *  Created on: 2 мар. 2020 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <core/calc/format.h>

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

        status_t LSPLocalString::fmt_intarnal(LSPString *out, IDictionary *dict, const LSPString *lang) const
        {
            LSPString path, templ;
            status_t res = STATUS_NOT_FOUND;

            // Search first template in target language if target language specified
            if (lang != NULL)
            {
                if (!path.append(lang))
                    return STATUS_NO_MEM;
                if (!path.append('.'))
                    return STATUS_NO_MEM;
                if (!path.append(&sText))
                    return STATUS_NO_MEM;

                res = dict->lookup(&path, &templ);
            }

            // Now search in default language
            if (res == STATUS_NOT_FOUND)
            {
                path.clear();
                if (!path.append_ascii("default"))
                    return STATUS_NO_MEM;
                if (!path.append('.'))
                    return STATUS_NO_MEM;
                if (!path.append(&sText))
                    return STATUS_NO_MEM;

                res = dict->lookup(&path, &templ);
            }

            // Still no template? Leave
            if (res == STATUS_NOT_FOUND)
            {
                out->clear();
                return STATUS_OK;
            }
            else if (res != STATUS_OK)
                return res;

            // Format the template
            return calc::format(out, &templ, &sParams);
        }

        status_t LSPLocalString::format(LSPString *out, IDictionary *dict, const char *lang) const
        {
            if (out == NULL)
                return STATUS_BAD_ARGUMENTS;

            if (!(nFlags & F_LOCALIZED))
                return (out->set(&sText)) ? STATUS_OK : STATUS_NO_MEM;

            if (dict == NULL)
            {
                out->clear();
                return STATUS_OK;
            }

            LSPString xlang;
            if (!xlang.set_utf8(lang))
                return STATUS_NO_MEM;

            return fmt_intarnal(out, dict, &xlang);
        }

        status_t LSPLocalString::format(LSPString *out, IDictionary *dict, const LSPString *lang) const
        {
            if (out == NULL)
                return STATUS_BAD_ARGUMENTS;

            if (!(nFlags & F_LOCALIZED))
                return (out->set(&sText)) ? STATUS_OK : STATUS_NO_MEM;

            if (dict == NULL)
            {
                out->clear();
                return STATUS_OK;
            }

            return fmt_intarnal(out, dict, lang);
        }

    } /* namespace tk */
} /* namespace lsp */
