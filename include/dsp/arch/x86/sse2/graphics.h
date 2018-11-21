/*
 * graphics.h
 *
 *  Created on: 15 нояб. 2018 г.
 *      Author: sadko
 */

#ifndef INCLUDE_DSP_ARCH_X86_SSE2_GRAPHICS_H_
#define INCLUDE_DSP_ARCH_X86_SSE2_GRAPHICS_H_

#ifndef DSP_ARCH_X86_SSE2_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE2_IMPL */

namespace sse2
{
    #define FVEC4(x)    x, x, x, x

    static const float HSL_RGB[] =
    {
        FVEC4(0.5f),                // 1/2
        FVEC4(0.333333333333f),     // 1/3
        FVEC4(1.0f),                // 1
        FVEC4(6.0f),                // 6
        FVEC4(0.166666666667f),     // 1/6
        FVEC4(0.666666666667f)      // 2/3
    };

    static const float RGB_HSL[] =
    {
        FVEC4(4.0f),
        FVEC4(2.0f),
        FVEC4(6.0f),
        FVEC4(1.0f),
        FVEC4(0.5f),
        FVEC4(0.166666666667f)      // 1/6
    };

    static const float RGBA_TO_BGRA32[] =
    {
        FVEC4(255.0f)
    };

    #undef FVEC4

#define HSLA_TRANSPOSE \
    __ASM_EMIT("movaps          %%xmm2, %%xmm4") \
    __ASM_EMIT("punpckldq       %%xmm3, %%xmm2") \
    __ASM_EMIT("punpckhdq       %%xmm3, %%xmm4") \
    __ASM_EMIT("movaps          %%xmm0, %%xmm3") \
    __ASM_EMIT("punpckldq       %%xmm1, %%xmm0") \
    __ASM_EMIT("punpckhdq       %%xmm1, %%xmm3") \
    __ASM_EMIT("movaps          %%xmm0, %%xmm1") \
    __ASM_EMIT("punpcklqdq      %%xmm2, %%xmm0") \
    __ASM_EMIT("punpckhqdq      %%xmm2, %%xmm1") \
    __ASM_EMIT("movaps          %%xmm3, %%xmm2") \
    __ASM_EMIT("punpcklqdq      %%xmm4, %%xmm2") \
    __ASM_EMIT("punpckhqdq      %%xmm4, %%xmm3") \

#define HSLA_TO_RGBA_CORE   \
    /*  Transpose */\
    HSLA_TRANSPOSE \
    \
    /*  xmm0 = h0 h1 h2 h3 = H */ \
    /*  xmm1 = s0 s1 s2 s3 = S */ \
    /*  xmm2 = l0 l1 l2 l3 = L */ \
    /*  xmm3 = a0 a1 a2 a3 = A */ \
    /*  Calc temp1 (T1) and temp2 (T2) */ \
    __ASM_EMIT("movaps          %%xmm1, %%xmm6")            /*  xmm6 = S */ \
    __ASM_EMIT("movaps          %%xmm2, %%xmm7")            /*  xmm7 = L */ \
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
    \
    /*  Transpose final result back */ \
    HSLA_TRANSPOSE

