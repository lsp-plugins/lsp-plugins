/*
 * LSPMesh3D.h
 *
 *  Created on: 14 мая 2019 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_3D_LSPMESH3D_H_
#define UI_TK_WIDGETS_3D_LSPMESH3D_H_

namespace lsp
{
    namespace tk
    {
        class LSPMesh3D: public LSPObject3D
        {
            public:
                static const w_class_t    metadata;

            protected:
                LSPWidgetColor  sColor;
                matrix3d_t      sMatrix;

                point3d_t       sPov;
                point3d_t      *pMesh;
                ray3d_t        *pView;
                size_t          nItems;

                bool            bRebuild;

            protected:
                void        do_destroy();
                status_t    rebuild_mesh();

            public:
                explicit LSPMesh3D(LSPDisplay *dpy);
                virtual ~LSPMesh3D();

                virtual status_t        init();
                virtual void            destroy();

            public:
                inline LSPColor            *color()                     { return &sColor;           };

                void                        get_position(point3d_t *dst);
                inline void                 get_view_point(point3d_t *dst) { *dst = sPov;           };

            public:
                void clear();
                status_t set_mesh(const point3d_t *mesh, size_t items);
                void set_transform(const matrix3d_t *matrix);

            public:
                virtual void render(IR3DBackend *r3d);

                virtual void set_view_point(const point3d_t *pov);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_3D_LSPMESH3D_H_ */
