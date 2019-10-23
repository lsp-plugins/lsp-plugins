/*
 * LSPColor.h
 *
 *  Created on: 5 нояб. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_SYS_LSPCOLOR_H_
#define UI_TK_SYS_LSPCOLOR_H_

namespace lsp
{
    namespace tk
    {
        class LSPWidget;
        class LSPStyle;

        class LSPColor
        {
            protected:
                class Listener: public IStyleListener
                {
                    private:
                        LSPColor   *pColor;
                        LSPStyle   *pStyle;
                        ui_atom_t   aR, aG, aB, aRGB;
                        ui_atom_t   aH, aS, aL, aHSL;
                        ui_atom_t   aA, aRGBA, aHSLA;

                    public:
                        explicit Listener(LSPColor *color);
                        virtual ~Listener();

                    public:
                        virtual void    notify(ui_atom_t property);
                        void            sync();
                        void            unbind();
                        void            reset();
                        status_t        bind(LSPDisplay *dpy, LSPStyle *style, const char *property);
                };

            protected:
                Color       sColor;
                LSPWidget  *pWidget;
                Listener    sListener;

            public:
                explicit LSPColor();
                explicit LSPColor(LSPWidget *widget);
                virtual ~LSPColor();

            public:
                status_t    bind(const char *property);
                status_t    bind(LSPStyle *style, const char *property);
                status_t    bind(LSPDisplay *dpy, LSPStyle *style, const char *property);
                inline void unbind() { sListener.unbind(); };

            public:
                void        set_default();

            protected:
                virtual void    color_changed();
                void            trigger_change();

            public:
                inline float red() const        { return sColor.red();      }
                inline float green() const      { return sColor.green();    }
                inline float blue() const       { return sColor.blue();     }
                inline float alpha() const      { return sColor.alpha();    }

                inline void get_rgb(float &r, float &g, float &b) const { sColor.get_rgb(r, g, b); }
                inline void get_rgba(float &r, float &g, float &b, float &a) const { sColor.get_rgba(r, g, b, a); }

                inline float hue() const        { return sColor.hue();          }
                inline float saturation() const { return sColor.saturation();   }
                inline float lightness() const  { return sColor.lightness();    }

                inline void get_hsl(float &h, float &s, float &l) const { sColor.get_hsl(h, s, l); }
                inline void get_hsla(float &h, float &s, float &l, float &a) const { sColor.get_hsla(h, s, l, a); }

            public:
                void red(float r);
                void green(float g);
                void blue(float b);
                void alpha(float a);

                void set_rgb(float r, float g, float b);
                void set_rgba(float r, float g, float b, float a);

                void hue(float h);
                void saturation(float s);
                void lightness(float l);

                void set_hsl(float h, float s, float l);
                void set_hsla(float h, float s, float l, float a);

            public:
                void blend(const Color &c, float alpha)     { sColor.blend(c, alpha); trigger_change(); }
                void blend(float r, float g, float b, float alpha) { sColor.blend(r, g, b, alpha); trigger_change(); }
                void darken(float amount) { sColor.darken(amount); trigger_change(); }
                void lighten(float amount) { sColor.lighten(amount); trigger_change(); }

                void copy(const Color &c) { sColor.copy(c); trigger_change(); }
                void copy(const Color *c) { sColor.copy(c); trigger_change(); }

                void copy(const LSPColor &c) { sColor.copy(c.sColor); trigger_change(); }
                void copy(const LSPColor *c) { sColor.copy(c->sColor); trigger_change(); }

                inline int format_rgb(char *dst, size_t len, size_t tolerance = 2) const
                {
                    return sColor.format_rgb(dst, len, tolerance);
                }

                inline int format_rgba(char *dst, size_t len, size_t tolerance = 2) const
                {
                    return sColor.format_rgba(dst, len, tolerance);
                }

                inline int format_hsl(char *dst, size_t len, size_t tolerance = 2) const
                {
                    return sColor.format_hsl(dst, len, tolerance);
                }

                inline int format_hsla(char *dst, size_t len, size_t tolerance = 2) const
                {
                    return sColor.format_hsla(dst, len, tolerance);
                }

                inline uint32_t rgb24() const { return sColor.rgb24(); }

            public:
                inline const Color *color() const { return &sColor; }

//                operator const Color &() const { return sColor;  }
                operator const Color *() const { return &sColor; }
        };

    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_SYS_LSPCOLOR_H_ */
