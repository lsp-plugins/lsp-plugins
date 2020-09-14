/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 12 мая 2019 г.
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
        const w_class_t LSPObject3D::metadata = { "LSPObject3D", &LSPWidget::metadata };
        
        LSPObject3D::LSPObject3D(LSPDisplay *dpy):
            LSPWidget(dpy)
        {
            pClass          = &metadata;
        }
        
        LSPObject3D::~LSPObject3D()
        {
        }

        void LSPObject3D::render(IR3DBackend *r3d)
        {
        }

        void LSPObject3D::set_view_point(const point3d_t *pov)
        {
        }

        LSPArea3D *LSPObject3D::area3d()
        {
            LSPComplexWidget *parent = pParent;

            while (parent != NULL)
            {
                LSPArea3D *area3d = widget_cast<LSPArea3D>(parent);
                if (area3d != NULL)
                    return area3d;
                parent  = parent->parent();
            }
            return NULL;
        }
    
    } /* namespace tk */
} /* namespace lsp */
