/*
 * LSPSelection.h
 *
 *  Created on: 4 сент. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_UTIL_LSPTEXTSELECTION_H_
#define UI_TK_UTIL_LSPTEXTSELECTION_H_

namespace lsp
{
    namespace tk
    {
        class LSPTextSelection
        {
            private:
                ssize_t     nFirst;
                ssize_t     nLast;

            protected:
                virtual ssize_t limit(ssize_t value);
                virtual void on_change();

            public:
                explicit LSPTextSelection();
                virtual ~LSPTextSelection();

            public:
                inline ssize_t first() const { return nFirst; }
                inline ssize_t last() const { return nLast; }
                inline bool valid() const { return (nFirst >= 0) && (nLast >= 0); }
                inline bool invalid() const { return (nFirst < 0) || (nLast < 0); }
                inline bool is_empty() const { return nFirst == nLast; }
                inline bool non_empty() const { return nFirst != nLast; }
                inline ssize_t length() const { return (nFirst < nLast) ? nLast - nFirst : nFirst - nLast; }
                inline bool reverted() const { return nFirst > nLast; }
                inline ssize_t starting() const { return (nFirst < nLast) ? nFirst : nLast; }
                inline ssize_t ending() const { return (nLast < nFirst) ? nFirst : nLast; }

            public:
                void set(ssize_t first, ssize_t last);
                void set(ssize_t first);
                void set_first(ssize_t value);
                void set_last(ssize_t value);
                void set_all();
                void truncate();
                void unset();
                inline void clear() { unset(); };

            public:
                bool contains(ssize_t value) const;
                bool intersection(ssize_t first, ssize_t last) const;
                bool intersection(const LSPTextSelection *src) const;

                /** Read range of selection, first index is always not greater than last
                 *
                 * @param first index of first occurence
                 * @param last index of last occurence
                 */
                void read_range(ssize_t *first, ssize_t *last) const;
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPTEXTSELECTION_H_ */
