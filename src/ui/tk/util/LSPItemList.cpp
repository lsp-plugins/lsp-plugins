/*
 * LSPItemList.cpp
 *
 *  Created on: 31 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        //-----------------------------------------------------------------------------
        // LSPListItem implementation

        LSPItemList::LSPListItem::LSPListItem(LSPItemList *list, const char *text, float value): LSPItem(text, value)
        {
            pList       = list;
        }

        LSPItemList::LSPListItem::~LSPListItem()
        {
            pList       = NULL;
        }

        void LSPItemList::LSPListItem::on_change()
        {
            if (pList != NULL)
                pList->on_item_change(this);
        }

        //-----------------------------------------------------------------------------
        // LSPItemList implementation

        LSPItemList::LSPItemList()
        {
        }

        LSPItemList::~LSPItemList()
        {
            drop_data();
        }

        void LSPItemList::drop_data()
        {
            size_t n            = vItems.size();
            LSPListItem **ptr   = vItems.get_array();

            if (ptr != NULL)
            {
                for (size_t i=0; i<n; ++i)
                {
                    if (ptr != NULL)
                        delete *ptr;
                    ptr ++;
                }
            }

            vItems.flush();
        }

        LSPItemList::LSPListItem *LSPItemList::create_item(const char *text, float value)
        {
            return new LSPListItem(this, text, value);
        }

        void LSPItemList::on_item_change(LSPListItem *item)
        {
        }

        void LSPItemList::on_item_add(size_t index)
        {
        }

        void LSPItemList::on_item_remove(size_t index)
        {
        }

        void LSPItemList::on_item_swap(size_t idx1, size_t idx2)
        {
        }

        void LSPItemList::on_item_clear()
        {
        }

        void LSPItemList::clear()
        {
            drop_data();
            on_item_clear();
        }

        status_t LSPItemList::add(const char *text, float value)
        {
            LSPListItem *item   = create_item(text, value);
            if (item == NULL)
                return STATUS_NO_MEM;

            size_t index = vItems.size();
            if (!vItems.add(item))
            {
                delete item;
                return STATUS_NO_MEM;
            }

            on_item_add(index);

            return STATUS_OK;
        }

        status_t LSPItemList::add(const LSPString *text, float value)
        {
            LSPListItem *item   = create_item(text->get_native(), value);
            if (item == NULL)
                return STATUS_NO_MEM;

            size_t index = vItems.size();
            if (!vItems.add(item))
            {
                delete item;
                return STATUS_NO_MEM;
            }

            on_item_add(index);

            return STATUS_OK;
        }

        status_t LSPItemList::add(const LSPItem *item)
        {
            return (item == NULL) ? add(static_cast<char *>(NULL), 0.0f) : add(item->text(), item->value());
        }

        status_t LSPItemList::insert(ssize_t idx, const char *text, float value)
        {
            LSPListItem *item   = create_item(text, value);
            if (item == NULL)
                return STATUS_NO_MEM;

            if (!vItems.insert(item, idx))
            {
                delete item;
                return STATUS_NO_MEM;
            }

            on_item_add(idx);
            return STATUS_OK;
        }

        status_t LSPItemList::insert(ssize_t idx,const LSPItem *item)
        {
            return (item == NULL) ? insert(idx, NULL, 0.0f) : insert(idx, item->text(), item->value());
        }

        status_t LSPItemList::remove(ssize_t idx)
        {
            LSPListItem *item = vItems.get(idx);
            if (!vItems.remove(idx))
                return STATUS_BAD_ARGUMENTS;

            if (item != NULL)
                delete item;

            on_item_remove(idx);
            return STATUS_OK;
        }

        status_t LSPItemList::swap(ssize_t idx1, ssize_t idx2)
        {
            if (!vItems.swap(idx1, idx2))
                return STATUS_BAD_ARGUMENTS;

            if (idx1 != idx2)
                on_item_swap(idx1, idx2);
            return STATUS_OK;
        }

        const char *LSPItemList::text(ssize_t idx) const
        {
            cvector<LSPListItem> *st = const_cast<cvector<LSPListItem> *>(&vItems);
            LSPListItem *item = st->get(idx);
            return (item != NULL) ? item->text() : NULL;
        }

        float LSPItemList::value(ssize_t idx) const
        {
            cvector<LSPListItem> *st = const_cast<cvector<LSPListItem> *>(&vItems);
            LSPListItem *item = st->get(idx);
            return (item != NULL) ? item->value() : 0.0f;
        }

        LSPItem *LSPItemList::get(ssize_t idx)
        {
            return vItems.get(idx);
        }

        status_t LSPItemList::get(ssize_t idx, const char **text, float *value) const
        {
            cvector<LSPListItem> *st = const_cast<cvector<LSPListItem> *>(&vItems);
            LSPListItem *item = st->get(idx);
            if (item == NULL)
                return STATUS_BAD_ARGUMENTS;

            if (text != NULL)
                *text = item->text();
            if (value != NULL)
                *value = item->value();

            return STATUS_OK;
        }

        status_t LSPItemList::set_text(ssize_t idx, const char *text)
        {
            LSPListItem *item = vItems.get(idx);
            if (item == NULL)
                return STATUS_BAD_ARGUMENTS;

            return item->set_text(text);
        }

        status_t LSPItemList::set_value(ssize_t idx, float value)
        {
            LSPListItem *item = vItems.get(idx);
            if (item == NULL)
                return STATUS_BAD_ARGUMENTS;

            item->set_value(value);
            return STATUS_OK;
        }

        status_t LSPItemList::set(ssize_t idx, const LSPItem *item)
        {
            LSPListItem *dst    = vItems.get(idx);
            if (dst == NULL)
                return STATUS_BAD_ARGUMENTS;

            return dst->set(item);
        }

        status_t LSPItemList::set(ssize_t idx, const char *text, float value)
        {
            LSPListItem *dst    = vItems.get(idx);
            if (dst == NULL)
                return STATUS_BAD_ARGUMENTS;

            return dst->set(text, value);
        }

        ssize_t LSPItemList::index_of(const LSPItem *item) const
        {
            if (item == NULL)
                return STATUS_BAD_ARGUMENTS;

            cvector<LSPListItem> *st = const_cast<cvector<LSPListItem> *>(&vItems);
            size_t n = st->size();
            LSPListItem **arr = st->get_array();

            for (size_t i=0; i<n; ++i, ++arr)
            {
                if (*arr == item)
                    return i;
            }

            return -1;
        }

    } /* namespace tk */
} /* namespace lsp */
