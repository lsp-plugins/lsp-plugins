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
        class LSPArea3D: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                LSPWidgetColor  sColor;
                LSPWidgetColor  sBgColor;

                matrix3d_t      sWorld;
                matrix3d_t      sView;
                matrix3d_t      sProjection;
                IR3DBackend    *pBackend;
                INativeWindow  *pBackendWnd;

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
                inline void     set_world_matrix(const matrix3d_t *m)       { sWorld        = *m; }
                inline void     set_view_matrix(const matrix3d_t *m)        { sView         = *m; }
                inline void     set_projection_matrix(const matrix3d_t *m)  { sProjection   = *m; }

                inline void     get_world_matrix(matrix3d_t *m) const       { *m        = sWorld; }
                inline void     get_view_matrix(matrix3d_t *m) const        { *m        = sView; }
                inline void     get_projection_matrix(matrix3d_t *m) const  { *m        = sProjection; }

                inline LSPColor        *color()         { return &sColor; }
                inline LSPColor        *bg_color()      { return &sBgColor; }

            public:
                virtual void render(ISurface *s, bool force);

                virtual bool hide();

                virtual bool show();

                virtual void draw(ISurface *s);

                virtual void realize(const realize_t *r);

                virtual void size_request(size_request_t *r);

                virtual status_t on_draw3d(IR3DBackend *r3d);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPAREA3D_H_ */
