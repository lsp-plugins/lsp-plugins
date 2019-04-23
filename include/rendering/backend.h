/*
 * backend.h
 *
 *  Created on: 18 апр. 2019 г.
 *      Author: sadko
 */

#ifndef RENDERING_BACKEND_H_
#define RENDERING_BACKEND_H_

#include <dsp/dsp.h>
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
    void        (* destroy)(r3d_backend_t *_this);

    /**
     * Initialize backend
     * @param window native window handle to work with
     * @param out_window pointer to store the created window handle
     * @return status of operation
     */
    status_t    (* init)(r3d_backend_t *_this, void *window, void **out_window);

    /**
     * Show the backend's view/window
     * @return status of operation
     */
    status_t    (* show)(r3d_backend_t *_this);

    /**
     * Hide the backend's view/window
     * @return the backend's view/window
     */
    status_t    (* hide)(r3d_backend_t *_this);

    /**
     * @param left the leftmost coordinate of the backend's viewport relative to the parent window
     * @param top the topmost coordinate of the backend's viewport relative to the parent window
     * @param width the width of the backend's viewport
     * @param height the heigh of the backend's viewport
     */
    status_t    (* locate)(r3d_backend_t *_this, ssize_t left, ssize_t top, ssize_t width, ssize_t height);

    /**
     * Start rendering on the window
     * @return status of operation
     */
    status_t    (* start)(r3d_backend_t *_this);

    /**
     * Complete rendering on the window
     * @return status of operation
     */
    status_t    (* finish)(r3d_backend_t *_this);

    /**
     * Set transformation matrix
     * @param type transformation matrix type
     * @param m transformation matrix data of 16 floats (column-major)
     * @return status of operation
     */
    status_t    (* set_matrix)(r3d_backend_t *_this, r3d_matrix_type_t type, const matrix3d_t *m);

    /**
     * Get transformation matrix
     * @param type transformation matrix type
     * @param m pointer to retrieve transformation matrix data of 16 floats (column-major)
     * @return status of operation
     */
    status_t    (* get_matrix)(r3d_backend_t *_this, r3d_matrix_type_t type, matrix3d_t *m);

    /**
     * Set current lighting schema according to passed array of lights
     * @param lights array of lights
     * @param count number of lights per scene
     * @return status of operation
     */
    status_t    (* set_lights)(r3d_backend_t *_this, const r3d_light_t *lights, size_t count);

    /**
     * Draw data
     * @param buffer buffer data to draw
     * @return status of operation
     */
    status_t    (* draw_primitives)(r3d_backend_t *_this, const r3d_buffer_t *buffer);

    /**
     * Set the default background color
     * @param color default background color
     * @return
     */
    status_t    (* set_bg_color)(r3d_backend_t *_this, const color3d_t *color);
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
