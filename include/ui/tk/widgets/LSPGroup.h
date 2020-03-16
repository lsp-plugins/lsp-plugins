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
                LSPLocalString  sText;
                LSPColor        sColor;
                size_t          nRadius;
                size_t          nBorder;
                LSPWidget      *pWidget;
                LSPFont         sFont;
                bool            bEmbed;

            protected:
                virtual LSPWidget  *find_widget(ssize_t x, ssize_t y);
                void                query_dimensions(dimensions_t *d);
                void                do_destroy();

            public:
                inline LSPLocalString *text()               { return &sText; }
                inline const LSPLocalString *text() const   { return &sText; }
                inline LSPColor    *color()                 { return &sColor; }
                inline LSPColor    *text_color()            { return sFont.color(); }
                inline size_t       radius() const          { return nRadius; }
                inline size_t       border() const          { return nBorder; }
                inline LSPFont     *font()                  { return &sFont; }
                inline bool         embed() const           { return bEmbed; }

            public:
                void                set_radius(size_t value);
                void                set_border(size_t value);
                void                set_embed(bool embed);

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
