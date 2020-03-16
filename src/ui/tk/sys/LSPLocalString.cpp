/*
 * LSPLocalString.cpp
 *
 *  Created on: 2 мар. 2020 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <core/calc/format.h>

#define LANG_ATOM_NAME      "language"

namespace lsp
{
    namespace tk
    {
        void LSPLocalString::Params::modified()
        {
            if (pString != NULL)
                pString->sync();
        }

        void LSPLocalString::Listener::notify(ui_atom_t property)
        {
            if (pString != NULL)
                pString->notify(property);
        }

        LSPLocalString::LSPLocalString():
            sParams(this),
            sListener(this)
        {
            pWidget     = NULL;
            nFlags      = 0;
            nAtom       = -1;
        }
        
        LSPLocalString::LSPLocalString(LSPWidget *widget):
            sParams(this),
            sListener(this)
        {
            pWidget     = widget;
            nFlags      = 0;
            nAtom       = -1;
        }

        LSPLocalString::~LSPLocalString()
        {
            unbind();

            pWidget     = NULL;
            nFlags      = 0;
            nAtom       = -1;
        }

        status_t LSPLocalString::bind(ui_atom_t property)
        {
            if (nAtom >= 0)
                return STATUS_ALREADY_BOUND;

            LSPDisplay *dpy = pWidget->display();
            LSPStyle *style = pWidget->style();
            if ((dpy == NULL) || (style == NULL))
                return STATUS_BAD_STATE;

            status_t res = style->bind_string(property, &sListener);
            if (res != STATUS_OK)
                return res;

            nAtom       = property;
            return STATUS_OK;
        }

        status_t LSPLocalString::bind()
        {
            if (pWidget == NULL)
                return STATUS_BAD_STATE;
            else if (nAtom >= 0)
                return STATUS_ALREADY_BOUND;
            LSPDisplay *dpy = pWidget->display();
            if (dpy == NULL)
                return STATUS_BAD_STATE;

            ui_atom_t atom = dpy->atom_id(LANG_ATOM_NAME);
            if (atom < 0)
                return -atom;

            return bind(atom);
        }

        status_t LSPLocalString::bind(const char *property)
        {
            if (pWidget == NULL)
                return STATUS_BAD_STATE;
            else if (nAtom >= 0)
                return STATUS_ALREADY_BOUND;
            LSPDisplay *dpy = pWidget->display();
            if (dpy == NULL)
                return STATUS_BAD_STATE;

            ui_atom_t atom = dpy->atom_id((property != NULL) ? property : LANG_ATOM_NAME);
            if (atom < 0)
                return -atom;

            return bind(atom);
        }

        status_t LSPLocalString::bind(const LSPString *property)
        {
            if (pWidget == NULL)
                return STATUS_BAD_STATE;
            else if (nAtom >= 0)
                return STATUS_ALREADY_BOUND;
            LSPDisplay *dpy = pWidget->display();
            if (dpy == NULL)
                return STATUS_BAD_STATE;

            const char *prop = (property != NULL) ? property->get_utf8() : LANG_ATOM_NAME;
            if (prop == NULL)
                return STATUS_NO_MEM;
            ui_atom_t atom = dpy->atom_id(prop);
            if (atom < 0)
                return -atom;

            return bind(atom);
        }


        status_t LSPLocalString::unbind()
        {
            if (pWidget == NULL)
                return STATUS_OK;

            // Unbind from previous owner
            if (nAtom >= 0)
            {
                status_t res = pWidget->style()->unbind(nAtom, &sListener);
                if ((res != STATUS_OK) && (res != STATUS_NOT_BOUND))
                    return res;
                nAtom = -1;
            }

            return STATUS_OK;
        }

        void LSPLocalString::notify(ui_atom_t property)
        {
            if (property == nAtom)
                sync();
        }

        void LSPLocalString::sync()
        {
            // Trigger the widget for resize if property has changed
            if (pWidget != NULL)
                pWidget->query_resize();
        }
    
        status_t LSPLocalString::set_raw(const LSPString *value)
        {
            if (value == NULL)
                sText.truncate();
            else if (!sText.set(value))
                return STATUS_NO_MEM;

            nFlags      = 0;
            sParams.clear();

            sync();
            return STATUS_OK;
        }

        status_t LSPLocalString::set_raw(const char *value)
        {
            if (value == NULL)
                sText.truncate();
            else if (!sText.set_utf8(value))
                return STATUS_NO_MEM;

            nFlags      = 0;
            sParams.clear();

            sync();
            return STATUS_OK;
        }

        status_t LSPLocalString::set_params(const calc::Parameters *params)
        {
            if (params != NULL)
                return sParams.set(params); // will call sync();

            sParams.clear(); // will call sync();
            return STATUS_OK;
        }

        status_t LSPLocalString::set(const LSPString *key, const calc::Parameters *params)
        {
            if (key == NULL)
            {
                clear();
                return STATUS_OK;
            }

            LSPString ts;
            calc::Parameters tp;

            // Create copies
            if (!ts.set(key))
                return STATUS_NO_MEM;
            if (params != NULL)
            {
                status_t res = tp.set(params);
                if (res != STATUS_OK)
                    return res;
            }
            else
                tp.clear();

            // Apply
            nFlags      = F_LOCALIZED;
            sText.swap(&ts);
            sParams.swap(&tp); // will call sync()

            return STATUS_OK;
        }

        status_t LSPLocalString::set_key(const char *key)
        {
            if (key == NULL)
            {
                sText.clear();
                sync();
                return STATUS_OK;
            }
            else if (!sText.set_utf8(key))
                return STATUS_NO_MEM;

            // Apply
            nFlags      = F_LOCALIZED;
            sync();
            return STATUS_OK;
        }

        status_t LSPLocalString::set_key(const LSPString *key)
        {
            if (key == NULL)
            {
                sText.clear();
                sync();
                return STATUS_OK;
            }
            else if (!sText.set(key))
                return STATUS_NO_MEM;

            // Apply
            nFlags      = /* F_DIRTY | */ F_LOCALIZED;
            sync();
            return STATUS_OK;
        }

        status_t LSPLocalString::set(const char *key, const calc::Parameters *params)
        {
            if (key == NULL)
            {
                clear();
                return STATUS_OK;
            }

            LSPString ts;
            calc::Parameters tp;

            // Create copies
            if (!ts.set_utf8(key))
                return STATUS_NO_MEM;
            if (params != NULL)
            {
                status_t res = tp.set(params);
                if (res != STATUS_OK)
                    return res;
            }
            else
                tp.clear();

            // Apply
            nFlags      = F_LOCALIZED;
            sText.swap(&ts);
            sParams.swap(&tp); // Will call sync()

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
            nFlags      = value->nFlags;
            sText.swap(&ts);
            sParams.swap(&tp); // Will call sync()

            return STATUS_OK;
        }

        void LSPLocalString::clear()
        {
            sText.truncate();
            sParams.clear();
            nFlags      = 0;
            sync();
        }

        status_t LSPLocalString::fmt_internal(LSPString *out, IDictionary *dict, const LSPString *lang) const
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
                return (out->set(&sText)) ? STATUS_OK : STATUS_NO_MEM; // By default output dictionary key
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

            return fmt_internal(out, dict, &xlang);
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

            return fmt_internal(out, dict, lang);
        }

        status_t LSPLocalString::format(LSPString *out, LSPDisplay *dpy, const LSPStyle *style) const
        {
            if ((dpy == NULL) || (style == NULL))
                return format(out, (IDictionary *)NULL, (const char *)NULL);

            // Get identifier of atom that describes language
            ssize_t atom = dpy->atom_id(LANG_ATOM_NAME);
            if (atom < 0)
                return format(out, (IDictionary *)NULL, (const char *)NULL);

            // Get language name from style property
            LSPString lang;
            status_t res = style->get_string(atom, &lang);
            if (res != STATUS_OK)
                return format(out, (IDictionary *)NULL, (const char *)NULL);

            // Perform formatting
            return format(out, dpy->dictionary(), &lang);
        }

        status_t LSPLocalString::format(LSPString *out, LSPWidget *widget) const
        {
            if (widget == NULL)
                return format(out, (IDictionary *)NULL, (const char *)NULL);
            return format(out, widget->display(), widget->style());
        }

        status_t LSPLocalString::format(LSPString *out) const
        {
            if (pWidget == NULL)
                return (out->set(&sText)) ? STATUS_OK : STATUS_NO_MEM;

            LSPDisplay *dpy = pWidget->display();
            LSPStyle *style = pWidget->style();
            if ((dpy == NULL) || (style == NULL))
                return format(out, (IDictionary *)NULL, (const char *)NULL);

            LSPString lang;
            status_t res = style->get_string(nAtom, &lang);
            if (res != STATUS_OK)
                return format(out, (IDictionary *)NULL, (const char *)NULL);

            return format(out, dpy->dictionary(), &lang);
        }

        void LSPLocalString::swap(LSPLocalString *dst)
        {
            sText.swap(&dst->sText);
            sParams.swap(&dst->sParams);
            ::swap(nFlags, dst->nFlags);
        }

    } /* namespace tk */
} /* namespace lsp */
