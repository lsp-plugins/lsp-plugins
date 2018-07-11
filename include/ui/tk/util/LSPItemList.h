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
                        LSPListItem(LSPItemList *list, const char *text, float value);
                        virtual ~LSPListItem();

                    protected:
                        virtual void        on_change();
                };

            protected:
                cvector<LSPListItem>    vItems;

            protected:
                virtual LSPListItem    *create_item(const char *text, float value = 0.0f);

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

                status_t        add(const char *text = NULL, float value = 0.0f);
                status_t        add(const LSPString *text = NULL, float value = 0.0f);
                status_t        add(const LSPItem *item);

                status_t        insert(ssize_t idx, const char *text = NULL, float value = 0.0f);
                status_t        insert(ssize_t idx, const LSPItem *item);

                status_t        remove(ssize_t idx);

                status_t        swap(ssize_t idx1, ssize_t idx2);

                inline bool     exists(ssize_t idx) { return (idx >= 0) && (idx < ssize_t(vItems.size())); }

                const char     *text(ssize_t idx) const;
                float           value(ssize_t idx) const;
                LSPItem        *get(ssize_t idx);
                status_t        get(ssize_t idx, const char **text, float *value) const;

                status_t        set_text(ssize_t idx, const char *text);
                status_t        set_value(ssize_t idx, float value);
                status_t        set_item(ssize_t idx, const LSPItem *item);
                status_t        set(ssize_t idx, const LSPItem *item);
                status_t        set(ssize_t idx, const char *text, float value);

                ssize_t         index_of(const LSPItem *item) const;
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPITEMLIST_H_ */
