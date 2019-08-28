/*
 * glx.cpp
 *
 *  Created on: 18 апр. 2019 г.
 *      Author: sadko
 */

// Architecture detection
#include <core/types.h>

#ifdef PLATFORM_UNIX_COMPATIBLE

// Implementation-specific libraries
#include <rendering/glx/backend.h>
#include <rendering/glx/factory.h>

namespace lsp
{
    const r3d_backend_metadata_t glx_factory_t::sMetadata[] =
    {
        { "glx_2x", "openGL 2.0+ (GLX)" }
    };

    const r3d_backend_metadata_t *glx_factory_t::metadata(glx_factory_t *_this, size_t id)
    {
        size_t count = sizeof(sMetadata) / sizeof(r3d_backend_metadata_t);
        return (id < count) ? &sMetadata[id] : NULL;
    }

    r3d_backend_t *glx_factory_t::create(glx_factory_t *_this, size_t id)
    {
        if (id == 0)
            return new glx_backend_t();
        return NULL;
    }

    glx_factory_t::glx_factory_t()
    {
        #define R3D_GLX_FACTORY_EXP(func)   export_func(r3d_factory_t::func, &glx_factory_t::func);
        R3D_GLX_FACTORY_EXP(create);
        R3D_GLX_FACTORY_EXP(metadata);
        #undef R3D_GLX_FACTORY_EXP
    }

    glx_factory_t::~glx_factory_t()
    {
    }

    // Create GLX factory
    glx_factory_t   glx_factory;
}

#if !defined(LSP_IDE_DEBUG) && !defined(LSP_PROFILING)
    #include <metadata/metadata.h>
    #include <core/stdlib/string.h>

    // Function for instantiating backend
    #ifdef __cplusplus
    extern "C"
    {
    #endif /* __cplusplus */

        LSP_LIBRARY_EXPORT
        r3d_factory_t *lsp_r3d_factory(const char *version)
        {
            // Check the LSP version
            if (::strcmp(version, LSP_MAIN_VERSION))
                return NULL;

            // Create the GLX backend
            return &glx_factory;
        }

    #ifdef __cplusplus
    }
    #endif /* __cplusplus */
#endif /* LSP_IDE_DEBUG || LSP_PROFILING */

#endif /* PLATFORM_UNIX_COMPATIBLE */


