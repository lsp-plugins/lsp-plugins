/*
 * glx.cpp
 *
 *  Created on: 18 апр. 2019 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>

#include <rendering/base_backend.h>

#ifdef PLATFORM_UNIX_COMPATIBLE

#define R3D_GLX_BACKEND_EXP(func)   export_func(r3d_backend_t::func, &r3d_base_backend_t::func);

typedef struct glx_backend_t: public r3d_base_backend_t
{
    void build_vtable();

    void destroy()
    {
        r3d_base_backend_t::destroy();
    }

    status_t init(void *window)
    {
        return r3d_base_backend_t::init();
    }

    status_t start(size_t x, size_t y, size_t width, size_t height)
    {
        return STATUS_OK;
    }

    status_t finish()
    {
        return STATUS_OK;
    }

} glx_backend_t;

void glx_backend_t::build_vtable()
{
    r3d_base_backend_t::build_vtable();

    R3D_GLX_BACKEND_EXP(init);
    R3D_GLX_BACKEND_EXP(destroy);
    R3D_GLX_BACKEND_EXP(start);
    R3D_GLX_BACKEND_EXP(finish);
}

#undef R3D_GLX_BACKEND_EXP

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


