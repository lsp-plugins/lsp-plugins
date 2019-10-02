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
        ILSPStyleListener::~ILSPStyleListener()
        {
        }

        void ILSPStyleListener::notify(ui_atom_t property)
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
            if (pParent != NULL)
            {
                pParent->vChildren.remove(this, true);
                pParent     = NULL;
            }
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

            property->type = -1;
        }

        void LSPStyle::destroy_property(property_t *property)
        {
            if (property == NULL)
                return;
            undef_property(property);
            ::free(property);
        }

        void LSPStyle::notify_all()
        {
            // Notify all child styles and widget
            for (size_t i=0, n=vChildren.size(); i<n; ++i)
            {
                LSPStyle *child = vChildren.at(i);
                if (child != NULL)
                    child->notify_all();
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
            child->notify_all();

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
            child->notify_all();

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
            notify_all();

            return STATUS_OK;
        }

        LSPStyle *LSPStyle::root()
        {
            LSPStyle *curr = this;
            while (curr->pParent != NULL)
                curr = curr->pParent;
            return curr;
        }

        status_t LSPStyle::bind(ui_atom_t id, ui_property_type_t type, ILSPStyleListener *listener)
        {
        }

        status_t LSPStyle::unbind(ui_atom_t id, ILSPStyleListener *listener)
        {
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

        status_t LSPStyle::set_property(ui_atom_t id, property_t *src)
        {
            property_t *p = get_property(id);
            if (p == NULL)
            {
                // Allocate new property
                p = vProperties.add();
                if (p == NULL)
                    return STATUS_NO_MEM;

                p->id       = id;
                p->type     = src->type;
                p->refs     = 1;
                p->dfl      = false;
                p->v        = src->v;
            }
            else
            {
                // Modify property
                undef_property(p);
                if (p->dfl)
                    ++p->refs;
                p->dfl      = false;
                p->type     = src->type;
                p->v        = src->v;
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
            tmp.type        = PT_BOOL;
            tmp.v.sValue    = value->clone_utf8();
            if (tmp.v.sValue == NULL)
                return STATUS_NO_MEM;

            status_t res = set_property(id, &tmp);
            if (res != STATUS_OK)
                ::free(tmp.v.sValue);

            return res;
        }

        status_t LSPStyle::set_default(ui_atom_t id)
        {
            property_t *p = get_property(id);
            if (p == NULL)
                return STATUS_NOT_FOUND;
            else if (p->dfl)
                return STATUS_OK;

            p->dfl = true;
            if ((--p->refs) <= 0)
            {
                undef_property(p);
                vProperties.remove(p);
                // TODO: notify chilren
            }

            return STATUS_OK;
        }
    
    } /* namespace tk */
} /* namespace lsp */
