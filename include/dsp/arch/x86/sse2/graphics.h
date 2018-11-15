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
        FVEC4(6.0f)                 // 6
    };

    #undef FVEC4

    void hsla_to_rgba(float *dst, const float *src, size_t count)
    {
        float hslm[16] __lsp_aligned16;

        ARCH_X86_ASM
        (
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jb              2f")

            // 4x loop
            __ASM_EMIT("1:")
            __ASM_EMIT("movups          0x00(%[src]), %%xmm0")      // xmm0 = h0 s0 l0 a0
            __ASM_EMIT("movups          0x10(%[src]), %%xmm0")      // xmm1 = h1 s1 l1 a1
            __ASM_EMIT("movups          0x20(%[src]), %%xmm0")      // xmm2 = h2 s2 l2 a2
            __ASM_EMIT("movups          0x30(%[src]), %%xmm0")      // xmm3 = h3 s3 l3 a3

            // Transpose
            __ASM_EMIT("movaps          %%xmm2, %%xmm4")
            __ASM_EMIT("punpckldq       %%xmm3, %%xmm2")
            __ASM_EMIT("punpckhdq       %%xmm3, %%xmm4")
            __ASM_EMIT("movaps          %%xmm0, %%xmm3")
            __ASM_EMIT("punpckldq       %%xmm1, %%xmm0")
            __ASM_EMIT("punpckhdq       %%xmm1, %%xmm3")
            __ASM_EMIT("movaps          %%xmm0, %%xmm1")
            __ASM_EMIT("punpcklqdq      %%xmm2, %%xmm0")
            __ASM_EMIT("punpckhqdq      %%xmm2, %%xmm1")
            __ASM_EMIT("movaps          %%xmm3, %%xmm2")
            __ASM_EMIT("punpcklqdq      %%xmm4, %%xmm2")
            __ASM_EMIT("punpckhqdq      %%xmm4, %%xmm3")

            // xmm0 = h0 h1 h2 h3 = H
            // xmm1 = s0 s1 s2 s3 = S
            // xmm2 = l0 l1 l2 l3 = L
            // xmm3 = a0 a1 a2 a3 = A
            // Calc temp1 (T1) and temp2 (T2)
            __ASM_EMIT("movaps          %%xmm1, %%xmm6")            // xmm6 = S
            __ASM_EMIT("movaps          %%xmm2, %%xmm7")            // xmm7 = L
            __ASM_EMIT("addps           %%xmm2, %%xmm6")            // xmm6 = L + S
            __ASM_EMIT("mulps           %%xmm1, %%xmm7")            // xmm7 = L * S
            __ASM_EMIT("subps           %%xmm7, %%xmm6")            // xmm6 = L + S - L * S
            __ASM_EMIT("addps           %%xmm2, %%xmm7")            // xmm7 = L + L * S
            __ASM_EMIT("movaps          %%xmm2, %%xmm5")            // xmm5 = L
            __ASM_EMIT("movaps          0x00 + %[XC], %%xmm4")      // xmm4 = 0.5
            __ASM_EMIT("addps           %%xmm2, %%xmm5")            // xmm5 = L + L
            __ASM_EMIT("cmpps           $6, %%xmm1, %%xmm4")        // xmm4 = [0.5f > L]
            __ASM_EMIT("andps           %%xmm4, xmm6")              // xmm6 = [0.5f > L] & (L+S - L*S)
            __ASM_EMIT("andnps          %%xmm7, %%xmm4")            // xmm4 = [0.5f <= L] & (L + L*S)
            __ASM_EMIT("orps            %%xmm6, %%xmm4")            // xmm4 = T2 = ([0.5f > L] & (L+S - L*S)) | ([0.5f <= L] & (L + L*S))
            __ASM_EMIT("movaps          %%xmm0, %%xmm1")            // xmm1 = TG = H
            __ASM_EMIT("subps           %%xmm5, %%xmm4")            // xmm5 = T1 = L + L - T2
            __ASM_EMIT("movaps          %%xmm0, %%xmm2")            // xmm2 = H

            __ASM_EMIT("movaps          0x10 + %[XC], %%xmm6")      // xmm6 = 1/3
            __ASM_EMIT("addps           %%xmm6, %%xmm0")            // xmm0 = H + 1/3
            __ASM_EMIT("subps           %%xmm6, %%xmm2")            // xmm2 = H - 1/3

            __ASM_EMIT("movaps          0x20 + %[XC], %%xmm7")      // xmm7 = 1
            __ASM_EMIT("movaps          %%xmm0, %%xmm6")            // xmm6 = H + 1/3
            __ASM_EMIT("subps           %%xmm7, %%xmm6")            // xmm6 = H + 1/3 - 1
            __ASM_EMIT("cmpps           $6, %%xmm0, %%xmm7")        // xmm7 = [1 > (H + 1/3)]
            __ASM_EMIT("andps           %%xmm7, %%xmm0")            // xmm0 = (H + 1/3) & [1 > (H + 1/3)]
            __ASM_EMIT("andnps          %%xmm6, %%xmm7")            // xmm7 = (H + 1/3 - 1) & [1 <= (H + 1/3)]
            __ASM_EMIT("orps            %%xmm7, %%xmm0")            // xmm0 = TR = (H + 1/3) & [1 > (H + 1/3)] | (H + 1/3 - 1) & [1 <= (H + 1/3)]

            __ASM_EMIT("movaps          0x20 + %[XC], %%xmm7")      // xmm7 = 1
            __ASM_EMIT("movaps          %%xmm2, %%xmm6")            // xmm6 = H - 1/3
            __ASM_EMIT("addps           %%xmm7, %%xmm6")            // xmm6 = H - 1/3 + 1
            __ASM_EMIT("xorps           %%xmm7, %%xmm7")            // xmm7 = 0
            __ASM_EMIT("cmpps           $2, %%xmm2, %%xmm7")        // xmm7 = [0 <= (H - 1/3)]
            __ASM_EMIT("andps           %%xmm7, %%xmm2")            // xmm2 = (H - 1/3) & [0 <= (H - 1/3)]
            __ASM_EMIT("andnps          %%xmm6, %%xmm7")            // xmm6 = (H - 1/3 + 1) & [0 > (H - 1/3)]
            __ASM_EMIT("orps            %%xmm7, %%xmm2")            // xmm2 = TB = ((H - 1/3) & [0 <= (H - 1/3)]) | ((H - 1/3 + 1) & [0 > (H - 1/3)])

            __ASM_EMIT("movaps          %%xmm4, %%xmm6")            // xmm6 = T2
            __ASM_EMIT("movaps          0x30 + %[XC], %%xmm7")      // xmm7 = 6.0
            __ASM_EMIT("subps           %%xmm5, %%xmm6")            // xmm6 = T2 - T1
            __ASM_EMIT("mulps           %%xmm7, %%xmm6")            // xmm6 = K = (T2 - T1)*6.0

            // xmm0 = TR
            // xmm1 = TG
            // xmm2 = TB
            // xmm3 = A
            // xmm4 = T2
            // xmm5 = T1
            // xmm6 = K
            __ASM_EMIT("movaps          %%xmm0, 0x00 + %[HSLM]")
            __ASM_EMIT("movaps          %%xmm1, 0x10 + %[HSLM]")
            __ASM_EMIT("movaps          %%xmm2, 0x20 + %[HSLM]")
            __ASM_EMIT("movaps          %%xmm3, 0x30 + %[HSLM]")

            __ASM_EMIT("")
            __ASM_EMIT("")
            __ASM_EMIT("")
            __ASM_EMIT("")
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
}

#endif /* INCLUDE_DSP_ARCH_X86_SSE2_GRAPHICS_H_ */
