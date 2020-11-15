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

#ifndef UI_TK_WIDGETS_3D_LSPCAPTURE3D_H_
#define UI_TK_WIDGETS_3D_LSPCAPTURE3D_H_

#include <core/3d/raytrace.h>

namespace lsp
{
    namespace tk
    {
        class LSPCapture3D: public LSPObject3D
        {
            public:
                static const w_class_t    metadata;

            protected:
                typedef struct v_capture_t: public rt_capture_settings_t
                {
                    bool            bEnabled;
                } v_capture_t;

            protected:
                LSPColor        sColor;
                LSPColor        sAxisColor;

                cstorage<v_capture_t> vItems;

                point3d_t       sLines[6];

            public:
                explicit LSPCapture3D(LSPDisplay *dpy);
                virtual ~LSPCapture3D();

                virtual status_t        init();
                virtual void            destroy();

            public:
                inline LSPColor            *color()                     { return &sColor;           };
                inline LSPColor            *axis_color()                { return &sAxisColor;       };

                status_t                    get_position(point3d_t *dst, size_t id);
                status_t                    get_direction(vector3d_t *dst, size_t id);
                status_t                    get_location(ray3d_t *dst, size_t id);
                bool                        enabled(size_t id) const;
                float                       radius(size_t id) const;
                inline size_t               items() const               { return vItems.size();     };

            public:
                void clear();
                status_t set_items(size_t items);
                status_t set_transform(size_t id, const matrix3d_t *matrix);
                status_t set_enabled(size_t id, bool enabled);
                void set_radius(size_t id, float radius);

            public:
                virtual void render(IR3DBackend *r3d);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_3D_LSPCAPTURE3D_H_ */
