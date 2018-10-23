/*
 * transform.h
 *
 *  Created on: 13 февр. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_FILTERS_TRANSFORM_H_
#define DSP_ARCH_NATIVE_FILTERS_TRANSFORM_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void bilinear_transform_x1(biquad_x1_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        if (count <= 0)
            return;

        float T[4], B[4], N;
        float kf2       = kf * kf;

        while (count--)
        {
            // Calculate top coefficients
            T[0]            = bc->t[0];
            T[1]            = bc->t[1]*kf;
            T[2]            = bc->t[2]*kf2;

            // Calculate bottom coefficients
            B[0]            = bc->b[0];
            B[1]            = bc->b[1]*kf;
            B[2]            = bc->b[2]*kf2;

            // Calculate the convolution
            N               = 1.0 / (B[0] + B[1] + B[2]);

            // Initialize filter parameters
            bf->a[0]        = (T[0] + T[1] + T[2]) * N;
            bf->a[1]        = bf->a[0];
            bf->a[2]        = 2.0 * (T[0] - T[2]) * N;
            bf->a[3]        = (T[0] - T[1] + T[2]) * N;

            bf->b[0]        = 2.0 * (B[2] - B[0]) * N;  // Sign negated
            bf->b[1]        = (B[1] - B[2] - B[0]) * N; // Sign negated
            bf->b[2]        = 0.0f;
            bf->b[3]        = 0.0f;

            // Increment pointers
            bc              ++;
            bf              ++;
        }
    }

    void bilinear_transform_x2(biquad_x2_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        if (count <= 0)
            return;

        float T[8], B[8], N[2];
        float kf2       = kf * kf;

        while (count--)
        {
            // Calculate top coefficients
            T[0]            = bc[0].t[0];
            T[4]            = bc[1].t[0];

            T[1]            = bc[0].t[1]*kf;
            T[5]            = bc[1].t[1]*kf;

            T[2]            = bc[0].t[2]*kf2;
            T[6]            = bc[1].t[2]*kf2;

            // Calculate bottom coefficients
            B[0]            = bc[0].b[0];
            B[4]            = bc[1].b[0];

            B[1]            = bc[0].b[1]*kf;
            B[5]            = bc[1].b[1]*kf;

            B[2]            = bc[0].b[2]*kf2;
            B[6]            = bc[1].b[2]*kf2;

            // Calculate the convolution
            N[0]            = 1.0 / (B[0] + B[1] + B[2]);
            N[1]            = 1.0 / (B[4] + B[5] + B[6]);

            // Initialize filter top coefficients
            bf->a[0]        = (T[0] + T[1] + T[2]) * N[0];
            bf->a[4]        = (T[4] + T[5] + T[6]) * N[1];

            bf->a[1]        = bf->a[0];
            bf->a[5]        = bf->a[4];

            bf->a[2]        = 2.0 * (T[0] - T[2]) * N[0];
            bf->a[6]        = 2.0 * (T[4] - T[6]) * N[1];

            bf->a[3]        = (T[0] - T[1] + T[2]) * N[0];
            bf->a[7]        = (T[4] - T[5] + T[6]) * N[1];

            // Initialize filter bottom coefficients
            bf->b[0]        = 2.0 * (B[2] - B[0]) * N[0];  // Sign negated
            bf->b[4]        = 2.0 * (B[6] - B[4]) * N[1];  // Sign negated

            bf->b[1]        = (B[1] - B[2] - B[0]) * N[0]; // Sign negated
            bf->b[5]        = (B[5] - B[6] - B[4]) * N[1]; // Sign negated

            bf->b[2]        = 0.0f;
            bf->b[6]        = 0.0f;

            bf->b[3]        = 0.0f;
            bf->b[7]        = 0.0f;

            // Increment pointers
            bc             += 2;
            bf              ++;
        }
    }

    void bilinear_transform_x4(biquad_x4_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        if (count <= 0)
            return;

        float T0[4], T1[4], T2[4];
        float B0[4], B1[4], B2[4], N[4];
        float kf2       = kf * kf;

        while (count--)
        {
            // Calculate top coefficients
            T0[0]           = bc[0].t[0];
            T0[1]           = bc[1].t[0];
            T0[2]           = bc[2].t[0];
            T0[3]           = bc[3].t[0];

            T1[0]           = bc[0].t[1]*kf;
            T1[1]           = bc[1].t[1]*kf;
            T1[2]           = bc[2].t[1]*kf;
            T1[3]           = bc[3].t[1]*kf;

            T2[0]           = bc[0].t[2]*kf2;
            T2[1]           = bc[1].t[2]*kf2;
            T2[2]           = bc[2].t[2]*kf2;
            T2[3]           = bc[3].t[2]*kf2;

            // Calculate bottom coefficients
            B0[0]           = bc[0].b[0];
            B0[1]           = bc[1].b[0];
            B0[2]           = bc[2].b[0];
            B0[3]           = bc[3].b[0];

            B1[0]           = bc[0].b[1]*kf;
            B1[1]           = bc[1].b[1]*kf;
            B1[2]           = bc[2].b[1]*kf;
            B1[3]           = bc[3].b[1]*kf;

            B2[0]           = bc[0].b[2]*kf2;
            B2[1]           = bc[1].b[2]*kf2;
            B2[2]           = bc[2].b[2]*kf2;
            B2[3]           = bc[3].b[2]*kf2;

            // Calculate the convolution
            N[0]            = 1.0 / (B0[0] + B1[0] + B2[0]);
            N[1]            = 1.0 / (B0[1] + B1[1] + B2[1]);
            N[2]            = 1.0 / (B0[2] + B1[2] + B2[2]);
            N[3]            = 1.0 / (B0[3] + B1[3] + B2[3]);

            // Initialize filter parameters
            bf->a0[0]       = (T0[0] + T1[0] + T2[0]) * N[0];
            bf->a0[1]       = (T0[1] + T1[1] + T2[1]) * N[1];
            bf->a0[2]       = (T0[2] + T1[2] + T2[2]) * N[2];
            bf->a0[3]       = (T0[3] + T1[3] + T2[3]) * N[3];

            bf->a1[0]       = 2.0 * (T0[0] - T2[0]) * N[0];
            bf->a1[1]       = 2.0 * (T0[1] - T2[1]) * N[1];
            bf->a1[2]       = 2.0 * (T0[2] - T2[2]) * N[2];
            bf->a1[3]       = 2.0 * (T0[3] - T2[3]) * N[3];

            bf->a2[0]       = (T0[0] - T1[0] + T2[0]) * N[0];
            bf->a2[1]       = (T0[1] - T1[1] + T2[1]) * N[1];
            bf->a2[2]       = (T0[2] - T1[2] + T2[2]) * N[2];
            bf->a2[3]       = (T0[3] - T1[3] + T2[3]) * N[3];

            bf->b1[0]       = 2.0 * (B2[0] - B0[0]) * N[0];     // Sign negated
            bf->b1[1]       = 2.0 * (B2[1] - B0[1]) * N[1];     // Sign negated
            bf->b1[2]       = 2.0 * (B2[2] - B0[2]) * N[2];     // Sign negated
            bf->b1[3]       = 2.0 * (B2[3] - B0[3]) * N[3];     // Sign negated

            bf->b2[0]       = (B1[0] - B2[0] - B0[0]) * N[0];   // Sign negated
            bf->b2[1]       = (B1[1] - B2[1] - B0[1]) * N[1];   // Sign negated
            bf->b2[2]       = (B1[2] - B2[2] - B0[2]) * N[2];   // Sign negated
            bf->b2[3]       = (B1[3] - B2[3] - B0[3]) * N[3];   // Sign negated

            // Increment pointers
            bc             += 4;
            bf              ++;
        }
    }

    void bilinear_transform_x8(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        if (count <= 0)
            return;

        float T0[4], T1[4], T2[4];
        float B0[4], B1[4], B2[4], N[4];
        float kf2       = kf * kf;

        // Calculate as two passes of x4 filters
        for (size_t n=0; n<=4; n+=4)
        {
            // two x4 filters are in parallel, shift by 4 floats stride
            biquad_x8_t *bq         = reinterpret_cast<biquad_x8_t *>(&bf->a0[n]);
            const f_cascade_t *fc   = &bc[n];

            for (size_t i=0; i<count; ++i)
            {
                // Calculate top coefficients
                T0[0]           = fc[0].t[0];
                T0[1]           = fc[1].t[0];
                T0[2]           = fc[2].t[0];
                T0[3]           = fc[3].t[0];

                T1[0]           = fc[0].t[1]*kf;
                T1[1]           = fc[1].t[1]*kf;
                T1[2]           = fc[2].t[1]*kf;
                T1[3]           = fc[3].t[1]*kf;

                T2[0]           = fc[0].t[2]*kf2;
                T2[1]           = fc[1].t[2]*kf2;
                T2[2]           = fc[2].t[2]*kf2;
                T2[3]           = fc[3].t[2]*kf2;

                // Calculate bottom coefficients
                B0[0]           = fc[0].b[0];
                B0[1]           = fc[1].b[0];
                B0[2]           = fc[2].b[0];
                B0[3]           = fc[3].b[0];

                B1[0]           = fc[0].b[1]*kf;
                B1[1]           = fc[1].b[1]*kf;
                B1[2]           = fc[2].b[1]*kf;
                B1[3]           = fc[3].b[1]*kf;

                B2[0]           = fc[0].b[2]*kf2;
                B2[1]           = fc[1].b[2]*kf2;
                B2[2]           = fc[2].b[2]*kf2;
                B2[3]           = fc[3].b[2]*kf2;

                // Calculate the convolution
                N[0]            = 1.0 / (B0[0] + B1[0] + B2[0]);
                N[1]            = 1.0 / (B0[1] + B1[1] + B2[1]);
                N[2]            = 1.0 / (B0[2] + B1[2] + B2[2]);
                N[3]            = 1.0 / (B0[3] + B1[3] + B2[3]);

                // Initialize filter parameters
                bq->a0[0]       = (T0[0] + T1[0] + T2[0]) * N[0];
                bq->a0[1]       = (T0[1] + T1[1] + T2[1]) * N[1];
                bq->a0[2]       = (T0[2] + T1[2] + T2[2]) * N[2];
                bq->a0[3]       = (T0[3] + T1[3] + T2[3]) * N[3];

                bq->a1[0]       = 2.0 * (T0[0] - T2[0]) * N[0];
                bq->a1[1]       = 2.0 * (T0[1] - T2[1]) * N[1];
                bq->a1[2]       = 2.0 * (T0[2] - T2[2]) * N[2];
                bq->a1[3]       = 2.0 * (T0[3] - T2[3]) * N[3];

                bq->a2[0]       = (T0[0] - T1[0] + T2[0]) * N[0];
                bq->a2[1]       = (T0[1] - T1[1] + T2[1]) * N[1];
                bq->a2[2]       = (T0[2] - T1[2] + T2[2]) * N[2];
                bq->a2[3]       = (T0[3] - T1[3] + T2[3]) * N[3];

                bq->b1[0]       = 2.0 * (B2[0] - B0[0]) * N[0];     // Sign negated
                bq->b1[1]       = 2.0 * (B2[1] - B0[1]) * N[1];     // Sign negated
                bq->b1[2]       = 2.0 * (B2[2] - B0[2]) * N[2];     // Sign negated
                bq->b1[3]       = 2.0 * (B2[3] - B0[3]) * N[3];     // Sign negated

                bq->b2[0]       = (B1[0] - B2[0] - B0[0]) * N[0];   // Sign negated
                bq->b2[1]       = (B1[1] - B2[1] - B0[1]) * N[1];   // Sign negated
                bq->b2[2]       = (B1[2] - B2[2] - B0[2]) * N[2];   // Sign negated
                bq->b2[3]       = (B1[3] - B2[3] - B0[3]) * N[3];   // Sign negated

                // Increment pointers
                fc             += 8;
                bq              ++;
            }
        }
    }

    static void matched_solve(float *p, float kf, float td, size_t count, size_t stride)
    {
        if (p[2] == 0.0) // Test polynom for second-order
        {
            if (p[1] == 0.0) // Test polynom for first order
            {
                while (count--)
                {
                    p[3]        = 1; // transfer function
                    p          += stride;
                }
            }
            else
            {
                // First-order polynom:
                //   p(s) = p[0] + p[1]*(s/f)
                //
                // Transformed polynom:
                //   P[z] = p[1]/f - p[1]/f * exp(-f*p[0]*T/p[1]) * z^-1
                while (count--)
                {
                    float k     = p[1]/kf;
                    float R     = -p[0]/k;
                    p[3]        = sqrtf(p[0]*p[0] + p[1]*p[1]*0.01f); // transfer function
                    p[0]        = k;
                    p[1]        = -k * expf(R*td);

                    p          += stride;
                }
            }
        }
        else
        {
            // Second-order polynom:
            //   p(s) = p[0] + p[1]*(s/f) + p[2]*(s/f)^2 = p[2]/f^2 * (p[0]*f^2/p[2] + p[1]*f/p[2]*s + s^2)
            //
            // Calculate the roots of the second-order polynom equation a*x^2 + b*x + c = 0
            float k, b, c, D;
            float a2   = 2.0f/(kf*kf);

            while (count--)
            {
                // Transfer function
                b           = p[0] - p[2]*0.01f;
                c           = p[1]*0.1f;
                p[3]        = sqrt(b*b + c*c);

                // Calculate parameters
                k           = p[2];
                b           = p[1]/(kf*p[2]);
                c           = p[0]/p[2];
                D           = b*b - 2.0f*a2*c;

                if (D >= 0.0f)
                {
                    // Has real roots R0 and R1
                    // Transformed form is:
                    //   P[z] = k*(1 - (exp(R0*T) + exp(R1*T))*z^-1 + exp((R0+R1)*T)*z^-2)
                    D           = sqrtf(D);
                    float R0    = td*(-b - D)/a2;
                    float R1    = td*(-b + D)/a2;
                    p[0]        = k;
                    p[1]        = -k * (expf(R0) + expf(R1));
                    p[2]        = k * expf(R0+R1);
                }
                else
                {
                    // Has complex roots R+j*K and R-j*K
                    // Transformed form is:
                    //   P[z] = k*(1 - 2*exp(R*T)*cos(K*T)*z^-1 + exp(2*R*T)*z^-2)
                    D           = sqrtf(-D);
                    float R     = -(td*b) /a2;
                    float K     = D /a2;
                    p[0]        = k;
                    p[1]        = -2.0 * k * expf(R) * cosf(K*td);
                    p[2]        = k * expf(R+R);
                }

                // Update pointer
                p          += stride;
            }
        }
    }

    void matched_transform_x1(biquad_x1_t *bf, f_cascade_t *bc, float kf, float td, size_t count)
    {
        // Find roots for top and bottom polynoms
        matched_solve(bc->t, kf, td, count, sizeof(f_cascade_t)/sizeof(float));
        matched_solve(bc->b, kf, td, count, sizeof(f_cascade_t)/sizeof(float));

        double w        = kf * td * 0.1;
        float cos_w     = cos(w);
        float sin_w     = sin(w);
        float cos_2w    = cos_w*cos_w - sin_w*sin_w; // cos(2x) = cos(x)^2 - sin(x)^2
        float sin_2w    = 2*sin_w*cos_w; // sin(2x) = 2 * cos(x) * sin(x)

        // We have to calculate the norming factor of the digital filter
        // To do this, we should get the amplitude of the discrete transfer function
        // at the control frequency and the amplitude of the continuous transfer function
        // at the same frequency.
        // As control frequency we take the f/10 value
        // For the discrete transfer function it will be PI*0.2*f / SR
        // For the normalized continuous transfer function it will be always 0.1

        // Iterate each cascade
        while (count--)
        {
            // Calculate the discrete transfer function part at specified frequency
            float re    = bc->t[0]*cos_2w + bc->t[1]*cos_w + bc->t[2];
            float im    = bc->t[0]*sin_2w + bc->t[1]*sin_w;
            float at    = sqrtf(re*re + im*im);

            re          = bc->b[0]*cos_2w + bc->b[1]*cos_w + bc->b[2];
            im          = bc->b[0]*sin_2w + bc->b[1]*sin_w;
            float ab    = sqrtf(re*re + im*im);

            // Now calculate the convolution for the new polynom:
            /*
                       T[0] + T[1]*z^-1 + T[2]*z^-2
              H(z) =  ------------------------------
                       B[0] + B[1]*z^-1 + B[2]*z^-2

             */
            float AN    = (ab * bc->t[3]) / (at * bc->b[3]); // Normalizing factor for the amplitude to match the analog filter
            float N2    = 1.0 / bc->b[0];
            float N1    = AN * N2;

            // Normalize filter parameters
            bf->a[0]    = bc->t[0] * N1;
            bf->a[1]    = bf->a[0];
            bf->a[2]    = bc->t[1] * N1;
            bf->a[3]    = bc->t[2] * N1;

            bf->b[0]    = -bc->b[1] * N2; // Sign negated
            bf->b[1]    = -bc->b[2] * N2; // Sign negated
            bf->b[2]    = 0.0f;
            bf->b[3]    = 0.0f;

            // Move to next filter
            bf          ++;
            bc          ++;
        } // for i
    }

    void matched_transform_x2(biquad_x2_t *bf, f_cascade_t *bc, float kf, float td, size_t count)
    {
        double w        = kf * td * 0.1;
        float cos_w     = cos(w);
        float sin_w     = sin(w);
        float cos_2w    = cos_w*cos_w - sin_w*sin_w; // cos(2x) = cos(x)^2 - sin(x)^2
        float sin_2w    = 2*sin_w*cos_w; // sin(2x) = 2 * cos(x) * sin(x)

        // Step 1. Solve filters
        for (size_t i=0; i<2; ++i)
        {
            f_cascade_t *xc = &bc[i*3];

            // Find roots for top and bottom polynoms
            matched_solve(xc->t, kf, td, count - 1, (2*sizeof(f_cascade_t))/sizeof(float));
            matched_solve(xc->b, kf, td, count - 1, (2*sizeof(f_cascade_t))/sizeof(float));
        }

        float re[2], im[2], at[2], ab[2], AN[2], N1[2], N2[2];

        // Iterate each cascade pair
        while (count--)
        {
            // Calculate the discrete transfer function part at specified frequency
            re[0]       = bc[0].t[0]*cos_2w + bc[0].t[1]*cos_w + bc[0].t[2];
            re[1]       = bc[1].t[0]*cos_2w + bc[1].t[1]*cos_w + bc[1].t[2];
            im[0]       = bc[0].t[0]*sin_2w + bc[0].t[1]*sin_w;
            im[1]       = bc[1].t[0]*sin_2w + bc[1].t[1]*sin_w;

            at[0]       = sqrtf(re[0]*re[0] + im[0]*im[0]);
            at[1]       = sqrtf(re[1]*re[1] + im[1]*im[1]);

            re[0]       = bc[0].b[0]*cos_2w + bc[0].b[1]*cos_w + bc[0].b[2];
            re[1]       = bc[1].b[0]*cos_2w + bc[1].b[1]*cos_w + bc[1].b[2];
            im[0]       = bc[0].b[0]*sin_2w + bc[0].b[1]*sin_w;
            im[1]       = bc[1].b[0]*sin_2w + bc[1].b[1]*sin_w;

            ab[0]       = sqrtf(re[0]*re[0] + im[0]*im[0]);
            ab[1]       = sqrtf(re[1]*re[1] + im[1]*im[1]);

            // Now calculate the convolution for the new polynom:
            AN[0]       = (ab[0] * bc[0].t[3]) / (at[0] * bc[0].b[3]); // Normalizing factor for the amplitude to match the analog filter
            AN[1]       = (ab[1] * bc[1].t[3]) / (at[1] * bc[1].b[3]); // Normalizing factor for the amplitude to match the analog filter

            N2[0]       = 1.0 / bc[0].b[0];
            N2[1]       = 1.0 / bc[1].b[0];
            N1[0]       = AN[0] * N2[0];
            N1[1]       = AN[1] * N2[1];

            // Normalize filter parameters
            bf->a[0]    = bc[0].t[0] * N1[0];
            bf->a[1]    = bf->a[0];
            bf->a[2]    = bc[0].t[1] * N1[0];
            bf->a[3]    = bc[0].t[2] * N1[0];

            bf->a[4]    = bc[1].t[0] * N1[1];
            bf->a[5]    = bf->a[4];
            bf->a[6]    = bc[1].t[1] * N1[1];
            bf->a[7]    = bc[1].t[2] * N1[1];

            bf->b[0]    = -bc[0].b[1] * N2[0]; // Sign negated
            bf->b[1]    = -bc[0].b[2] * N2[0]; // Sign negated
            bf->b[2]    = 0.0f;
            bf->b[3]    = 0.0f;

            bf->b[4]    = -bc[1].b[1] * N2[1]; // Sign negated
            bf->b[5]    = -bc[1].b[2] * N2[1]; // Sign negated
            bf->b[6]    = 0.0f;
            bf->b[7]    = 0.0f;

            // Move to next filter
            bf          ++;
            bc          += 2;
        } // for i
    }

    void matched_transform_x4(biquad_x4_t *bf, f_cascade_t *bc, float kf, float td, size_t count)
    {
        double w        = kf * td * 0.1;
        float cos_w     = cos(w);
        float sin_w     = sin(w);
        float cos_2w    = cos_w*cos_w - sin_w*sin_w; // cos(2x) = cos(x)^2 - sin(x)^2
        float sin_2w    = 2*sin_w*cos_w; // sin(2x) = 2 * cos(x) * sin(x)

        // Step 1. Solve filters
        for (size_t i=0; i<4; ++i)
        {
            f_cascade_t *xc = &bc[i*5];

            // Find roots for top and bottom polynoms
            matched_solve(xc->t, kf, td, count - 3, (4*sizeof(f_cascade_t))/sizeof(float));
            matched_solve(xc->b, kf, td, count - 3, (4*sizeof(f_cascade_t))/sizeof(float));
        }

        float re[4], im[4], at[4], ab[4], AN[4], N1[4], N2[4];

        // Iterate each cascade pair
        while (count--)
        {
            // Calculate the discrete transfer function part at specified frequency
            re[0]       = bc[0].t[0]*cos_2w + bc[0].t[1]*cos_w + bc[0].t[2];
            re[1]       = bc[1].t[0]*cos_2w + bc[1].t[1]*cos_w + bc[1].t[2];
            re[2]       = bc[2].t[0]*cos_2w + bc[2].t[1]*cos_w + bc[2].t[2];
            re[3]       = bc[3].t[0]*cos_2w + bc[3].t[1]*cos_w + bc[3].t[2];

            im[0]       = bc[0].t[0]*sin_2w + bc[0].t[1]*sin_w;
            im[1]       = bc[1].t[0]*sin_2w + bc[1].t[1]*sin_w;
            im[2]       = bc[2].t[0]*sin_2w + bc[2].t[1]*sin_w;
            im[3]       = bc[3].t[0]*sin_2w + bc[3].t[1]*sin_w;

            at[0]       = sqrtf(re[0]*re[0] + im[0]*im[0]);
            at[1]       = sqrtf(re[1]*re[1] + im[1]*im[1]);
            at[2]       = sqrtf(re[2]*re[2] + im[2]*im[2]);
            at[3]       = sqrtf(re[3]*re[3] + im[3]*im[3]);

            re[0]       = bc[0].b[0]*cos_2w + bc[0].b[1]*cos_w + bc[0].b[2];
            re[1]       = bc[1].b[0]*cos_2w + bc[1].b[1]*cos_w + bc[1].b[2];
            re[2]       = bc[2].b[0]*cos_2w + bc[2].b[1]*cos_w + bc[2].b[2];
            re[3]       = bc[3].b[0]*cos_2w + bc[3].b[1]*cos_w + bc[3].b[2];

            im[0]       = bc[0].b[0]*sin_2w + bc[0].b[1]*sin_w;
            im[1]       = bc[1].b[0]*sin_2w + bc[1].b[1]*sin_w;
            im[2]       = bc[2].b[0]*sin_2w + bc[2].b[1]*sin_w;
            im[3]       = bc[3].b[0]*sin_2w + bc[3].b[1]*sin_w;

            ab[0]       = sqrtf(re[0]*re[0] + im[0]*im[0]);
            ab[1]       = sqrtf(re[1]*re[1] + im[1]*im[1]);
            ab[2]       = sqrtf(re[2]*re[2] + im[2]*im[2]);
            ab[3]       = sqrtf(re[3]*re[3] + im[3]*im[3]);

            // Now calculate the convolution for the new polynom:
            AN[0]       = (ab[0] * bc[0].t[3]) / (at[0] * bc[0].b[3]); // Normalizing factor for the amplitude to match the analog filter
            AN[1]       = (ab[1] * bc[1].t[3]) / (at[1] * bc[1].b[3]); // Normalizing factor for the amplitude to match the analog filter
            AN[2]       = (ab[2] * bc[2].t[3]) / (at[2] * bc[2].b[3]); // Normalizing factor for the amplitude to match the analog filter
            AN[3]       = (ab[3] * bc[3].t[3]) / (at[3] * bc[3].b[3]); // Normalizing factor for the amplitude to match the analog filter

            N2[0]       = 1.0 / bc[0].b[0];
            N2[1]       = 1.0 / bc[1].b[0];
            N2[2]       = 1.0 / bc[2].b[0];
            N2[3]       = 1.0 / bc[3].b[0];

            N1[0]       = AN[0] * N2[0];
            N1[1]       = AN[1] * N2[1];
            N1[2]       = AN[2] * N2[2];
            N1[3]       = AN[3] * N2[3];

            // Normalize filter parameters
            bf->a0[0]   = bc[0].t[0] * N1[0];
            bf->a0[1]   = bc[1].t[0] * N1[1];
            bf->a0[2]   = bc[2].t[0] * N1[2];
            bf->a0[3]   = bc[3].t[0] * N1[3];

            bf->a1[0]   = bc[0].t[1] * N1[0];
            bf->a1[1]   = bc[1].t[1] * N1[1];
            bf->a1[2]   = bc[2].t[1] * N1[2];
            bf->a1[3]   = bc[3].t[1] * N1[3];

            bf->a2[0]   = bc[0].t[2] * N1[0];
            bf->a2[1]   = bc[1].t[2] * N1[1];
            bf->a2[2]   = bc[2].t[2] * N1[2];
            bf->a2[3]   = bc[3].t[2] * N1[3];

            bf->b1[0]   = -bc[0].b[1] * N2[0]; // Sign negated
            bf->b1[1]   = -bc[1].b[1] * N2[1]; // Sign negated
            bf->b1[2]   = -bc[2].b[1] * N2[2]; // Sign negated
            bf->b1[3]   = -bc[3].b[1] * N2[3]; // Sign negated

            bf->b2[0]   = -bc[0].b[2] * N2[0]; // Sign negated
            bf->b2[1]   = -bc[1].b[2] * N2[1]; // Sign negated
            bf->b2[2]   = -bc[2].b[2] * N2[2]; // Sign negated
            bf->b2[3]   = -bc[3].b[2] * N2[3]; // Sign negated

            // Move to next filter
            bf          ++;
            bc          += 4;
        } // for i
    }

    void matched_transform_x8(biquad_x8_t *bf, f_cascade_t *bc, float kf, float td, size_t count)
    {
        double w        = kf * td * 0.1;
        float cos_w     = cos(w);
        float sin_w     = sin(w);
        float cos_2w    = cos_w*cos_w - sin_w*sin_w; // cos(2x) = cos(x)^2 - sin(x)^2
        float sin_2w    = 2*sin_w*cos_w; // sin(2x) = 2 * cos(x) * sin(x)

        // Step 1. Solve filters
        for (size_t i=0; i<8; ++i)
        {
            f_cascade_t *xc = &bc[i*9];

            // Find roots for top and bottom polynoms
            matched_solve(xc->t, kf, td, count - 7, (8*sizeof(f_cascade_t))/sizeof(float));
            matched_solve(xc->b, kf, td, count - 7, (8*sizeof(f_cascade_t))/sizeof(float));
        }

        float re[8], im[8], at[8], ab[8], AN[8], N1[8], N2[8];

        // Iterate each cascade pair
        while (count--)
        {
            // Calculate the discrete transfer function part at specified frequency
            re[0]       = bc[0].t[0]*cos_2w + bc[0].t[1]*cos_w + bc[0].t[2];
            re[1]       = bc[1].t[0]*cos_2w + bc[1].t[1]*cos_w + bc[1].t[2];
            re[2]       = bc[2].t[0]*cos_2w + bc[2].t[1]*cos_w + bc[2].t[2];
            re[3]       = bc[3].t[0]*cos_2w + bc[3].t[1]*cos_w + bc[3].t[2];
            re[4]       = bc[4].t[0]*cos_2w + bc[4].t[1]*cos_w + bc[4].t[2];
            re[5]       = bc[5].t[0]*cos_2w + bc[5].t[1]*cos_w + bc[5].t[2];
            re[6]       = bc[6].t[0]*cos_2w + bc[6].t[1]*cos_w + bc[6].t[2];
            re[7]       = bc[7].t[0]*cos_2w + bc[7].t[1]*cos_w + bc[7].t[2];

            im[0]       = bc[0].t[0]*sin_2w + bc[0].t[1]*sin_w;
            im[1]       = bc[1].t[0]*sin_2w + bc[1].t[1]*sin_w;
            im[2]       = bc[2].t[0]*sin_2w + bc[2].t[1]*sin_w;
            im[3]       = bc[3].t[0]*sin_2w + bc[3].t[1]*sin_w;
            im[4]       = bc[4].t[0]*sin_2w + bc[4].t[1]*sin_w;
            im[5]       = bc[5].t[0]*sin_2w + bc[5].t[1]*sin_w;
            im[6]       = bc[6].t[0]*sin_2w + bc[6].t[1]*sin_w;
            im[7]       = bc[7].t[0]*sin_2w + bc[7].t[1]*sin_w;

            at[0]       = sqrtf(re[0]*re[0] + im[0]*im[0]);
            at[1]       = sqrtf(re[1]*re[1] + im[1]*im[1]);
            at[2]       = sqrtf(re[2]*re[2] + im[2]*im[2]);
            at[3]       = sqrtf(re[3]*re[3] + im[3]*im[3]);
            at[4]       = sqrtf(re[4]*re[4] + im[4]*im[4]);
            at[5]       = sqrtf(re[5]*re[5] + im[5]*im[5]);
            at[6]       = sqrtf(re[6]*re[6] + im[6]*im[6]);
            at[7]       = sqrtf(re[7]*re[7] + im[7]*im[7]);

            re[0]       = bc[0].b[0]*cos_2w + bc[0].b[1]*cos_w + bc[0].b[2];
            re[1]       = bc[1].b[0]*cos_2w + bc[1].b[1]*cos_w + bc[1].b[2];
            re[2]       = bc[2].b[0]*cos_2w + bc[2].b[1]*cos_w + bc[2].b[2];
            re[3]       = bc[3].b[0]*cos_2w + bc[3].b[1]*cos_w + bc[3].b[2];
            re[4]       = bc[4].b[0]*cos_2w + bc[4].b[1]*cos_w + bc[4].b[2];
            re[5]       = bc[5].b[0]*cos_2w + bc[5].b[1]*cos_w + bc[5].b[2];
            re[6]       = bc[6].b[0]*cos_2w + bc[6].b[1]*cos_w + bc[6].b[2];
            re[7]       = bc[7].b[0]*cos_2w + bc[7].b[1]*cos_w + bc[7].b[2];

            im[0]       = bc[0].b[0]*sin_2w + bc[0].b[1]*sin_w;
            im[1]       = bc[1].b[0]*sin_2w + bc[1].b[1]*sin_w;
            im[2]       = bc[2].b[0]*sin_2w + bc[2].b[1]*sin_w;
            im[3]       = bc[3].b[0]*sin_2w + bc[3].b[1]*sin_w;
            im[4]       = bc[4].b[0]*sin_2w + bc[4].b[1]*sin_w;
            im[5]       = bc[5].b[0]*sin_2w + bc[5].b[1]*sin_w;
            im[6]       = bc[6].b[0]*sin_2w + bc[6].b[1]*sin_w;
            im[7]       = bc[7].b[0]*sin_2w + bc[7].b[1]*sin_w;

            ab[0]       = sqrtf(re[0]*re[0] + im[0]*im[0]);
            ab[1]       = sqrtf(re[1]*re[1] + im[1]*im[1]);
            ab[2]       = sqrtf(re[2]*re[2] + im[2]*im[2]);
            ab[3]       = sqrtf(re[3]*re[3] + im[3]*im[3]);
            ab[4]       = sqrtf(re[4]*re[4] + im[4]*im[4]);
            ab[5]       = sqrtf(re[5]*re[5] + im[5]*im[5]);
            ab[6]       = sqrtf(re[6]*re[6] + im[6]*im[6]);
            ab[7]       = sqrtf(re[7]*re[7] + im[7]*im[7]);

            // Now calculate the convolution for the new polynom:
            AN[0]       = (ab[0] * bc[0].t[3]) / (at[0] * bc[0].b[3]); // Normalizing factor for the amplitude to match the analog filter
            AN[1]       = (ab[1] * bc[1].t[3]) / (at[1] * bc[1].b[3]); // Normalizing factor for the amplitude to match the analog filter
            AN[2]       = (ab[2] * bc[2].t[3]) / (at[2] * bc[2].b[3]); // Normalizing factor for the amplitude to match the analog filter
            AN[3]       = (ab[3] * bc[3].t[3]) / (at[3] * bc[3].b[3]); // Normalizing factor for the amplitude to match the analog filter
            AN[4]       = (ab[4] * bc[4].t[3]) / (at[4] * bc[4].b[3]); // Normalizing factor for the amplitude to match the analog filter
            AN[5]       = (ab[5] * bc[5].t[3]) / (at[5] * bc[5].b[3]); // Normalizing factor for the amplitude to match the analog filter
            AN[6]       = (ab[6] * bc[6].t[3]) / (at[6] * bc[6].b[3]); // Normalizing factor for the amplitude to match the analog filter
            AN[7]       = (ab[7] * bc[7].t[3]) / (at[7] * bc[7].b[3]); // Normalizing factor for the amplitude to match the analog filter

            N2[0]       = 1.0 / bc[0].b[0];
            N2[1]       = 1.0 / bc[1].b[0];
            N2[2]       = 1.0 / bc[2].b[0];
            N2[3]       = 1.0 / bc[3].b[0];
            N2[4]       = 1.0 / bc[4].b[0];
            N2[5]       = 1.0 / bc[5].b[0];
            N2[6]       = 1.0 / bc[6].b[0];
            N2[7]       = 1.0 / bc[7].b[0];

            N1[0]       = AN[0] * N2[0];
            N1[1]       = AN[1] * N2[1];
            N1[2]       = AN[2] * N2[2];
            N1[3]       = AN[3] * N2[3];
            N1[4]       = AN[4] * N2[4];
            N1[5]       = AN[5] * N2[5];
            N1[6]       = AN[6] * N2[6];
            N1[7]       = AN[7] * N2[7];

            // Normalize filter parameters
            bf->a0[0]   = bc[0].t[0] * N1[0];
            bf->a0[1]   = bc[1].t[0] * N1[1];
            bf->a0[2]   = bc[2].t[0] * N1[2];
            bf->a0[3]   = bc[3].t[0] * N1[3];
            bf->a0[4]   = bc[4].t[0] * N1[4];
            bf->a0[5]   = bc[5].t[0] * N1[5];
            bf->a0[6]   = bc[6].t[0] * N1[6];
            bf->a0[7]   = bc[7].t[0] * N1[7];

            bf->a1[0]   = bc[0].t[1] * N1[0];
            bf->a1[1]   = bc[1].t[1] * N1[1];
            bf->a1[2]   = bc[2].t[1] * N1[2];
            bf->a1[3]   = bc[3].t[1] * N1[3];
            bf->a1[4]   = bc[4].t[1] * N1[4];
            bf->a1[5]   = bc[5].t[1] * N1[5];
            bf->a1[6]   = bc[6].t[1] * N1[6];
            bf->a1[7]   = bc[7].t[1] * N1[7];

            bf->a2[0]   = bc[0].t[2] * N1[0];
            bf->a2[1]   = bc[1].t[2] * N1[1];
            bf->a2[2]   = bc[2].t[2] * N1[2];
            bf->a2[3]   = bc[3].t[2] * N1[3];
            bf->a2[4]   = bc[4].t[2] * N1[4];
            bf->a2[5]   = bc[5].t[2] * N1[5];
            bf->a2[6]   = bc[6].t[2] * N1[6];
            bf->a2[7]   = bc[7].t[2] * N1[7];

            bf->b1[0]   = -bc[0].b[1] * N2[0]; // Sign negated
            bf->b1[1]   = -bc[1].b[1] * N2[1]; // Sign negated
            bf->b1[2]   = -bc[2].b[1] * N2[2]; // Sign negated
            bf->b1[3]   = -bc[3].b[1] * N2[3]; // Sign negated
            bf->b1[4]   = -bc[4].b[1] * N2[4]; // Sign negated
            bf->b1[5]   = -bc[5].b[1] * N2[5]; // Sign negated
            bf->b1[6]   = -bc[6].b[1] * N2[6]; // Sign negated
            bf->b1[7]   = -bc[7].b[1] * N2[7]; // Sign negated

            bf->b2[0]   = -bc[0].b[2] * N2[0]; // Sign negated
            bf->b2[1]   = -bc[1].b[2] * N2[1]; // Sign negated
            bf->b2[2]   = -bc[2].b[2] * N2[2]; // Sign negated
            bf->b2[3]   = -bc[3].b[2] * N2[3]; // Sign negated
            bf->b2[4]   = -bc[4].b[2] * N2[4]; // Sign negated
            bf->b2[5]   = -bc[5].b[2] * N2[5]; // Sign negated
            bf->b2[6]   = -bc[6].b[2] * N2[6]; // Sign negated
            bf->b2[7]   = -bc[7].b[2] * N2[7]; // Sign negated

            // Move to next filter
            bf          ++;
            bc          += 8;
        } // for i
    }
}

#endif /* DSP_ARCH_NATIVE_FILTERS_TRANSFORM_H_ */
