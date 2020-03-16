/*
 * LSPFileFilter.h
 *
 *  Created on: 6 мая 2018 г.
 *      Author: sadko
 */

#ifndef UI_TK_UTIL_LSPFILEFILTER_H_
#define UI_TK_UTIL_LSPFILEFILTER_H_

namespace lsp
{
    namespace tk
    {
        class LSPFileFilter
        {
            private:
                LSPFileFilter & operator = (const LSPFileFilter &);

            protected:
                class FilterItem: public LSPFileFilterItem
                {
                    private:
                        friend class LSPFileFilter;

                    protected:
                        LSPFileFilter *pFilter;

                    protected:
                        virtual void        sync();

                        inline void bind(LSPFileFilter *filter)     { pFilter = filter; }

                    public:
                        explicit inline FilterItem()                { pFilter = NULL; }
                };

            protected:
                ssize_t                 nDefault;
                cvector<FilterItem>     vItems;

            protected:
                virtual status_t item_updated(size_t idx, LSPFileFilterItem *flt);

                virtual status_t item_removed(size_t idx, LSPFileFilterItem *flt);

                virtual status_t item_added(size_t idx, LSPFileFilterItem *flt);

                virtual void default_updated(ssize_t idx);

            public:
                explicit LSPFileFilter();
                virtual ~LSPFileFilter();

            public:
                inline size_t   size() const        { return vItems.size(); }
                inline ssize_t  get_default() const { return nDefault; }
                status_t        clear();

                ssize_t         add(const LSPFileFilterItem *item);
                status_t        insert(size_t index, const LSPFileFilterItem *item);
                status_t        remove(size_t index, LSPFileFilterItem *res);

                LSPFileFilterItem *get(size_t index);
                const LSPFileFilterItem *get(size_t index) const;

                status_t        set(size_t index, const LSPFileFilterItem *item);

                status_t        set_default(size_t value);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPFILEFILTER_H_ */
