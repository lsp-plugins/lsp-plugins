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

#ifndef UI_TK_WIDGETS_LSPOBJECT3D_H_
#define UI_TK_WIDGETS_LSPOBJECT3D_H_

namespace lsp
{
    namespace tk
    {
        class LSPObject3D: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            public:
                explicit LSPObject3D(LSPDisplay *dpy);
                virtual ~LSPObject3D();

            public:
                LSPArea3D *area3d();

            public:
                virtual void set_view_point(const point3d_t *pov);

                virtual void render(IR3DBackend *r3d);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPOBJECT3D_H_ */
