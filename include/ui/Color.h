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

        public:
            inline Color(): R(0), G(0), B(0), H(0), S(0), L(0), nMask(M_RGB) {};
            inline Color(float r, float g, float b): R(r), G(g), B(b), H(0), S(0), L(0), nMask(M_RGB) {};
            inline Color(const Color &src): R(src.R), G(src.G), B(src.B), H(src.H), S(src.S), L(src.L), nMask(src.nMask) {};
            inline Color(Theme &theme, const char *name) { set(theme, name); }
            inline Color(Theme &theme, color_t color) { set(theme, color); }

            inline float red() const        { calc_rgb(); return R; }
            inline float green() const      { calc_rgb(); return G; }
            inline float blue() const       { calc_rgb(); return B; }

            inline void red(float r)    { nMask  = M_RGB; R = r; };
            inline void green(float g)  { nMask  = M_RGB; G = g; };
            inline void blue(float b)   { nMask  = M_RGB; B = b; };

            inline void get_rgb(float &r, float &g, float &b) const { calc_rgb(); r = R; g = G; b = B; }
            inline void set_rgb(float r, float g, float b)
            {
                nMask = M_RGB;
                R = r;
                G = g;
                B = b;
            }

            inline float hue() const        { calc_hsl(); return H; }
            inline float saturation() const { calc_hsl(); return S; }
            inline float lightness() const  { calc_hsl(); return L; }

            inline void hue(float h)        { nMask  = M_HSL; H = h; };
            inline void saturation(float s) { nMask  = M_HSL; S = s; };
            inline void lightness(float l)  { nMask  = M_HSL; L = l; };

            inline void get_hsl(float &h, float &s, float &l) const { calc_hsl(); h = H; s = S; l = L; }
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
    };

} /* namespace lsp */

#endif /* COLOR_H_ */
