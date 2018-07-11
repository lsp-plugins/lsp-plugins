/*
 * Color.h
 *
 *  Created on: 05 нояб. 2015 г.
 *      Author: sadko
 */

#ifndef COLOR_H_
#define COLOR_H_

#include <sys/types.h>

namespace lsp
{
    class Theme;

    class Color
    {
        protected:
            static const size_t M_RGB           = (1 << 0);
            static const size_t M_HSL           = (1 << 1);
            static const size_t M_MASK_BITS     = 2;

            mutable float   R, G, B;
            mutable float   H, S, L;
            mutable size_t  nMask;

            void calc_rgb() const;
            void calc_hsl() const;

        protected:
            inline Color(float r, float g, float b, size_t mask): R(r), G(g), B(b), H(0), S(0), L(0), nMask(mask) {};

            inline void check_rgb() const { if (!(nMask & M_RGB)) { calc_rgb(); nMask |= M_RGB; } };
            inline void check_hsl() const { if (!(nMask & M_HSL)) { calc_hsl(); nMask |= M_HSL; } };

        public:
            inline Color(): R(0), G(0), B(0), H(0), S(0), L(0), nMask(M_RGB) {};
            inline Color(float r, float g, float b): R(r), G(g), B(b), H(0), S(0), L(0), nMask(M_RGB) {};
            inline Color(const Color &src): R(src.R), G(src.G), B(src.B), H(src.H), S(src.S), L(src.L), nMask(src.nMask) {};
            inline Color(Theme &theme, const char *name) { set(theme, name); }
            inline Color(Theme &theme, color_t color) { set(theme, color); }

            inline float red() const        { check_rgb(); return R; }
            inline float green() const      { check_rgb(); return G; }
            inline float blue() const       { check_rgb(); return B; }

            inline void red(float r)    { check_rgb(); R = r; nMask = M_RGB; };
            inline void green(float g)  { check_rgb(); G = g; nMask = M_RGB; };
            inline void blue(float b)   { check_rgb(); B = b; nMask = M_RGB; };

            inline void get_rgb(float &r, float &g, float &b) const { check_rgb(); r = R; g = G; b = B; }
            inline void set_rgb(float r, float g, float b)
            {
                nMask = M_RGB;
                R = r;
                G = g;
                B = b;
            }

            inline float hue() const        { check_hsl(); return H; }
            inline float saturation() const { check_hsl(); return S; }
            inline float lightness() const  { check_hsl(); return L; }

            inline void hue(float h)        { check_hsl(); H = h; nMask = M_HSL;  };
            inline void saturation(float s) { check_hsl(); S = s; nMask = M_HSL;  };
            inline void lightness(float l)  { check_hsl(); L = l; nMask = M_HSL;  };

            inline void get_hsl(float &h, float &s, float &l) const { check_hsl(); h = H; s = S; l = L; }
            inline void set_hsl(float h, float s, float l)
            {
                nMask   = M_HSL;
                H = h;
                S = s;
                L = l;
            }

            void blend(const Color &c, float alpha);
            void blend(float r, float g, float b, float alpha);
            void darken(float amount);
            void lighten(float amount);
            static Color blend(const Color &c1, const Color &c2, float alpha);

            void copy(const Color &c);
            bool set(Theme &theme, const char *name);
            bool set(Theme &theme, color_t color);

            int format_rgb(char *dst, size_t len, size_t tolerance = 2);

            uint32_t    rgb24() const;
    };

    class ColorHolder
    {
        private:
            enum component_t
            {
                C_BASIC, C_R, C_G, C_B, C_H, C_S, C_L,
                C_TOTAL
            };

        private:
            IWidget    *pWidget;
            Color       sColor;
            IUIPort    *vComponents[C_TOTAL];
            size_t      vAttributes[C_TOTAL];

        public:
            ColorHolder();

            void init(IWidget *widget, color_t dfl, size_t basic, size_t r, size_t g, size_t b, size_t h, size_t s, size_t l);

            bool notify(IUIPort *port);

            bool set(widget_attribute_t att, const char *value);

            inline Color &color()               { return sColor; }

            inline const Color &color() const   { return sColor; }
    };

} /* namespace lsp */

#endif /* COLOR_H_ */
