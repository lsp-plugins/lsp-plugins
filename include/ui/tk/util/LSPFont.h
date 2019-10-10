/*
 * LSPFont.h
 *
 *  Created on: 5 сент. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_UTIL_LSPFONT_H_
#define UI_TK_UTIL_LSPFONT_H_

namespace lsp
{
    namespace tk
    {
        class LSPWidget;

        class LSPFont
        {
            protected:
                LSPDisplay         *pDisplay;
                LSPWidget          *pWidget;

                Font                sFont;
                LSPColor            sColor;

                mutable font_parameters_t   sFP;

            private:
                inline bool sync_font_parameters() const;
                inline bool sync_font_parameters(ISurface *s) const;
                inline void construct(LSPDisplay *dpy, LSPWidget *widget);

            protected:
                virtual void    on_change();

                void            trigger_change();

            public:
                explicit LSPFont(LSPDisplay *dpy);
                explicit LSPFont(LSPWidget *widget);
                explicit LSPFont(LSPDisplay *dpy, LSPWidget *widget);
                explicit LSPFont(LSPWidget *widget, LSPDisplay *dpy);
                virtual ~LSPFont();

                void init();
                void init(const LSPFont *f);

            public:
                inline bool bold() const            { return sFont.is_bold();   }
                inline bool italic() const          { return sFont.is_italic(); }
                inline bool underline() const       { return sFont.is_underline(); }
                inline float size() const           { return sFont.get_size(); }
                inline const char *name() const     { return sFont.get_name(); }
                float ascent() const;
                float descent() const;
                float height() const;
                float max_x_advance() const;
                float max_y_advance() const;
                inline LSPColor *color()            { return &sColor; }
                inline const Color *raw_color() const { return sColor.color(); }

            public:
                void set_bold(bool b = true);
                void set_italic(bool i = true);
                void set_underline(bool u = true);
                void set_size(float s);
                void set_name(const char *name);

            public:
                bool get_parameters(font_parameters_t *fp);
                bool get_parameters(ISurface *s, font_parameters_t *fp);

                bool get_text_parameters(ISurface *s, text_parameters_t *tp, const char *text);
                bool get_text_parameters(ISurface *s, text_parameters_t *tp, const LSPString *text);
                bool get_text_parameters(ISurface *s, text_parameters_t *tp, const LSPString *text, ssize_t first);
                bool get_text_parameters(ISurface *s, text_parameters_t *tp, const LSPString *text, ssize_t first, ssize_t last);

                bool get_multiline_text_parameters(ISurface *s, text_parameters_t *tp, const char *text);
                bool get_multiline_text_parameters(ISurface *s, text_parameters_t *tp, const LSPString *text);
                bool get_multiline_text_parameters(ISurface *s, text_parameters_t *tp, const LSPString *text, ssize_t first);
                bool get_multiline_text_parameters(ISurface *s, text_parameters_t *tp, const LSPString *text, ssize_t first, ssize_t last);

                bool estimate_text_parameters(text_parameters_t *tp, const char *text);
                bool estimate_text_parameters(text_parameters_t *tp, const LSPString *text);
                bool estimate_text_parameters(text_parameters_t *tp, const LSPString *text, ssize_t first);
                bool estimate_text_parameters(text_parameters_t *tp, const LSPString *text, ssize_t first, ssize_t last);

                void draw(ISurface *s, float x, float y, const char *text);
                void draw(ISurface *s, float x, float y, const LSPString *text);
                void draw(ISurface *s, float x, float y, const LSPString *text, size_t first);
                void draw(ISurface *s, float x, float y, const LSPString *text, size_t first, size_t last);

                void draw(ISurface *s, float x, float y, const Color &c, const char *text);
                void draw(ISurface *s, float x, float y, const Color &c, const LSPString *text);
                void draw(ISurface *s, float x, float y, const Color &c, const LSPString *text, size_t first);
                void draw(ISurface *s, float x, float y, const Color &c, const LSPString *text, size_t first, size_t last);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPFONT_H_ */
