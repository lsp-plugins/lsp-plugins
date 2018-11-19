/*
 * graphics.h
 *
 *  Created on: 17 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_GRAPHICS_H_
#define DSP_ARCH_ARM_NEON_D32_GRAPHICS_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    void axis_apply_log(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count)
    {
        IF_ARCH_ARM(
            float params[12] __lsp_aligned16;
            params[0] = zero;
            params[1] = norm_x;
            params[2] = norm_y;
            const uint32_t *iptr = &LOG_IARGS[4];
            const float *fptr = LOG_FARGS;
        );

        //---------------------------------------------------------------
        // Prepare constants
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.32         {q0}, [%[params]]")
            __ASM_EMIT("vdup.32         q3, d1[0]")                     // q3 = norm_y
            __ASM_EMIT("vdup.32         q2, d0[1]")                     // q2 = norm_x
            __ASM_EMIT("vdup.32         q0, d0[0]")                     // q0 = zero
            __ASM_EMIT("vst1.32         {q0}, [%[params]]!")            // params += 4
            __ASM_EMIT("vst1.32         {q2-q3}, [%[params]]")

            __ASM_EMIT("subs            %[count], $16")
            __ASM_EMIT("blo             2f")

            // Do 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {q4-q5}, [%[v]]!")              // q4  = v, q5 = v, v += 8
            __ASM_EMIT("sub             %[params], $0x10")              // params -= 4
            __ASM_EMIT("vld1.32         {q6-q7}, [%[v]]!")
            __ASM_EMIT("vld1.32         {q12}, [%[fptr]]!")             // q12 = X_AMP, fptr += 4
            __ASM_EMIT("vabs.f32        q4, q4")                        // q4  = abs(v)
            __ASM_EMIT("vabs.f32        q5, q5")
            __ASM_EMIT("vabs.f32        q6, q6")
            __ASM_EMIT("vabs.f32        q7, q7")
            __ASM_EMIT("vld1.32         {q13}, [%[params]]!")           // q13 = zero, params += 4
            __ASM_EMIT("vmax.f32        q4, q12")                       // q12 = max(X_AMP, abs(v)), ignores denormalized values
            __ASM_EMIT("vmax.f32        q5, q12")
            __ASM_EMIT("vmax.f32        q6, q12")
            __ASM_EMIT("vmax.f32        q7, q12")
            __ASM_EMIT("vmul.f32        q0, q4, q13")                   // q13 = max(X_AMP, abs(v)) * zero
            __ASM_EMIT("vmul.f32        q1, q5, q13")
            __ASM_EMIT("vmul.f32        q2, q6, q13")
            __ASM_EMIT("vmul.f32        q3, q7, q13")
            // Step 2: parse float value
            __ASM_EMIT("vld1.32         {q12-q13}, [%[iptr]]")          // q12 = MANT_MASK, q13 = FRAC_SUB = 127
            __ASM_EMIT("vshr.u32        q4, q0, $23")                   // q4  = frac(v)
            __ASM_EMIT("vshr.u32        q5, q1, $23")
            __ASM_EMIT("vshr.u32        q6, q2, $23")
            __ASM_EMIT("vshr.u32        q7, q3, $23")
            __ASM_EMIT("vand.u32        q8, q0, q12")                   // q8  = mant(v)
            __ASM_EMIT("vand.u32        q9, q1, q12")
            __ASM_EMIT("vand.u32        q10, q2, q12")
            __ASM_EMIT("vand.u32        q11, q3, q12")
            __ASM_EMIT("vld1.32         {q14-q15}, [%[fptr]]!")         // q14 = 0.5f, q15 = sqrt(1/2), fptr += 8
            __ASM_EMIT("vsub.i32        q4, q4, q13")                   // q4  = frac(v) - 127
            __ASM_EMIT("vsub.i32        q5, q5, q13")
            __ASM_EMIT("vsub.i32        q6, q6, q13")
            __ASM_EMIT("vsub.i32        q7, q7, q13")
            __ASM_EMIT("vorr.i32        q8, q8, q14")                   // q8  = V = mant(v)+0.5
            __ASM_EMIT("vorr.i32        q9, q9, q14")
            __ASM_EMIT("vorr.i32        q10, q10, q14")
            __ASM_EMIT("vorr.i32        q11, q11, q14")
            __ASM_EMIT("vcvt.f32.s32    q4, q4")                        // q4  = E = float(frac(v)-127)
            __ASM_EMIT("vcvt.f32.s32    q5, q5")
            __ASM_EMIT("vcvt.f32.s32    q6, q6")
            __ASM_EMIT("vcvt.f32.s32    q7, q7")
            // Prepare logarithm approximation calculations
            __ASM_EMIT("vld1.32         {q12-q13}, [%[fptr]]!")         // q12  = 1.0, q13 = L0 , fptr += 4
            __ASM_EMIT("vclt.f32        q0, q8, q15")                   // q0   = [ V < sqrt(1/2) ]
            __ASM_EMIT("vclt.f32        q1, q9, q15")
            __ASM_EMIT("vclt.f32        q2, q10, q15")
            __ASM_EMIT("vclt.f32        q3, q11, q15")
            __ASM_EMIT("vand.u32        q0, q0, q8")                    // q0   = V * [ V < sqrt(1/2) ]
            __ASM_EMIT("vand.u32        q1, q1, q9")
            __ASM_EMIT("vand.u32        q2, q2, q10")
            __ASM_EMIT("vand.u32        q3, q3, q11")
            __ASM_EMIT("vadd.f32        q0, q0, q8")                    // q0   = V + V * [ V < sqrt(1/2) ]
            __ASM_EMIT("vadd.f32        q1, q1, q9")
            __ASM_EMIT("vadd.f32        q2, q2, q10")
            __ASM_EMIT("vadd.f32        q3, q3, q11")
            __ASM_EMIT("vclt.f32        q8, q8, q15")                   // q8   = [ V < sqrt(1/2) ]
            __ASM_EMIT("vclt.f32        q9, q9, q15")
            __ASM_EMIT("vclt.f32        q10, q10, q15")
            __ASM_EMIT("vclt.f32        q11, q11, q15")
            __ASM_EMIT("vsub.f32        q0, q0, q12")                   // q0   = A = V + V & [ V < sqrt(1/2) ] - 1.0
            __ASM_EMIT("vsub.f32        q1, q1, q12")
            __ASM_EMIT("vsub.f32        q2, q2, q12")
            __ASM_EMIT("vsub.f32        q3, q3, q12")
            __ASM_EMIT("vmvn            q8, q8")                        // q8   = [ V >= sqrt(1/2) ]
            __ASM_EMIT("vmvn            q9, q9")
            __ASM_EMIT("vmvn            q10, q10")
            __ASM_EMIT("vmvn            q11, q11")
            __ASM_EMIT("vand.u32        q8, q8, q12")                   // q8   = 1.0 & [ V >= sqrt(1/2) ]
            __ASM_EMIT("vand.u32        q9, q9, q12")
            __ASM_EMIT("vand.u32        q10, q10, q12")
            __ASM_EMIT("vand.u32        q11, q11, q12")
            __ASM_EMIT("vadd.f32        q4, q4, q8")                    // q4   = B = E + 1.0 & [ V >= sqrt(1/2) ]
            __ASM_EMIT("vadd.f32        q5, q5, q9")
            __ASM_EMIT("vadd.f32        q6, q6, q10")
            __ASM_EMIT("vadd.f32        q7, q7, q11")
            // Calculate logarithmic values
            __ASM_EMIT("vld1.32         {q14-q15}, [%[fptr]]!")         // q14  = L1, q15 = L2, fptr += 8
            __ASM_EMIT("vmul.f32        q8, q0, q13")                   // q8   = L0*A
            __ASM_EMIT("vmul.f32        q9, q1, q13")
            __ASM_EMIT("vmul.f32        q10, q2, q13")
            __ASM_EMIT("vmul.f32        q11, q3, q13")
            __ASM_EMIT("vadd.f32        q8, q8, q14")                   // q8   = L1+L0*A
            __ASM_EMIT("vadd.f32        q9, q9, q14")
            __ASM_EMIT("vadd.f32        q10, q10, q14")
            __ASM_EMIT("vadd.f32        q11, q11, q14")
            __ASM_EMIT("vld1.32         {q12-q13}, [%[fptr]]!")         // q12  = L3, q13 = L4, fptr += 8
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L1+L0*A)
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("vmul.f32        q10, q10, q2")
            __ASM_EMIT("vmul.f32        q11, q11, q3")
            __ASM_EMIT("vadd.f32        q8, q8, q15")                   // q8   = L2+A*(L1+L0*A)
            __ASM_EMIT("vadd.f32        q9, q9, q15")
            __ASM_EMIT("vadd.f32        q10, q10, q15")
            __ASM_EMIT("vadd.f32        q11, q11, q15")
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L2+A*(L1+L0*A))
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("vmul.f32        q10, q10, q2")
            __ASM_EMIT("vmul.f32        q11, q11, q3")
            __ASM_EMIT("vadd.f32        q8, q8, q12")                   // q8   = L3+A*(L2+A*(L1+L0*A))
            __ASM_EMIT("vadd.f32        q9, q9, q12")
            __ASM_EMIT("vadd.f32        q10, q10, q12")
            __ASM_EMIT("vadd.f32        q11, q11, q12")
            __ASM_EMIT("vld1.32         {q14-q15}, [%[fptr]]!")         // q14  = L5, q15 = L6, fptr += 8
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L3+A*(L2+A*(L1+L0*A)))
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("vmul.f32        q10, q10, q2")
            __ASM_EMIT("vmul.f32        q11, q11, q3")
            __ASM_EMIT("vadd.f32        q8, q8, q13")                   // q8   = L4+A*(L3+A*(L2+A*(L1+L0*A)))
            __ASM_EMIT("vadd.f32        q9, q9, q13")
            __ASM_EMIT("vadd.f32        q10, q10, q13")
            __ASM_EMIT("vadd.f32        q11, q11, q13")
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("vmul.f32        q10, q10, q2")
            __ASM_EMIT("vmul.f32        q11, q11, q3")
            __ASM_EMIT("vadd.f32        q8, q8, q14")                   // q8   = L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
            __ASM_EMIT("vadd.f32        q9, q9, q14")
            __ASM_EMIT("vadd.f32        q10, q10, q14")
            __ASM_EMIT("vadd.f32        q11, q11, q14")
            __ASM_EMIT("vld1.32         {q12-q13}, [%[fptr]]!")         // q12  = L7, q13 = L8, fptr += 8
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("vmul.f32        q10, q10, q2")
            __ASM_EMIT("vmul.f32        q11, q11, q3")
            __ASM_EMIT("vadd.f32        q8, q8, q15")                   // q8   = L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
            __ASM_EMIT("vadd.f32        q9, q9, q15")
            __ASM_EMIT("vadd.f32        q10, q10, q15")
            __ASM_EMIT("vadd.f32        q11, q11, q15")
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("vmul.f32        q10, q10, q2")
            __ASM_EMIT("vmul.f32        q11, q11, q3")
            __ASM_EMIT("vadd.f32        q8, q8, q12")                   // q8   = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
            __ASM_EMIT("vadd.f32        q9, q9, q12")
            __ASM_EMIT("vadd.f32        q10, q10, q12")
            __ASM_EMIT("vadd.f32        q11, q11, q12")
            __ASM_EMIT("vld1.32         {q14-q15}, [%[fptr]]")          // q14  = L9, q15 = (LXE + LN2), fptr += 8
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("vmul.f32        q10, q10, q2")
            __ASM_EMIT("vmul.f32        q11, q11, q3")
            __ASM_EMIT("vadd.f32        q8, q8, q13")                   // q8   = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
            __ASM_EMIT("vadd.f32        q9, q9, q13")
            __ASM_EMIT("vadd.f32        q10, q10, q13")
            __ASM_EMIT("vadd.f32        q11, q11, q13")
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("vmul.f32        q10, q10, q2")
            __ASM_EMIT("vmul.f32        q11, q11, q3")
            __ASM_EMIT("vadd.f32        q8, q8, q14")                   // q8   = L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
            __ASM_EMIT("vadd.f32        q9, q9, q14")
            __ASM_EMIT("vadd.f32        q10, q10, q14")
            __ASM_EMIT("vadd.f32        q11, q11, q14")
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("vmul.f32        q10, q10, q2")
            __ASM_EMIT("vmul.f32        q11, q11, q3")
            __ASM_EMIT("sub             %[fptr], $0xd0")                // fptr -= 52
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("vmul.f32        q10, q10, q2")
            __ASM_EMIT("vmul.f32        q11, q11, q3")
            __ASM_EMIT("vmla.f32        q8, q4, q15")                   // q8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
            __ASM_EMIT("vmla.f32        q9, q5, q15")
            __ASM_EMIT("vmla.f32        q10, q6, q15")
            __ASM_EMIT("vmla.f32        q11, q7, q15")
            __ASM_EMIT("vadd.f32        q8, q8, q0")                    // q8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))) + A
            __ASM_EMIT("vadd.f32        q9, q9, q1")
            __ASM_EMIT("vadd.f32        q10, q10, q2")
            __ASM_EMIT("vadd.f32        q11, q11, q3")
            // Apply values to axes, q0 = log(abs(v*zero))
            __ASM_EMIT("vld1.32         {q4-q5}, [%[x]]!")              // q4 = x, q5 = x, x += 8
            __ASM_EMIT("vld1.32         {q0-q1}, [%[y]]!")              // q0 = y, q1 = y, y += 8
            __ASM_EMIT("vld1.32         {q6-q7}, [%[x]]")
            __ASM_EMIT("vld1.32         {q2-q3}, [%[y]]")
            __ASM_EMIT("vld1.32         {q12-q13}, [%[params]]")        // q12 = norm_x, q13 = norm_y
            __ASM_EMIT("vmla.f32        q4, q8, q12")                   // q4 = x + log(abs(v*zero)) * norm_x
            __ASM_EMIT("vmla.f32        q5, q9, q12")
            __ASM_EMIT("vmla.f32        q6, q10, q12")
            __ASM_EMIT("vmla.f32        q7, q11, q12")
            __ASM_EMIT("sub             %[x], $0x20")
            __ASM_EMIT("sub             %[y], $0x20")
            __ASM_EMIT("vmla.f32        q0, q8, q13")                   // q0 = y + log(abs(v*zero)) * norm_y
            __ASM_EMIT("vmla.f32        q1, q9, q13")
            __ASM_EMIT("vmla.f32        q2, q10, q13")
            __ASM_EMIT("vmla.f32        q3, q11, q13")
            __ASM_EMIT("subs            %[count], $16")
            __ASM_EMIT("vst1.32         {q4-q5}, [%[x]]!")              // x += 8
            __ASM_EMIT("vst1.32         {q0-q1}, [%[y]]!")              // y += 8
            __ASM_EMIT("vst1.32         {q6-q7}, [%[x]]!")
            __ASM_EMIT("vst1.32         {q2-q3}, [%[y]]!")
            __ASM_EMIT("bge             1b")

            // Do 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("blt             4f")

            __ASM_EMIT("sub             %[params], $0x10")              // params -= 4
            __ASM_EMIT("vld1.32         {q4-q5}, [%[v]]!")              // q4  = v, q5 = v, v += 8
            __ASM_EMIT("vld1.32         {q12}, [%[fptr]]!")             // q12 = X_AMP, fptr += 4
            __ASM_EMIT("vabs.f32        q4, q4")                        // q4  = abs(v)
            __ASM_EMIT("vabs.f32        q5, q5")
            __ASM_EMIT("vld1.32         {q13}, [%[params]]!")           // q13 = zero, params += 4
            __ASM_EMIT("vmax.f32        q4, q12")                       // q12 = max(X_AMP, abs(v)), ignores denormalized values
            __ASM_EMIT("vmax.f32        q5, q12")
            __ASM_EMIT("vmul.f32        q0, q4, q13")                   // q13 = max(X_AMP, abs(v)) * zero
            __ASM_EMIT("vmul.f32        q1, q5, q13")
            // Step 2: parse float value
            __ASM_EMIT("vld1.32         {q12-q13}, [%[iptr]]")          // q12 = MANT_MASK, q13 = FRAC_SUB = 127
            __ASM_EMIT("vshr.u32        q4, q0, $23")                   // q4  = frac(v)
            __ASM_EMIT("vshr.u32        q5, q1, $23")
            __ASM_EMIT("vand.u32        q8, q0, q12")                   // q8  = mant(v)
            __ASM_EMIT("vand.u32        q9, q1, q12")
            __ASM_EMIT("vld1.32         {q14-q15}, [%[fptr]]!")         // q14 = 0.5f, q15 = sqrt(1/2), fptr += 8
            __ASM_EMIT("vsub.i32        q4, q4, q13")                   // q4  = frac(v) - 127
            __ASM_EMIT("vsub.i32        q5, q5, q13")
            __ASM_EMIT("vorr.i32        q8, q8, q14")                   // q8  = V = mant(v)+0.5
            __ASM_EMIT("vorr.i32        q9, q9, q14")
            __ASM_EMIT("vcvt.f32.s32    q4, q4")                        // q4  = E = float(frac(v)-127)
            __ASM_EMIT("vcvt.f32.s32    q5, q5")
            // Prepare logarithm approximation calculations
            __ASM_EMIT("vld1.32         {q12-q13}, [%[fptr]]!")         // q12  = 1.0, q13 = L0 , fptr += 4
            __ASM_EMIT("vclt.f32        q0, q8, q15")                   // q0   = [ V < sqrt(1/2) ]
            __ASM_EMIT("vclt.f32        q1, q9, q15")
            __ASM_EMIT("vand.u32        q0, q0, q8")                    // q0   = V & [ V < sqrt(1/2) ]
            __ASM_EMIT("vand.u32        q1, q1, q9")
            __ASM_EMIT("vadd.f32        q0, q0, q8")                    // q0   = V + V & [ V < sqrt(1/2) ]
            __ASM_EMIT("vadd.f32        q1, q1, q9")
            __ASM_EMIT("vclt.f32        q8, q8, q15")                   // q8   = [ V < sqrt(1/2) ]
            __ASM_EMIT("vclt.f32        q9, q9, q15")
            __ASM_EMIT("vsub.f32        q0, q0, q12")                   // q0   = A = V + V & [ V < sqrt(1/2) ] - 1.0
            __ASM_EMIT("vsub.f32        q1, q1, q12")
            __ASM_EMIT("vmvn            q8, q8")                        // q8   = [ V >= sqrt(1/2) ]
            __ASM_EMIT("vmvn            q9, q9")
            __ASM_EMIT("vand.u32        q8, q8, q12")                   // q8   = 1.0 & [ V >= sqrt(1/2) ]
            __ASM_EMIT("vand.u32        q9, q9, q12")
            __ASM_EMIT("vadd.f32        q4, q4, q8")                    // q4   = B = E + 1.0 & [ V >= sqrt(1/2) ]
            __ASM_EMIT("vadd.f32        q5, q5, q9")
            // Calculate logarithmic values
            __ASM_EMIT("vld1.32         {q14-q15}, [%[fptr]]!")         // q14  = L1, q15 = L2, fptr += 8
            __ASM_EMIT("vmul.f32        q8, q0, q13")                   // q8   = L0*A
            __ASM_EMIT("vmul.f32        q9, q1, q13")
            __ASM_EMIT("vadd.f32        q8, q8, q14")                   // q8   = L1+L0*A
            __ASM_EMIT("vadd.f32        q9, q9, q14")
            __ASM_EMIT("vld1.32         {q12-q13}, [%[fptr]]!")         // q12  = L3, q13 = L4, fptr += 8
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L1+L0*A)
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("vadd.f32        q8, q8, q15")                   // q8   = L2+A*(L1+L0*A)
            __ASM_EMIT("vadd.f32        q9, q9, q15")
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L2+A*(L1+L0*A))
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("vadd.f32        q8, q8, q12")                   // q8   = L3+A*(L2+A*(L1+L0*A))
            __ASM_EMIT("vadd.f32        q9, q9, q12")
            __ASM_EMIT("vld1.32         {q14-q15}, [%[fptr]]!")         // q14  = L5, q15 = L6, fptr += 8
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L3+A*(L2+A*(L1+L0*A)))
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("vadd.f32        q8, q8, q13")                   // q8   = L4+A*(L3+A*(L2+A*(L1+L0*A)))
            __ASM_EMIT("vadd.f32        q9, q9, q13")
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("vadd.f32        q8, q8, q14")                   // q8   = L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
            __ASM_EMIT("vadd.f32        q9, q9, q14")
            __ASM_EMIT("vld1.32         {q12-q13}, [%[fptr]]!")         // q12  = L7, q13 = L8, fptr += 8
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("vadd.f32        q8, q8, q15")                   // q8   = L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
            __ASM_EMIT("vadd.f32        q9, q9, q15")
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("vadd.f32        q8, q8, q12")                   // q8   = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
            __ASM_EMIT("vadd.f32        q9, q9, q12")
            __ASM_EMIT("vld1.32         {q14-q15}, [%[fptr]]")          // q14  = L9, q15 = (LXE + LN2), fptr += 8
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("vadd.f32        q8, q8, q13")                   // q8   = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
            __ASM_EMIT("vadd.f32        q9, q9, q13")
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("vadd.f32        q8, q8, q14")                   // q8   = L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
            __ASM_EMIT("vadd.f32        q9, q9, q14")
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("sub             %[fptr], $0xd0")                // fptr -= 52
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
            __ASM_EMIT("vmul.f32        q9, q9, q1")
            __ASM_EMIT("vmla.f32        q8, q4, q15")                   // q8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
            __ASM_EMIT("vmla.f32        q9, q5, q15")
            __ASM_EMIT("vadd.f32        q8, q8, q0")                    // q8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))) + A
            __ASM_EMIT("vadd.f32        q9, q9, q1")
            // Apply values to axes, q0 = log(abs(v*zero))
            __ASM_EMIT("vld1.32         {q4-q5}, [%[x]]")               // q4 = x, q5 = x
            __ASM_EMIT("vld1.32         {q0-q1}, [%[y]]")               // q0 = y, q1 = y
            __ASM_EMIT("vld1.32         {q12-q13}, [%[params]]")        // q12 = norm_x, q13 = norm_y
            __ASM_EMIT("vmla.f32        q4, q8, q12")                   // q4 = x + log(abs(v*zero)) * norm_x
            __ASM_EMIT("vmla.f32        q5, q9, q12")
            __ASM_EMIT("vmla.f32        q0, q8, q13")                   // q0 = y + log(abs(v*zero)) * norm_y
            __ASM_EMIT("vmla.f32        q1, q9, q13")
            __ASM_EMIT("sub             %[count], $8")
            __ASM_EMIT("vst1.32         {q4-q5}, [%[x]]!")              // x += 8
            __ASM_EMIT("vst1.32         {q0-q1}, [%[y]]!")              // y += 8

            // Do x4 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             6f")

            __ASM_EMIT("sub             %[params], $0x10")              // params -= 4
            __ASM_EMIT("vld1.32         {q4}, [%[v]]!")                 // q4  = v, v += 4
            __ASM_EMIT("vld1.32         {q12}, [%[fptr]]!")             // q12 = X_AMP, fptr += 4
            __ASM_EMIT("vabs.f32        q4, q4")                        // q4  = abs(v)
            __ASM_EMIT("vld1.32         {q13}, [%[params]]!")           // q13 = zero, params += 4
            __ASM_EMIT("vmax.f32        q4, q12")                       // q4  = max(X_AMP, abs(v)), ignores denormalized values
            __ASM_EMIT("vmul.f32        q0, q4, q13")                   // q0  = max(X_AMP, abs(v)) * zero
            // Step 2: parse float value
            __ASM_EMIT("vld1.32         {q12-q13}, [%[iptr]]")          // q12 = MANT_MASK, q13 = FRAC_SUB = 127
            __ASM_EMIT("vshr.u32        q4, q0, $23")                   // q4  = frac(v)
            __ASM_EMIT("vand.u32        q8, q0, q12")                   // q8  = mant(v)
            __ASM_EMIT("vld1.32         {q14-q15}, [%[fptr]]!")         // q14 = 0.5f, q15 = sqrt(1/2), fptr += 8
            __ASM_EMIT("vsub.i32        q4, q4, q13")                   // q4  = frac(v) - 127
            __ASM_EMIT("vorr.i32        q8, q8, q14")                   // q8  = V = mant(v)+0.5
            __ASM_EMIT("vcvt.f32.s32    q4, q4")                        // q4  = E = float(frac(v)-127)
            // Prepare logarithm approximation calculations
            __ASM_EMIT("vld1.32         {q12-q13}, [%[fptr]]!")         // q12  = 1.0, q13 = L0 , fptr += 4
            __ASM_EMIT("vclt.f32        q0, q8, q15")                   // q0   = [ V < sqrt(1/2) ]
            __ASM_EMIT("vand.u32        q0, q0, q8")                    // q0   = V & [ V < sqrt(1/2) ]
            __ASM_EMIT("vadd.f32        q0, q0, q8")                    // q0   = V + V * [ V < sqrt(1/2) ]
            __ASM_EMIT("vclt.f32        q8, q8, q15")                   // q8   = [ V < sqrt(1/2) ]
            __ASM_EMIT("vsub.f32        q0, q0, q12")                   // q0   = A = V + V & [ V < sqrt(1/2) ] - 1.0
            __ASM_EMIT("vmvn            q8, q8")                        // q8   = [ V >= sqrt(1/2) ]
            __ASM_EMIT("vand.u32        q8, q8, q12")                   // q8   = 1.0 & [ V >= sqrt(1/2) ]
            __ASM_EMIT("vadd.f32        q4, q4, q8")                    // q4   = B = E + 1.0 & [ V >= sqrt(1/2) ]
            // Calculate logarithmic values
            __ASM_EMIT("vld1.32         {q14-q15}, [%[fptr]]!")         // q14  = L1, q15 = L2, fptr += 8
            __ASM_EMIT("vmul.f32        q8, q0, q13")                   // q8   = L0*A
            __ASM_EMIT("vadd.f32        q8, q8, q14")                   // q8   = L1+L0*A
            __ASM_EMIT("vld1.32         {q12-q13}, [%[fptr]]!")         // q12  = L3, q13 = L4, fptr += 8
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L1+L0*A)
            __ASM_EMIT("vadd.f32        q8, q8, q15")                   // q8   = L2+A*(L1+L0*A)
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L2+A*(L1+L0*A))
            __ASM_EMIT("vadd.f32        q8, q8, q12")                   // q8   = L3+A*(L2+A*(L1+L0*A))
            __ASM_EMIT("vld1.32         {q14-q15}, [%[fptr]]!")         // q14  = L5, q15 = L6, fptr += 8
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L3+A*(L2+A*(L1+L0*A)))
            __ASM_EMIT("vadd.f32        q8, q8, q13")                   // q8   = L4+A*(L3+A*(L2+A*(L1+L0*A)))
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
            __ASM_EMIT("vadd.f32        q8, q8, q14")                   // q8   = L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
            __ASM_EMIT("vld1.32         {q12-q13}, [%[fptr]]!")         // q12  = L7, q13 = L8, fptr += 8
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
            __ASM_EMIT("vadd.f32        q8, q8, q15")                   // q8   = L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
            __ASM_EMIT("vadd.f32        q8, q8, q12")                   // q8   = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
            __ASM_EMIT("vld1.32         {q14-q15}, [%[fptr]]")          // q14  = L9, q15 = (LXE + LN2), fptr += 8
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
            __ASM_EMIT("vadd.f32        q8, q8, q13")                   // q8   = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
            __ASM_EMIT("vadd.f32        q8, q8, q14")                   // q8   = L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
            __ASM_EMIT("sub             %[fptr], $0xd0")                // fptr -= 52
            __ASM_EMIT("vmul.f32        q8, q8, q0")                    // q8   = A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
            __ASM_EMIT("vmla.f32        q8, q4, q15")                   // q8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
            __ASM_EMIT("vadd.f32        q8, q8, q0")                    // q8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))) + A
            // Apply values to axes, q0 = log(abs(v*zero))
            __ASM_EMIT("vld1.32         {q4}, [%[x]]")                  // q4 = x
            __ASM_EMIT("vld1.32         {q0}, [%[y]]")                  // q0 = y
            __ASM_EMIT("vld1.32         {q12-q13}, [%[params]]")        // q12 = norm_x, q13 = norm_y
            __ASM_EMIT("vmla.f32        q4, q8, q12")                   // q4 = x + log(abs(v*zero)) * norm_x
            __ASM_EMIT("vmla.f32        q0, q8, q13")                   // q0 = y + log(abs(v*zero)) * norm_y
            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("vst1.32         {q4}, [%[x]]!")                 // x += 4
            __ASM_EMIT("vst1.32         {q0}, [%[y]]!")                 // y += 4

            // Do x1 block
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[count], $3")
            __ASM_EMIT("blt             8f")

            __ASM_EMIT("7:")
            __ASM_EMIT("sub             %[params], $0x10")              // params -= 4
            __ASM_EMIT("vldm            %[v]!, {s8}")                   // d4  = v, v += 4
            __ASM_EMIT("vld1.32         {q6}, [%[fptr]]!")              // d12 = X_AMP, fptr += 4
            __ASM_EMIT("vabs.f32        d4, d4")                        // d4  = abs(v)
            __ASM_EMIT("vld1.32         {q7}, [%[params]]!")            // d14 = zero, params += 4
            __ASM_EMIT("vmax.f32        d4, d12")                       // d4  = max(X_AMP, abs(v)), ignores denormalized values
            __ASM_EMIT("vmul.f32        d0, d4, d14")                   // d0  = max(X_AMP, abs(v)) * zero
            // Step 2: parse float value
            __ASM_EMIT("vld1.32         {q6-q7}, [%[iptr]]")            // d12 = MANT_MASK, d14 = FRAC_SUB = 127
            __ASM_EMIT("vshr.u32        d4, d0, $23")                   // d4  = frac(v)
            __ASM_EMIT("vand.u32        d8, d0, d12")                   // d8  = mant(v)
            __ASM_EMIT("vld1.32         {q8-q9}, [%[fptr]]!")           // d16 = 0.5f, d18 = sqrt(1/2), fptr += 8
            __ASM_EMIT("vsub.i32        d4, d4, d14")                   // d4  = frac(v) - 127
            __ASM_EMIT("vorr.i32        d8, d8, d16")                   // d8  = V = mant(v)+0.5
            __ASM_EMIT("vcvt.f32.s32    d4, d4")                        // d4  = E = float(frac(v)-127)
            // Prepare logarithm approximation calculations
            __ASM_EMIT("vld1.32         {q6-q7}, [%[fptr]]!")           // d12  = 1.0, d14 = L0 , fptr += 4
            __ASM_EMIT("vclt.f32        d0, d8, d18")                   // d0   = [ V < sqrt(1/2) ]
            __ASM_EMIT("vand.u32        d0, d0, d8")                    // d0   = V & [ V < sqrt(1/2) ]
            __ASM_EMIT("vadd.f32        d0, d0, d8")                    // d0   = V + V & [ V < sqrt(1/2) ]
            __ASM_EMIT("vclt.f32        d8, d8, d18")                   // d8   = [ V < sqrt(1/2) ]
            __ASM_EMIT("vsub.f32        d0, d0, d12")                   // d0   = A = V + V & [ V < sqrt(1/2) ] - 1.0
            __ASM_EMIT("vmvn            d8, d8")                        // d8   = [ V >= sqrt(1/2) ]
            __ASM_EMIT("vand.u32        d8, d8, d12")                   // d8   = 1.0 & [ V >= sqrt(1/2) ]
            __ASM_EMIT("vadd.f32        d4, d4, d8")                    // d4   = B = E + 1.0 & [ V >= sqrt(1/2) ]
            // Calculate logarithmic values
            __ASM_EMIT("vld1.32         {q8-q9}, [%[fptr]]!")           // d16  = L1, d18 = L2, fptr += 8
            __ASM_EMIT("vmul.f32        d8, d0, d14")                   // d8   = L0*A
            __ASM_EMIT("vadd.f32        d8, d8, d16")                   // d8   = L1+L0*A
            __ASM_EMIT("vld1.32         {q6-q7}, [%[fptr]]!")           // d12  = L3, d14 = L4, fptr += 8
            __ASM_EMIT("vmul.f32        d8, d8, d0")                    // d8   = A*(L1+L0*A)
            __ASM_EMIT("vadd.f32        d8, d8, d18")                   // d8   = L2+A*(L1+L0*A)
            __ASM_EMIT("vmul.f32        d8, d8, d0")                    // d8   = A*(L2+A*(L1+L0*A))
            __ASM_EMIT("vadd.f32        d8, d8, d12")                   // d8   = L3+A*(L2+A*(L1+L0*A))
            __ASM_EMIT("vld1.32         {q8-q9}, [%[fptr]]!")           // d16  = L5, d18 = L6, fptr += 8
            __ASM_EMIT("vmul.f32        d8, d8, d0")                    // d8   = A*(L3+A*(L2+A*(L1+L0*A)))
            __ASM_EMIT("vadd.f32        d8, d8, d14")                   // d8   = L4+A*(L3+A*(L2+A*(L1+L0*A)))
            __ASM_EMIT("vmul.f32        d8, d8, d0")                    // d8   = A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
            __ASM_EMIT("vadd.f32        d8, d8, d16")                   // d8   = L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
            __ASM_EMIT("vld1.32         {q6-q7}, [%[fptr]]!")           // d12  = L7, d14 = L8, fptr += 8
            __ASM_EMIT("vmul.f32        d8, d8, d0")                    // d8   = A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
            __ASM_EMIT("vadd.f32        d8, d8, d18")                   // d8   = L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
            __ASM_EMIT("vmul.f32        d8, d8, d0")                    // d8   = A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
            __ASM_EMIT("vadd.f32        d8, d8, d12")                   // d8   = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
            __ASM_EMIT("vld1.32         {q8-q9}, [%[fptr]]")            // d16  = L9, d18 = (LXE + LN2), fptr += 8
            __ASM_EMIT("vmul.f32        d8, d8, d0")                    // d8   = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
            __ASM_EMIT("vadd.f32        d8, d8, d14")                   // d8   = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
            __ASM_EMIT("vmul.f32        d8, d8, d0")                    // d8   = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
            __ASM_EMIT("vadd.f32        d8, d8, d16")                   // d8   = L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
            __ASM_EMIT("vmul.f32        d8, d8, d0")                    // d8   = A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
            __ASM_EMIT("sub             %[fptr], $0xd0")                // fptr -= 52
            __ASM_EMIT("vmul.f32        d8, d8, d0")                    // d8   = A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
            __ASM_EMIT("vmla.f32        d8, d4, d18")                   // d8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
            __ASM_EMIT("vadd.f32        d8, d8, d0")                    // d8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))) + A
            // Apply values to axes, d0 = log(abs(v*zero))
            __ASM_EMIT("vldm            %[x], {s8}")                    // d4 = x
            __ASM_EMIT("vldm            %[y], {s0}")                    // d0 = y
            __ASM_EMIT("vld1.32         {q6-q7}, [%[params]]")          // d12 = norm_x, d14 = norm_y
            __ASM_EMIT("vmla.f32        d4, d8, d12")                   // d4 = x + log(abs(v*zero)) * norm_x
            __ASM_EMIT("vmla.f32        d0, d8, d14")                   // d0 = y + log(abs(v*zero)) * norm_y
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("vstm            %[x]!, {s8}")                   // x++
            __ASM_EMIT("vstm            %[y]!, {s0}")                   // y++
            __ASM_EMIT("bge             7b")

            __ASM_EMIT("8:")

            : [v] "+r" (v), [x] "+r" (x) , [y] "+r" (y),
              [count] "+r" (count),
              [iptr] "+r" (iptr), [fptr] "+r" (fptr)
            : [params] "r" (params)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void rgba32_to_bgra32(void *dst, const void *src, size_t count)
    {
        IF_ARCH_ARM(
            uint32_t mask;
            uint32_t t1, t2;
        );

        ARCH_ARM_ASM(
            // 64x blocks
            __ASM_EMIT("subs        %[count], $64")
            __ASM_EMIT("blo         2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vld4.8      {q0-q1}, [%[src]]!") // d0 = R, d1 = G, d2 = B, d3 = A
            __ASM_EMIT("vld4.8      {q2-q3}, [%[src]]!")
            __ASM_EMIT("vswp        d0, d2")
            __ASM_EMIT("vld4.8      {q4-q5}, [%[src]]!")
            __ASM_EMIT("vst4.8      {q0-q1}, [%[dst]]!")
            __ASM_EMIT("vswp        d4, d6")
            __ASM_EMIT("vld4.8      {q6-q7}, [%[src]]!")
            __ASM_EMIT("vst4.8      {q2-q3}, [%[dst]]!")
            __ASM_EMIT("vswp        d8, d10")
            __ASM_EMIT("vld4.8      {q8-q9}, [%[src]]!")
            __ASM_EMIT("vst4.8      {q4-q5}, [%[dst]]!")
            __ASM_EMIT("vswp        d12, d14")
            __ASM_EMIT("vld4.8      {q10-q11}, [%[src]]!")
            __ASM_EMIT("vst4.8      {q6-q7}, [%[dst]]!")
            __ASM_EMIT("vswp        d16, d18")
            __ASM_EMIT("vld4.8      {q12-q13}, [%[src]]!")
            __ASM_EMIT("vst4.8      {q8-q9}, [%[dst]]!")
            __ASM_EMIT("vswp        d20, d22")
            __ASM_EMIT("vld4.8      {q14-q15}, [%[src]]!")
            __ASM_EMIT("vst4.8      {q10-q11}, [%[dst]]!")
            __ASM_EMIT("vswp        d24, d26")
            __ASM_EMIT("vswp        d28, d30")
            __ASM_EMIT("vst4.8      {q12-q13}, [%[dst]]!")
            __ASM_EMIT("vst4.8      {q14-q15}, [%[dst]]!")
            __ASM_EMIT("subs        %[count], $64")
            __ASM_EMIT("bhs         1b")

            // 32x blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $32")
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vld4.8      {q0-q1}, [%[src]]!") // d0 = R, d1 = G, d2 = B, d3 = A
            __ASM_EMIT("vld4.8      {q2-q3}, [%[src]]!")
            __ASM_EMIT("vswp        d0, d2")
            __ASM_EMIT("vld4.8      {q4-q5}, [%[src]]!")
            __ASM_EMIT("vst4.8      {q0-q1}, [%[dst]]!")
            __ASM_EMIT("vswp        d4, d6")
            __ASM_EMIT("vld4.8      {q6-q7}, [%[src]]!")
            __ASM_EMIT("vst4.8      {q2-q3}, [%[dst]]!")
            __ASM_EMIT("vswp        d8, d10")
            __ASM_EMIT("vswp        d12, d14")
            __ASM_EMIT("vst4.8      {q4-q5}, [%[dst]]!")
            __ASM_EMIT("vst4.8      {q6-q7}, [%[dst]]!")
            __ASM_EMIT("sub         %[count], $32")

            // 16x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $16")
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vld4.8      {q0-q1}, [%[src]]!") // d0 = R, d1 = G, d2 = B, d3 = A
            __ASM_EMIT("vld4.8      {q2-q3}, [%[src]]!")
            __ASM_EMIT("vswp        d0, d2")
            __ASM_EMIT("vswp        d4, d6")
            __ASM_EMIT("vst4.8      {q0-q1}, [%[dst]]!")
            __ASM_EMIT("vst4.8      {q2-q3}, [%[dst]]!")
            __ASM_EMIT("sub         %[count], $16")

            // 8x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $8")
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("vld4.8      {q0-q1}, [%[src]]!") // d0 = R, d1 = G, d2 = B, d3 = A
            __ASM_EMIT("vswp        d0, d2")
            __ASM_EMIT("vst4.8      {q0-q1}, [%[dst]]!")
            __ASM_EMIT("sub         %[count], $8")

            // 4x blocks
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], $4")
            __ASM_EMIT("blt         10f")
            __ASM_EMIT("vld2.8      {q0}, [%[src]]!") // d0 = RB, d1 = GA
            __ASM_EMIT("vshl.i16    d2, d0, $8")
            __ASM_EMIT("vshr.u16    d0, d0, $8")
            __ASM_EMIT("vorr        d0, d2")
            __ASM_EMIT("vst2.8      {q0}, [%[dst]]!")
            __ASM_EMIT("sub         %[count], $4")

            // 1x blocks
            __ASM_EMIT("10:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         12f")
            __ASM_EMIT("mov         %[mask], $0xff")
            __ASM_EMIT("orr         %[mask], $0xff0000")           // mask = ff 00 ff 00
            __ASM_EMIT("11:")
            __ASM_EMIT("ldr         %[t1], [%[src]], $4")               // t1 = R G B A
            __ASM_EMIT("and         %[t2], %[t1], %[mask]")             // t2 = R 0 B 0
            __ASM_EMIT("and         %[t1], %[t1], %[mask], lsl $8")     // t1 = 0 G 0 A
            __ASM_EMIT("orr         %[t1], %[t1], %[t2], ror $16")      // t1 = B G R A
            __ASM_EMIT("str         %[t1], [%[dst]], $4")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("bge         11b")

            __ASM_EMIT("12:")
            : [src] "+r" (src), [dst] "+r" (dst), [count] "+r" (count),
              [t1] "=&r" (t1), [t2] "=&r" (t2), [mask] "=&r" (mask)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#define FILL4_CORE \
    __ASM_EMIT("vld1.32     {q0}, [%[c4]]")     /* q0 = c0 c1 c2 c3 */ \
    __ASM_EMIT("vmov        q1, q0")            /* q1 = c0 c1 c2 c3 */ \
    __ASM_EMIT("vmov        q2, q0")            /* q2 = c0 c1 c2 c3 */ \
    __ASM_EMIT("vmov        q3, q1")            /* q3 = c0 c1 c2 c3 */ \
    \
    /* 8x blocks */ \
    __ASM_EMIT("subs        %[count], $8") \
    __ASM_EMIT("blo         2f") \
    __ASM_EMIT("vmov        q4, q0")            /* q4 = c0 c1 c2 c3 */ \
    __ASM_EMIT("vmov        q5, q1")            /* q5 = c0 c1 c2 c3 */ \
    __ASM_EMIT("vmov        q6, q2")            /* q6 = c0 c1 c2 c3 */ \
    __ASM_EMIT("vmov        q7, q3")            /* q7 = c0 c1 c2 c3 */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vstm        %[dst], {q0-q7}") \
    __ASM_EMIT("subs        %[count], $8") \
    __ASM_EMIT("add         %[dst], $0x80") \
    __ASM_EMIT("bhs         1b") \
    /* 4x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], $4") \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("vstm        %[dst], {q0-q3}") \
    __ASM_EMIT("sub         %[count], $4") \
    __ASM_EMIT("add         %[dst], $0x40") \
    /* 2x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], $2") \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("vst1.32     {q0-q1}, [%[dst]]") \
    __ASM_EMIT("sub         %[count], $2") \
    __ASM_EMIT("add         %[dst], $0x20") \
    /* 1x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], $1") \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("vst1.32     {q0}, [%[dst]]") \
    __ASM_EMIT("8:")

    void fill_rgba(float *dst, float r, float g, float b, float a, size_t count)
    {
        IF_ARCH_ARM(
            float c4[4] __lsp_aligned16;
            c4[0] = r;
            c4[1] = g;
            c4[2] = b;
            c4[3] = a;
        )

        ARCH_ARM_ASM
        (
            FILL4_CORE
            : [dst] "+r" (dst), [count] "+r" (count)
            : [c4] "r" (&c4[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7"
        );
    }

    void fill_hsla(float *dst, float h, float s, float l, float a, size_t count)
    {
        IF_ARCH_ARM(
            float c4[4] __lsp_aligned16;
            c4[0] = h;
            c4[1] = s;
            c4[2] = l;
            c4[3] = a;
        );

        ARCH_ARM_ASM
        (
            FILL4_CORE
            : [dst] "+r" (dst), [count] "+r" (count)
            : [c4] "r" (&c4[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7"
        );
    }

#undef FILL4_CORE

IF_ARCH_ARM(
    static const float HSL_RGB[] =
    {
        FVEC4(0.5f),                // 1/2
        FVEC4(0.333333333333f),     // 1/3
        FVEC4(1.0f),                // 1
        FVEC4(6.0f),                // 6
        FVEC4(0.166666666667f),     // 1/6
        FVEC4(0.666666666667f)      // 2/3
    };
)

#define HSLA_TO_RGBA_CORE   \
    /*  q10 = 1/2, q11 = 1/3, q12 = 1, q13 = 6, q14 = 1/6, q15 = 2/3 */ \
    /*  q0 = h0 h1 h2 h3 = H */ \
    /*  q1 = s0 s1 s2 s3 = S */ \
    /*  q2 = l0 l1 l2 l3 = L */ \
    /*  q3 = a0 a1 a2 a3 = A */ \
    /*  Calc temp1 (T1) and temp2 (T2) */ \
    __ASM_EMIT("vadd.f32        q4, q1, q2")                /* q4 = L+S */ \
    __ASM_EMIT("vadd.f32        q9, q2, q2")                /* q9 = L+L */ \
    __ASM_EMIT("vmul.f32        q5, q1, q2")                /* q5 = L*S */ \
    __ASM_EMIT("vsub.f32        q4, q4, q5")                /* q4 = L+S - L*S */ \
    __ASM_EMIT("vadd.f32        q7, q2, q5")                /* q7 = L + L*S */ \
    __ASM_EMIT("vclt.f32        q8, q2, q10")               /* q8 = [L < 0.5] */ \
    __ASM_EMIT("vbif            q4, q7, q8")                /* q4 = T2 = ((L+S-L*S) & [L<0.5]) | ((L+L*S) & [L>=0.5]) */ \
    __ASM_EMIT("vmov            q1, q0")                    /* q1 = TG = H */ \
    __ASM_EMIT("vsub.f32        q5, q9, q4")                /* q5 = T1 = L+L-T2 */ \
    __ASM_EMIT("vadd.f32        q0, q0, q11")               /* q0 = H + 1/3 */ \
    __ASM_EMIT("vsub.f32        q2, q1, q11")               /* q2 = H - 1/3 */ \
    __ASM_EMIT("vcgt.f32        q9, q0, q12")               /* q9 = [H+1/3 > 1] */ \
    __ASM_EMIT("veor            q8, q8")                    /* q8 = 0 */ \
    __ASM_EMIT("vsub.f32        q6, q0, q12")               /* q6 = H + 1/3 - 1 */ \
    __ASM_EMIT("vadd.f32        q7, q1, q12")               /* q7 = H - 1/3 + 1 */ \
    __ASM_EMIT("vclt.f32        q8, q2, q8")                /* q8 = [H-1/3 < 0] */ \
    __ASM_EMIT("vbit            q0, q6, q9")                /* q0 = TR = ((H+1/3) & [H+1/3 <= 1]) | ((H+1/3-1) & [H+1/3 > 1]) */ \
    __ASM_EMIT("vbit            q2, q7, q8")                /* q2 = TB = ((H-1/3) & [H-1/3 >= 0]) | ((H-1/3+1) & [H-1/3 < 0]) */ \
    __ASM_EMIT("vsub.f32        q6, q4, q5")                /* q6 = T2 - T1 */ \
    __ASM_EMIT("vmul.f32        q6, q6, q13")               /* q6 = K = (T2 - T1)*6 */ \
    /* q0 = TR */ \
    /* q1 = TG */ \
    /* q2 = TB */ \
    /* q3 = A */ \
    /* q4 = T2 */ \
    /* q5 = T1 */ \
    /* q6 = K */ \
    __ASM_EMIT("vmov            q7, q5")                    /* q7 = T1 */ \
    __ASM_EMIT("vmov            q8, q5")                    /* q8 = T1 */ \
    __ASM_EMIT("vsub.f32        q9, q15, q0")               /* q9 = 2/3 - TR */ \
    __ASM_EMIT("vmla.f32        q7, q6, q0")                /* q7 = T1 + K*TR */ \
    __ASM_EMIT("vmla.f32        q8, q6, q9")                /* q8 = T1 + K*(2/3-TR) */ \


