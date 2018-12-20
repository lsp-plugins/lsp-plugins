/*
 * dynamic.h
 *
 *  Created on: 12 февр. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_FILTERS_DYNAMIC_H_
#define DSP_ARCH_NATIVE_FILTERS_DYNAMIC_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f)
    {
        while (count--)
        {
            float s     = *(src++);
            //   a: a0 a0 a1 a2
            //   b: b1 b2 0  0
            float s2    = f->a[0]*s + d[0];
            float p1    = f->a[2]*s + f->b[0]*s2;
            float p2    = f->a[3]*s + f->b[1]*s2;

            // Shift buffer
            d[0]        = d[1] + p1;
            d[1]        = p2;

            // Store result
            *(dst++)    = s2;
            f++;
        }
    }

    void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f)
    {
        if (count <= 0)
            return;

        float s, r, s2, r2, p1, q1, p2, q2;

        //   a: a0 a0 a1 a2 i0 i0 i1 i2
        //   b: b1 b2 0  0  j1 j2 0  0

        // First filter only
        s           = *(src++);
        s2          = f->a[0]*s + d[0];
        p1          = f->a[2]*s + f->b[0]*s2;
        p2          = f->a[3]*s + f->b[1]*s2;
        r           = s2;
        d[0]        = d[1] + p1;
        d[1]        = p2;
        f           ++;

        // Both filters
        for (size_t i=1; i<count; ++i)
        {
            s           = *(src++);
            r2          = f->a[4]*r + d[2];
            s2          = f->a[0]*s + d[0];

            q1          = f->a[6]*r + f->b[4]*r2;
            p1          = f->a[2]*s + f->b[0]*s2;
            q2          = f->a[7]*r + f->b[5]*r2;
            p2          = f->a[3]*s + f->b[1]*s2;

            r           = s2;
            *(dst++)    = r2;

            // Shift buffers
            d[2]        = d[3] + q1;
            d[0]        = d[1] + p1;
            d[3]        = q2;
            d[1]        = p2;

            f           ++;
        }

        // Second filter only
        r2          = f->a[4]*r + d[2];
        q1          = f->a[6]*r + f->b[4]*r2;
        q2          = f->a[7]*r + f->b[5]*r2;
        *dst        = r2;
        d[2]        = d[3] + q1;
        d[3]        = q2;
    }

    void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f)
    {
        if (count <= 0)
            return;

        float s[4], s2[4], p1[4], p2[4];
        s[0]        = 0.0f;
        s[1]        = 0.0f;
        s[2]        = 0.0f;
        s[3]        = 0.0f;
        s2[0]       = 0.0f;
        s2[1]       = 0.0f;
        s2[2]       = 0.0f;
        s2[3]       = 0.0f;

        size_t mask = 1;

        // Start filters, mask enables the specific filter
        do
        {
            // Push sample
            s[0]        = *(src++);

            // Calculate filters by mask and shift buffers
            s2[0]       = f->a0[0]*s[0] + d[0];
            p1[0]       = f->a1[0]*s[0] + f->b1[0]*s2[0];
            p2[0]       = f->a2[0]*s[0] + f->b2[0]*s2[0];
            d[0]        = d[4] + p1[0];
            d[4]        = p2[0];

            if (mask & 0x2)
            {
                s2[1]       = f->a0[1]*s[1] + d[1];
                p1[1]       = f->a1[1]*s[1] + f->b1[1]*s2[1];
                p2[1]       = f->a2[1]*s[1] + f->b2[1]*s2[1];
                d[1]        = d[5] + p1[1];
                d[5]        = p2[1];
            }
            if (mask & 0x4)
            {
                s2[2]       = f->a0[2]*s[2] + d[2];
                p1[2]       = f->a1[2]*s[2] + f->b1[2]*s2[2];
                p2[2]       = f->a2[2]*s[2] + f->b2[2]*s2[2];
                d[2]        = d[6] + p1[2];
                d[6]        = p2[2];
            }
            f           ++;

            // Shift buffer
            s[3]        = s2[2];
            s[2]        = s2[1];
            s[1]        = s2[0];
            if ((--count) <= 0)
                break;
            mask        = (mask << 1) | 1;
        } while (mask != 0x0f);

        // Process all filters simultaneously, mask = 0x0f
        for ( ; count > 0; --count)
        {
            // Push sample
            s[0]        = *(src++);

            // Calculate filters by mask and shift buffers
            s2[0]       = f->a0[0]*s[0] + d[0];
            s2[1]       = f->a0[1]*s[1] + d[1];
            s2[2]       = f->a0[2]*s[2] + d[2];
            s2[3]       = f->a0[3]*s[3] + d[3];

            p1[0]       = f->a1[0]*s[0] + f->b1[0]*s2[0];
            p1[1]       = f->a1[1]*s[1] + f->b1[1]*s2[1];
            p1[2]       = f->a1[2]*s[2] + f->b1[2]*s2[2];
            p1[3]       = f->a1[3]*s[3] + f->b1[3]*s2[3];

            p2[0]       = f->a2[0]*s[0] + f->b2[0]*s2[0];
            p2[1]       = f->a2[1]*s[1] + f->b2[1]*s2[1];
            p2[2]       = f->a2[2]*s[2] + f->b2[2]*s2[2];
            p2[3]       = f->a2[3]*s[3] + f->b2[3]*s2[3];

            d[0]        = d[4] + p1[0];
            d[1]        = d[5] + p1[1];
            d[2]        = d[6] + p1[2];
            d[3]        = d[7] + p1[3];

            d[4]        = p2[0];
            d[5]        = p2[1];
            d[6]        = p2[2];
            d[7]        = p2[3];
            f           ++;

            // Shift buffer
            *(dst++)    = s2[3];
            s[3]        = s2[2];
            s[2]        = s2[1];
            s[1]        = s2[0];
        }

        // Finish processing
        mask      <<= 1;
        do
        {
            // Calculate filters by mask and shift buffers
            if (mask & 0x2)
            {
                s2[1]       = f->a0[1]*s[1] + d[1];
                p1[1]       = f->a1[1]*s[1] + f->b1[1]*s2[1];
                p2[1]       = f->a2[1]*s[1] + f->b2[1]*s2[1];
                d[1]        = d[5] + p1[1];
                d[5]        = p2[1];
            }
            if (mask & 0x4)
            {
                s2[2]       = f->a0[2]*s[2] + d[2];
                p1[2]       = f->a1[2]*s[2] + f->b1[2]*s2[2];
                p2[2]       = f->a2[2]*s[2] + f->b2[2]*s2[2];
                d[2]        = d[6] + p1[2];
                d[6]        = p2[2];
            }
            if (mask & 0x08)
            {
                s2[3]       = f->a0[3]*s[3] + d[3];
                p1[3]       = f->a1[3]*s[3] + f->b1[3]*s2[3];
                p2[3]       = f->a2[3]*s[3] + f->b2[3]*s2[3];
                d[3]        = d[7] + p1[3];
                d[7]        = p2[3];

                *(dst++)    = s2[3];
            }
            f           ++;

            // Shift buffer
            s[3]        = s2[2];
            s[2]        = s2[1];
            s[1]        = s2[0];

            // Update mask
            mask      <<= 1;
        } while (mask & 0x0f);
    }

    void dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f)
    {
        // This code already works badly instead of biquad_process_x4
        if (count <= 0)
            return;

        float s[4], s2[4], p1[4], p2[4];
        s[0]            = 0.0f;
        s[1]            = 0.0f;
        s[2]            = 0.0f;
        s[3]            = 0.0f;
        s2[0]           = 0.0f;
        s2[1]           = 0.0f;
        s2[2]           = 0.0f;
        s2[3]           = 0.0f;

        const float *sp = src;

        // Calculate as two passes of x4 filters
        for (size_t n=0; n<=4; n +=4)
        {
            // two x4 filters are in parallel, shift by 4 floats stride
            const biquad_x8_t *bq   = reinterpret_cast<const biquad_x8_t *>(&f[n].a0[n]);
            size_t mask             = 1;
            size_t i                = 0;
            float *dp               = dst;

            // Start filters, mask enables the specific filter
            do
            {
                // Push sample
                s[0]        = *(sp++);
                mask       |= 1;

                // Calculate filters by mask and shift buffers
                s2[0]       = bq->a0[0]*s[0] + d[0];
                p1[0]       = bq->a1[0]*s[0] + bq->b1[0]*s2[0];
                p2[0]       = bq->a2[0]*s[0] + bq->b2[0]*s2[0];
                d[0]        = d[8]   + p1[0];
                d[8]        = p2[0];

                if (mask & 0x2)
                {
                    s2[1]       = bq->a0[1]*s[1] + d[1];
                    p1[1]       = bq->a1[1]*s[1] + bq->b1[1]*s2[1];
                    p2[1]       = bq->a2[1]*s[1] + bq->b2[1]*s2[1];
                    d[1]        = d[9]   + p1[1];
                    d[9]        = p2[1];
                }
                if (mask & 0x4)
                {
                    s2[2]       = bq->a0[2]*s[2] + d[2];
                    p1[2]       = bq->a1[2]*s[2] + bq->b1[2]*s2[2];
                    p2[2]       = bq->a2[2]*s[2] + bq->b2[2]*s2[2];
                    d[2]        = d[10]  + p1[2];
                    d[10]       = p2[2];
                }
                bq          ++;

                // Shift buffer
                s[3]        = s2[2];
                s[2]        = s2[1];
                s[1]        = s2[0];

                // Update mask
                if ((++i) >= count)
                    break;
                mask        = (mask << 1) | 1;
            } while (mask != 0x0f);

            // Process all filters simultaneously
            for ( ; i < count; ++i)
            {
                // Push sample
                s[0]        = *(sp++);

                // Calculate filters by mask and shift buffers
                s2[0]       = bq->a0[0]*s[0] + d[0];
                s2[1]       = bq->a0[1]*s[1] + d[1];
                s2[2]       = bq->a0[2]*s[2] + d[2];
                s2[3]       = bq->a0[3]*s[3] + d[3];

                p1[0]       = bq->a1[0]*s[0] + bq->b1[0]*s2[0];
                p1[1]       = bq->a1[1]*s[1] + bq->b1[1]*s2[1];
                p1[2]       = bq->a1[2]*s[2] + bq->b1[2]*s2[2];
                p1[3]       = bq->a1[3]*s[3] + bq->b1[3]*s2[3];

                p2[0]       = bq->a2[0]*s[0] + bq->b2[0]*s2[0];
                p2[1]       = bq->a2[1]*s[1] + bq->b2[1]*s2[1];
                p2[2]       = bq->a2[2]*s[2] + bq->b2[2]*s2[2];
                p2[3]       = bq->a2[3]*s[3] + bq->b2[3]*s2[3];

                d[0]        = d[8]   + p1[0];
                d[1]        = d[9]   + p1[1];
                d[2]        = d[10]  + p1[2];
                d[3]        = d[11]  + p1[3];

                d[8]        = p2[0];
                d[9]        = p2[1];
                d[10]       = p2[2];
                d[11]       = p2[3];
                bq          ++;

                // Shift buffer
                *(dp++)     = s2[3];
                s[3]        = s2[2];
                s[2]        = s2[1];
                s[1]        = s2[0];
            }

            // Finish processing
            mask      <<= 1;
            do
            {
                // Calculate filters by mask and shift buffers
                if (mask & 0x2)
                {
                    s2[1]       = bq->a0[1]*s[1] + d[1];
                    p1[1]       = bq->a1[1]*s[1] + bq->b1[1]*s2[1];
                    p2[1]       = bq->a2[1]*s[1] + bq->b2[1]*s2[1];
                    d[1]        = d[9]   + p1[1];
                    d[9]        = p2[1];
                }
                if (mask & 0x4)
                {
                    s2[2]       = bq->a0[2]*s[2] + d[2];
                    p1[2]       = bq->a1[2]*s[2] + bq->b1[2]*s2[2];
                    p2[2]       = bq->a2[2]*s[2] + bq->b2[2]*s2[2];
                    d[2]        = d[10]  + p1[2];
                    d[10]       = p2[2];
                }
                if (mask & 0x08)
                {
                    s2[3]       = bq->a0[3]*s[3] + d[3];
                    p1[3]       = bq->a1[3]*s[3] + bq->b1[3]*s2[3];
                    p2[3]       = bq->a2[3]*s[3] + bq->b2[3]*s2[3];
                    d[3]        = d[11]  + p1[3];
                    d[11]       = p2[3];

                    *(dp++)     = s2[3];
                }
                bq          ++;

                // Shift buffer
                s[3]        = s2[2];
                s[2]        = s2[1];
                s[1]        = s2[0];

                // Update mask
                mask      <<= 1;
            } while (mask & 0x0f);

            // Now all data is in the destination buffer
            sp          = dst; // Now all source data is in destination buffer
            d          += 4;   // Shift memory pointer by 4 floats
        }
    }
}

#endif /* DSP_ARCH_NATIVE_FILTERS_DYNAMIC_H_ */
