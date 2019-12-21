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

                enum layer_type_t
                {
                    LT_TRIANGLES,
                    LT_LINES
                };

                typedef struct mesh_layer_t
                {
                    layer_type_t    type;
                    point3d_t      *mesh;           // Original data
                    vector3d_t     *normals;        // Normal data
                    point3d_t      *vbuffer;        // Vertex Buffer for rendering
                    vector3d_t     *nbuffer;        // Normal Buffer for rendering
                    size_t          primitives;     // Number of primitives
                    size_t          draw;           // Number of primitives to draw
                    bool            rebuild;        // Rebuild flag
                    void           *pdata;          // Allocation pointer
                } mesh_layer_t;

            protected:
                LSPColor        sColor;
                LSPColor        sLineColor;
                matrix3d_t      sMatrix;

                point3d_t       sPov;

                cstorage<mesh_layer_t> vLayers;

            protected:
                void        do_destroy();
                void        rebuild_triangles(mesh_layer_t *layer);
                void        rebuild_lines(mesh_layer_t *layer);
                void        mark_for_rebuild();

                static status_t slot_draw3d(LSPWidget *sender, void *ptr, void *data);

            public:
                explicit LSPMesh3D(LSPDisplay *dpy);
                virtual ~LSPMesh3D();

                virtual status_t        init();
                virtual void            destroy();

            public:
                inline LSPColor            *color()                     { return &sColor;           };
                inline LSPColor            *line_color()                { return &sLineColor;       };

                void                        get_position(point3d_t *dst);
                inline void                 get_view_point(point3d_t *dst) { *dst = sPov;           };

            public:
                void clear();

                status_t add_triangles(const point3d_t *mesh, const point3d_t *normals, size_t items);
                inline status_t add_triangles(const point3d_t *mesh, size_t items) { return add_triangles(mesh, NULL, items); }
                status_t add_lines(const point3d_t *mesh, size_t items);

                void set_transform(const matrix3d_t *matrix);

            public:
                virtual void render(IR3DBackend *r3d);

                virtual void set_view_point(const point3d_t *pov);

                virtual status_t on_draw3d(IR3DBackend *r3d);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_3D_LSPMESH3D_H_ */
