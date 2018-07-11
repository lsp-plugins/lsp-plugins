/*
 * LSPGroup.h
 *
 *  Created on: 17 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPGROUP_H_
#define UI_TK_LSPGROUP_H_

namespace lsp
{
    namespace tk
    {
        class LSPGroup: public LSPWidgetContainer
        {
            public:
                static const w_class_t    metadata;

            protected:
                typedef struct dimensions_t
                {
                    size_t      nGapLeft;
                    size_t      nGapTop;
                    size_t      nGapRight;
                    size_t      nGapBottom;
                    size_t      nMinWidth;
                    size_t      nMinHeight;
                } dimensions_t;

            protected:
                char           *sText;
                Color           sColor;
                Color           sTextColor;
                Color           sBgColor;
                size_t          nRadius;
                size_t          nBorder;
                LSPWidget      *pWidget;


            protected:
                virtual LSPWidget  *find_widget(ssize_t x, ssize_t y);
                void                query_dimensions(dimensions_t *d);
                void                do_destroy();

            public:
                inline const char  *text() const            { return sText; }

                inline Color       *color()                 { return &sColor; }

                inline Color       *bg_color()              { return &sBgColor; }

                inline Color       *text_color()            { return &sTextColor; }

                inline size_t       radius() const          { return nRadius; }

                inline size_t       border() const          { return nBorder; }

            public:
                status_t            set_text(const char *text);

                void                set_radius(size_t value);

                void                set_border(size_t value);

            public:
                explicit LSPGroup(LSPDisplay *dpy);
                virtual ~LSPGroup();

                virtual status_t init();
                virtual void destroy();

            public:
                virtual void render(ISurface *s, bool force);

                virtual status_t add(LSPWidget *widget);

                virtual status_t remove(LSPWidget *widget);

                virtual void size_request(size_request_t *r);

                virtual void realize(const realize_t *r);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPGROUP_H_ */
