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
                LSPWidgetColor  sColor;
                LSPWidgetColor  sAxisColor;
                point3d_t       sPosition;
                vector3d_t      sDirection;
                float           fRadius;

            public:
                explicit LSPCapture3D(LSPDisplay *dpy);
                virtual ~LSPCapture3D();

            public:
                inline LSPColor            *color()                 { return &sColor;           };
                inline LSPColor            *axis_color()            { return &sAxisColor;       };
                inline const point3d_t     *position() const        { return &sPosition;        };
                inline const vector3d_t    *direction() const       { return &sDirection;       };
                inline float                radius() const          { return fRadius;           };

            public:
                void set_position(const point3d_t *pos);
                void set_direction(const vector3d_t *dir);
                void set_radius(float radius);

            public:
                virtual void render(IR3DBackend *r3d);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_3D_LSPCAPTURE3D_H_ */
