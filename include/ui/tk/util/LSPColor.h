/*
 * LSPColor.h
 *
 *  Created on: 5 нояб. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_UTIL_LSPCOLOR_H_
#define UI_TK_UTIL_LSPCOLOR_H_

namespace lsp
{
    namespace tk
    {
        class LSPColor
        {
            protected:
                Color       sColor;

            public:
                LSPColor();
                virtual ~LSPColor();

            protected:
                virtual void    color_changed();

            public:
                inline float red() const        { return sColor.red();      }
                inline float green() const      { return sColor.green();    }
                inline float blue() const       { return sColor.blue();     }
                inline float alpha() const      { return sColor.alpha();    }

                inline void red(float r)        { sColor.red(r);    color_changed();    };
                inline void green(float g)      { sColor.green(g);  color_changed();    };
                inline void blue(float b)       { sColor.blue(b);   color_changed();    };
                inline void alpha(float a)      { sColor.alpha(a);  color_changed();    };

                inline void get_rgb(float &r, float &g, float &b) const { sColor.get_rgb(r, g, b); }
                inline void set_rgb(float r, float g, float b) { sColor.set_rgb(r, g, b); color_changed(); }
                inline void set_rgba(float r, float g, float b, float a) { sColor.set_rgba(r, g, b, a); color_changed(); }

                inline float hue() const        { return sColor.hue();          }
                inline float saturation() const { return sColor.saturation();   }
                inline float lightness() const  { return sColor.lightness();    }

                inline void hue(float h)        { sColor.hue(h);        color_changed();    };
                inline void saturation(float s) { sColor.saturation(s); color_changed();    };
                inline void lightness(float l)  { sColor.lightness(l);  color_changed();    };

                inline void get_hsl(float &h, float &s, float &l) const { sColor.get_hsl(h, s, l); }
                inline void set_hsl(float h, float s, float l) { sColor.set_hsl(h, s, l); color_changed(); }
                inline void set_hsla(float h, float s, float l, float a) { sColor.set_hsla(h, s, l, a); color_changed(); }

                void blend(const Color &c, float alpha)     { sColor.blend(c, alpha); color_changed(); }
                void blend(float r, float g, float b, float alpha) { sColor.blend(r, g, b, alpha); color_changed(); }
                void darken(float amount) { sColor.darken(amount); color_changed(); }
                void lighten(float amount) { sColor.lighten(amount); color_changed(); }

                void copy(const Color &c) { sColor.copy(c); color_changed(); }
                void copy(const Color *c) { sColor.copy(c); color_changed(); }

                void copy(const LSPColor &c) { sColor.copy(c.sColor); color_changed(); }
                void copy(const LSPColor *c) { sColor.copy(c->sColor); color_changed(); }

                int format_rgb(char *dst, size_t len, size_t tolerance = 2) const
                {
                    return sColor.format_rgb(dst, len, tolerance);
                }

                uint32_t    rgb24() const { return sColor.rgb24(); }

            public:
                inline const Color *color() { return &sColor; }

                operator const Color &() const { return sColor;  }
                operator const Color *() const { return &sColor; }
        };

    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPCOLOR_H_ */
