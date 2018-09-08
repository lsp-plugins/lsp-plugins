/*
 * convolution.h
 *
 *  Created on: 7 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_CONVOLUTION_H_
#define DSP_ARCH_NATIVE_CONVOLUTION_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void convolve(float *dst, const float *src, const float *conv, size_t length, size_t count)
    {
        // Previous rotate register
        float p[4];
        const float *k  = src;

        while (count >= 4)
        {
            // Clear memory register
            p[0]            = 0.0f;
            p[1]            = 0.0f;
            p[2]            = 0.0f;
            p[3]            = 0.0f;

            size_t clen     = length;
            float *d        = dst;
            const float *c  = conv;

            if (clen >= 4)
            {
                do
                {
                    // Apply convolution
                    d[0]   += k[0]*c[0] + k[1]*p[3] + k[2]*p[2] + k[3]*p[1];
                    d[1]   += k[0]*c[1] + k[1]*c[0] + k[2]*p[3] + k[3]*p[2];
                    d[2]   += k[0]*c[2] + k[1]*c[1] + k[2]*c[0] + k[3]*p[3];
                    d[3]   += k[0]*c[3] + k[1]*c[2] + k[2]*c[1] + k[3]*c[0];

                    // Shift memory
                    p[0]    = c[0];
                    p[1]    = c[1];
                    p[2]    = c[2];
                    p[3]    = c[3];

                    // Decrement current convolution length and increment destination length
                    clen   -= 4;
                    d      += 4;
                    c      += 4;
                } while (clen >= 4);

                // Apply tail
                d[0]   += /*  0  */ + k[1]*p[3] + k[2]*p[2] + k[3]*p[1];
                d[1]   += /*  0  */ + /*  0  */ + k[2]*p[3] + k[3]*p[2];
                d[2]   += /*  0  */ + /*  0  */ + /*  0  */ + k[3]*p[3];
//                d[3]   += /*  0  */ + /*  0  */ + /*  0  */ + /*  0  */;
            }

            // Apply tail
            for (size_t j=0; j<clen; ++j)
            {
                d[0]    += k[0] * c[j];
                d[1]    += k[1] * c[j];
                d[2]    += k[2] * c[j];
                d[3]    += k[3] * c[j];
                d       ++;
            }

            k      += 4;
            dst    += 4;
            count  -= 4;
        }

        while (count--)
        {
            size_t clen     = length;
            float *d        = dst++;
            const float *c  = conv;

            while (clen >= 4)
            {
                // Apply convolution
                d[0]   += k[0]*c[0];
                d[1]   += k[0]*c[1];
                d[2]   += k[0]*c[2];
                d[3]   += k[0]*c[3];

                // Decrement current convolution length and increment destination length
                clen   -= 4;
                d      += 4;
                c      += 4;
            }

            // Apply tail
            while (clen--)
                *(d++)  += k[0] * *(c++);

            k++;
        }
    }
}

#endif /* DSP_ARCH_NATIVE_CONVOLUTION_H_ */
