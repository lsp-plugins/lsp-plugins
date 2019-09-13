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
        class LSPWidget;

        class LSPColor
        {
            protected:
                Color       sColor;
                LSPWidget  *pWidget;

            public:
                explicit LSPColor();
                explicit LSPColor(LSPWidget *widget);
                virtual ~LSPColor();

            protected:
                virtual void    color_changed();
                void            trigger_change();

            public:
                inline float red() const        { return sColor.red();      }
                inline float green() const      { return sColor.green();    }
                inline float blue() const       { return sColor.blue();     }
                inline float alpha() const      { return sColor.alpha();    }

                inline void red(float r)        { sColor.red(r);    trigger_change();    };
                inline void green(float g)      { sColor.green(g);  trigger_change();    };
                inline void blue(float b)       { sColor.blue(b);   trigger_change();    };
                inline void alpha(float a)      { sColor.alpha(a);  trigger_change();    };

                inline void get_rgb(float &r, float &g, float &b) const { sColor.get_rgb(r, g, b); }
                inline void set_rgb(float r, float g, float b) { sColor.set_rgb(r, g, b); trigger_change(); }
                inline void set_rgba(float r, float g, float b, float a) { sColor.set_rgba(r, g, b, a); trigger_change(); }

                inline float hue() const        { return sColor.hue();          }
                inline float saturation() const { return sColor.saturation();   }
                inline float lightness() const  { return sColor.lightness();    }

                inline void hue(float h)        { sColor.hue(h);        trigger_change();    };
                inline void saturation(float s) { sColor.saturation(s); trigger_change();    };
                inline void lightness(float l)  { sColor.lightness(l);  trigger_change();    };

                inline void get_hsl(float &h, float &s, float &l) const { sColor.get_hsl(h, s, l); }
                inline void set_hsl(float h, float s, float l) { sColor.set_hsl(h, s, l); trigger_change(); }
                inline void set_hsla(float h, float s, float l, float a) { sColor.set_hsla(h, s, l, a); trigger_change(); }

                void blend(const Color &c, float alpha)     { sColor.blend(c, alpha); trigger_change(); }
                void blend(float r, float g, float b, float alpha) { sColor.blend(r, g, b, alpha); trigger_change(); }
                void darken(float amount) { sColor.darken(amount); trigger_change(); }
                void lighten(float amount) { sColor.lighten(amount); trigger_change(); }

                void copy(const Color &c) { sColor.copy(c); trigger_change(); }
                void copy(const Color *c) { sColor.copy(c); trigger_change(); }

                void copy(const LSPColor &c) { sColor.copy(c.sColor); trigger_change(); }
                void copy(const LSPColor *c) { sColor.copy(c->sColor); trigger_change(); }

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
