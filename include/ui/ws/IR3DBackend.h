/*
 * IR3DBackend.h
 *
 *  Created on: 24 апр. 2019 г.
 *      Author: sadko
 */

#ifndef UI_WS_IR3DBACKEND_H_
#define UI_WS_IR3DBACKEND_H_

#include <rendering/backend.h>

namespace lsp
{
    namespace ws
    {
        /**
         * This class is a wrapper around r3d_backend_t that allows dynamic change of backend
         */
        class IR3DBackend
        {
            protected:
                friend class        IDisplay;

                r3d_backend_t      *pBackend;   // Currently used backend
                void               *hParent;    // Parent window
                void               *hWindow;    // Currently used window
                IDisplay           *pDisplay;

            private:
                IR3DBackend & operator = (const IR3DBackend &);

            protected:
                explicit IR3DBackend(IDisplay *dpy, r3d_backend_t *backend, void *parent, void *window);
                void    replace_backend(r3d_backend_t *factory, void *window);

            public:
                ~IR3DBackend();

            public:
                status_t destroy();

                /**
                 * Return native window handle if it is present
                 */
                inline void *handle()       { return hWindow; };

                inline bool valid() const { return pBackend != NULL; }

                status_t locate(ssize_t left, ssize_t top, ssize_t width, ssize_t height);
                status_t get_location(ssize_t *left, ssize_t *top, ssize_t *width, ssize_t *height);

                status_t begin_draw();
                status_t sync();
                status_t read_pixels(void *buf, size_t stride, r3d_pixel_format_t format);
                status_t end_draw();

                status_t set_matrix(r3d_matrix_type_t type, const matrix3d_t *m);
                status_t get_matrix(r3d_matrix_type_t type, matrix3d_t *m);
                status_t set_lights(const r3d_light_t *lights, size_t count);
                status_t draw_primitives(const r3d_buffer_t *buffer);
                status_t set_bg_color(const color3d_t *color);
        };
    
    } /* namespace io */
} /* namespace lsp */

#endif /* UI_WS_IR3DBACKEND_H_ */
