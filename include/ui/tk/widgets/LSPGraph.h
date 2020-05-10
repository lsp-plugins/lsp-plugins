/*
 * LSPGraph.h
 *
 *  Created on: 18 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPGRAPH_H_
#define UI_TK_LSPGRAPH_H_

namespace lsp
{
    namespace tk
    {
        class LSPGraphItem;
        class LSPAxis;
        class LSPCenter;

        class LSPGraph: public LSPWidgetContainer
        {
            public:
                static const w_class_t  metadata;

            protected:
                size_t                  nMinWidth;
                size_t                  nMinHeight;
                size_t                  nBorder;
                size_t                  nRadius;
                float                   fCanvasLeft;
                float                   fCanvasTop;
                float                   fCanvasWidth;
                float                   fCanvasHeight;
                ISurface               *pGlass;
                ISurface               *pCanvas;

                LSPPadding              sIPadding;
                LSPColor                sColor;
                cvector<LSPGraphItem>   vObjects;
                cvector<LSPAxis>        vAxises;
                cvector<LSPAxis>        vBasises;
                cvector<LSPCenter>      vCenters;

                #ifdef LSP_TRACE
                struct timespec sClock;
                size_t          nFrames;
                #endif /* LSP_TRACE */

            protected:
                ISurface       *get_canvas(ISurface *s, ssize_t w, ssize_t h, const Color &color);
                void            do_destroy();

            protected:
                virtual LSPWidget       *find_widget(ssize_t x, ssize_t y);

            public:
                explicit LSPGraph(LSPDisplay *dpy);
                virtual ~LSPGraph();

                virtual status_t    init();
                virtual void        destroy();

            public:
                inline size_t   items() const               { return vObjects.size();   };
                size_t          axes() const                { return vAxises.size();    };
                size_t          basis_axes() const          { return vBasises.size();   };
                size_t          centers() const             { return vCenters.size();   };

                LSPGraphItem   *item(size_t index)          { return vObjects[index];   };
                LSPAxis        *axis(size_t index)          { return vAxises[index];    };
                LSPAxis        *basis_axis(size_t index)    { return vBasises[index];   };
                LSPCenter      *center(size_t index)        { return vCenters[index];   };

                LSPColor       *color()                     { return &sColor;           };

                bool            center(size_t index, float *x, float *y);
                bool            center(LSPCenter *center, float *x, float *y);

                size_t          get_axes(LSPAxis **dst, size_t start, size_t count);
                size_t          get_basis_axes(LSPAxis **dst, size_t start, size_t count);
                size_t          get_items(LSPGraphItem **dst, size_t start, size_t count);

                LSPPadding     *internal_padding()          { return &sIPadding;        };
                size_t          min_width() const           { return nMinWidth;         };
                size_t          min_height() const          { return nMinHeight;        };
                size_t          border() const              { return nBorder;           };
                size_t          radius() const              { return nRadius;           };
                inline float    canvas_left() const         { return fCanvasLeft;       };
                inline float    canvas_top() const          { return fCanvasTop;        };

            public:
                void            set_min_width(size_t value);
                void            set_min_height(size_t value);
                void            set_border(size_t value);
                void            set_radius(size_t value);

            public:
                inline float    area_left() const           { return 1.0f; }
                inline float    area_top() const            { return (pCanvas != NULL) ? pCanvas->height() - 1.0f: 0.0f; }
                inline float    area_bottom() const         { return 1.0f; }
                inline float    area_right() const          { return (pCanvas != NULL) ? pCanvas->width() - 1.0f: 0.0f; }
                inline float    area_width() const          { return fCanvasWidth; }
                inline float    area_height() const         { return fCanvasHeight; }

            public:
                virtual void        query_draw(size_t flags);

                virtual void        size_request(size_request_t *r);

                virtual status_t    add(LSPWidget *widget);

                virtual status_t    remove(LSPWidget *widget);

                virtual status_t    on_mouse_down(const ws_event_t *e);

                virtual status_t    on_resize(const realize_t *r);

                virtual void        draw(ISurface *s);

                virtual void        realize(const realize_t *r);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPGRAPH_H_ */
