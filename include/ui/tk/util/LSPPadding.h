/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 12 июл. 2017 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef UI_TK_LSPPADDING_H_
#define UI_TK_LSPPADDING_H_

namespace lsp
{
    namespace tk
    {
        class LSPWidget;
        
        class LSPPadding
        {
            protected:
                LSPWidget      *pWidget;
                padding_t       sPadding;

            public:
                explicit LSPPadding(LSPWidget *widget);
                ~LSPPadding();

                inline size_t left() const          { return sPadding.nLeft;     }
                inline size_t right() const         { return sPadding.nRight;    }
                inline size_t top() const           { return sPadding.nTop;      }
                inline size_t bottom() const        { return sPadding.nBottom;   }
                inline size_t horizontal()  const   { return sPadding.nLeft + sPadding.nRight; }
                inline size_t vertical()  const     { return sPadding.nTop + sPadding.nBottom; }
                inline void get(padding_t *p) const { *p = sPadding; }

                size_t set_left(size_t value);
                size_t set_right(size_t value);
                size_t set_top(size_t value);
                size_t set_bottom(size_t value);
                void   set_all(size_t value);
                void   set_horizontal(size_t left, size_t right);
                void   set_vertical(size_t top, size_t bottom);

                void set(size_t left, size_t right, size_t top, size_t bottom);
                void set(const padding_t *p);
                void set(const LSPPadding *p);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPPADDING_H_ */
