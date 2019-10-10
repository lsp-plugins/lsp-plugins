/*
 * LSPFont.cpp
 *
 *  Created on: 5 сент. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        void LSPFont::construct(LSPDisplay *dpy, LSPWidget *widget)
        {
            pDisplay        = dpy;
            pWidget         = widget;

            sFP.Ascent      = 0;
            sFP.Descent     = 0;
            sFP.Height      = -1.0f;
            sFP.MaxXAdvance = 0;
            sFP.MaxYAdvance = 0;
        }

        LSPFont::LSPFont(LSPDisplay *dpy)
        {
            construct(dpy, NULL);
        }

        LSPFont::LSPFont(LSPWidget *widget)
        {
            construct(widget->display(), widget);
        }

        LSPFont::LSPFont(LSPDisplay *dpy, LSPWidget *widget)
        {
            construct(dpy, widget);
        }

        LSPFont::LSPFont(LSPWidget *widget, LSPDisplay *dpy)
        {
            construct(dpy, widget);
        }

        LSPFont::~LSPFont()
        {
            pDisplay        = NULL;
            pWidget         = NULL;
        }

        void LSPFont::on_change()
        {
        }

        void LSPFont::trigger_change()
        {
            on_change();
            if (pWidget != NULL)
                pWidget->query_draw();
        }

        void LSPFont::init()
        {
            LSPTheme *theme = pDisplay->theme();
            if (theme == NULL)
                return;

            LSPFont *src = theme->font();
            if ((this == src) || (src == NULL))
                return;

            sFont.set(&src->sFont);
            sFP.Height      = -1.0f;
        }

        void LSPFont::init(const LSPFont *src)
        {
            sFont.set(&src->sFont);
            sFP     = src->sFP;
        }

        void LSPFont::set_bold(bool b)
        {
            if (b == sFont.is_bold())
                return;
            sFP.Height  = -1.0f;
            sFont.set_bold(b);
            trigger_change();
        }

        void LSPFont::set_italic(bool i)
        {
            if (i == sFont.is_italic())
                return;
            sFP.Height  = -1.0f;
            sFont.set_italic(i);
            trigger_change();
        }

        void LSPFont::set_underline(bool u)
        {
            if (u == sFont.is_underline())
                return;
            sFont.set_underline(u);
            trigger_change();
        }

        void LSPFont::set_size(float s)
        {
            if (s == sFont.get_size())
                return;

            sFP.Height  = -1.0f;
            sFont.set_size(s);
            trigger_change();
        }

        void LSPFont::set_name(const char *name)
        {
            const char *sname   = sFont.get_name();
            if (name == sname)
                return;
            else if ((name != NULL) && (sname != NULL))
            {
                if (!strcmp(name, sname))
                    return;
            }

            sFP.Height  = -1.0f;
            sFont.set_name(name);
            trigger_change();
        }

        inline bool LSPFont::sync_font_parameters() const
        {
            if (sFP.Height >= 0.0f)
                return true;

            if (pDisplay == NULL)
                return false;
            ISurface *s = pDisplay->create_surface(1, 1);
            if (s == NULL)
                return false;

            bool result = s->get_font_parameters(sFont, &sFP);
            s->destroy();
            delete s;

            return result;
        }

        float LSPFont::ascent() const
        {
            sync_font_parameters();
            return sFP.Ascent;
        }

        float LSPFont::descent() const
        {
            sync_font_parameters();
            return sFP.Descent;
        }

        float LSPFont::height() const
        {
            sync_font_parameters();
            return sFP.Height;
        }

        float LSPFont::max_x_advance() const
        {
            sync_font_parameters();
            return sFP.MaxXAdvance;
        }

        float LSPFont::max_y_advance() const
        {
            sync_font_parameters();
            return sFP.MaxYAdvance;
        }

        bool LSPFont::get_parameters(font_parameters_t *fp)
        {
            if (!sync_font_parameters())
                return false;

            *fp     = sFP;
            return true;
        }

        bool LSPFont::get_parameters(ISurface *s, font_parameters_t *fp)
        {
            if (!s->get_font_parameters(sFont, fp))
                return false;
            sFP     = *fp;
            return true;
        }

        bool LSPFont::get_text_parameters(ISurface *s, text_parameters_t *tp, const LSPString *text)
        {
            const char *str = text->get_utf8();
            return (str != NULL) ? s->get_text_parameters(sFont, tp, str) : false;
        }

        bool LSPFont::get_text_parameters(ISurface *s, text_parameters_t *tp, const LSPString *text, ssize_t first)
        {
            const char *str = text->get_utf8(first);
            return (str != NULL) ? s->get_text_parameters(sFont, tp, str) : false;
        }

        bool LSPFont::get_text_parameters(ISurface *s, text_parameters_t *tp, const LSPString *text, ssize_t first, ssize_t last)
        {
            const char *str = text->get_utf8(first, last);
            return (str != NULL) ? s->get_text_parameters(sFont, tp, str) : false;
        }

        bool LSPFont::estimate_text_parameters(text_parameters_t *tp, const char *text)
        {
            if (pDisplay == NULL)
                return false;
            ISurface *s = pDisplay->create_surface(1, 1);
            if (s == NULL)
                return false;

            bool result = s->get_text_parameters(sFont, tp, text);
            s->destroy();
            delete s;

            return result;
        }

        bool LSPFont::estimate_text_parameters(text_parameters_t *tp, const LSPString *text)
        {
            const char *str = text->get_utf8();
            return (str != NULL) ? estimate_text_parameters(tp, str) : false;
        }

        bool LSPFont::estimate_text_parameters(text_parameters_t *tp, const LSPString *text, ssize_t first)
        {
            const char *str = text->get_utf8(first);
            return (str != NULL) ? estimate_text_parameters(tp, str) : false;
        }

        bool LSPFont::estimate_text_parameters(text_parameters_t *tp, const LSPString *text, ssize_t first, ssize_t last)
        {
            const char *str = text->get_utf8(first, last);
            return (str != NULL) ? estimate_text_parameters(tp, str) : false;
        }

        bool LSPFont::get_text_parameters(ISurface *s, text_parameters_t *tp, const char *text)
        {
            return s->get_text_parameters(sFont, tp, text);
        }

        bool LSPFont::get_multiline_text_parameters(ISurface *s, text_parameters_t *tp, const char *text)
        {
            LSPString tmp;
            if (!tmp.set_native(text))
                return false;
            return get_multiline_text_parameters(s, tp, &tmp);
        }

        bool LSPFont::get_multiline_text_parameters(ISurface *s, text_parameters_t *tp, const LSPString *text)
        {
            return get_multiline_text_parameters(s, tp, text, 0, text->length());
        }

        bool LSPFont::get_multiline_text_parameters(ISurface *s, text_parameters_t *tp, const LSPString *text, ssize_t first)
        {
            ssize_t len = text->length();
            if ((first < 0) || (first > len))
                return false;
            return get_multiline_text_parameters(s, tp, text, first, len);
        }

        bool LSPFont::get_multiline_text_parameters(ISurface *s, text_parameters_t *tp, const LSPString *text, ssize_t first, ssize_t last)
        {
            ssize_t prev = 0, curr = 0, tail = 0;
            font_parameters_t fp;
            text_parameters_t xp;

            sFont.get_parameters(s, &fp);
            float w = 0, h = 0;

            while (curr < last)
            {
                curr    = text->index_of(prev, '\n');
                if ((curr < 0) || (curr > last))
                {
                    curr        = last;
                    tail        = last;
                }
                else
                {
                    tail        = curr;
                    if ((tail > last) && (text->at(tail-1) == '\r'))
                        --tail;
                }

                // Get text parameters
                const char *str = text->get_utf8(prev, tail);
                if (str == NULL)
                    return false;

                if (!s->get_text_parameters(sFont, &xp, str))
                    return false;

                if (w < xp.Width)
                    w       = xp.Width;
                h      += fp.Height;

                prev    = curr + 1;
            }

            // Store font parameters
            xp.Width    = w;
            xp.Height   = h;
            *tp         = xp;
            return true;
        }

        void LSPFont::draw(ISurface *s, float x, float y, const char *text)
        {
            Color tmp(sColor);
            s->out_text(sFont, x, y, text, tmp);
        }

        void LSPFont::draw(ISurface *s, float x, float y, const LSPString *text)
        {
            const char *str = text->get_utf8();
            if (str != NULL)
            {
                Color tmp(sColor);
                s->out_text(sFont, x, y, str, tmp);
            }
        }

        void LSPFont::draw(ISurface *s, float x, float y, const LSPString *text, size_t first)
        {
            const char *str = text->get_utf8(first);
            if (str != NULL)
            {
                Color tmp(sColor);
                s->out_text(sFont, x, y, str, tmp);
            }
        }

        void LSPFont::draw(ISurface *s, float x, float y, const LSPString *text, size_t first, size_t last)
        {
            const char *str = text->get_utf8(first, last);
            if (str != NULL)
            {
                Color tmp(sColor);
                s->out_text(sFont, x, y, str, tmp);
            }
        }

        void LSPFont::draw(ISurface *s, float x, float y, const Color & c, const char *text)
        {
            s->out_text(sFont, x, y, text, c);
        }

        void LSPFont::draw(ISurface *s, float x, float y, const Color & c, const LSPString *text)
        {
            const char *str = text->get_utf8();
            if (str != NULL)
                s->out_text(sFont, x, y, str, c);
        }

        void LSPFont::draw(ISurface *s, float x, float y, const Color & c, const LSPString *text, size_t first)
        {
            const char *str = text->get_utf8(first);
            if (str != NULL)
                s->out_text(sFont, x, y, str, c);
        }

        void LSPFont::draw(ISurface *s, float x, float y, const Color & c, const LSPString *text, size_t first, size_t last)
        {
            const char *str = text->get_utf8(first, last);
            if (str != NULL)
                s->out_text(sFont, x, y, str, c);
        }

    } /* namespace tk */
} /* namespace lsp */
