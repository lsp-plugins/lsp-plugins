/*
 * LSPProgressBar.h
 *
 *  Created on: 2 июл. 2019 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_LSPPROGRESSBAR_H_
#define UI_TK_WIDGETS_LSPPROGRESSBAR_H_

namespace lsp
{
    namespace tk
    {
        
        class LSPProgressBar: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                float           fMin;
                float           fMax;
                float           fValue;
                LSPString       sText;
                Color           sBgColor;
                LSPWidgetFont   sFont;

            public:
                explicit LSPProgressBar(LSPDisplay *dpy);
                virtual ~LSPProgressBar();

            public:
                /** Get font
                 *
                 * @return font
                 */
                LSPFont                *font() { return &sFont; }

                /** Get current label text
                 *
                 * @return current label text
                 */
                inline const char      *text() const { return sText.get_utf8(); }
                inline status_t         get_text(LSPString *dst) const { return (dst->set(&sText)) ? STATUS_OK : STATUS_NO_MEM; };

                /** Get background color
                 *
                 * @return background color
                 */
                inline Color           *bg_color() { return &sBgColor; }

            public:
                status_t                set_text(const char *text);

                status_t                set_text(const LSPString *text);

            public:
                virtual void            draw(ISurface *s);

                virtual void            size_request(size_request_t *r);

        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPPROGRESSBAR_H_ */