    void hsla_to_rgba(float *dst, const float *src, size_t count)
    {
        #pragma pack(push, 1)
        struct {
            float tr[4], tg[4], tb[4], a[4];
            float t2[4], t1[4];
            float ktr[4], ktg[4], ktb[4];
            float rtr[4], rtg[4], rtb[4];
        } hslm __lsp_aligned16;
        #pragma pack(pop)

        ARCH_X86_ASM
        (
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jb              2f")

            //-----------------------------------------------------------------
            // 4x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups          0x00(%[src]), %%xmm0")      // xmm0 = h0 s0 l0 a0
            __ASM_EMIT("movups          0x10(%[src]), %%xmm1")      // xmm1 = h1 s1 l1 a1
            __ASM_EMIT("movups          0x20(%[src]), %%xmm2")      // xmm2 = h2 s2 l2 a2
            __ASM_EMIT("movups          0x30(%[src]), %%xmm3")      // xmm3 = h3 s3 l3 a3

            HSLA_TO_RGBA_CORE

            // Store result
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups          %%xmm1, 0x10(%[dst])")
            __ASM_EMIT("movups          %%xmm2, 0x20(%[dst])")
            __ASM_EMIT("movups          %%xmm3, 0x30(%[dst])")

            // Repeat loop
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jae             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             10f")

            //-----------------------------------------------------------------
            // 1x - 3x block
            // Load last variable-sized chunk
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              4f")
            __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("4:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              6f")
            __ASM_EMIT("movups          0x00(%[src]), %%xmm1")
            __ASM_EMIT("movups          0x10(%[src]), %%xmm2")
            __ASM_EMIT("6:")

            HSLA_TO_RGBA_CORE

            // Store last chunk
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              8f")
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("8:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("movups          %%xmm1, 0x00(%[dst])")
            __ASM_EMIT("movups          %%xmm2, 0x10(%[dst])")

            __ASM_EMIT("10:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [XC] "o" (HSL_RGB), [HSLM] "r" (&hslm)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
/*
        //Set the temporary values
        if  (HSL_RGB_0_5 > L)
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
    }

#undef HSLA_TO_RGBA_CORE

#define RGBA_TO_HSLA_CORE \
    /*  Transpose */\
    HSLA_TRANSPOSE \
    \
    /*  xmm0 = r0 r1 r2 r3 = R */ \
    /*  xmm1 = g0 h1 g2 g3 = G */ \
    /*  xmm2 = b0 b1 b2 b3 = B */ \
    /*  xmm3 = a0 a1 a2 a3 = A */ \
    __ASM_EMIT("movaps      %%xmm0, %%xmm6") \
    __ASM_EMIT("movaps      %%xmm1, %%xmm7") \
    __ASM_EMIT("minps       %%xmm2, %%xmm6") \
    __ASM_EMIT("maxps       %%xmm0, %%xmm7") \
    __ASM_EMIT("minps       %%xmm1, %%xmm6")            /* xmm6 = CMIN */ \
    __ASM_EMIT("maxps       %%xmm2, %%xmm7")            /* xmm7 = CMAX */ \
    __ASM_EMIT("movaps      %%xmm0, 0x00(%[RGBM])")     /* R */ \
    __ASM_EMIT("movaps      %%xmm1, 0x10(%[RGBM])")     /* G */ \
    __ASM_EMIT("movaps      %%xmm2, 0x20(%[RGBM])")     /* B */ \
    __ASM_EMIT("movaps      %%xmm3, 0x30(%[RGBM])")     /* A */ \
    __ASM_EMIT("movaps      %%xmm6, 0x40(%[RGBM])")     /* CMIN */ \
    __ASM_EMIT("movaps      %%xmm7, 0x50(%[RGBM])")     /* CMAX */ \
    __ASM_EMIT("movaps      %%xmm7, %%xmm5")            /* xmm5 = CMAX */ \
    __ASM_EMIT("subps       %%xmm6, %%xmm7")            /* xmm7 = D = CMAX - CMIN */ \
    \
    __ASM_EMIT("movaps      %%xmm0, %%xmm3")            /* xmm3 = R */ \
    __ASM_EMIT("subps       %%xmm1, %%xmm0")            /* xmm0 = R - G */ \
    __ASM_EMIT("subps       %%xmm2, %%xmm1")            /* xmm1 = G - B */ \
    __ASM_EMIT("divps       %%xmm7, %%xmm0")            /* xmm0 = (R-G)/D */ \
    __ASM_EMIT("subps       %%xmm3, %%xmm2")            /* xmm2 = B - R */ \
    __ASM_EMIT("addps       0x00 + %[XC], %%xmm0")      /* xmm0 = HB = (R-G)/D + 4 */ \
    __ASM_EMIT("divps       %%xmm7, %%xmm2")            /* xmm2 = (B-R)/D */ \
    __ASM_EMIT("xorps       %%xmm3, %%xmm3")            /* xmm3 = 0 */ \
    __ASM_EMIT("divps       %%xmm7, %%xmm1")            /* xmm1 = (G-B)/D */ \
    __ASM_EMIT("addps       0x10 + %[XC], %%xmm2")      /* xmm2 = HG = (B-R)/D + 2 */ \
    __ASM_EMIT("cmpps       $6, %%xmm1, %%xmm3")        /* xmm3 = (G-B)/D < 0 */ \
    __ASM_EMIT("andps       0x20 + %[XC], %%xmm3")      /* xmm3 = [ (G-B)/D < 0 ] & 6 */ \
    __ASM_EMIT("addps       %%xmm3, %%xmm1")            /* xmm1 = HR = (G-B)/D + [ (G-B)/D < 0 ] & 6 */ \
    \
    /* xmm0 = HB        */ \
    /* xmm1 = HR        */ \
    /* xmm2 = HG        */ \
    /* xmm5 = CMAX      */ \
    /* xmm6 = CMIN      */ \
    /* xmm7 = D         */ \
    __ASM_EMIT("movaps      %%xmm5, %%xmm6")            /* xmm6 = CMAX */ \
    __ASM_EMIT("cmpps       $0, 0x00(%[RGBM]), %%xmm5") /* xmm5 = [ R == CMAX ] */ \
    __ASM_EMIT("cmpps       $0, 0x10(%[RGBM]), %%xmm6") /* xmm6 = [ G == CMAX ] */ \
    __ASM_EMIT("movaps      %%xmm5, %%xmm3")            /* xmm3 = [ R == CMAX ] */ \
    __ASM_EMIT("movaps      %%xmm6, %%xmm4")            /* xmm4 = [ G == CMAX ] */ \
    __ASM_EMIT("andps       %%xmm5, %%xmm1")            /* xmm1 = HR & [ R == CMAX ] */ \
    __ASM_EMIT("andnps      %%xmm0, %%xmm3")            /* xmm3 = HB & [ R != CMAX ] */ \
    __ASM_EMIT("andnps      %%xmm2, %%xmm5")            /* xmm5 = HG & [ R != CMAX ] */ \
    __ASM_EMIT("andnps      %%xmm3, %%xmm4")            /* xmm4 = HB & [ R != CMAX ] & [ G != CMAX ] */ \
    __ASM_EMIT("andps       %%xmm6, %%xmm5")            /* xmm5 = HG & [ R != CMAX ] & [ G == CMAX ] */ \
    __ASM_EMIT("orps        %%xmm4, %%xmm1")            /* xmm1 = (HR & [ R == CMAX ]) | (HB & [ R != CMAX ] & [ G != CMAX ]) */ \
    __ASM_EMIT("xorps       %%xmm6, %%xmm6")            /* xmm6 = 0 */ \
    __ASM_EMIT("orps        %%xmm5, %%xmm1")            /* xmm1 = (HR & [ R == CMAX ]) | (HG & [ R != CMAX ] & [ G == CMAX ]) | (HB & [ R != CMAX ] & [ G != CMAX ]) */ \
    __ASM_EMIT("cmpps       $4, %%xmm7, %%xmm6")        /* xmm6 = [ D != 0 ] */ \
    __ASM_EMIT("andps       %%xmm6, %%xmm1")            /* xmm1 = [ D !- 0 ] & ((HR & [ R == CMAX ]) | (HG & [ R != CMAX ] & [ G == CMAX ]) | (HB & [ R != CMAX ] & [ G != CMAX ])) */ \
    \
    __ASM_EMIT("movaps      0x40(%[RGBM]), %%xmm2")     /* xmm2 = CMIN */ \
    __ASM_EMIT("movaps      0x30 + %[XC], %%xmm6")      /* xmm6 = 1 */ \
    __ASM_EMIT("movaps      %%xmm1, %%xmm0")            /* xmm0 = h */ \
    __ASM_EMIT("addps       0x50(%[RGBM]), %%xmm2")     /* xmm2 = CMAX + CMIN */ \
    __ASM_EMIT("movaps      %%xmm6, %%xmm5")            /* xmm5 = 1 */ \
    __ASM_EMIT("movaps      %%xmm7, %%xmm1")            /* xmm1 = D */ \
    __ASM_EMIT("mulps       0x40 + %[XC], %%xmm2")      /* xmm2 = L = 0.5 * (CMAX+CMIN) */ \
    __ASM_EMIT("xorps       %%xmm4, %%xmm4")            /* xmm4 = 0 */ \
    __ASM_EMIT("movaps      %%xmm2, %%xmm3")            /* xmm3 = L */ \
    __ASM_EMIT("subps       %%xmm2, %%xmm5")            /* xmm5 = 1 - L */ \
    __ASM_EMIT("cmpps       $4, %%xmm2, %%xmm4")        /* xmm4 = [ L != 0 ] */ \
    __ASM_EMIT("cmpps       $4, 0x30 + %[XC], %%xmm3")  /* xmm3 = [ L != 1 ] */ \
    __ASM_EMIT("divps       %%xmm2, %%xmm1")            /* xmm1 = D / L */ \
    __ASM_EMIT("divps       %%xmm5, %%xmm7")            /* xmm7 = D / (1-L) */ \
    __ASM_EMIT("cmpps       $6, %%xmm2, %%xmm6")        /* xmm6 = [ L < 1 ] */ \
    __ASM_EMIT("andps       %%xmm4, %%xmm1")            /* xmm1 = [ L != 0 ] & (D/L) */ \
    __ASM_EMIT("andps       %%xmm3, %%xmm7")            /* xmm7 = [ L != 1 ] & (D/(1-L)) */ \
    __ASM_EMIT("andps       %%xmm6, %%xmm1")            /* xmm1 = [ L != 0 ] & [ L < 1 ] & (D/L) */ \
    __ASM_EMIT("mulps       0x50 + %[XC], %%xmm0")      /* xmm0 = H = h * 1/6 */ \
    __ASM_EMIT("andnps      %%xmm7, %%xmm6")            /* xmm6 = [ L > 1 ] & (D/(1-L)) */ \
    __ASM_EMIT("orps        %%xmm6, %%xmm1")            /* xmm1 = s = ([ L != 0 ] & [ L < 1 ] & (D/L)) | ([ L != 1 ] & (D/(1-L))) */ \
    __ASM_EMIT("movaps      0x30(%[RGBM]), %%xmm3")     /* xmm3 = A */ \
    __ASM_EMIT("mulps       0x40 + %[XC], %%xmm1")      /* xmm1 = S = s * 0.5 */ \
    \
    /*  Transpose back */ \
    HSLA_TRANSPOSE

/*
    float cmax = (R < G) ? ((B < G) ? G : B) : ((B < R) ? R : B);
    float cmin = (R < G) ? ((B < R) ? B : R) : ((B < G) ? B : G);
    float d = cmax - cmin;

    H = 0.0f;
    S = 0.0f;
    L = HSL_RGB_0_5 * (cmax + cmin);

    // Calculate hue
    if (R == cmax)
    {
        H = (G - B) / d;
        if (H < 0.0f)
            H += 6.0f;
    }
    else if (G == cmax)
        H = (B - R) / d + 2.0f;
    else
        H = (R - G) / d + 4.0f;

    // Calculate saturation
    if (L < 1.0f)
        S = d / L;
    else if (L > 1.0f)
        S = d / (1.0f - L);

    // Normalize hue and saturation
    H  *= HSL_RGB_1_6;
    S  *= HSL_RGB_0_5;
 */

    void rgba_to_hsla(float *dst, const float *src, size_t count)
    {
        #pragma pack(push, 1)
        struct {
            float r[4], g[4], b[4], a[4];
            float cmin[4], cmax[4];
        } rgbm __lsp_aligned16;
        #pragma pack(pop)

        ARCH_X86_ASM
        (
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jb              2f")

            //-----------------------------------------------------------------
            // 4x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups          0x00(%[src]), %%xmm0")      // xmm0 = r0 g0 b0 a0
            __ASM_EMIT("movups          0x10(%[src]), %%xmm1")      // xmm1 = r1 g1 b1 a1
            __ASM_EMIT("movups          0x20(%[src]), %%xmm2")      // xmm2 = r2 g2 b2 a2
            __ASM_EMIT("movups          0x30(%[src]), %%xmm3")      // xmm3 = r3 g3 b3 a3

            RGBA_TO_HSLA_CORE

            // Store result
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups          %%xmm1, 0x10(%[dst])")
            __ASM_EMIT("movups          %%xmm2, 0x20(%[dst])")
            __ASM_EMIT("movups          %%xmm3, 0x30(%[dst])")

            // Repeat loop
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jae             1b")

            __ASM_EMIT("2:")

            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             10f")

            //-----------------------------------------------------------------
            // 1x - 3x block
            // Load last variable-sized chunk
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              4f")
            __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("4:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              6f")
            __ASM_EMIT("movups          0x00(%[src]), %%xmm1")
            __ASM_EMIT("movups          0x10(%[src]), %%xmm2")
            __ASM_EMIT("6:")

            RGBA_TO_HSLA_CORE

            // Store last chunk
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              8f")
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("8:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("movups          %%xmm1, 0x00(%[dst])")
            __ASM_EMIT("movups          %%xmm2, 0x10(%[dst])")

            __ASM_EMIT("10:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [XC] "o" (RGB_HSL), [RGBM] "r" (&rgbm)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

#undef RGBA_TO_HSLA_CORE

#define RGBA_TO_RGBA32_CORE \
    HSLA_TRANSPOSE \
    \
    /* xmm0     = r */ \
    /* xmm1     = g */ \
    /* xmm2     = b */ \
    /* xmm3     = a */ \
    __ASM_EMIT("movaps          0x00 + %[XC], %%xmm7")          /* xmm7 = 255       */ \
    __ASM_EMIT("xorps           %%xmm0, %%xmm2")                /* xmm2 = b^r       */ \
    __ASM_EMIT("xorps           %%xmm4, %%xmm4")                /* xmm4 = 0         */ \
    __ASM_EMIT("xorps           %%xmm2, %%xmm0")                /* xmm0 = r^b^r = b */ \
    __ASM_EMIT("xorps           %%xmm0, %%xmm2")                /* xmm2 = b^r^b = r */ \
    __ASM_EMIT("xorps           %%xmm5, %%xmm5")                /* xmm5 = 0         */ \
    __ASM_EMIT("mulps           %%xmm7, %%xmm3")                /* xmm3 = a * 255   */ \
    __ASM_EMIT("subps           %%xmm3, %%xmm7")                /* xmm7 = A = 255 - a*255 */ \
    __ASM_EMIT("movaps          %%xmm7, %%xmm3")                /* xmm3 = A */ \
    __ASM_EMIT("mulps           %%xmm7, %%xmm0")                /* xmm0 = B = b * A */ \
    __ASM_EMIT("mulps           %%xmm3, %%xmm1")                /* xmm1 = G = g * A */ \
    __ASM_EMIT("mulps           %%xmm7, %%xmm2")                /* xmm2 = R = r * A */ \
    __ASM_EMIT("cmpps           $2, %%xmm0, %%xmm4")            /* xmm4 = [ B >= 0 ] */ \
    __ASM_EMIT("cmpps           $2, %%xmm1, %%xmm5")            /* xmm5 = [ G >= 0 ] */ \
    __ASM_EMIT("xorps           %%xmm6, %%xmm6")                /* xmm6 = 0 */ \
    __ASM_EMIT("xorps           %%xmm7, %%xmm7")                /* xmm7 = 0 */ \
    __ASM_EMIT("cmpps           $2, %%xmm2, %%xmm6")            /* xmm6 = [ R >= 0 ] */ \
    __ASM_EMIT("cmpps           $2, %%xmm3, %%xmm7")            /* xmm7 = [ A >= 0 ] */ \
    __ASM_EMIT("andps           %%xmm4, %%xmm0")                /* xmm0 = B & [ B >= 0 ] */ \
    __ASM_EMIT("andps           %%xmm5, %%xmm1")                /* xmm1 = G & [ G >= 0 ] */ \
    __ASM_EMIT("andps           %%xmm6, %%xmm2")                /* xmm2 = R & [ R >= 0 ] */ \
    __ASM_EMIT("andps           %%xmm7, %%xmm3")                /* xmm3 = A & [ A >= 0 ] */ \
    \
    HSLA_TRANSPOSE \
    /* xmm0     = b0 g0 r0 a0 */ \
    /* xmm1     = b1 g1 r1 a1 */ \
    /* xmm2     = b2 g2 r2 a2 */ \
    /* xmm3     = b3 g3 r3 a3 */ \
    \
    __ASM_EMIT("cvtps2dq        %%xmm0, %%xmm0")                /* xmm0 = int(b0 g0 r0 a0) */ \
    __ASM_EMIT("cvtps2dq        %%xmm1, %%xmm1")                /* xmm1 = int(b1 g1 r1 a1) */ \
    __ASM_EMIT("cvtps2dq        %%xmm2, %%xmm2")                /* xmm2 = int(b2 g2 r2 a2) */ \
    __ASM_EMIT("cvtps2dq        %%xmm3, %%xmm3")                /* xmm3 = int(b3 g3 r3 a3) */ \
    __ASM_EMIT("packssdw        %%xmm1, %%xmm0")                /* xmm0 = b0 g0 r0 a0 b1 g1 r1 a1 */ \
    __ASM_EMIT("packssdw        %%xmm3, %%xmm2")                /* xmm2 = b2 g2 r2 a2 b3 g3 r3 a3 */ \
    __ASM_EMIT("packuswb        %%xmm2, %%xmm0")                /* xmm0 = b0 g0 r0 a0 b1 g1 r1 a1 b2 g2 r2 a2 b3 g3 r3 a3 */


    void rgba_to_bgra32(void *dst, const float *src, size_t count)
    {
        uint32_t mxcsr[2];
        uint32_t tmp;

        ARCH_X86_ASM
        (
            // Set rounding mode to zero
            __ASM_EMIT("stmxcsr         %[mxcsr]")
            __ASM_EMIT("movl            %[mxcsr], %[tmp]")
            __ASM_EMIT("or              $0x6000, %[tmp]")
            __ASM_EMIT("movl            %[tmp], 0x04 + %[mxcsr]")
            __ASM_EMIT("ldmxcsr         0x04 + %[mxcsr]")

            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jb              2f")

            //-----------------------------------------------------------------
            // 4x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups          0x00(%[src]), %%xmm0")      // xmm0 = r0 g0 b0 a0
            __ASM_EMIT("movups          0x10(%[src]), %%xmm1")      // xmm1 = r1 g1 b1 a1
            __ASM_EMIT("movups          0x20(%[src]), %%xmm2")      // xmm2 = r2 g2 b2 a2
            __ASM_EMIT("movups          0x30(%[src]), %%xmm3")      // xmm3 = r3 g3 b3 a3

            RGBA_TO_RGBA32_CORE

            // Store result
            __ASM_EMIT("movdqu          %%xmm0, 0x00(%[dst])")

            // Repeat loop
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jae             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             10f")

            //-----------------------------------------------------------------
            // 1x - 3x block
            // Load last variable-sized chunk
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              4f")
            __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
            __ASM_EMIT("movups          0x10(%[src]), %%xmm1")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("4:")
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              6f")
            __ASM_EMIT("movups          0x00(%[src]), %%xmm2")
            __ASM_EMIT("6:")

            RGBA_TO_RGBA32_CORE

            // Store last chunk
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              8f")
            __ASM_EMIT("movlps          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $0x08, %[dst]")
            __ASM_EMIT("8:")
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("movhlps         %%xmm0, %%xmm0")
            __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")

            __ASM_EMIT("10:")

            // Restore rounding mode
            __ASM_EMIT("ldmxcsr         %[mxcsr]")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
              [tmp] "=&r" (tmp)
            : [XC] "o" (RGBA_TO_BGRA32), [mxcsr] "o" (mxcsr)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

#undef RGBA_TO_RGBA32_CORE

#undef HSLA_TRANSPOSE

}

#endif /* INCLUDE_DSP_ARCH_X86_SSE2_GRAPHICS_H_ */
