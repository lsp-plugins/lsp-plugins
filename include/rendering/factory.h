/*
 * factory.h
 *
 *  Created on: 24 апр. 2019 г.
 *      Author: sadko
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
