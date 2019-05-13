/*
 * LSPCapture3D.h
 *
 *  Created on: 12 мая 2019 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_3D_LSPCAPTURE3D_H_
#define UI_TK_WIDGETS_3D_LSPCAPTURE3D_H_

namespace lsp
{
    namespace tk
    {
        class LSPCapture3D: public LSPObject3D
        {
            public:
                static const w_class_t    metadata;

            protected:
                typedef struct v_capture_t
                {
                    ray3d_t         sPosition;
                    bool            bEnabled;
                } v_capture_t;

            protected:
                LSPWidgetColor  sColor;
                LSPWidgetColor  sAxisColor;

                float           fRadius;
                cstorage<v_capture_t> vItems;

                point3d_t       sLines[12];
                ray3d_t         sBody[24];

            public:
                explicit LSPCapture3D(LSPDisplay *dpy);
                virtual ~LSPCapture3D();

                virtual status_t        init();
                virtual void            destroy();

            public:
                inline LSPColor            *color()                     { return &sColor;           };
                inline LSPColor            *axis_color()                { return &sAxisColor;       };

                const point3d_t            *position(size_t id);
                const vector3d_t           *direction(size_t id);
                const ray3d_t              *location(size_t id);
                bool                        enabled(size_t id);
                float                       radius() const              { return fRadius;           };
                inline size_t               items() const               { return vItems.size();     };

            public:
                void clear();
                status_t set_items(size_t items);
                status_t set_position(size_t id, const point3d_t *pos);
                status_t set_direction(size_t id, const vector3d_t *dir);
                status_t set_location(size_t id, const ray3d_t *loc);
                status_t set_enabled(size_t id, bool enabled);
                void set_radius(float radius);

            public:
                virtual void render(IR3DBackend *r3d);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_3D_LSPCAPTURE3D_H_ */
