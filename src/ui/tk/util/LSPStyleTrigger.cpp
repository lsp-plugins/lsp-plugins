/*
 * LSPStyleTrigger.cpp
 *
 *  Created on: 4 мар. 2020 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        
        LSPStyleTrigger::LSPStyleTrigger(LSPWidget *widget)
        {
            pWidget = widget;
        }
        
        LSPStyleTrigger::~LSPStyleTrigger()
        {
            vBindings.flush();
            pWidget = NULL;
        }

        void LSPStyleTrigger::notify(ui_atom_t property)
        {
            if ((pWidget == NULL) || (vBindings.size() <= 0))
                return;
            LSPStyle *style = pWidget->style();
            if (style == NULL)
                return;

            // Select list of events to trigger
            cvector<binding_t> events;
            for (size_t i=0, n=vBindings.size(); i<n; ++i)
            {
                binding_t *bind = vBindings.at(i);
                if (bind->id != property)
                    continue;
                else if (!events.add(bind))
                    return;
            }

            // Trigger events
            for (size_t i=0, n=events.size(); i<n; ++i)
            {
                binding_t *bind = vBindings.at(i);
                switch (bind->action)
                {
                    case STYLE_TRG_REDRAW:
                        pWidget->query_draw();
                        break;
                    case STYLE_TRG_RESIZE:
                        pWidget->query_resize();
                        break;
                    default:
                        break;
                }
            }
        }

        status_t LSPStyleTrigger::bind(ui_atom_t id, ui_property_type_t type, style_trigger_action_t action)
        {
            binding_t *bind;
            for (size_t i=0, n=vBindings.size(); i<n; ++i)
            {
                bind = vBindings.at(i);
                if ((bind->id == id) && (bind->action == action))
                    return STATUS_ALREADY_BOUND;
            }

            if ((bind = vBindings.add()) == NULL)
                return STATUS_NO_MEM;

            bind->id        = id;
            bind->action    = action;

            return STATUS_OK;
        }

        status_t LSPStyleTrigger::bind(const char *name, ui_property_type_t type, style_trigger_action_t action)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPDisplay *dpy = (pWidget != NULL) ? pWidget->display() : NULL;
            if (dpy == NULL)
                return STATUS_BAD_STATE;
            ui_atom_t id = dpy->atom_id(name);
            if (id < 0)
                return -id;
            return bind(id, type, action);
        }

        status_t LSPStyleTrigger::bind(const LSPString *name, ui_property_type_t type, style_trigger_action_t action)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPDisplay *dpy = (pWidget != NULL) ? pWidget->display() : NULL;
            if (dpy == NULL)
                return STATUS_BAD_STATE;
            const char *xname = name->get_utf8();
            if (xname == NULL)
                return STATUS_NO_MEM;
            ui_atom_t id = dpy->atom_id(xname);
            if (id < 0)
                return -id;
            return bind(id, type, action);
        }
    
        bool LSPStyleTrigger::is_bound(ui_atom_t id, style_trigger_action_t action) const
        {
            LSPStyleTrigger *_this = const_cast<LSPStyleTrigger *>(this);

            LSPStyle *style = (_this->pWidget != NULL) ? _this->pWidget->style() : NULL;
            if (style == NULL)
                return false;

            for (size_t i=0, n=_this->vBindings.size(); i<n; )
            {
                binding_t * bind = _this->vBindings.at(i);
                if ((bind->id == id) && (bind->action == action))
                    return style->is_bound(id, _this);
            }

            return false;
        }

        bool LSPStyleTrigger::is_bound(const char *name, style_trigger_action_t action) const
        {
            if (name == NULL)
                return false;
            LSPDisplay *dpy = (pWidget != NULL) ? pWidget->display() : NULL;
            if (dpy == NULL)
                return false;
            ui_atom_t id = dpy->atom_id(name);
            if (id < 0)
                return false;
            return is_bound(id, action);
        }

        bool LSPStyleTrigger::is_bound(const LSPString *name, style_trigger_action_t action) const
        {
            if (name == NULL)
                return false;
            LSPDisplay *dpy = (pWidget != NULL) ? pWidget->display() : NULL;
            if (dpy == NULL)
                return false;
            const char *xname = name->get_utf8();
            if (xname == NULL)
                return false;
            ui_atom_t id = dpy->atom_id(xname);
            if (id < 0)
                return false;
            return is_bound(id, action);
        }

        status_t LSPStyleTrigger::unbind(ui_atom_t id, style_trigger_action_t action)
        {
            LSPStyle *style = (pWidget != NULL) ? pWidget->style() : NULL;
            if (style == NULL)
                return STATUS_BAD_STATE;

            status_t res = STATUS_NOT_BOUND;
            size_t counter = 0;

            for (size_t i=0, n=vBindings.size(); i<n; )
            {
                binding_t * bind = vBindings.at(i);
                if (bind->id != id)
                    continue;

                if (bind->action == action)
                {
                    if (!vBindings.remove(i))
                        return STATUS_CORRUPTED;
                    res     = STATUS_OK;
                    --n;
                }
                else
                {
                    ++i;
                    ++counter;
                }
            }

            // Nothing was removed?
            if (res != STATUS_OK)
                return res;

            if (counter <= 0)
                res = style->unbind(id, this);

            return res;
        }

        status_t LSPStyleTrigger::unbind(const char *name, style_trigger_action_t action)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPDisplay *dpy = (pWidget != NULL) ? pWidget->display() : NULL;
            if (dpy == NULL)
                return STATUS_BAD_STATE;
            ui_atom_t id = dpy->atom_id(name);
            if (id < 0)
                return -id;
            return unbind(id, action);
        }

        status_t LSPStyleTrigger::unbind(const LSPString *name, style_trigger_action_t action)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPDisplay *dpy = (pWidget != NULL) ? pWidget->display() : NULL;
            if (dpy == NULL)
                return STATUS_BAD_STATE;
            const char *xname = name->get_utf8();
            if (xname == NULL)
                return STATUS_NO_MEM;
            ui_atom_t id = dpy->atom_id(xname);
            if (id < 0)
                return -id;
            return unbind(id, action);
        }

    } /* namespace tk */
} /* namespace lsp */