/*
        //Set the temporary values
        if  (L < HSL_RGB_0_5)
            temp2 = (L + S) - (L * S)
        else
            temp2 = L + (L * S);

        temp1 = L + L - temp2;

        tempr = H + HSL_RGB_1_3;
        tempg = H;
        tempb = H - HSL_RGB_1_3;

        if (tempr > 1.0f)
            tempr   -= 1.0f;
        if (tempb < 0.0f)
            tempb   += 1.0f;

        k = (temp2 - temp1) * 6.0f;

        //Red
        if (tempr < HSL_RGB_0_5)
            R = (tempr < HSL_RGB_1_6) ? temp1 + k * tempr : temp2;
        else
            R = (tempr < HSL_RGB_2_3) ? temp1 + k * (HSL_RGB_2_3 - tempr) : temp1;

        //Green
        if (tempg < HSL_RGB_0_5)
            G = (tempg < HSL_RGB_1_6) ? temp1 + k * tempg : temp2;
        else
            G = (tempg < HSL_RGB_2_3) ? temp1 + k * (HSL_RGB_2_3 - tempg) : temp1;

        //Blue
        if (tempb < HSL_RGB_0_5)
            B = (tempb < HSL_RGB_1_6) ? temp1 + k * tempb : temp2;
        else
            B = (tempb < HSL_RGB_2_3) ? temp1 + k * (HSL_RGB_2_3 - tempb) : temp1;
*/

