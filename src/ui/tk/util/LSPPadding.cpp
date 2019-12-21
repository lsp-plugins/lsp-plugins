/*
 * LSPPadding.cpp
 *
 *  Created on: 12 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        LSPPadding::LSPPadding(LSPWidget *widget)
        {
            pWidget             = widget;
            sPadding.nLeft      = 0;
            sPadding.nRight     = 0;
            sPadding.nTop       = 0;
            sPadding.nBottom    = 0;
        }

        LSPPadding::~LSPPadding()
        {
            pWidget     = NULL;
        }

        size_t LSPPadding::set_left(size_t value)
        {
            size_t old      = sPadding.nLeft;
            sPadding.nLeft  = value;
            if (pWidget != NULL)
                pWidget->query_resize();
            return old;
        }

        size_t LSPPadding::set_right(size_t value)
        {
            size_t old      = sPadding.nRight;
            sPadding.nRight = value;
            if (pWidget != NULL)
                pWidget->query_resize();
            return old;
        }

        size_t LSPPadding::set_top(size_t value)
        {
            size_t old      = sPadding.nTop;
            sPadding.nTop   = value;
            if (pWidget != NULL)
                pWidget->query_resize();
            return old;
        }

        size_t LSPPadding::set_bottom(size_t value)
        {
            size_t old      = sPadding.nBottom;
            sPadding.nBottom= value;
            if (pWidget != NULL)
                pWidget->query_resize();
            return old;
        }

        void LSPPadding::set_all(size_t value)
        {
            if ((sPadding.nLeft == value) &&
                (sPadding.nTop == value) &&
                (sPadding.nRight == value) &&
                (sPadding.nBottom == value))
                return;

            sPadding.nLeft      = value;
            sPadding.nTop       = value;
            sPadding.nRight     = value;
            sPadding.nBottom    = value;

            if (pWidget != NULL)
                pWidget->query_resize();
        }

        void LSPPadding::set_horizontal(size_t left, size_t right)
        {
            if ((sPadding.nLeft == left) &&
                (sPadding.nRight == right))
                return;

            sPadding.nLeft      = left;
            sPadding.nRight     = right;

            if (pWidget != NULL)
                pWidget->query_resize();
        }

        void LSPPadding::set_vertical(size_t top, size_t bottom)
        {
            if ((sPadding.nTop == top) &&
                (sPadding.nBottom == bottom))
                return;

            sPadding.nTop       = top;
            sPadding.nBottom    = bottom;

            if (pWidget != NULL)
                pWidget->query_resize();
        }

        void LSPPadding::set(size_t left, size_t right, size_t top, size_t bottom)
        {
            if ((sPadding.nLeft == left) &&
                (sPadding.nRight == right) &&
                (sPadding.nTop == top) &&
                (sPadding.nBottom == bottom))
                return;

            sPadding.nLeft      = left;
            sPadding.nRight     = right;
            sPadding.nTop       = top;
            sPadding.nBottom    = bottom;

            if (pWidget != NULL)
                pWidget->query_resize();
        }

        void LSPPadding::set(const padding_t *p)
        {
            if ((sPadding.nLeft == p->nLeft) &&
                (sPadding.nRight == p->nRight) &&
                (sPadding.nTop == p->nTop) &&
                (sPadding.nBottom == p->nBottom))
                return;

            sPadding            = *p;

            if (pWidget != NULL)
                pWidget->query_resize();
        }

        void LSPPadding::set(const LSPPadding *p)
        {
            if ((sPadding.nLeft == p->sPadding.nLeft) &&
                (sPadding.nRight == p->sPadding.nRight) &&
                (sPadding.nTop == p->sPadding.nTop) &&
                (sPadding.nBottom == p->sPadding.nBottom))
                return;

            sPadding            = p->sPadding;

            if (pWidget != NULL)
                pWidget->query_resize();
        }
    } /* namespace tk */
} /* namespace lsp */
