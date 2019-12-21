/*
 * LSPSizeConstraints.cpp
 *
 *  Created on: 8 окт. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        LSPSizeConstraints::LSPSizeConstraints(LSPWidget *w)
        {
            sSize.nMinWidth     = -1;
            sSize.nMinHeight    = -1;
            sSize.nMaxWidth     = -1;
            sSize.nMaxHeight    = -1;
            pWidget             = w;
        }

        LSPSizeConstraints::~LSPSizeConstraints()
        {
        }

        ssize_t LSPSizeConstraints::max_width() const
        {
            return ((sSize.nMaxWidth >= 0) && (sSize.nMinWidth > sSize.nMaxWidth)) ?
                    sSize.nMinWidth : sSize.nMaxWidth;
        }

        ssize_t LSPSizeConstraints::max_height() const
        {
            return ((sSize.nMaxHeight >= 0) && (sSize.nMinHeight > sSize.nMaxHeight)) ?
                    sSize.nMinHeight : sSize.nMaxHeight;
        }

        void LSPSizeConstraints::get(size_request_t *dst) const
        {
            dst->nMinWidth  = sSize.nMinWidth;
            dst->nMinHeight = sSize.nMinHeight;
            dst->nMaxWidth  = max_width();
            dst->nMaxHeight = max_height();
        }

        void LSPSizeConstraints::set_min_width(ssize_t value)
        {
            if (sSize.nMinWidth == value)
                return;

            sSize.nMinWidth     = value;
            if (value < 0)
                return;

            ssize_t size        = pWidget->width();
            if (size < value)
                pWidget->query_resize();
        }

        void LSPSizeConstraints::set_min_height(ssize_t value)
        {
            if (sSize.nMinHeight == value)
                return;

            sSize.nMinHeight    = value;
            if (value < 0)
                return;

            ssize_t size        = pWidget->height();
            if (size < value)
                pWidget->query_resize();
        }

        void LSPSizeConstraints::set_max_width(ssize_t value)
        {
            if (sSize.nMaxWidth == value)
                return;

            sSize.nMaxWidth     = value;
            if (value < 0)
                return;

            ssize_t size        = pWidget->width();
            if (size > value)
                pWidget->query_resize();
        }

        void LSPSizeConstraints::set_max_height(ssize_t value)
        {
            if (sSize.nMaxHeight == value)
                return;

            sSize.nMaxHeight    = value;
            if (value < 0)
                return;

            ssize_t size        = pWidget->height();
            if (size > value)
                pWidget->query_resize();
        }

        void LSPSizeConstraints::set_width(ssize_t min, ssize_t max)
        {
            if ((sSize.nMinWidth == min) && (sSize.nMaxWidth == max))
                return;

            sSize.nMinWidth     = min;
            sSize.nMaxWidth     = max;
            ssize_t size        = pWidget->width();

            if (((min >= 0) && (size < min)) ||
                ((max >= 0) && (size > max)))
                pWidget->query_resize();
        }

        void LSPSizeConstraints::set_height(ssize_t min, ssize_t max)
        {
            if ((sSize.nMinHeight == min) && (sSize.nMaxHeight == max))
                return;

            sSize.nMinHeight    = min;
            sSize.nMaxHeight    = max;
            ssize_t size        = pWidget->height();

            if (((min >= 0) && (size < min)) ||
                ((max >= 0) && (size > max)))
                pWidget->query_resize();
        }

        void LSPSizeConstraints::set(ssize_t min_width, ssize_t min_height, ssize_t max_width, ssize_t max_height)
        {
            if ((sSize.nMinWidth == min_width) && (sSize.nMaxHeight == max_width) &&
                (sSize.nMinHeight == min_height) && (sSize.nMaxHeight == max_width))
                return;

            sSize.nMinWidth     = min_width;
            sSize.nMinHeight    = min_height;
            sSize.nMaxWidth     = max_width;
            sSize.nMaxHeight    = max_height;

            ssize_t width       = pWidget->width();
            ssize_t height      = pWidget->height();

            if (((min_width >= 0) && (width < min_width)) ||
                ((max_width >= 0) && (width > max_width)) ||
                ((min_height >= 0) && (height < min_height)) ||
                ((max_height >= 0) && (height > max_height)))
                pWidget->query_resize();
        }

        void LSPSizeConstraints::set_min(ssize_t min_width, ssize_t min_height)
        {
            if ((sSize.nMinWidth == min_width) &&
                (sSize.nMinHeight == min_height))
                return;

            sSize.nMinWidth     = min_width;
            sSize.nMinHeight    = min_height;

            ssize_t width       = pWidget->width();
            ssize_t height      = pWidget->height();

            if (((min_width >= 0) && (width < min_width)) ||
                ((min_height >= 0) && (height < min_height)))
                pWidget->query_resize();
        }

        void LSPSizeConstraints::set_max(ssize_t max_width, ssize_t max_height)
        {
            if ((sSize.nMaxHeight == max_width) &&
                (sSize.nMaxHeight == max_width))
                return;

            sSize.nMaxWidth     = max_width;
            sSize.nMaxHeight    = max_height;

            ssize_t width       = pWidget->width();
            ssize_t height      = pWidget->height();

            if (((max_width >= 0) && (width > max_width)) ||
                ((max_height >= 0) && (height > max_height)))
                pWidget->query_resize();
        }

        void LSPSizeConstraints::set(const size_request_t *sr)
        {
            set(sr->nMinWidth, sr->nMinHeight, sr->nMaxWidth, sr->nMaxHeight);
        }

        void LSPSizeConstraints::apply(size_request_t *sr) const
        {
            if ((sSize.nMinWidth >= 0) && (sr->nMinWidth < sSize.nMinWidth))
                sr->nMinWidth       = sSize.nMinWidth;
            if ((sSize.nMinHeight >= 0) && (sr->nMinHeight < sSize.nMinHeight))
                sr->nMinHeight      = sSize.nMinHeight;
            if ((sSize.nMaxWidth >= 0) && (sr->nMaxWidth < sSize.nMaxWidth))
                sr->nMaxWidth       = sSize.nMaxWidth;
            if ((sSize.nMaxHeight >= 0) && (sr->nMaxHeight < sSize.nMaxHeight))
                sr->nMaxHeight      = sSize.nMaxHeight;
//            if ((sSize.nMaxWidth >= 0) && (sr->nMaxWidth >= 0) && (sr->nMaxWidth < sSize.nMaxWidth))
//                sr->nMaxWidth       = sSize.nMaxWidth;
//            if ((sSize.nMaxHeight >= 0) && (sr->nMaxHeight >= 0) && (sr->nMaxHeight < sSize.nMaxHeight))
//                sr->nMaxHeight      = sSize.nMaxHeight;

            if ((sr->nMaxWidth >= 0) && (sr->nMinWidth >= 0) && (sr->nMaxWidth < sr->nMinWidth))
                sr ->nMaxWidth      = sr->nMinWidth;
            if ((sr->nMaxHeight >= 0) && (sr->nMinHeight >= 0) && (sr->nMaxHeight < sr->nMinHeight))
                sr ->nMaxHeight     = sr->nMinHeight;
        }
    } /* namespace tk */
} /* namespace lsp */