#if 0
    __ASM_EMIT("addps           %%xmm2, %%xmm6")            /*  xmm6 = L + S */ \
    __ASM_EMIT("mulps           %%xmm1, %%xmm7")            /*  xmm7 = L * S */ \
    __ASM_EMIT("subps           %%xmm7, %%xmm6")            /*  xmm6 = L + S - L * S */ \
    __ASM_EMIT("addps           %%xmm2, %%xmm7")            /*  xmm7 = L + L * S */ \
    __ASM_EMIT("movaps          %%xmm2, %%xmm5")            /*  xmm5 = L */ \
    __ASM_EMIT("movaps          0x00 + %[XC], %%xmm4")      /*  xmm4 = 0.5 */ \
    __ASM_EMIT("addps           %%xmm2, %%xmm5")            /*  xmm5 = L + L */ \
    __ASM_EMIT("cmpps           $2, %%xmm2, %%xmm4")        /*  xmm4 = [L >= 0.5f] */ \
    __ASM_EMIT("andps           %%xmm4, %%xmm6")            /*  xmm6 = [L >= 0.5f] & (L+S - L*S) */ \
    __ASM_EMIT("andnps          %%xmm7, %%xmm4")            /*  xmm4 = [L < 0.5f] & (L + L*S) */ \
    __ASM_EMIT("orps            %%xmm6, %%xmm4")            /*  xmm4 = T2 = ([0.5f < L] & (L + L*S)) | ([0.5f >= L] & (L+S - L*S)) */ \
    __ASM_EMIT("movaps          %%xmm0, %%xmm1")            /*  xmm1 = TG = H */ \
    __ASM_EMIT("subps           %%xmm4, %%xmm5")            /*  xmm5 = T1 = L + L - T2 */ \
    __ASM_EMIT("movaps          %%xmm0, %%xmm2")            /*  xmm2 = H */ \
    \
    __ASM_EMIT("movaps          0x10 + %[XC], %%xmm6")      /*  xmm6 = 1/3 */ \
    __ASM_EMIT("addps           %%xmm6, %%xmm0")            /*  xmm0 = H + 1/3 */ \
    __ASM_EMIT("subps           %%xmm6, %%xmm2")            /*  xmm2 = H - 1/3 */ \
    \
    __ASM_EMIT("movaps          0x20 + %[XC], %%xmm7")      /*  xmm7 = 1 */ \
    __ASM_EMIT("movaps          %%xmm0, %%xmm6")            /*  xmm6 = H + 1/3 */ \
    __ASM_EMIT("subps           %%xmm7, %%xmm6")            /*  xmm6 = H + 1/3 - 1 */ \
    __ASM_EMIT("cmpps           $5, %%xmm0, %%xmm7")        /*  xmm7 = [(H + 1/3) <= 1] */ \
    __ASM_EMIT("andps           %%xmm7, %%xmm0")            /*  xmm0 = (H + 1/3) & [(H + 1/3) <= 1] */ \
    __ASM_EMIT("andnps          %%xmm6, %%xmm7")            /*  xmm7 = (H + 1/3 - 1) & [(H + 1/3) > 1] */ \
    __ASM_EMIT("orps            %%xmm7, %%xmm0")            /*  xmm0 = TR = ((H + 1/3) & [(H + 1/3) <= 1]) | ((H + 1/3 - 1) & [(H + 1/3) > 1]) */ \
    \
    __ASM_EMIT("movaps          0x20 + %[XC], %%xmm7")      /*  xmm7 = 1 */ \
    __ASM_EMIT("movaps          %%xmm2, %%xmm6")            /*  xmm6 = H - 1/3 */ \
    __ASM_EMIT("addps           %%xmm7, %%xmm6")            /*  xmm6 = H - 1/3 + 1 */ \
    __ASM_EMIT("xorps           %%xmm7, %%xmm7")            /*  xmm7 = 0 */ \
    __ASM_EMIT("cmpps           $2, %%xmm2, %%xmm7")        /*  xmm7 = [(H - 1/3) >= 0] */ \
    __ASM_EMIT("andps           %%xmm7, %%xmm2")            /*  xmm2 = (H - 1/3) & [(H - 1/3) >= 0] */ \
    __ASM_EMIT("andnps          %%xmm6, %%xmm7")            /*  xmm6 = (H - 1/3 + 1) & [(H - 1/3) < 0] */ \
    __ASM_EMIT("orps            %%xmm7, %%xmm2")            /*  xmm2 = TB = ((H - 1/3) & [(H - 1/3) >= 0]) | ((H - 1/3 + 1) & [(H - 1/3) < 0]) */ \
    \
    __ASM_EMIT("movaps          %%xmm4, %%xmm6")            /*  xmm6 = T2 */ \
    __ASM_EMIT("subps           %%xmm5, %%xmm6")            /*  xmm6 = T2 - T1 */ \
    __ASM_EMIT("mulps           0x30 + %[XC], %%xmm6")      /*  xmm6 = K = (T2 - T1)*6.0 */ \
    \
    /*  xmm0 = TR */ \
    /*  xmm1 = TG */ \
    /*  xmm2 = TB */ \
    /*  xmm3 = A */ \
    /*  xmm4 = T2 */ \
    /*  xmm5 = T1 */ \
    /*  xmm6 = K */ \
    __ASM_EMIT("movaps          %%xmm0, 0x00(%[HSLM])")     /*  TR */ \
    __ASM_EMIT("movaps          %%xmm1, 0x10(%[HSLM])")     /*  TG */ \
    __ASM_EMIT("movaps          %%xmm2, 0x20(%[HSLM])")     /*  TB */ \
    __ASM_EMIT("movaps          %%xmm3, 0x30(%[HSLM])")     /*  A */ \
    __ASM_EMIT("movaps          %%xmm4, 0x40(%[HSLM])")     /*  T2 */ \
    __ASM_EMIT("movaps          %%xmm5, 0x50(%[HSLM])")     /*  T1 */ \
    \
    __ASM_EMIT("movaps          0x50 + %[XC], %%xmm3")      /*  xmm3 = 2/3 */ \
    __ASM_EMIT("movaps          %%xmm5, %%xmm7")            /*  xmm7 = T1 */ \
    __ASM_EMIT("mulps           %%xmm6, %%xmm0")            /*  xmm0 = k*TR */ \
    __ASM_EMIT("mulps           %%xmm6, %%xmm3")            /*  xmm3 = K * 2/3 */ \
    __ASM_EMIT("mulps           %%xmm6, %%xmm1")            /*  xmm1 = k*TG */ \
    __ASM_EMIT("movaps          %%xmm3, %%xmm4")            /*  xmm4 = K * 2/3 */ \
    __ASM_EMIT("mulps           %%xmm6, %%xmm2")            /*  xmm2 = k*TB */ \
    __ASM_EMIT("movaps          %%xmm3, %%xmm5")            /*  xmm5 = K * 2/3 */ \
    __ASM_EMIT("subps           %%xmm0, %%xmm3")            /*  xmm3 = K * (2/3 - TR) */ \
    __ASM_EMIT("subps           %%xmm1, %%xmm4")            /*  xmm4 = K * (2/3 - TG) */ \
    __ASM_EMIT("subps           %%xmm2, %%xmm5")            /*  xmm5 = K * (2/3 - TB) */ \
    __ASM_EMIT("addps           %%xmm7, %%xmm0")            /*  xmm0 = KTR = k*TR + T1 */ \
    __ASM_EMIT("addps           %%xmm7, %%xmm1")            /*  xmm1 = KTG = k*TG + T1 */ \
    __ASM_EMIT("addps           %%xmm7, %%xmm2")            /*  xmm2 = KTB = k*TB + T1 */ \
    __ASM_EMIT("addps           %%xmm7, %%xmm3")            /*  xmm3 = RTR = K * (2/3 - TR) + T1 */ \
    __ASM_EMIT("addps           %%xmm7, %%xmm4")            /*  xmm4 = RTG = K * (2/3 - TG) + T1 */ \
    __ASM_EMIT("addps           %%xmm7, %%xmm5")            /*  xmm5 = RTB = K * (2/3 - TB) + T1 */ \
    \
    __ASM_EMIT("movaps          %%xmm0, 0x60(%[HSLM])")     /*  KTR */ \
    __ASM_EMIT("movaps          %%xmm1, 0x70(%[HSLM])")     /*  KTG */ \
    __ASM_EMIT("movaps          %%xmm2, 0x80(%[HSLM])")     /*  KTB */ \
    __ASM_EMIT("movaps          %%xmm3, 0x90(%[HSLM])")     /*  RTR */ \
    __ASM_EMIT("movaps          %%xmm4, 0xa0(%[HSLM])")     /*  RTG */ \
    __ASM_EMIT("movaps          %%xmm5, 0xb0(%[HSLM])")     /*  RTB */ \
    \
    /*  Now we have enough data to process */ \
    __ASM_EMIT("movaps          0x00(%[HSLM]), %%xmm0")     /*  xmm0 = TR */ \
    __ASM_EMIT("movaps          0x10(%[HSLM]), %%xmm1")     /*  xmm1 = TG */ \
    __ASM_EMIT("movaps          0x20(%[HSLM]), %%xmm2")     /*  xmm2 = TB */ \
    __ASM_EMIT("movaps          0x30(%[HSLM]), %%xmm3")     /*  xmm3 = A */ \
    \
    /*  Process red */ \
    __ASM_EMIT("movaps          %%xmm0, %%xmm5")            /*  xmm5 = TR */ \
    __ASM_EMIT("movaps          %%xmm0, %%xmm6")            /*  xmm6 = TR */ \
    __ASM_EMIT("cmpps           $1, 0x00 + %[XC], %%xmm0")  /*  xmm0 = [ TR < 0.5 ] */ \
    __ASM_EMIT("cmpps           $1, 0x40 + %[XC], %%xmm5")  /*  xmm5 = [ TR < 1/6 ] */ \
    __ASM_EMIT("cmpps           $1, 0x50 + %[XC], %%xmm6")  /*  xmm6 = [ TR < 2/3 ] */ \
    __ASM_EMIT("movaps          %%xmm5, %%xmm7")            /*  xmm7 = [ TR < 1/6 ] */ \
    __ASM_EMIT("andnps          %%xmm0, %%xmm7")            /*  xmm7 = [ TR >= 1/6 ] & [ TR < 0.5 ] */ \
    __ASM_EMIT("andnps          %%xmm6, %%xmm0")            /*  xmm0 = [ TR >= 0.5 ] & [ TR < 2/3 ] */ \
    __ASM_EMIT("andps           0x60(%[HSLM]), %%xmm5")     /*  xmm5 = KTR & [ TR < 1/6 ] */ \
    __ASM_EMIT("andps           0x40(%[HSLM]), %%xmm7")     /*  xmm7 = T2 & [ TR < 1/6 ] */ \
    __ASM_EMIT("andnps          0x50(%[HSLM]), %%xmm6")     /*  xmm6 = T1 & [ TR >= 2/3 ] */ \
    __ASM_EMIT("andps           0x90(%[HSLM]), %%xmm0")     /*  xmm0 = RTR & [ TR >= 0.5 ] & [ TR < 2/3 ] */ \
    __ASM_EMIT("orps            %%xmm7, %%xmm6") \
    __ASM_EMIT("orps            %%xmm5, %%xmm0") \
    __ASM_EMIT("orps            %%xmm6, %%xmm0") \
    \
    /*  Process green */ \
    __ASM_EMIT("movaps          %%xmm1, %%xmm5")            /*  xmm5 = TG */ \
    __ASM_EMIT("movaps          %%xmm1, %%xmm6")            /*  xmm6 = TG */ \
    __ASM_EMIT("cmpps           $1, 0x00 + %[XC], %%xmm1")  /*  xmm1 = [ TG < 0.5 ] */ \
    __ASM_EMIT("cmpps           $1, 0x40 + %[XC], %%xmm5")  /*  xmm5 = [ TG < 1/6 ] */ \
    __ASM_EMIT("cmpps           $1, 0x50 + %[XC], %%xmm6")  /*  xmm6 = [ TG < 2/3 ] */ \
    __ASM_EMIT("movaps          %%xmm5, %%xmm7")            /*  xmm7 = [ TG < 1/6 ] */ \
    __ASM_EMIT("andnps          %%xmm1, %%xmm7")            /*  xmm7 = [ TG >= 1/6 ] & [ TG < 0.5 ] */ \
    __ASM_EMIT("andnps          %%xmm6, %%xmm1")            /*  xmm1 = [ TG >= 0.5 ] & [ TG < 2/3 ] */ \
    __ASM_EMIT("andps           0x70(%[HSLM]), %%xmm5")     /*  xmm5 = KTG & [ TG < 1/6 ] */ \
    __ASM_EMIT("andps           0x40(%[HSLM]), %%xmm7")     /*  xmm7 = T2 & [ TG < 1/6 ] */ \
    __ASM_EMIT("andnps          0x50(%[HSLM]), %%xmm6")     /*  xmm6 = T1 & [ TG >= 2/3 ] */ \
    __ASM_EMIT("andps           0xa0(%[HSLM]), %%xmm1")     /*  xmm1 = RTG & [ TG >= 0.5 ] & [ TG < 2/3 ] */ \
    __ASM_EMIT("orps            %%xmm7, %%xmm6") \
    __ASM_EMIT("orps            %%xmm5, %%xmm1") \
    __ASM_EMIT("orps            %%xmm6, %%xmm1") \
    \
    /*  Process blue */ \
    __ASM_EMIT("movaps          %%xmm2, %%xmm5")            /*  xmm5 = TB */ \
    __ASM_EMIT("movaps          %%xmm2, %%xmm6")            /*  xmm6 = TB */ \
    __ASM_EMIT("cmpps           $1, 0x00 + %[XC], %%xmm2")  /*  xmm2 = [ TB < 0.5 ] */ \
    __ASM_EMIT("cmpps           $1, 0x40 + %[XC], %%xmm5")  /*  xmm5 = [ TB < 1/6 ] */ \
    __ASM_EMIT("cmpps           $1, 0x50 + %[XC], %%xmm6")  /*  xmm6 = [ TB < 2/3 ] */ \
    __ASM_EMIT("movaps          %%xmm5, %%xmm7")            /*  xmm7 = [ TB < 1/6 ] */ \
    __ASM_EMIT("andnps          %%xmm2, %%xmm7")            /*  xmm7 = [ TB >= 1/6 ] & [ TB < 0.5 ] */ \
    __ASM_EMIT("andnps          %%xmm6, %%xmm2")            /*  xmm2 = [ TB >= 0.5 ] & [ TB < 2/3 ] */ \
    __ASM_EMIT("andps           0x80(%[HSLM]), %%xmm5")     /*  xmm5 = KTB & [ TB < 1/6 ] */ \
    __ASM_EMIT("andps           0x40(%[HSLM]), %%xmm7")     /*  xmm7 = T2 & [ TB < 1/6 ] */ \
    __ASM_EMIT("andnps          0x50(%[HSLM]), %%xmm6")     /*  xmm6 = T1 & [ TB >= 2/3 ] */ \
    __ASM_EMIT("andps           0xb0(%[HSLM]), %%xmm2")     /*  xmm2 = RTB & [ TB >= 0.5 ] & [ TB < 2/3 ] */ \
    __ASM_EMIT("orps            %%xmm7, %%xmm6") \
    __ASM_EMIT("orps            %%xmm5, %%xmm2") \
    __ASM_EMIT("orps            %%xmm6, %%xmm2") \

#endif


    void hsla_to_rgba(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vldm            %[XC], {q10-q15}")          // q10 = 1/2, q11 = 1/3, q12 = 1, q13 = 6, q14 = 1/6, q15 = 2/3
            __ASM_EMIT("subs            %[count], $4")
            __ASM_EMIT("blo             2f")

            //-----------------------------------------------------------------
            // 4x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld4.32         {q0-q1}, [%[src]]!")        // q0 = h0 h1 s0 s1, q1 = l0 l1 a0 a1
            __ASM_EMIT("vld4.32         {q2-q3}, [%[src]]!")        // q2 = h2 h3 s2 s3, q3 = l2 l3 a2 a3
            __ASM_EMIT("vswp            d1, d4")                    // q0 = h0 h1 h2 h3, q2 = s0 s1 s2 s3
            __ASM_EMIT("vswp            d3, d6")                    // q1 = l0 l1 l2 l3, q3 = a0 a1 a2 a3

            HSLA_TO_RGBA_CORE


            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [XC] "r" (&HSL_RGB[0])
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

    }
}


#endif /* DSP_ARCH_ARM_NEON_D32_GRAPHICS_H_ */
