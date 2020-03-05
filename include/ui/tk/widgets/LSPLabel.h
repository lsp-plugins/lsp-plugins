/*
 * LSPLabel.h
 *
 *  Created on: 6 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPLABEL_H_
#define UI_TK_LSPLABEL_H_

namespace lsp
{
    namespace tk
    {
        class LSPLabel: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                float               fVAlign;
                float               fHAlign;
                LSPFont             sFont;
                LSPLocalString      sText;
                ssize_t             nBorder;

            protected:
                void    query_safe_resize();

            public:
                explicit LSPLabel(LSPDisplay *dpy);
                virtual ~LSPLabel();

                virtual status_t init();

                virtual void destroy();

            public:
                /** Get font
                 *
                 * @return font
                 */
                inline LSPFont             *font()  { return &sFont; }

                /**
                 * Get text
                 * @return
                 */
                inline LSPLocalString      *text()  { return &sText; };
                inline const LSPLocalString  *text() const      { return &sText; }

                /** Get vertical alignment
                 *
                 * @return vertical alignment
                 */
                inline float                valign() const { return fVAlign; }

                /** Get horizontal alignment
                 *
                 * @return horizontal alignment
                 */
                inline float                halign() const { return fHAlign; }

                /** Get border
                 *
                 * @return border
                 */
                inline float                border() const { return nBorder; }

            public:
                void set_valign(float align);

                void set_halign(float align);

                void set_align(float halign, float valign);

                void set_border(float border);

            public:
                virtual void draw(ISurface *s);

                virtual void size_request(size_request_t *r);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPLABEL_H_ */
