/*
 * LSPStyle.cpp
 *
 *  Created on: 1 окт. 2019 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        IStyleListener::~IStyleListener()
        {
        }

        void IStyleListener::notify(ui_atom_t property)
        {
        }
        
        LSPStyle::LSPStyle()
        {
            pParent     = NULL;
        }
        
        LSPStyle::~LSPStyle()
        {
            do_destroy();
        }

        status_t LSPStyle::init()
        {
            return STATUS_OK;
        }

        void LSPStyle::destroy()
        {
            do_destroy();
        }

        void LSPStyle::do_destroy()
        {
            // Unlink listeners
            vListeners.flush();

            // Destroy properties
            for (size_t i=0, n=vProperties.size(); i<n; ++i)
                undef_property(vProperties.at(i));
            vProperties.flush();

            // Unlink from parent
            if (pParent != NULL)
            {
                pParent->vChildren.remove(this, true);
                pParent     = NULL;
            }

            // Unlink from children
            for (size_t i=0, n=vChildren.size(); i<n; ++i)
            {
                LSPStyle *child = vChildren.at(i);
                if (child != NULL)
                    child->pParent = NULL;
            }
            vChildren.flush();
        }

        void LSPStyle::undef_property(property_t *property)
        {
            if (property == NULL)
                return;

            switch (property->type)
            {
                case PT_STRING:
                    if (property->v.sValue != NULL)
                        ::free(property->v.sValue);
                    break;
                default:
                    break;
            }

            property->type = PT_UNKNOWN;
        }

        status_t LSPStyle::copy_property(property_t *dst, const property_t *src)
        {
            // Check type of property
            if (src->type != dst->type)
                return STATUS_OK;

            // Update contents
            switch (src->type)
            {
                case PT_INT:
                    if (dst->v.iValue != src->v.iValue)
                        ++dst->changes;
                    dst->v.iValue   = src->v.iValue;
                    break;
                case PT_FLOAT:
                    if (dst->v.fValue != src->v.fValue)
                        ++dst->changes;
                    dst->v.fValue   = src->v.fValue;
                    break;
                case PT_BOOL:
                    if (dst->v.bValue != src->v.bValue)
                        ++dst->changes;
                    dst->v.bValue   = src->v.bValue;
                    break;
                case PT_STRING:
                {
                    // Value does match?
                    if (::strcmp(dst->v.sValue, src->v.sValue) == 0)
                        break;

                    // Update value
                    char *tmp = ::strdup(src->v.sValue);
                    if (tmp == NULL)
                        return STATUS_NO_MEM;
                    ::free(dst->v.sValue);
                    dst->v.sValue = tmp;
                    ++dst->changes;
                    break;
                }
            }

            return STATUS_OK;
        }

        status_t LSPStyle::init_property(property_t *dst, const property_t *src)
        {
            // Init contents
            switch (src->type)
            {
                case PT_INT:
                    dst->v.iValue   = src->v.iValue;
                    break;
                case PT_FLOAT:
                    dst->v.fValue   = src->v.fValue;
                    break;
                case PT_BOOL:
                    dst->v.bValue   = src->v.bValue;
                    break;
                case PT_STRING:
                {
                    // Update value
                    char *tmp = ::strdup(src->v.sValue);
                    if (tmp == NULL)
                        return STATUS_NO_MEM;
                    dst->v.sValue = tmp;
                    break;
                }
                default:
                    return STATUS_BAD_TYPE;
            }

            dst->changes  = 0;
            dst->type     = src->type;

            return STATUS_OK;
        }

        status_t LSPStyle::init_property(property_t *p, ui_atom_t id, size_t type)
        {
            p->type     = type;
            p->dfl      = true;

            property_t *parent = (pParent != NULL) ? pParent->get_property_recursive(id) : NULL;
            if (parent != NULL)
                return copy_property(p, parent);

            switch (type)
            {
                case PT_INT: p->v.iValue = 0; break;
                case PT_FLOAT: p->v.fValue = 0; break;
                case PT_BOOL: p->v.bValue = false; break;
                case PT_STRING:
                {
                    char *v = ::strdup("");
                    if (v == NULL)
                        return STATUS_NO_MEM;
                    p->v.sValue = v;
                    break;
                }
            }

            ++p->changes;

            return STATUS_OK;
        }

        void LSPStyle::sync()
        {
            // For each property: copy value from parent and notify children and listeners for changes
            property_t *vp = vProperties.get_array();
            for (size_t i=0, n=vProperties.size(); i < n; ++i)
            {
                property_t *p = &vp[i];
                if (!p->dfl) // Skip non-default properties
                    continue;

                // Lookup for a parent property
                property_t *parent = (pParent != NULL) ? pParent->get_property_recursive(p->id) : NULL;
                if (parent == NULL)
                    continue;

                // Try to deploy the value
                size_t change = p->changes;
                status_t res = copy_property(p, parent);
                if ((res == STATUS_OK) && (change != p->changes))
                {
                    notify_children(p);
                    notify_listeners(p);
                }
            }

            // Call all children for sync()
            for (size_t i=0, n=vChildren.size(); i<n; ++i)
            {
                LSPStyle *child = vChildren.at(i);
                if (child != NULL)
                    child->sync();
            }
        }

        void LSPStyle::notify_change(const property_t *prop)
        {
            // Find the matching property (if present) and update it's value
            for (size_t i=0, n=vProperties.size(); i < n; ++i)
            {
                property_t *p = vProperties.at(i);
                if ((p != NULL) || (p->id == prop->id))
                {
                    // Do not modify non-default properties
                    if (!p->dfl)
                        return;

                    // Try to deploy value
                    size_t change = p->changes;
                    copy_property(p, prop);
                    if (change != p->changes)
                    {
                        notify_children(p);
                        notify_listeners(p);
                    }
                }
            }
        }

        void LSPStyle::notify_children(const property_t *prop)
        {
            for (size_t i=0, n=vChildren.size(); i<n; ++i)
            {
                LSPStyle *child = vChildren.at(i);
                if (child != NULL)
                    child->notify_change(prop);
            }
        }

        void LSPStyle::notify_listeners(const property_t *prop)
        {
            ui_atom_t id = prop->id;
            for (size_t i=0, n=vListeners.size(); i<n; ++i)
            {
                listener_t *lst = vListeners.at(i);
                if ((lst != NULL) && (lst->nId == id))
                    lst->pListener->notify(id);
            }
        }

        status_t LSPStyle::add(LSPStyle *child)
        {
            if ((child == NULL) || (child == this))
                return STATUS_BAD_ARGUMENTS;
            if (child->pParent == this)
                return STATUS_ALREADY_EXISTS;

            if (!vChildren.add(child))
                return STATUS_NO_MEM;
            if (child->pParent != NULL)
                child->pParent->vChildren.remove(child, true);

            child->pParent  = this;
            sync();

            return STATUS_OK;
        }

        status_t LSPStyle::remove(LSPStyle *child)
        {
            if (child == NULL)
                return STATUS_BAD_ARGUMENTS;

            ssize_t idx = vChildren.index_of(child);
            if (idx < 0)
                return STATUS_NOT_FOUND;

            if (!vChildren.remove(idx, true))
                return STATUS_UNKNOWN_ERR;

            child->pParent = NULL;
            sync();

            return STATUS_OK;
        }

        status_t LSPStyle::set_parent(LSPStyle *parent)
        {
            if (parent == this)
                return STATUS_BAD_ARGUMENTS;
            else if (pParent == parent)
                return STATUS_OK;

            if (parent == NULL)
            {
                if (!pParent->vChildren.remove(this, true))
                    return STATUS_UNKNOWN_ERR;
            }
            else
            {
                if (!parent->vChildren.add(this))
                    return STATUS_NO_MEM;
                if (pParent != NULL)
                    pParent->vChildren.remove(this, true);
            }

            pParent     = parent;
            sync();

            return STATUS_OK;
        }

        LSPStyle *LSPStyle::root()
        {
            LSPStyle *curr = this;
            while (curr->pParent != NULL)
                curr = curr->pParent;
            return curr;
        }

        bool LSPStyle::is_bound(ui_atom_t id, IStyleListener *listener) const
        {
            const listener_t *pv = vListeners.get_array();
            for (size_t i=0, n=vListeners.size(); i<n; ++i)
            {
                const listener_t *p = &pv[i];
                if ((p->nId == id) && (p->pListener == listener))
                    return true;
            }
            return false;
        }

        status_t LSPStyle::bind(ui_atom_t id, ui_property_type_t type, IStyleListener *listener)
        {
            property_t *p = get_property(id);
            listener_t *lst = NULL;

            // Property has been found?
            if (p == NULL)
            {
                // Allocate new property
                p = vProperties.add();
                if (p == NULL)
                    return STATUS_NO_MEM;
                p->id       = id;
                p->refs     = 0;

                // Initialize property
                status_t res = init_property(p, id, type);
                if (res != STATUS_OK)
                {
                    vProperties.remove(p);
                    return res;
                }

                // Allocate listener binding
                lst = vListeners.add();
                if (listener == NULL)
                {
                    undef_property(p);
                    vProperties.remove(p);
                    return STATUS_NO_MEM;
                }
            }
            else
            {
                // Check that not already bound
                if (is_bound(id, listener))
                    return STATUS_ALREADY_BOUND;

                // Just allocate listener binding
                lst = vListeners.add();
                if (listener == NULL)
                    return STATUS_NO_MEM;
            }

            // Save listener to allocated binding
            lst->nId        = p->id;
            lst->pListener  = listener;
            ++p->refs;

            return STATUS_OK;
        }

        status_t LSPStyle::unbind(ui_atom_t id, IStyleListener *listener)
        {
            // Find listener binding
            listener_t *lst = NULL;
            listener_t *pv = vListeners.get_array();
            for (size_t i=0, n=vListeners.size(); i<n; ++i)
            {
                listener_t *p = &pv[i];
                if ((p->nId == id) && (p->pListener == listener))
                {
                    lst = p;
                    break;
                }
            }

            if (lst == NULL)
                return STATUS_NOT_BOUND;

            // Get property
            property_t *p = get_property(id);
            if (p == NULL)
                return STATUS_CORRUPTED; // This actually should not ever happen

            // Decrement number of references
            if ((--p->refs) <= 0)
            {
                undef_property(p);
                property_t *parent = (pParent != NULL) ? pParent->get_property(p->id) : NULL;
                notify_children((parent != NULL) ? parent : p);
                vProperties.remove(p);
            }

            // Remove listener binding
            vListeners.remove(lst);

            return STATUS_OK;
        }

        LSPStyle::property_t *LSPStyle::get_property(ui_atom_t id)
        {
            property_t *pv = vProperties.get_array();
            for (size_t i=0, n=vProperties.size(); i<n; ++i)
            {
                property_t *p = &pv[i];
                if ((p != NULL) && (p->id == id))
                    return p;
            }
            return NULL;
        }

        LSPStyle::property_t *LSPStyle::get_property_recursive(ui_atom_t id)
        {
            property_t *p = NULL;

            for (LSPStyle *curr = this; curr != NULL; curr = curr->pParent)
                if ((p = curr->get_property(id)) != NULL)
                    break;

            return p;
        }

        status_t LSPStyle::get_int(ui_atom_t id, ssize_t *dst) const
        {
            const property_t *prop = get_property_recursive(id);
            if (prop == NULL)
                return STATUS_NOT_FOUND;
            else if (prop->type != PT_INT)
                return STATUS_BAD_TYPE;
            *dst = prop->v.iValue;
            return STATUS_OK;
        }

        status_t LSPStyle::get_float(ui_atom_t id, float *dst) const
        {
            const property_t *prop = get_property_recursive(id);
            if (prop == NULL)
                return STATUS_NOT_FOUND;
            else if (prop->type != PT_FLOAT)
                return STATUS_BAD_TYPE;
            *dst = prop->v.fValue;
            return STATUS_OK;
        }

        status_t LSPStyle::get_bool(ui_atom_t id, bool *dst) const
        {
            const property_t *prop = get_property_recursive(id);
            if (prop == NULL)
                return STATUS_NOT_FOUND;
            else if (prop->type != PT_BOOL)
                return STATUS_BAD_TYPE;
            *dst = prop->v.bValue;
            return STATUS_OK;
        }

        status_t LSPStyle::get_string(ui_atom_t id, LSPString *dst) const
        {
            const property_t *prop = get_property_recursive(id);
            if (prop == NULL)
                return STATUS_NOT_FOUND;
            else if (prop->type != PT_STRING)
                return STATUS_BAD_TYPE;

            return (dst->set_utf8(prop->v.sValue)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t LSPStyle::get_string(ui_atom_t id, const char **dst) const
        {
            const property_t *prop = get_property_recursive(id);
            if (prop == NULL)
                return STATUS_NOT_FOUND;
            else if (prop->type != PT_STRING)
                return STATUS_BAD_TYPE;

            *dst = prop->v.sValue;
            return STATUS_OK;
        }

        bool LSPStyle::is_default(ui_atom_t id) const
        {
            const property_t *prop = get_property_recursive(id);
            return (prop != NULL) ? prop->dfl : false;
        }

        bool LSPStyle::exists(ui_atom_t id) const
        {
            const property_t *prop = get_property_recursive(id);
            return (prop != NULL);
        }

        ssize_t LSPStyle::get_type(ui_atom_t id) const
        {
            const property_t *prop = get_property_recursive(id);
            return (prop != NULL) ? prop->type : PT_UNKNOWN;
        }

        status_t LSPStyle::set_property(ui_atom_t id, property_t *src)
        {
            status_t res = STATUS_OK;
            property_t *p = get_property(id);
            if (p == NULL)
            {
                // Allocate new property
                p = vProperties.add();
                if (p == NULL)
                    return STATUS_NO_MEM;

                p->id       = id;
                p->refs     = 1;
                p->dfl      = false;

                res         = init_property(p, src);
                if (res != STATUS_OK)
                    vProperties.remove(p);
            }
            else
                res         = copy_property(p, src);

            // Notify listeners and children for changes
            if (res == STATUS_OK)
            {
                notify_listeners(p);
                notify_children(p);
            }

            return STATUS_OK;
        }

        status_t LSPStyle::set_int(ui_atom_t id, ssize_t value)
        {
            property_t tmp;
            tmp.type        = PT_INT;
            tmp.v.iValue    = value;
            return set_property(id, &tmp);
        }

        status_t LSPStyle::set_float(ui_atom_t id, float value)
        {
            property_t tmp;
            tmp.type        = PT_FLOAT;
            tmp.v.fValue    = value;
            return set_property(id, &tmp);
        }

        status_t LSPStyle::set_bool(ui_atom_t id, bool value)
        {
            property_t tmp;
            tmp.type        = PT_BOOL;
            tmp.v.bValue    = value;
            return set_property(id, &tmp);
        }

        status_t LSPStyle::set_string(ui_atom_t id, const LSPString *value)
        {
            if (value == NULL)
                return STATUS_BAD_ARGUMENTS;

            property_t tmp;
            tmp.type        = PT_STRING;
            tmp.v.sValue    = const_cast<char *>(value->get_utf8());
            return set_property(id, &tmp);
        }

        status_t LSPStyle::set_string(ui_atom_t id, const char *value)
        {
            if (value == NULL)
                return STATUS_BAD_ARGUMENTS;

            property_t tmp;
            tmp.type        = PT_STRING;
            tmp.v.sValue    = const_cast<char *>(value);
            return set_property(id, &tmp);
        }

        status_t LSPStyle::set_default(ui_atom_t id)
        {
            property_t *p = get_property(id);
            if (p == NULL)
                return STATUS_NOT_FOUND;
            else if (p->dfl)
                return STATUS_OK;

            // Initialize property with default value
            size_t change = p->changes;
            status_t res = init_property(p, p->id, p->type);
            if (res != STATUS_OK)
                return res;

            // Notify if changed
            if (change != p->changes)
            {
                notify_children(p);
                notify_listeners(p);
            }

            return STATUS_OK;
        }
    
    } /* namespace tk */
} /* namespace lsp */
