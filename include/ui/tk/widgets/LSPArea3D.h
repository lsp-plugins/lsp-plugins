/*
 * LSPArea3D.h
 *
 *  Created on: 25 апр. 2019 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_LSPAREA3D_H_
#define UI_TK_WIDGETS_LSPAREA3D_H_

namespace lsp
{
    namespace tk
    {
        class LSPObject3D;

        class LSPArea3D: public LSPWidgetContainer
        {
            public:
                static const w_class_t    metadata;

            protected:
                LSPColor        sColor;
                LSPPadding      sIPadding;

                point3d_t       sPov;
                matrix3d_t      sWorld;
                matrix3d_t      sView;
                matrix3d_t      sProjection;
                IR3DBackend    *pBackend;
                ISurface       *pGlass;
                size_t          nBorder;
                size_t          nRadius;
                size_t          nMinWidth;
                size_t          nMinHeight;
                realize_t       sContext;

                cvector<LSPObject3D>    vObjects;

            protected:
                void            do_destroy();
                IR3DBackend    *backend();

                static status_t slot_draw3d(LSPWidget *sender, void *ptr, void *data);

            public:
                explicit LSPArea3D(LSPDisplay *dpy);
                virtual ~LSPArea3D();

                virtual status_t        init();
                virtual void            destroy();

            public:
                inline void     set_world_matrix(const matrix3d_t *m)       { sWorld        = *m; query_draw(); }
                inline void     set_view_matrix(const matrix3d_t *m)        { sView         = *m; query_draw(); }
                inline void     set_projection_matrix(const matrix3d_t *m)  { sProjection   = *m; query_draw(); }

                inline void     get_world_matrix(matrix3d_t *m) const       { *m        = sWorld; }
                inline void     get_view_matrix(matrix3d_t *m) const        { *m        = sView; }
                inline void     get_projection_matrix(matrix3d_t *m) const  { *m        = sProjection; }

                inline LSPPadding     *internal_padding()   { return &sIPadding;        };
                inline LSPColor       *color()              { return &sColor;           };
                inline LSPColor       *bg_color()           { return &sBgColor;         };
                inline size_t          border() const       { return nBorder;           };
                inline size_t          radius() const       { return nRadius;           };
                inline size_t          min_width() const    { return nMinWidth;         };
                inline size_t          min_height() const   { return nMinHeight;        };

                inline ssize_t      context_width() const   { return sContext.nWidth;   };
                inline ssize_t      context_height() const  { return sContext.nHeight;  };

                inline size_t       num_objects3d() const   { return vObjects.size();   };
                LSPObject3D        *object3d(size_t id);

                inline void         get_view_point(point3d_t *dst) { *dst = sPov;           };

            public:
                void            set_min_width(size_t value);
                void            set_min_height(size_t value);
                void            set_border(size_t value);
                void            set_radius(size_t value);

            public:
                virtual void query_draw(size_t flags = REDRAW_SURFACE);

                virtual void draw(ISurface *s);

                virtual void realize(const realize_t *r);

                virtual void size_request(size_request_t *r);

                virtual status_t on_draw3d(IR3DBackend *r3d);

                virtual status_t add(LSPWidget *child);

                virtual status_t remove(LSPWidget *child);

                virtual void set_view_point(const point3d_t *pov);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPAREA3D_H_ */
