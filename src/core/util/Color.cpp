/*
 * Color.cpp
 *
 *  Created on: 05 нояб. 2015 г.
 *      Author: sadko
 */

#include <core/util/Color.h>
#include <stdio.h>

namespace lsp
{
    void Color::calc_rgb() const
    {
        if (nMask & M_RGB)
            return;

        if (S > 0.0)
        {
            float temp1, temp2, tempr, tempg, tempb;

            //Set the temporary values
            if  (L < 0.5)
                temp2 = L * (1 + S);
            else
                temp2 = (L + S) - (L * S);

            temp1 = 2 * L - temp2;
            tempr = H + 1.0 / 3.0;
            if (tempr > 1)
                tempr--;

            tempg = H;
            tempb = H - 1.0 / 3.0;

            if (tempb < 0)
                tempb++;

            //Red
            if (tempr < 1.0 / 6.0)
                R = temp1 + (temp2 - temp1) * 6.0 * tempr;
            else if (tempr < 0.5)
                R = temp2;
            else if (tempr < 2.0 / 3.0)
                R = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempr) * 6.0;
            else
                R = temp1;

            //Green
            if (tempg < 1.0 / 6.0)
                G = temp1 + (temp2 - temp1) * 6.0 * tempg;
            else if (tempg < 0.5)
                G = temp2;
            else if (tempg < 2.0 / 3.0)
                G = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempg) * 6.0;
            else
                G = temp1;

            //Blue
            if (tempb < 1.0 / 6.0)
                B = temp1 + (temp2 - temp1) * 6.0 * tempb;
            else if (tempb < 0.5)
                B = temp2;
            else if (tempb < 2.0 / 3.0)
                B = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempb) * 6.0;
            else
                B = temp1;
        }
        else
        {
            R = L;
            G = L;
            B = L;
        }

        nMask |= M_RGB;
    }

    void Color::calc_hsl() const
    {
        if (nMask & M_HSL)
            return;

        float cmax = (R < G) ? ((B < G) ? G : B) : ((B < R) ? R : B);
        float cmin = (R < G) ? ((B < R) ? B : R) : ((B < G) ? B : G);
        float d = cmax - cmin;

        H = 0.0;
        S = 0.0;
        L = 0.5 * (cmax + cmin);

        // Calculate hue
        if (R == cmax)
        {
            H = (G - B) / d;
            if (G < B)
                H += 6.0;
        }
        else if (G == cmax)
            H = (B - R) / d + 2.0;
        else if (B == cmax)
            H = (R - G) / d + 4.0;

        // Calculate saturation
        if (L < 1.0)
            S = d / L;
        else if (L > 1.0)
            S = d / (1.0 - L);

        // Normalize hue
        H  /= 6.0;
        S  *= 0.5;

        nMask |= M_HSL;
    }

    void Color::blend(const Color &c, float alpha)
    {
        float r1, g1, b1, r2, g2, b2;
        get_rgb(r1, g1, b1);
        c.get_rgb(r2, g2, b2);
        set_rgb(r2 + (r1 - r2) * alpha, g2 + (g1 - g2) * alpha, b2 + (b1 - b2) * alpha);
    }

    void Color::blend(float r, float g, float b, float alpha)
    {
        float r1, g1, b1;
        get_rgb(r1, g1, b1);
        set_rgb(r + (r1 - r) * alpha, g + (g1 - g) * alpha, b + (b1 - b) * alpha);
    }

    Color Color::blend(const Color &c1, const Color &c2, float alpha)
    {
        float r1, g1, b1, r2, g2, b2;
        c1.get_rgb(r1, g1, b1);
        c2.get_rgb(r2, g2, b2);

        return Color(r2 + (r1 - r2) * alpha, g2 + (g1 - g2) * alpha, b2 + (b1 - b2) * alpha);
    }

    void Color::darken(float amount)
    {
        float r, g, b;
        get_rgb(r, g, b);

        float a = 1.0 - amount;
        set_rgb(r * a, g * a, b * a);
    }

    void Color::lighten(float amount)
    {
        float r, g, b;
        get_rgb(r, g, b);

        float a = 1.0 - amount;
        set_rgb(r + (1.0 - r) * a, g + (1.0 - g) * a, b + (1.0 - b) * a);
    }

    void Color::copy(const Color &c)
    {
        R       = c.R;
        G       = c.G;
        B       = c.B;
        H       = c.H;
        S       = c.S;
        L       = c.L;
        A       = c.A;
        nMask   = c.nMask & (M_RGB | M_HSL);
    }

    void Color::copy(const Color *c)
    {
        R       = c->R;
        G       = c->G;
        B       = c->B;
        H       = c->H;
        S       = c->S;
        L       = c->L;
        A       = c->A;
        nMask   = c->nMask & (M_RGB | M_HSL);
    }

    int Color::format_rgb(char *dst, size_t len, size_t tolerance) const
    {
        if ((tolerance <= 0) || (tolerance > 4))
            return 0;
        size_t required = (tolerance * 3) + 2; // Number of hex characters x number of colors + 2 symbols
        if (len < required)
            return 0;

        // Calculate maximum value
        const char *fmt;
        size_t tol;

        switch (tolerance)
        {
            case 1:
                fmt = "#%01x%01x%01x";
                tol = 0xf;
                break;
            case 3:
                fmt = "#%03x%03x%03x";
                tol = 0xfff;
                break;
            case 4:
                fmt = "#%04x%04x%04x";
                tol = 0xffff;
                break;
            default:
                fmt = "#%02x%02x%02x";
                tol = 0xff;
                break;
        }

        float r, g, b;
        get_rgb(r, g, b);
        size_t i_r = r * tol, i_g = g * tol, i_b = b * tol;

        return sprintf(dst, fmt, i_r, i_g, i_b);
    }

    uint32_t Color::rgb24() const
    {
        check_rgb();
        return
            (uint32_t(R * 0xff) << 16) |
            (uint32_t(G * 0xff) << 8) |
            (uint32_t(B * 0xff) << 0);
    }

} /* namespace lsp */
