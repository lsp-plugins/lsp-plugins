/*
 * effects.h
 *
 *  Created on: 21 нояб. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_GRAPHICS_EFFECTS_H_
#define DSP_ARCH_ARM_NEON_D32_GRAPHICS_EFFECTS_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    IF_ARCH_ARM
    (
        static const float EFF_HSLA_HUE_XC[] __lsp_aligned16 =
        {
            1.0f, 1.0f, 1.0f, 1.0f
        };
    )


#define X8_TRANSPOSE \
    __ASM_EMIT("vtrn.32         q0, q1") \
    __ASM_EMIT("vtrn.32         q2, q3") \
    __ASM_EMIT("vtrn.32         q4, q5") \
    __ASM_EMIT("vtrn.32         q6, q7") \
    __ASM_EMIT("vswp            d4, d1") \
    __ASM_EMIT("vswp            d12, d9") \
    __ASM_EMIT("vswp            d6, d3") \
    __ASM_EMIT("vswp            d14, d11")

#define EFF_HSLA_HUE_CORE   \
    /* q0   = v0        */ \
    /* q1   = v1        */ \
    /* q8   = h         */ \
    /* q9   = s         */ \
    /* q10  = l         */ \
    /* q11  = a         */ \
    /* q12  = T         */ \
    /* q13  = KT        */ \
    /* q14  = 0         */ \
    /* q15  = 1         */ \
    __ASM_EMIT("vmov            q4, q1")                /* q4   = v1 */ \
    __ASM_EMIT("vsub.f32        q2, q15, q0")           /* q2   = 1 - v */ \
    __ASM_EMIT("vsub.f32        q6, q15, q4") \
    __ASM_EMIT("vcge.f32        q3, q14, q0")           /* q3   = 0 > v  */ \
    __ASM_EMIT("vcge.f32        q7, q14, q4") \
    __ASM_EMIT("vadd.f32        q0, q0, q15")           /* q0   = v + 1 */ \
    __ASM_EMIT("vadd.f32        q4, q4, q15") \
    __ASM_EMIT("vbif            q0, q2, q3")            /* q0   = V = (v+1)&[0>v] | (1-v)&[0<=v] */ \
    __ASM_EMIT("vbif            q4, q6, q7") \
    __ASM_EMIT("vsub.f32        q3, q0, q12")           /* q3   = V - T */ \
    __ASM_EMIT("vsub.f32        q7, q4, q12") \
    __ASM_EMIT("vcge.f32        q1, q3, q14")           /* q1   = [(V-T)>=0] */ \
    __ASM_EMIT("vcge.f32        q5, q7, q14") \
    __ASM_EMIT("vmul.f32        q3, q3, q13")           /* q3   = (V-T)*KT */ \
    __ASM_EMIT("vmul.f32        q7, q7, q13") \
    __ASM_EMIT("vbit            q0, q12, q1")           /* q0   = EH = T&[(V-T)>=0] | V&[(V-T)<0] */ \
    __ASM_EMIT("vbit            q4, q12, q5") \
    __ASM_EMIT("vand            q3, q3, q1")            /* q3   = A = ((V-T)*KT) & [(V-T)>=0] */ \
    __ASM_EMIT("vand            q7, q7, q5") \
    __ASM_EMIT("vadd.f32        q0, q0, q8")            /* q0   = H = h + T&[(V-T)>=0] | V&[(V-T)<0] */ \
    __ASM_EMIT("vadd.f32        q4, q4, q8") \
    __ASM_EMIT("vmov            q1, q9")                /* q1   = s */ \
    __ASM_EMIT("vmov            q2, q10")               /* q2   = l */ \
    __ASM_EMIT("vmov            q5, q9") \
    __ASM_EMIT("vmov            q6, q10") \
    /* Transpose back */ \
    X8_TRANSPOSE

    /*
        float t     = 1.0f - eff->thresh;
        float kt    = 1.0f / eff->thresh;

        value   = v[i];
        value   = (0 > value) ? 1.0f + value : 1.0f - value;

        if ((value - t) >= 0)
        {
            hue         = eff->h + t;
            alpha       = ((value - t) * kt);
        }
        else
        {
            hue         = eff->h + value;
            alpha       = 0.0f;
        }

        dst[0]      = (hue < 1.0f) ? hue : hue - 1.0f;
        dst[1]      = eff->s;
        dst[2]      = eff->l;
        dst[3]      = alpha;
    */

    void eff_hsla_hue(float *dst, const float *v, const dsp::hsla_hue_eff_t *eff, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.32         {q8}, [%[eff]]!")           /* q8   = hsla */
            __ASM_EMIT("vldm            %[XC], {q15}")              /* q15  = 1 */
            __ASM_EMIT("vmov            q9, q8")                    /* q9   = hsla */
            __ASM_EMIT("vld1.32         {d24[], d25[]}, [%[eff]]")  /* q12  = t */
            __ASM_EMIT("vmov            q10, q8")                   /* q10  = hsla */
            __ASM_EMIT("veor            q14, q14")                  /* q14  = 0 */
            __ASM_EMIT("vmov            q11, q8")                   /* q11  = hsla */
            __ASM_EMIT("vrecpe.f32      q0, q12")                   /* q0   = TD */
            __ASM_EMIT("vtrn.32         q8, q9")
            __ASM_EMIT("vrecps.f32      q1, q0, q12")               /* q1   = (2 - TD*T) */
            __ASM_EMIT("vtrn.32         q10, q11")
            __ASM_EMIT("vmul.f32        q0, q1, q0")                /* q0   = t' = TD * (2 - TD*T) */
            __ASM_EMIT("vswp            d20, d17")
            __ASM_EMIT("vrecps.f32      q1, q0, q12")               /* q1   = (2 - TD*t') */
            __ASM_EMIT("vswp            d22, d19")
            __ASM_EMIT("vsub.f32        q12, q15, q12")             /* q12  = T = 1 - t */
            __ASM_EMIT("vmul.f32        q13, q1, q0")               /* q13  = KT = 1/t = t' * (2 - TD*t') */

            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("blo             2f")

            //-----------------------------------------------------------------
            // 8x blocks
            __ASM_EMIT("1:")

            __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")        /* q0 = v0, q1 = v1 */

            EFF_HSLA_HUE_CORE

            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vstm            %[dst]!, {q0-q7}")
            __ASM_EMIT("bhs             1b")

            //-----------------------------------------------------------------
            // 1x-8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("bls             14f")

            __ASM_EMIT("tst             %[count], $4")
            __ASM_EMIT("beq             4f")
            __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
            __ASM_EMIT("4:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             6f")
            __ASM_EMIT("vld1.32         {d2}, [%[src]]!")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             8f")
            __ASM_EMIT("vldm            %[src], {s6}")
            __ASM_EMIT("8:")

            EFF_HSLA_HUE_CORE

            __ASM_EMIT("tst             %[count], $4")
            __ASM_EMIT("beq             10f")
            __ASM_EMIT("vstm            %[dst]!, {q0-q3}")
            __ASM_EMIT("10:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             12f")
            __ASM_EMIT("vstm            %[dst]!, {q4-q5}")
            __ASM_EMIT("12:")
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             14f")
            __ASM_EMIT("vstm            %[dst]!, {q6}")

            __ASM_EMIT("14:")


            : [dst] "+r" (dst), [src] "+r" (v), [count] "+r" (count),
              [eff] "+r" (eff)
            : [XC] "r" (&EFF_HSLA_HUE_XC[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef EFF_HSLA_HUE_CORE

#define EFF_HSLA_SAT_CORE   \
    /* q0   = v0        */ \
    /* q1   = v1        */ \
    /* q8   = h         */ \
    /* q9   = s         */ \
    /* q10  = l         */ \
    /* q11  = a         */ \
    /* q13  = 0         */ \
    /* q14  = T         */ \
    /* q15  = KT        */ \
    __ASM_EMIT("vabs.f32        q5, q1")                /* q5   = V1 = abs(v1) */ \
    __ASM_EMIT("vabs.f32        q1, q0")                /* q1   = V  = abs(v) */ \
    __ASM_EMIT("vsub.f32        q3, q14, q1")           /* q3   = T - V */ \
    __ASM_EMIT("vsub.f32        q7, q14, q5") \
    __ASM_EMIT("vcgt.f32        q2, q3, q13")           /* q2   = [(T-V) > 0] */ \
    __ASM_EMIT("vcgt.f32        q6, q7, q13") \
    __ASM_EMIT("vmul.f32        q3, q3, q15")           /* q3   = (T-V)*KT */ \
    __ASM_EMIT("vmul.f32        q7, q7, q15") \
    __ASM_EMIT("vbit            q1, q14, q2")           /* q1   = ES = V&[(T-V) <= 0] | T&[(T-V) > 0] */ \
    __ASM_EMIT("vbit            q5, q14, q6") \
    __ASM_EMIT("vand            q3, q3, q2")            /* q3   = A = ((T-V)*KT) & [(T-V) > 0] */ \
    __ASM_EMIT("vand            q7, q7, q6") \
    __ASM_EMIT("vmul.f32        q1, q1, q9")            /* q1   = ES*s = S */ \
    __ASM_EMIT("vmul.f32        q5, q5, q9") \
    __ASM_EMIT("vmov            q0, q8") \
    __ASM_EMIT("vmov            q2, q10") \
    __ASM_EMIT("vmov            q4, q8") \
    __ASM_EMIT("vmov            q6, q10") \
    /* Transpose back */ \
    X8_TRANSPOSE

    /*
        kt      = 1.0f / eff->thresh;
        value   = (value >= 0.0f) ? value : -value;

        if ((eff->thresh - value) <= 0)
        {
            dst[1]      = eff->s * value;
            dst[3]      = 0.0f;
        }
        else
        {
            dst[1]      = eff->s * eff->thresh;
            dst[3]      = (eff->thresh - value) * kt;
        }

        dst[0]      = eff->h;
        dst[2]      = eff->l;
     */

    void eff_hsla_sat(float *dst, const float *v, const dsp::hsla_sat_eff_t *eff, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.32         {q8}, [%[eff]]!")           /* q8   = hsla */
            __ASM_EMIT("vld1.32         {d28[], d29[]}, [%[eff]]")  /* q14  = T */
            __ASM_EMIT("vmov            q9, q8")                    /* q9   = hsla */
            __ASM_EMIT("vrecpe.f32      q0, q14")                   /* q0   = TD */
            __ASM_EMIT("vtrn.32         q8, q9")
            __ASM_EMIT("vrecps.f32      q1, q0, q14")               /* q1   = (2 - TD*T) */
            __ASM_EMIT("vmov            q10, q8")                   /* q10  = hsla */
            __ASM_EMIT("vmul.f32        q0, q1, q0")                /* q0   = t' = TD * (2 - TD*T) */
            __ASM_EMIT("vmov            q11, q9")                   /* q11  = hsla */
            __ASM_EMIT("vrecps.f32      q1, q0, q14")               /* q1   = (2 - TD*t') */
            __ASM_EMIT("vswp            d20, d17")
            __ASM_EMIT("vmul.f32        q15, q1, q0")               /* q15  = KT = 1/t = t' * (2 - TD*t') */
            __ASM_EMIT("vswp            d22, d19")
            __ASM_EMIT("veor            q13, q13")                  /* q13  = 0 */

            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("blo             2f")

            //-----------------------------------------------------------------
            // 8x blocks
            __ASM_EMIT("1:")

            __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")        /* q0 = v0, q1 = v1 */

            EFF_HSLA_SAT_CORE

            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vstm            %[dst]!, {q0-q7}")
            __ASM_EMIT("bhs             1b")

            //-----------------------------------------------------------------
            // 1x-8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("bls             14f")

            __ASM_EMIT("tst             %[count], $4")
            __ASM_EMIT("beq             4f")
            __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
            __ASM_EMIT("4:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             6f")
            __ASM_EMIT("vld1.32         {d2}, [%[src]]!")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             8f")
            __ASM_EMIT("vldm            %[src], {s6}")
            __ASM_EMIT("8:")

            EFF_HSLA_SAT_CORE

            __ASM_EMIT("tst             %[count], $4")
            __ASM_EMIT("beq             10f")
            __ASM_EMIT("vstm            %[dst]!, {q0-q3}")
            __ASM_EMIT("10:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             12f")
            __ASM_EMIT("vstm            %[dst]!, {q4-q5}")
            __ASM_EMIT("12:")
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             14f")
            __ASM_EMIT("vstm            %[dst]!, {q6}")

            __ASM_EMIT("14:")


            : [dst] "+r" (dst), [src] "+r" (v), [count] "+r" (count),
              [eff] "+r" (eff)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef EFF_HSLA_SAT_CORE


#define EFF_HSLA_LIGHT_CORE   \
    /* q0   = v0        */ \
    /* q1   = v1        */ \
    /* q8   = h         */ \
    /* q9   = s         */ \
    /* q10  = l         */ \
    /* q11  = a         */ \
    /* q13  = 0         */ \
    /* q14  = T         */ \
    /* q15  = KT        */ \
    __ASM_EMIT("vabs.f32        q5, q1")                /* q5   = V1 = abs(v1) */ \
    __ASM_EMIT("vabs.f32        q1, q0")                /* q1   = V  = abs(v) */ \
    __ASM_EMIT("vsub.f32        q3, q14, q1")           /* q3   = T - V */ \
    __ASM_EMIT("vsub.f32        q7, q14, q5") \
    __ASM_EMIT("vcgt.f32        q2, q3, q13")           /* q2   = [(T-V) > 0] */ \
    __ASM_EMIT("vcgt.f32        q6, q7, q13") \
    __ASM_EMIT("vmul.f32        q3, q3, q15")           /* q3   = (T-V)*KT */ \
    __ASM_EMIT("vmul.f32        q7, q7, q15") \
    __ASM_EMIT("vbit            q1, q14, q2")           /* q1   = EL = V&[(T-V) <= 0] | T&[(T-V) > 0] */ \
    __ASM_EMIT("vbit            q5, q14, q6") \
    __ASM_EMIT("vand            q3, q3, q2")            /* q3   = A = ((T-V)*KT) & [(T-V) > 0] */ \
    __ASM_EMIT("vand            q7, q7, q6") \
    __ASM_EMIT("vmul.f32        q2, q1, q10")           /* q2   = EL*l = L */ \
    __ASM_EMIT("vmul.f32        q6, q5, q10") \
    __ASM_EMIT("vmov            q0, q8") \
    __ASM_EMIT("vmov            q1, q9") \
    __ASM_EMIT("vmov            q4, q8") \
    __ASM_EMIT("vmov            q5, q9") \
    /* Transpose back */ \
    X8_TRANSPOSE

    /*
        kt      = 1.0f / eff->thresh;
        value   = (value >= 0.0f) ? value : -value;

        if ((eff->thresh - value) <= 0)
        {
            dst[2]      = eff->l * value;
            dst[3]      = 0.0f;
        }
        else
        {
            dst[2]      = eff->l * eff->thresh;
            dst[3]      = (eff->thresh - value) * kt;
        }

        dst[0]      = eff->h;
        dst[1]      = eff->s;
     */

    void eff_hsla_light(float *dst, const float *v, const dsp::hsla_light_eff_t *eff, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.32         {q8}, [%[eff]]!")           /* q8   = hsla */
            __ASM_EMIT("vld1.32         {d28[], d29[]}, [%[eff]]")  /* q14  = T */
            __ASM_EMIT("vmov            q9, q8")                    /* q9   = hsla */
            __ASM_EMIT("vrecpe.f32      q0, q14")                   /* q0   = TD */
            __ASM_EMIT("vtrn.32         q8, q9")
            __ASM_EMIT("vrecps.f32      q1, q0, q14")               /* q1   = (2 - TD*T) */
            __ASM_EMIT("vmov            q10, q8")                   /* q10  = hsla */
            __ASM_EMIT("vmul.f32        q0, q1, q0")                /* q0   = t' = TD * (2 - TD*T) */
            __ASM_EMIT("vmov            q11, q9")                   /* q11  = hsla */
            __ASM_EMIT("vrecps.f32      q1, q0, q14")               /* q1   = (2 - TD*t') */
            __ASM_EMIT("vswp            d20, d17")
            __ASM_EMIT("vmul.f32        q15, q1, q0")               /* q15  = KT = 1/t = t' * (2 - TD*t') */
            __ASM_EMIT("vswp            d22, d19")
            __ASM_EMIT("veor            q13, q13")                  /* q13  = 0 */

            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("blo             2f")

            //-----------------------------------------------------------------
            // 8x blocks
            __ASM_EMIT("1:")

            __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")        /* q0 = v0, q1 = v1 */

            EFF_HSLA_LIGHT_CORE

            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vstm            %[dst]!, {q0-q7}")
            __ASM_EMIT("bhs             1b")

            //-----------------------------------------------------------------
            // 1x-8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("bls             14f")

            __ASM_EMIT("tst             %[count], $4")
            __ASM_EMIT("beq             4f")
            __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
            __ASM_EMIT("4:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             6f")
            __ASM_EMIT("vld1.32         {d2}, [%[src]]!")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             8f")
            __ASM_EMIT("vldm            %[src], {s6}")
            __ASM_EMIT("8:")

            EFF_HSLA_LIGHT_CORE

            __ASM_EMIT("tst             %[count], $4")
            __ASM_EMIT("beq             10f")
            __ASM_EMIT("vstm            %[dst]!, {q0-q3}")
            __ASM_EMIT("10:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             12f")
            __ASM_EMIT("vstm            %[dst]!, {q4-q5}")
            __ASM_EMIT("12:")
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             14f")
            __ASM_EMIT("vstm            %[dst]!, {q6}")

            __ASM_EMIT("14:")


            : [dst] "+r" (dst), [src] "+r" (v), [count] "+r" (count),
              [eff] "+r" (eff)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef EFF_HSLA_LIGHT_CORE

IF_ARCH_ARM
(
    static const float EFF_HSLA_ALPHA_XC[] __lsp_aligned16 =
    {
        1.0f, 1.0f, 1.0f, 1.0f
    };
)


#define EFF_HSLA_ALPHA_CORE   \
    /* q0   = v0        */ \
    /* q1   = v1        */ \
    /* q8   = h         */ \
    /* q9   = s         */ \
    /* q10  = l         */ \
    /* q11  = a         */ \
    /* q14  = 0         */ \
    /* q15  = 1         */ \
    __ASM_EMIT("vsub.f32        q2, q15, q0")           /* q2   = 1 - v */ \
    __ASM_EMIT("vsub.f32        q6, q15, q1") \
    __ASM_EMIT("vcgt.f32        q4, q14, q0")           /* q4   = 0 > v  */ \
    __ASM_EMIT("vcgt.f32        q5, q14, q1") \
    __ASM_EMIT("vadd.f32        q3, q0, q15")           /* q3   = v + 1 */ \
    __ASM_EMIT("vadd.f32        q7, q1, q15") \
    __ASM_EMIT("vbif            q3, q2, q4")            /* q0   = V = (v+1)&[0>v] | (1-v)&[0<=v] */ \
    __ASM_EMIT("vbif            q7, q6, q5") \
    __ASM_EMIT("vmov            q0, q8") \
    __ASM_EMIT("vmov            q1, q9") \
    __ASM_EMIT("vmov            q2, q10") \
    __ASM_EMIT("vmov            q4, q8") \
    __ASM_EMIT("vmov            q5, q9") \
    __ASM_EMIT("vmov            q6, q10") \
    /* Transpose back */ \
    X8_TRANSPOSE

    /*
        value   = v[i];
        value   = (0.0f > value) ? 1.0f + value : 1.0f - value;

        dst[0]  = eff->h;
        dst[1]  = eff->s;
        dst[2]  = eff->l;
        dst[3]  = value; // Fill alpha channel
     */

    void eff_hsla_alpha(float *dst, const float *v, const dsp::hsla_alpha_eff_t *eff, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.32         {q8}, [%[eff]]")            /* q8   = hsla */
            __ASM_EMIT("vldm            %[XC], {q15}")              /* q15  = 1 */
            __ASM_EMIT("vmov            q9, q8")                    /* q9   = hsla */
            __ASM_EMIT("veor            q14, q14")                  /* q14  = 0 */
            __ASM_EMIT("vtrn.32         q8, q9")
            __ASM_EMIT("vmov            q10, q8")
            __ASM_EMIT("vmov            q11, q9")
            __ASM_EMIT("vswp            d20, d17")
            __ASM_EMIT("vswp            d22, d19")

            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("blo             2f")

            //-----------------------------------------------------------------
            // 8x blocks
            __ASM_EMIT("1:")

            __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")        /* q0 = v0, q1 = v1 */

            EFF_HSLA_ALPHA_CORE

            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vstm            %[dst]!, {q0-q7}")
            __ASM_EMIT("bhs             1b")

            //-----------------------------------------------------------------
            // 1x-8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("bls             14f")

            __ASM_EMIT("tst             %[count], $4")
            __ASM_EMIT("beq             4f")
            __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
            __ASM_EMIT("4:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             6f")
            __ASM_EMIT("vld1.32         {d2}, [%[src]]!")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             8f")
            __ASM_EMIT("vldm            %[src], {s6}")
            __ASM_EMIT("8:")

            EFF_HSLA_ALPHA_CORE

            __ASM_EMIT("tst             %[count], $4")
            __ASM_EMIT("beq             10f")
            __ASM_EMIT("vstm            %[dst]!, {q0-q3}")
            __ASM_EMIT("10:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             12f")
            __ASM_EMIT("vstm            %[dst]!, {q4-q5}")
            __ASM_EMIT("12:")
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             14f")
            __ASM_EMIT("vstm            %[dst]!, {q6}")

            __ASM_EMIT("14:")


            : [dst] "+r" (dst), [src] "+r" (v), [count] "+r" (count)
            : [XC] "r" (&EFF_HSLA_ALPHA_XC[0]), [eff] "r" (eff)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef EFF_HSLA_ALPHA_CORE

#undef X8_TRANSPOSE

}

#endif /* DSP_ARCH_ARM_NEON_D32_GRAPHICS_EFFECTS_H_ */
