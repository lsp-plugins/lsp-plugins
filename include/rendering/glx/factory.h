/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 24 апр. 2019 г.
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

#ifndef RENDERING_GLX_FACTORY_H_
#define RENDERING_GLX_FACTORY_H_

#include <rendering/factory.h>

namespace lsp
{
    // GLX backend factory
    typedef struct glx_factory_t: public r3d_factory_t
    {
        static const r3d_backend_metadata_t sMetadata[];

        static const r3d_backend_metadata_t    *metadata(glx_factory_t *_this, size_t id);
        static r3d_backend_t                   *create(glx_factory_t *_this, size_t id);

        explicit glx_factory_t();
        ~glx_factory_t();

    } glx_factory_t;
}

#endif /* RENDERING_GLX_FACTORY_H_ */
