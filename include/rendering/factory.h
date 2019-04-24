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
    const backend_metadata_t    *(* metadata)(r3d_factory_t *_this, size_t id);

    /**
     * Create backend
     * @param id backend identifier
     * @return status of operation
     */
    r3d_backend_t               *(* create)(r3d_factory_t *_this, size_t id);

} r3d_factory_t;

/**
 * Load backend
 * @param path path (UTF-8), optional, can be NULL (for auto-detection)
 * @param libname library name (UTF-8)
 * @param version required backend version (ASCII string)
 * @return status of operation
 */
status_t load_backend(const char *path, const char *libname, const char *version);


#endif /* RENDERING_FACTORY_H_ */
