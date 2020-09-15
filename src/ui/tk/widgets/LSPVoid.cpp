/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 11 апр. 2020 г.
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

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPVoid::metadata = { "LSPVoid", &LSPWidget::metadata };
        
        LSPVoid::LSPVoid(LSPDisplay *dpy):
            LSPWidget(dpy),
            sConstraints(this)
        {
            pClass          = &metadata;
        }
        
        LSPVoid::~LSPVoid()
        {
        }

        void LSPVoid::render(ISurface *s, bool force)
        {
            if ((sSize.nWidth > 0) && (sSize.nHeight > 0))
            {
                Color bgColor(sBgColor);
                s->fill_rect(sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight, bgColor);
            }
        }

        void LSPVoid::size_request(size_request_t *r)
        {
            // Add external size constraints
            sConstraints.apply(r);
        }

    } /* namespace tk */
} /* namespace lsp */
