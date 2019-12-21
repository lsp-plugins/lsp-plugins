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
            nLock       = 0;
            bDelayed    = false;
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
            // Unlock all pending transactions
            nLock   = 0;
            delayed_notify();

            // Unlink from parents and remove all children
            for (size_t i=0, n=vParents.size(); i<n; ++i)
            {
                LSPStyle *parent = vParents.at(i);
                if (parent != NULL)
                    parent->vChildren.remove(this);
            }

            // Unlink from children and remove all children
            for (size_t i=0, n=vChildren.size(); i<n; ++i)
            {
                LSPStyle *child = vChildren.at(i);
                if (child != NULL)
                {
                    child->vParents.remove(this);
                    child->sync();
                }
            }
            vChildren.flush();

            // Synchronize state with listeners and remove them all
            sync();
            vListeners.flush();

            // Destroy stored properties
            for (size_t i=0, n=vProperties.size(); i<n; ++i)
                undef_property(vProperties.at(i));
            vProperties.flush();
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

        LSPStyle::property_t *LSPStyle::create_property(ui_atom_t id, const property_t *src)
        {
            // Allocate property
            property_t *dst = vProperties.add();
            if (dst == NULL)
                return NULL;

            // Init contents
            switch (src->type)
            {
                case PT_INT:    dst->v.iValue   = src->v.iValue;    break;
                case PT_FLOAT:  dst->v.fValue   = src->v.fValue;    break;
                case PT_BOOL:   dst->v.bValue   = src->v.bValue;    break;
                case PT_STRING:
                {
                    // Update value
                    if ((dst->v.sValue = ::strdup(src->v.sValue)) == NULL)
                    {
                        vProperties.remove(dst);
                        return NULL;
                    }
                    break;
                }
                default:
                    return NULL;
            }

            dst->id         = id;
            dst->refs       = 0;
            dst->type       = src->type;
            dst->changes    = 0;
            dst->flags      = F_DEFAULT;
            dst->owner      = this;

            return dst;
        }

        LSPStyle::property_t *LSPStyle::create_property(ui_atom_t id, ui_property_type_t type)
        {
            // Allocate property
            property_t *dst = vProperties.add();
            if (dst == NULL)
                return NULL;

            // Init contents
            switch (type)
            {
                case PT_INT:    dst->v.iValue = 0;      break;
                case PT_FLOAT:  dst->v.fValue = 0.0;    break;
                case PT_BOOL:   dst->v.bValue = 0;      break;
                case PT_STRING:
                    if ((dst->v.sValue = ::strdup("")) == NULL)
                    {
                        vProperties.remove(dst);
                        return NULL;
                    }
                    break;
                default:
                    return NULL;
            }

            dst->id         = id;
            dst->refs       = 0;
            dst->type       = type;
            dst->changes    = 0;
            dst->flags      = F_DEFAULT;
            dst->owner      = this;

            return dst;
        }

        status_t LSPStyle::sync_property(property_t *p)
        {
            if (!(p->flags & F_DEFAULT))
                return STATUS_OK;

            property_t *parent  = get_parent_property(p->id);
            size_t changes      = p->changes;
            status_t res        = (parent != NULL) ? copy_property(p, parent) : set_property_default(p);
            if ((res == STATUS_OK) && (changes != p->changes))
            {
                notify_listeners(p);
                notify_children(p);
            }
            return res;
        }

        status_t LSPStyle::set_property_default(property_t *p)
        {
            switch (p->type)
            {
                case PT_INT:
                    if (p->v.iValue == 0)
                        return STATUS_OK;
                    p->v.iValue = 0;
                    break;
                case PT_FLOAT:
                    if (p->v.fValue == 0)
                        return STATUS_OK;
                    p->v.fValue = 0;
                    break;
                case PT_BOOL:
                    if (p->v.bValue == false)
                        return STATUS_OK;
                    p->v.bValue = false;
                    break;
                case PT_STRING:
                {
                    char *tmp = ::strdup("");
                    if (tmp == NULL)
                        return STATUS_NO_MEM;
                    ::free(p->v.sValue);
                    p->v.sValue = tmp;
                    break;
                }
                default:
                    return STATUS_BAD_TYPE;
            }

            p->flags   |= F_DEFAULT;
            ++p->changes;
            return STATUS_OK;
        }

        void LSPStyle::sync()
        {
            // For each property: copy value from parent and notify children and listeners for changes
            property_t *vp = vProperties.get_array();
            for (size_t i=0, n=vProperties.size(); i < n; ++i)
                sync_property(&vp[i]);

            // Call all children for sync()
            for (size_t i=0, n=vChildren.size(); i<n; ++i)
            {
                LSPStyle *child = vChildren.at(i);
                if (child != NULL)
                    child->sync();
            }
        }

        void LSPStyle::delayed_notify()
        {
            size_t notified;

            if (bDelayed)
                return;

            bDelayed = true; // Disallow delayed notify because it is already active
            do
            {
                notified = 0;
                for (size_t i=0, n=vProperties.size(); i < n; ++i)
                {
                    property_t *prop = vProperties.at(i);

                    // Notify if notification is pending
                    if (prop->flags & F_NTF_LISTENERS)
                    {
                        prop->flags &= ~F_NTF_LISTENERS;
                        notify_listeners(prop);
                        ++notified;
                    }
                    if (prop->flags & F_NTF_CHILDREN)
                    {
                        prop->flags &= ~F_NTF_CHILDREN;
                        notify_children(prop);
                        ++notified;
                    }
                }
            } while (notified > 0);
            bDelayed = false;
        }

        void LSPStyle::notify_change(property_t *prop)
        {
            // Find the matching property (if present)
            property_t *p = get_property(prop->id);

            // Property not found?
            if (p == NULL)
            {
                notify_children(prop); // Just bypass event to children
                return;
            }
            else if (!(p->flags & F_DEFAULT)) // Not default property? Ignore the event
                return;

            // Get parent Property
            property_t *parent = get_parent_property(prop->id);
            if (parent != NULL)
            {
                // Parent property has been changed?
                size_t change = p->changes;
                status_t res = copy_property(p, parent);
                if ((res == STATUS_OK) && (change == p->changes))
                    return;
            }
            else
            {
                // Copy property value and notify listener and children only if property has changed
                size_t change = p->changes;
                status_t res = copy_property(p, prop);
                if ((res == STATUS_OK) && (change == p->changes))
                    return;
            }

            // Notify children and listeners about property change
            notify_listeners(p);
            notify_children(p);
        }

        void LSPStyle::notify_children(property_t *prop)
        {
            // In transaction, just set notification flag instead of issuing notification procedure
            if ((nLock > 0) && (prop->owner == this))
            {
                prop->flags    |= F_NTF_CHILDREN;
                return;
            }

            // Notify all children about property change
            for (size_t i=0, n=vChildren.size(); i<n; ++i)
            {
                LSPStyle *child = vChildren.at(i);
                if (child != NULL)
                    child->notify_change(prop);
            }
        }

        void LSPStyle::notify_listeners(property_t *prop)
        {
            // In transaction, just set notification flag instead of issuing notification procedure
            if ((nLock > 0) && (prop->owner == this))
            {
                prop->flags    |= F_NTF_LISTENERS;
                return;
            }

            // Notify all listeners about property change
            ui_atom_t id = prop->id;
            for (size_t i=0, n=vListeners.size(); i<n; ++i)
            {
                listener_t *lst = vListeners.at(i);
                if ((lst != NULL) && (lst->nId == id))
                    lst->pListener->notify(id);
            }
        }

        status_t LSPStyle::add_child(LSPStyle *child, ssize_t idx)
        {
            // Check arguments
            if (child == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (idx < 0)
                idx = vChildren.size();
            else if (size_t(idx) > vChildren.size())
                return STATUS_INVALID_VALUE;

            // Check
            if (vChildren.index_of(child) >= 0)
                return STATUS_ALREADY_EXISTS;
            if ((child == this) || (child->has_child(this, true)))
                return STATUS_BAD_HIERARCHY;

            // Make bindings
            if (!vChildren.insert(child, idx))
                return STATUS_NO_MEM;
            if (!child->vParents.add(this))
            {
                vChildren.remove(child);
                return STATUS_NO_MEM;
            }

            // Synchronize state
            child->sync();

            return STATUS_OK;
        }

        status_t LSPStyle::add_parent(LSPStyle *parent, ssize_t idx)
        {
            // Check arguments
            if (parent == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (idx < 0)
                idx = vParents.size();
            else if (size_t(idx) > vParents.size())
                return STATUS_INVALID_VALUE;

            // Check
            if (vParents.index_of(parent) >= 0)
                return STATUS_ALREADY_EXISTS;
            if ((parent == this) || (this->has_child(parent, true)))
                return STATUS_BAD_HIERARCHY;

            // Make bindings
            if (!vParents.insert(parent, idx))
                return STATUS_NO_MEM;
            if (!parent->vChildren.add(this))
            {
                vParents.remove(parent);
                return STATUS_NO_MEM;
            }

            // Synchronize state
            sync();

            return STATUS_OK;
        }

        status_t LSPStyle::remove_child(LSPStyle *child)
        {
            if (child == NULL)
                return STATUS_BAD_ARGUMENTS;

            if (!vChildren.remove(child, true))
                return STATUS_NOT_FOUND;

            child->vParents.remove(this);
            child->sync();

            return STATUS_OK;
        }

        status_t LSPStyle::remove_parent(LSPStyle *parent)
        {
            if (parent == NULL)
                return STATUS_BAD_ARGUMENTS;

            if (!vParents.remove(parent))
                return STATUS_NOT_FOUND;

            parent->vChildren.remove(this);
            sync();

            return STATUS_OK;
        }

        bool LSPStyle::has_child(LSPStyle *child, bool recursive)
        {
            if ((child == NULL) || (child == this))
                return false;

            // First, lookup self children
            if (vChildren.index_of(child) >= 0)
                return true;
            else if (!recursive)
                return false;

            // Second, lookup recursive
            for (size_t i=0, n=vChildren.size(); i<n; ++i)
            {
                LSPStyle *s = vChildren.at(i);
                if ((s != NULL) && (s->has_child(child, recursive)))
                    return true;
            }

            return false;
        }

        bool LSPStyle::has_parent(LSPStyle *parent, bool recursive)
        {
            if ((parent == NULL) || (parent == this))
                return false;

            // First, lookup self children
            if (vParents.index_of(parent) >= 0)
                return true;
            else if (!recursive)
                return false;

            // Second, lookup recursively parents
            for (size_t i=0, n=vParents.size(); i<n; ++i)
            {
                LSPStyle *s = vParents.at(i);
                if ((s != NULL) && (s->has_parent(parent, recursive)))
                    return true;
            }

            return false;
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
                // Lookup parent property
                property_t *parent = get_parent_property(id);

                // Create property
                p = (parent != NULL) ? create_property(id, parent) : create_property(id, type);
                if (p == NULL)
                    return STATUS_NO_MEM;

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

            notify_listeners(p);
            notify_children(p);

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
                property_t *parent = get_parent_property(p->id);
                notify_children((parent != NULL) ? parent : p);
                vProperties.remove(p);
            }

            // Remove listener binding
            vListeners.remove(lst);

            return STATUS_OK;
        }

        LSPStyle::property_t *LSPStyle::get_property(ui_atom_t id)
        {
            for (size_t i=0, n=vProperties.size(); i<n; ++i)
            {
                property_t *p   = vProperties.at(i);
                if ((p != NULL) && (p->id == id))
                    return p;
            }
            return NULL;
        }

        LSPStyle::property_t *LSPStyle::get_parent_property(ui_atom_t id)
        {
            // Lookup parents in reverse order
            for (ssize_t i=vParents.size() - 1; i >= 0; --i)
            {
                LSPStyle *curr = vParents.at(i);
                if (curr == NULL)
                    continue;

                // Try to fetch property first
                property_t *p = curr->get_property(id);
                if (p == NULL) // Property not found?
                    p = curr->get_parent_property(id); // Search parents recursively
                if (p != NULL)
                    return p;
            }

            return NULL;
        }

        LSPStyle::property_t *LSPStyle::get_property_recursive(ui_atom_t id)
        {
            property_t *p = get_property(id);
            return (p != NULL) ? p : get_parent_property(id);
        }

        void LSPStyle::begin()
        {
            ++nLock;
        }

        void LSPStyle::end()
        {
            if (nLock == 0)
                return;
            if (!(--nLock)) // last end() ?
                delayed_notify();
        }

        status_t LSPStyle::get_int(ui_atom_t id, ssize_t *dst) const
        {
            const property_t *prop = get_property_recursive(id);
            if (prop == NULL)
            {
                *dst = 0;
                return STATUS_OK;
            }
            else if (prop->type != PT_INT)
                return STATUS_BAD_TYPE;
            if (dst != NULL)
                *dst = prop->v.iValue;
            return STATUS_OK;
        }

        status_t LSPStyle::get_float(ui_atom_t id, float *dst) const
        {
            const property_t *prop = get_property_recursive(id);
            if (prop == NULL)
            {
                *dst = 0.0f;
                return STATUS_OK;
            }
            else if (prop->type != PT_FLOAT)
                return STATUS_BAD_TYPE;
            if (dst != NULL)
                *dst = prop->v.fValue;
            return STATUS_OK;
        }

        status_t LSPStyle::get_bool(ui_atom_t id, bool *dst) const
        {
            const property_t *prop = get_property_recursive(id);
            if (prop == NULL)
            {
                *dst = false;
                return STATUS_OK;
            }
            else if (prop->type != PT_BOOL)
                return STATUS_BAD_TYPE;
            if (dst != NULL)
                *dst = prop->v.bValue;
            return STATUS_OK;
        }

        status_t LSPStyle::get_string(ui_atom_t id, LSPString *dst) const
        {
            const property_t *prop = get_property_recursive(id);
            if (prop == NULL)
            {
                if (dst != NULL)
                    dst->truncate();
                return STATUS_OK;
            }
            else if (prop->type != PT_STRING)
                return STATUS_BAD_TYPE;

            if (dst == NULL)
                return STATUS_OK;
            return (dst->set_utf8(prop->v.sValue)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t LSPStyle::get_string(ui_atom_t id, const char **dst) const
        {
            const property_t *prop = get_property_recursive(id);
            if (prop == NULL)
            {
                if (dst != NULL)
                    *dst = "";
                return STATUS_OK;
            }
            else if (prop->type != PT_STRING)
                return STATUS_BAD_TYPE;

            if (dst != NULL)
                *dst = prop->v.sValue;
            return STATUS_OK;
        }

        bool LSPStyle::is_default(ui_atom_t id) const
        {
            const property_t *prop = get_property_recursive(id);
            return (prop != NULL) ? (prop->flags & F_DEFAULT) : false;
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
                p = create_property(id, src);
                if (p == NULL)
                    return STATUS_NO_MEM;
                p->flags   &= ~F_DEFAULT;
                notify_listeners(p);
                notify_children(p);
            }
            else
            {
                // Notify only if value has changed
                size_t change = p->changes;
                res         = copy_property(p, src);

                if (res == STATUS_OK)
                {
                    p->flags   &= ~F_DEFAULT;
                    if (change != p->changes)
                    {
                        notify_listeners(p);
                        notify_children(p);
                    }
                }
            }

            return res;
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
            else if (p->flags & F_DEFAULT)
                return STATUS_OK;

            // Initialize property with default value
            p->flags   |= F_DEFAULT;
            return sync_property(p);
        }
    
    } /* namespace tk */
} /* namespace lsp */
