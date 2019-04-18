/*
 * glx.cpp
 *
 *  Created on: 18 апр. 2019 г.
 *      Author: sadko
 */

// Architecture detection
#include <core/types.h>

#ifdef PLATFORM_UNIX_COMPATIBLE

// Standard libraries
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

// Common libraries
#include <dsp/dsp.h>
#include <rendering/base_backend.h>

// Implementation-specific libraries
#include <rendering/glx/backend.h>


// Function for instantiating backend
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    LSP_LIBRARY_EXPORT
    r3d_backend_t *lsp_r3d_instantiate(const char *version)
    {
        glx_backend_t *glx = reinterpret_cast<glx_backend_t *>(::malloc(sizeof(glx_backend_t)));
        if (glx == NULL)
            return NULL;
        ::bzero(glx, sizeof(glx_backend_t));
        glx->build_vtable();
        return glx;
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PLATFORM_UNIX_COMPATIBLE */


