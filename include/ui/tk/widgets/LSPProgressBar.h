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
                ssize_t         nMinWidth;
                ssize_t         nMinHeight;
                LSPString       sText;
                LSPFont         sFont;
                LSPColor        sColor;
                LSPColor        sSelColor;

            public:
                explicit LSPProgressBar(LSPDisplay *dpy);
                virtual ~LSPProgressBar();

                virtual status_t        init();

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
                inline LSPColor        *color()     { return &sColor;       }
                inline LSPColor        *sel_color() { return &sSelColor;    }

                inline float            get_min_value() const   { return fMin;      }
                inline float            get_max_value() const   { return fMax;      }
                inline float            get_value() const       { return fValue;    }

            public:
                status_t                set_text(const char *text);

                status_t                set_text(const LSPString *text);

                bool                    set_min_value(const float v);
                bool                    set_max_value(const float v);
                bool                    set_value(const float v);

                void                    set_min_width(ssize_t value);
                void                    set_min_height(ssize_t value);

            public:
                virtual void            draw(ISurface *s);

                virtual void            size_request(size_request_t *r);

        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPPROGRESSBAR_H_ */
