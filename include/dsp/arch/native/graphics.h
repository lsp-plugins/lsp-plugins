/*
 * graphics.h
 *
 *  Created on: 07 авг. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_GRAPHICS_H_
#define DSP_ARCH_NATIVE_GRAPHICS_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void axis_apply_log(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float vec    = v[i];
            if (vec < 0.0f)
                vec     = -vec;
            if (vec < AMPLIFICATION_THRESH)
                vec     = AMPLIFICATION_THRESH;
            float k     = logf(vec * zero);
            x[i]       += norm_x * k;
            y[i]       += norm_y * k;
        }
    }

    void rgba32_to_bgra32(void *dst, const void *src, size_t count)
    {
        const uint32_t *s   = reinterpret_cast<const uint32_t *>(src);
        uint32_t *d         = reinterpret_cast<uint32_t *>(dst);

        for (size_t i=0; i<count; ++i)
        {
            uint32_t c      = s[i];
            d[i]            = ((c&0xff0000) >> 16) | ((c&0xff)<<16) | (c&0xff00ff00);
        }
    }

    void fill_rgba(float *dst, float r, float g, float b, float a, size_t count)
    {
        while (count--)
        {
            dst[0]  = r;
            dst[1]  = g;
            dst[2]  = b;
            dst[3]  = a;
            dst    += 4;
        }
    }

    void fill_hsla(float *dst, float h, float s, float l, float a, size_t count)
    {
        while (count--)
        {
            dst[0]  = h;
            dst[1]  = s;
            dst[2]  = l;
            dst[3]  = a;
            dst    += 4;
        }
    }

    void rgba_to_hsla(float *dst, const float *src, size_t count)
    {
        float R, G, B, H, S, L;

        for (size_t i=0; i<count; ++i, dst += 4, src += 4)
        {
            R = src[0];
            G = src[1];
            B = src[2];

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

            // Normalize hue and saturation
            H  /= 6.0;
            S  *= 0.5;

            dst[0]  = H;
            dst[1]  = S;
            dst[2]  = L;
            dst[3]  = src[3];
        }
    }

    void hsla_to_rgba(float *dst, const float *src, size_t count)
    {
        float R, G, B, H, S, L;
        float temp1, temp2, tempr, tempg, tempb;

        for (size_t i=0; i<count; ++i, dst += 4, src += 4)
        {
            H   = src[0];
            S   = src[1];
            L   = src[2];

            if (S <= 0.0f)
            {
                dst[0] = L;
                dst[1] = L;
                dst[2] = L;
                dst[3] = src[3];
                continue;
            }

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

            dst[0]  = R;
            dst[1]  = G;
            dst[2]  = B;
            dst[3]  = src[3];
        }
    }


}

#endif /* DSP_ARCH_NATIVE_GRAPHICS_H_ */
