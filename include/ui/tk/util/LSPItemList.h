/*
 * LSPItemList.h
 *
 *  Created on: 31 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPITEMLIST_H_
#define UI_TK_LSPITEMLIST_H_

namespace lsp
{
    namespace tk
    {
        
        class LSPItemList
        {
            protected:
                class LSPListItem : public LSPItem
                {
                    protected:
                        LSPItemList    *pList;

                    public:
                        explicit LSPListItem(LSPItemList *list);
                        explicit LSPListItem(LSPItemList *list, const LSPItem *item);
                        virtual ~LSPListItem();

                    protected:
                        virtual void        on_change();
                };

            protected:
                cvector<LSPListItem>    vItems;

            protected:
                virtual LSPListItem    *create_item();

                virtual LSPListItem    *create_item(const LSPItem *item);

                virtual void            on_item_change(LSPListItem *item); // Triggered when the content of item has been changed

                virtual void            on_item_add(size_t index);

                virtual void            on_item_remove(size_t index);

                virtual void            on_item_swap(size_t idx1, size_t idx2);

                virtual void            on_item_clear();

            protected:
                void            drop_data();

            public:
                explicit LSPItemList();
                virtual ~LSPItemList();

            public:
                void            clear();
                inline size_t   size()      { return vItems.size(); }

                inline bool     exists(ssize_t idx) { return (idx >= 0) && (idx < ssize_t(vItems.size())); }

                status_t        add(const LSPItem *item);
                status_t        add(LSPItem **item = NULL);
                status_t        insert(ssize_t idx, const LSPItem *item);
                status_t        insert(ssize_t idx, LSPItem **item = NULL);
                LSPItem        *get(ssize_t idx);
                status_t        set(ssize_t idx, const LSPItem *item);
                ssize_t         index_of(const LSPItem *item) const;
                status_t        remove(ssize_t idx, LSPItem *item = NULL);
                status_t        truncate(size_t size);

                status_t        swap(ssize_t idx1, ssize_t idx2);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPITEMLIST_H_ */
