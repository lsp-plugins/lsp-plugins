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
                LSPString       sText;
                Color           sBgColor;
                float           fVAlign;
                float           fHAlign;
                LSPWidgetFont   sFont;
                ssize_t         nBorder;

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
                LSPFont        *font() { return &sFont; }

                /** Get current label text
                 *
                 * @return current label text
                 */
                inline const char     *text() const { return sText.get_native(); }
                inline status_t        get_text(LSPString *dst) const { return (dst->set(&sText)) ? STATUS_OK : STATUS_NO_MEM; };

                /** Get background color
                 *
                 * @return background color
                 */
                inline Color          *bg_color() { return &sBgColor; }

                /** Get vertical alignment
                 *
                 * @return vertical alignment
                 */
                inline float           valign() const { return fVAlign; }

                /** Get horizontal alignment
                 *
                 * @return horizontal alignment
                 */
                inline float           halign() const { return fHAlign; }

                /** Get border
                 *
                 * @return border
                 */
                inline float           border() const { return nBorder; }

            public:
                status_t set_text(const char *text);

                status_t set_text(const LSPString *text);

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
