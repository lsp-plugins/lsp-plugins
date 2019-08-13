/*
 * factory.h
 *
 *  Created on: 24 апр. 2019 г.
 *      Author: sadko
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
