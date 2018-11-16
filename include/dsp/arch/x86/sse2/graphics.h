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

    #undef FVEC4

#define HSLA_TO_RGBA_CORE   \
    /*  Transpose */\
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
    __ASM_EMIT("punpckhqdq      %%xmm4, %%xmm3")

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

}

#endif /* INCLUDE_DSP_ARCH_X86_SSE2_GRAPHICS_H_ */
