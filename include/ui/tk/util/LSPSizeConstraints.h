/*
 * LSPSizeConstraints.h
 *
 *  Created on: 8 окт. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_UTIL_LSPSIZECONSTRAINTS_H_
#define UI_TK_UTIL_LSPSIZECONSTRAINTS_H_

namespace lsp
{
    namespace tk
    {
        class LSPWidget;

        class LSPSizeConstraints
        {
            protected:
                size_request_t      sSize;
                LSPWidget          *pWidget;

            public:
                LSPSizeConstraints(LSPWidget *w);
                virtual ~LSPSizeConstraints();

            public:
                inline ssize_t      min_width() const   { return sSize.nMinWidth; }
                inline ssize_t      min_height() const  { return sSize.nMinHeight; }
                ssize_t      max_width() const;
                ssize_t      max_height() const;

                /** Get size constraints. It is guaranteed that in case when
                 * both minimum and maximum range values are defined, maximum value
                 * will be always not less than minimum value. Minimum value has
                 * priority over the maximum value, so if maximum value is less than
                 * minimum, it will be returned being equal to a minimum value
                 *
                 * @param dst destination parameter to return value
                 */
                void        get(size_request_t *dst) const;

            public:
                void        set_min_width(ssize_t value);
                void        set_min_height(ssize_t value);
                void        set_max_width(ssize_t value);
                void        set_max_height(ssize_t value);

                void        set_width(ssize_t min, ssize_t max);
                void        set_height(ssize_t min, ssize_t max);
                void        set_min(ssize_t min_width, ssize_t min_height);
                void        set_max(ssize_t max_width, ssize_t max_height);

                void        set(ssize_t min_width, ssize_t min_height, ssize_t max_width, ssize_t max_height);
                void        set(const size_request_t *sr);

                void        apply(size_request_t *sr) const;
        };

    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPSIZECONSTRAINTS_H_ */
