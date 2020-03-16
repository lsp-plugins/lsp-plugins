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

        LSPItemList::LSPListItem::LSPListItem(LSPItemList *list):
            LSPItem()
        {
            pList       = list;
        }

        LSPItemList::LSPListItem::LSPListItem(LSPItemList *list, const LSPItem *item):
            LSPItem(item)
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

        LSPItemList::LSPListItem *LSPItemList::create_item()
        {
            return new LSPListItem(this);
        }

        LSPItemList::LSPListItem *LSPItemList::create_item(const LSPItem *item)
        {
            return new LSPListItem(this, item);
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

        status_t LSPItemList::add(const LSPItem *src)
        {
            LSPListItem *item   = create_item(src);
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

        status_t LSPItemList::add(LSPItem **dst)
        {
            LSPListItem *item   = create_item();
            if (item == NULL)
                return STATUS_NO_MEM;

            size_t index = vItems.size();
            if (!vItems.add(item))
            {
                delete item;
                return STATUS_NO_MEM;
            }

            on_item_add(index);
            if (dst != NULL)
                *dst = item;

            return STATUS_OK;
        }

        status_t LSPItemList::insert(ssize_t idx, const LSPItem *src)
        {
            if (idx > ssize_t(vItems.size()))
                return STATUS_INVALID_VALUE;

            LSPListItem *item   = create_item(src);
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

        status_t LSPItemList::insert(ssize_t idx, LSPItem **dst)
        {
            if (idx > ssize_t(vItems.size()))
                return STATUS_INVALID_VALUE;

            LSPListItem *item   = create_item();
            if (item == NULL)
                return STATUS_NO_MEM;

            if (!vItems.insert(item, idx))
            {
                delete item;
                return STATUS_NO_MEM;
            }

            on_item_add(idx);
            if (dst != NULL)
                *dst = item;
            return STATUS_OK;
        }

        status_t LSPItemList::remove(ssize_t idx, LSPItem *dst)
        {
            LSPListItem *item = vItems.get(idx);
            if (item == NULL)
                return STATUS_INVALID_VALUE;
            else if (!vItems.remove(idx))
                return STATUS_BAD_ARGUMENTS;

            on_item_remove(idx);

            status_t res = (dst != NULL) ? dst->set(item) : STATUS_OK;
            delete item;

            return res;
        }

        status_t LSPItemList::truncate(size_t size)
        {
            for (size_t n = vItems.size(); n > size; --n)
            {
                LSPListItem *item = vItems.get(n);
                if (!vItems.remove(n))
                    return STATUS_BAD_STATE;

                if (item != NULL)
                    delete item;

                on_item_remove(n);
            }

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

        LSPItem *LSPItemList::get(ssize_t idx)
        {
            return vItems.get(idx);
        }

        status_t LSPItemList::set(ssize_t idx, const LSPItem *item)
        {
            LSPListItem *dst    = vItems.get(idx);
            if (dst == NULL)
                return STATUS_BAD_ARGUMENTS;

            return dst->set(item);
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
