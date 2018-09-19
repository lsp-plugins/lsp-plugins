/*
 * graphics.h
 *
 *  Created on: 17 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_GRAPHICS_H_
#define DSP_ARCH_ARM_NEON_D32_GRAPHICS_H_

namespace neon_d32
{
    void axis_apply_log(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count)
    {
        IF_ARCH_ARM(
            float params[16] __lsp_aligned16;
            params[0] = zero;
            params[1] = norm_x;
            params[2] = norm_y;
        );

        //---------------------------------------------------------------
        // Prepare constants
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vldm            {s0, s8, s12}, %[params]")
            __ASM_EMIT("vmov            s1, s0")
            __ASM_EMIT("vmov            s9, s8")
            __ASM_EMIT("vmov            s13, s12")
            __ASM_EMIT("add             %[params], $0x10")              // params += 4
            __ASM_EMIT("vmov            d1, d0")                        // q0 = zero
            __ASM_EMIT("vmov            d5, d4")                        // q2 = norm_x
            __ASM_EMIT("vmov            d7, d6")                        // q3 = norm_y
            __ASM_EMIT("vst1.32         {q0}, [%[params]]!")            // params += 4
            __ASM_EMIT("vst1.32         {q2-q3}, [%[params]]")

            __ASM_EMIT("subs            %[count], $4")
            __ASM_EMIT("blo             2f")

            // TODO: 16x blocks
            // TODO: 8x blocks

            // Do x4 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("sub             %[params], $0x10")              // params -= 4
            __ASM_EMIT("vld1.32         {q4}, [%[v]]!")                 // q4 = v
            __ASM_EMIT("vld1.32         {q3}, [%[fptr]]!")              // q3 = X_AMP, fptr += 4
            __ASM_EMIT("vabs.f32        q4, q4")                        // q4 = abs(v)
            __ASM_EMIT("vld1.32         {q0}, [%[params]]!")            // q0 = zero, params += 4
            __ASM_EMIT("vmax.f32        q4, q3")                        // q4 = max(X_AMP, abs(v)), ignores denormalized values
            __ASM_EMIT("vmul.f32        q0, q4, q0")                    // q0 = max(X_AMP, abs(v)) * zero
            // Step 2: parse float value
            __ASM_EMIT("vld1.32         {q12}, %[iptr]!")               // q12 = MANT_MASK, iptr += 4
            __ASM_EMIT("vshr.u32        q4, q0, $23")                   // q4  = frac(v)
            __ASM_EMIT("vld1.32         {q13}, %[iptr]!")               // q13 = FRAC_SUB= 127, iptr += 4
            __ASM_EMIT("vand.u32        q8, q0, q12")                   // q8  = mant(v)
            __ASM_EMIT("vld1.32         {q14}, %[fptr]!")               // q14 = 0.5f
            __ASM_EMIT("vsub.i32        q4, q4, q13")                   // q4  = frac(v) - 127
            __ASM_EMIT("vorr.i32        q8, q8, q14")                   // q8  = V = mant(v)+0.5
            __ASM_EMIT("vcvt.f32.i32    q4, q4")                        // q4  = E = float(frac(v)-127)
            // Prepare logarithm approximation calculations
            __ASM_EMIT("vld1.32         {q12}, %[fptr]!")               // q12  = sqrt(1/2), fptr += 4
            __ASM_EMIT("vclt.f32        q0, q8, q12")                   // q0   = [ V < sqrt(1/2) ]
            __ASM_EMIT("vld1.32         {q13}, %[fptr]!")               // q13  = 1.0
            __ASM_EMIT("vand.u32        q0, q0, q8")                    // q0   = V * [ V < sqrt(1/2) ]
            __ASM_EMIT("vadd.f32        q0, q0, q8")                    // q0   = V + V * [ V < sqrt(1/2) ]
            __ASM_EMIT("vclt.f32        q8, q8, q12")                   // q8   = [ V < sqrt(1/2) ]
            __ASM_EMIT("vsub.f32        q0, q0, q13")                   // q0   = A = V + V * [ V < sqrt(1/2) ] - 1.0
            __ASM_EMIT("vmvn            q8, q8")                        // q8   = [ V >= sqrt(1/2) ]
            __ASM_EMIT("vmla.f32        q4, q8, q13")                   // q4   = B = E + 1.0 * [ V >= sqrt(1/2) ]
            __ASM_EMIT("vmov            q8, q0")                        // q8   = A
            // Calculate logarithmic values
            __ASM_EMIT("vld1.32         {q12-q13}, %[fptr]!")           // q12  = L0, q13 = L1, fptr += 8
            __ASM_EMIT("vmul.f32        q0, q8, q12")                   // q0   = L0*A
            __ASM_EMIT("vadd.f32        q0, q0, q13")                   // q0   = L1+L0*A
            __ASM_EMIT("vld1.32         {q12-q13}, %[fptr]!")           // q12  = L2, q13 = L3, fptr += 8
            __ASM_EMIT("vmul.f32        q0, q0, q8")                    // q0   = A*(L1+L0*A)
            __ASM_EMIT("vadd.f32        q0, q0, q12")                   // q0   = L2+A*(L1+L0*A)
            __ASM_EMIT("vmul.f32        q0, q0, q8")                    // q0   = A*(L2+A*(L1+L0*A))
            __ASM_EMIT("vadd.f32        q0, q0, q13")                   // q0   = L3+A*(L2+A*(L1+L0*A))
            __ASM_EMIT("vld1.32         {q12-q13}, %[fptr]!")           // q12  = L4, q13 = L5, fptr += 8
            __ASM_EMIT("vmul.f32        q0, q0, q8")                    // q0   = A*(L3+A*(L2+A*(L1+L0*A)))
            __ASM_EMIT("vadd.f32        q0, q0, q12")                   // q0   = L4+A*(L3+A*(L2+A*(L1+L0*A)))
            __ASM_EMIT("vmul.f32        q0, q0, q8")                    // q0   = A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
            __ASM_EMIT("vadd.f32        q0, q0, q13")                   // q0   = L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
            __ASM_EMIT("vld1.32         {q12-q13}, %[fptr]!")           // q12  = L6, q13 = L7, fptr += 8
            __ASM_EMIT("vmul.f32        q0, q0, q8")                    // q0   = A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
            __ASM_EMIT("vadd.f32        q0, q0, q12")                   // q0   = L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
            __ASM_EMIT("vmul.f32        q0, q0, q8")                    // q0   = A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
            __ASM_EMIT("vadd.f32        q0, q0, q13")                   // q0   = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
            __ASM_EMIT("vld1.32         {q12-q13}, %[fptr]!")           // q12  = L8, q13 = L9, fptr += 8
            __ASM_EMIT("vmul.f32        q0, q0, q8")                    // q0   = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
            __ASM_EMIT("vadd.f32        q0, q0, q12")                   // q0   = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
            __ASM_EMIT("vmul.f32        q0, q0, q8")                    // q0   = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
            __ASM_EMIT("vadd.f32        q0, q0, q13")                   // q0   = L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
            __ASM_EMIT("vmul.f32        q0, q0, q8")                    // q0   = A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
            __ASM_EMIT("vld1.32         {q12}, %[fptr]")                // q12  = (LXE + LN2)
            __ASM_EMIT("vmul.f32        q0, q0, q8")                    // q0   = A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
            __ASM_EMIT("vmla.f32        q0, q4, q12")                   // q0   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
            __ASM_EMIT("vadd.f32        q0, q0, q8")                    // q0   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))) + A
            __ASM_EMIT("sub             %[iptr], $0x20")                // iptr -= 8
            __ASM_EMIT("sub             %[fptr], $0xe0")                // fptr -= 48
            // Apply values to axes, q0 = log(abs(v*zero))
            __ASM_EMIT("vld1.32         {q4}, [%[x]]")                  // q4 = x
            __ASM_EMIT("vld1.32         {q8}, [%[y]]")                  // q8 = y
            __ASM_EMIT("vld1.32         {q12-q13}, [%[params]]")        // q12 = norm_x, q13 = norm_y
            __ASM_EMIT("vmla.f32        q4, q0, q12")                   // q4 = x + log(abs(v*zero)) * norm_x
            __ASM_EMIT("vmla.f32        q8, q0, q13")                   // q8 = y + log(abs(v*zero)) * norm_y
            __ASM_EMIT("vst1.32         {q4}, [%[x]]!")                 // x += 4
            __ASM_EMIT("vst1.32         {q8}, [%[y]]!")                 // y += 4

            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("bge             1b")

            // TODO: 1x blocks
            __ASM_EMIT("4:")

            : [v] "+r" (v), [x] "+r" (x) , [y] "+r" (y),
              [count] "+r" (count),
              [iptr] "+r" (&LOG_IARGS[4]), [fptr] "+r" (LOG_FARGS)
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
}


#endif /* DSP_ARCH_ARM_NEON_D32_GRAPHICS_H_ */
