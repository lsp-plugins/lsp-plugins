/*
 * Color.h
 *
 *  Created on: 05 нояб. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_UTIL_COLOR_H_
#define CORE_UTIL_COLOR_H_

#include <core/types.h>

namespace lsp
{
    class Color
    {
        protected:
            enum mask_t
            {
                M_RGB           = 1 << 0,
                M_HSL           = 1 << 1
            };

            mutable float   R, G, B;
            mutable float   H, S, L;
            mutable size_t  nMask;
            mutable float   A;

            void calc_rgb() const;
            void calc_hsl() const;

        protected:
            inline Color(float r, float g, float b, size_t mask): R(r), G(g), B(b), H(0), S(0), L(0), nMask(mask), A(0) {};

            inline void check_rgb() const { if (!(nMask & M_RGB)) { calc_rgb(); nMask |= M_RGB; } };
            inline void check_hsl() const { if (!(nMask & M_HSL)) { calc_hsl(); nMask |= M_HSL; } };

        public:
            inline Color(): R(0), G(0), B(0), H(0), S(0), L(0), nMask(M_RGB), A(0) {};
            inline Color(float r, float g, float b): R(r), G(g), B(b), H(0), S(0), L(0), nMask(M_RGB), A(0) {};
            inline Color(const Color &src): R(src.R), G(src.G), B(src.B), H(src.H), S(src.S), L(src.L), nMask(src.nMask), A(src.A) {};
            inline Color(const Color &src, float a): R(src.R), G(src.G), B(src.B), H(src.H), S(src.S), L(src.L), nMask(src.nMask), A(a) {};
            inline Color(uint32_t rgb): R(float((rgb >> 16) & 0xff)/255.0f), G(float((rgb >> 8) & 0xff)/255.0f), B(float(rgb & 0xff)/255.0f), H(0.0f), S(0.0f), L(0.0f), nMask(M_RGB), A(0) {};
            inline Color(uint32_t rgb, float a): R(float((rgb >> 16) & 0xff)/255.0f), G(float((rgb >> 8) & 0xff)/255.0f), B(float(rgb & 0xff)/255.0f), H(0.0f), S(0.0f), L(0.0f), nMask(M_RGB), A(a) {};

            inline float red() const        { check_rgb(); return R; }
            inline float green() const      { check_rgb(); return G; }
            inline float blue() const       { check_rgb(); return B; }
            inline float alpha() const      { return A;              }

            inline void red(float r)    { check_rgb(); R = r; nMask = M_RGB; };
            inline void green(float g)  { check_rgb(); G = g; nMask = M_RGB; };
            inline void blue(float b)   { check_rgb(); B = b; nMask = M_RGB; };
            inline void alpha(float a)  { A = a; };

            inline void get_rgb(float &r, float &g, float &b) const { check_rgb(); r = R; g = G; b = B; }
            inline void set_rgb(float r, float g, float b)
            {
                nMask = M_RGB;
                R = r;
                G = g;
                B = b;
            }

            inline void set_rgba(float r, float g, float b, float a)
            {
                nMask = M_RGB;
                R = r;
                G = g;
                B = b;
                A = a;
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
            inline void set_hsla(float h, float s, float l, float a)
            {
                nMask   = M_HSL;
                H = h;
                S = s;
                L = l;
                A = a;
            }

            void blend(const Color &c, float alpha);
            void blend(float r, float g, float b, float alpha);
            void darken(float amount);
            void lighten(float amount);
            static Color blend(const Color &c1, const Color &c2, float alpha);

            void copy(const Color &c);
            void copy(const Color *c);

            void copy(const Color &c, float a);
            void copy(const Color *c, float a);

            int format_rgb(char *dst, size_t len, size_t tolerance = 2) const;

            uint32_t    rgb24() const;
    };

} /* namespace lsp */

#endif /* CORE_UTIL_COLOR_H_ */
