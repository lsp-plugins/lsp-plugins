/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 9 окт. 2019 г.
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
        LSPFloat::Listener::Listener(LSPFloat *property)
        {
            pProperty   = property;
            pStyle      = NULL;
            aValue      = -1;
        }

        LSPFloat::Listener::~Listener()
        {
            unbind();
            pProperty   = NULL;
            pStyle      = NULL;
        }

        void LSPFloat::Listener::notify(ui_atom_t property)
        {
            if ((pStyle == NULL) || (pProperty == NULL))
                return;

            if (property != aValue)
                return;

            if (pStyle->get_float(aValue, &pProperty->fValue) != STATUS_OK)
                return;

            // Query widget for redraw
            if (pProperty->pWidget != NULL)
                pProperty->pWidget->query_draw();
        }

        void LSPFloat::Listener::sync()
        {
            if ((pStyle == NULL) || (pProperty == NULL))
                return;
            if (aValue >= 0)
                pStyle->set_float(aValue, pProperty->get());
        }

        void LSPFloat::Listener::unbind()
        {
            if (pStyle == NULL)
                return;

            if (aValue >= 0)
            {
                pStyle->unbind(aValue, this);
                aValue = -1;
            }

            pStyle  = NULL;
        }

        status_t LSPFloat::Listener::bind(LSPDisplay *dpy, LSPStyle *style, const char *property)
        {
            if (pStyle == style)
                return STATUS_OK;

            unbind();

            ui_atom_t atom = dpy->atom_id(property);
            if (atom <= 0)
                return -atom;

            style->begin();
            status_t res = style->bind_float(atom, this);
            if (res == STATUS_OK)
            {
                aValue      = atom;
                pStyle      = style;
            }
            style->end();

            return res;
        }
        
        LSPFloat::LSPFloat() : sListener(this)
        {
            pWidget     = NULL;
            fValue      = 0.0f;
        }

        LSPFloat::LSPFloat(LSPWidget *widget) : sListener(this)
        {
            pWidget     = widget;
            fValue      = 0.0f;
        }
        
        LSPFloat::~LSPFloat()
        {
            sListener.unbind();
        }

        status_t LSPFloat::bind(const char *property)
        {
            if (property == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (pWidget == NULL)
                return STATUS_BAD_STATE;
            return sListener.bind(pWidget->display(), pWidget->style(), property);
        }

        status_t LSPFloat::bind(LSPStyle *style, const char *property)
        {
            if ((property == NULL) || (style == NULL))
                return STATUS_BAD_ARGUMENTS;
            if (pWidget == NULL)
                return STATUS_BAD_STATE;

            return sListener.bind(pWidget->display(), style, property);
        }

        status_t LSPFloat::bind(LSPDisplay *dpy, LSPStyle *style, const char *property)
        {
            if ((property == NULL) || (style == NULL) || (dpy == NULL))
                return STATUS_BAD_ARGUMENTS;

            return sListener.bind(dpy, style, property);
        }

        float LSPFloat::set(float v)
        {
            float prev = fValue;
            if (v != prev)
            {
                fValue  = v;
                sListener.sync();

                if (pWidget != NULL)
                    pWidget->query_draw();
            }

            return prev;
        }
    
    } /* namespace calc */
} /* namespace lsp */
