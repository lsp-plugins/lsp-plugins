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

#ifndef RENDERING_FACTORY_H_
#define RENDERING_FACTORY_H_

#include <rendering/backend.h>

typedef struct r3d_factory_t
{
    /**
     * Get backend metadata
     * @param id backend identifier, starting with 0
     * @return status of operation or NULL if there is no such backend
     */
    const r3d_backend_metadata_t    *(* metadata)(r3d_factory_t *_this, size_t id);

    /**
     * Create backend
     * @param id backend identifier
     * @return status of operation
     */
    r3d_backend_t                   *(* create)(r3d_factory_t *_this, size_t id);

} r3d_factory_t;

#define R3D_FACTORY_FUNCTION_NAME       "lsp_r3d_factory"

typedef LSP_LIBRARY_IMPORT r3d_factory_t *(* lsp_r3d_factory_function_t)(const char *version);

#endif /* RENDERING_FACTORY_H_ */
