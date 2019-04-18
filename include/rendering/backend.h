/*
 * backend.h
 *
 *  Created on: 18 апр. 2019 г.
 *      Author: sadko
 */

#ifndef RENDERING_BACKEND_H_
#define RENDERING_BACKEND_H_

#include <core/types.h>
#include <core/status.h>
#include <rendering/types.h>

using namespace lsp;

typedef struct r3d_backend_t r3d_backend_t;

#define R3D_EXPORT

/**
 * 3D Rendering backend interface, just functions that should be called on
 * a structure instance
 */
struct r3d_backend_t
{
    /**
     * Destroy backend
     */
    void        (* destroy)();

    /**
     * Initialize backend
     * @param window native window handle to work with
     * @return
     */
    status_t    (* init)(void *window);

    /**
     * Start rendering on the window
     * @param x the leftmost coordinate of the backend's viewport
     * @param y the topmost coordinate of the backend's viewport
     * @param width the width of the backend's viewport
     * @param height the heigh of the backend's viewport
     * @return status of operation
     */
    status_t    (* start)(size_t x, size_t y, size_t width, size_t height);

    /**
     * Complete rendering on the window
     * @return status of operation
     */
    status_t    (* finish)();

    /**
     * Set transformation matrix
     * @param type transformation matrix type
     * @param m transformation matrix data of 16 floats (column-major)
     * @return status of operation
     */
    status_t    (* set_matrix)(r3d_matrix_type_t type, const float *m);

    /**
     * Get transformation matrix
     * @param type transformation matrix type
     * @param m pointer to retrieve transformation matrix data of 16 floats (column-major)
     * @return status of operation
     */
    status_t    (* get_matrix)(r3d_matrix_type_t type, float *m);
};

/**
 * Load backend
 * @param path path (UTF-8), optional, can be NULL (for auto-detection)
 * @param libname library name (UTF-8)
 * @param version required backend version (ASCII string)
 * @return status of operation
 */
status_t load_backend(const char *path, const char *libname, const char *version);

#endif /* RENDERING_BACKEND_H_ */
