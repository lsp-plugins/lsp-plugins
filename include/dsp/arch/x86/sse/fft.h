/*
 * fft.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_FFT_H_
#define DSP_ARCH_X86_SSE_FFT_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
#define X4VEC(v)        v, v, v, v

    /* FFT vectors */
    static const float XFFT_W_RE[] __lsp_aligned16 =
    {
        X4VEC(0.0000000000000000f),
        X4VEC(0.0000000000000000f),
        X4VEC(0.7071067811865475f),
        X4VEC(0.9238795325112868f),
        X4VEC(0.9807852804032305f),
        X4VEC(0.9951847266721969f),
        X4VEC(0.9987954562051724f),
        X4VEC(0.9996988186962042f),
        X4VEC(0.9999247018391445f),
        X4VEC(0.9999811752826011f),
        X4VEC(0.9999952938095762f),
        X4VEC(0.9999988234517019f),
        X4VEC(0.9999997058628822f),
        X4VEC(0.9999999264657179f),
        X4VEC(0.9999999816164293f),
        X4VEC(0.9999999954041073f),
        X4VEC(0.9999999988510268f)
    };

    static const float XFFT_W_IM[] __lsp_aligned16 =
    {
        X4VEC(1.0000000000000000f),
        X4VEC(1.0000000000000000f),
        X4VEC(0.7071067811865475f),
        X4VEC(0.3826834323650898f),
        X4VEC(0.1950903220161283f),
        X4VEC(0.0980171403295606f),
        X4VEC(0.0490676743274180f),
        X4VEC(0.0245412285229123f),
        X4VEC(0.0122715382857199f),
        X4VEC(0.0061358846491545f),
        X4VEC(0.0030679567629660f),
        X4VEC(0.0015339801862848f),
        X4VEC(0.0007669903187427f),
        X4VEC(0.0003834951875714f),
        X4VEC(0.0001917475973107f),
        X4VEC(0.0000958737990960f),
        X4VEC(0.0000479368996031f)
    };

    static const float XFFT_W[] __lsp_aligned16 =
    {
        X4VEC(0.0000000000000000f), X4VEC(1.0000000000000000f),
        X4VEC(0.0000000000000000f), X4VEC(1.0000000000000000f),
        X4VEC(0.7071067811865475f), X4VEC(0.7071067811865475f),
        X4VEC(0.9238795325112868f), X4VEC(0.3826834323650898f),
        X4VEC(0.9807852804032305f), X4VEC(0.1950903220161283f),
        X4VEC(0.9951847266721969f), X4VEC(0.0980171403295606f),
        X4VEC(0.9987954562051724f), X4VEC(0.0490676743274180f),
        X4VEC(0.9996988186962042f), X4VEC(0.0245412285229123f),
        X4VEC(0.9999247018391445f), X4VEC(0.0122715382857199f),
        X4VEC(0.9999811752826011f), X4VEC(0.0061358846491545f),
        X4VEC(0.9999952938095762f), X4VEC(0.0030679567629660f),
        X4VEC(0.9999988234517019f), X4VEC(0.0015339801862848f),
        X4VEC(0.9999997058628822f), X4VEC(0.0007669903187427f),
        X4VEC(0.9999999264657179f), X4VEC(0.0003834951875714f),
        X4VEC(0.9999999816164293f), X4VEC(0.0001917475973107f),
        X4VEC(0.9999999954041073f), X4VEC(0.0000958737990960f),
        X4VEC(0.9999999988510268f), X4VEC(0.0000479368996031f)
    };

#undef X4VEC

    static const float XFFT_A_RE[] __lsp_aligned16 =
    {
        1.0000000000000000f, 0.7071067811865475f, 0.0000000000000000f, -0.7071067811865475f,
        1.0000000000000000f, 0.9238795325112868f, 0.7071067811865475f, 0.3826834323650898f,
        1.0000000000000000f, 0.9807852804032305f, 0.9238795325112868f, 0.8314696123025452f,
        1.0000000000000000f, 0.9951847266721969f, 0.9807852804032305f, 0.9569403357322089f,
        1.0000000000000000f, 0.9987954562051724f, 0.9951847266721969f, 0.9891765099647810f,
        1.0000000000000000f, 0.9996988186962042f, 0.9987954562051724f, 0.9972904566786902f,
        1.0000000000000000f, 0.9999247018391445f, 0.9996988186962042f, 0.9993223845883495f,
        1.0000000000000000f, 0.9999811752826011f, 0.9999247018391445f, 0.9998305817958234f,
        1.0000000000000000f, 0.9999952938095762f, 0.9999811752826011f, 0.9999576445519639f,
        1.0000000000000000f, 0.9999988234517019f, 0.9999952938095762f, 0.9999894110819284f,
        1.0000000000000000f, 0.9999997058628822f, 0.9999988234517019f, 0.9999973527669782f,
        1.0000000000000000f, 0.9999999264657179f, 0.9999997058628822f, 0.9999993381915255f,
        1.0000000000000000f, 0.9999999816164293f, 0.9999999264657179f, 0.9999998345478677f,
        1.0000000000000000f, 0.9999999954041073f, 0.9999999816164293f, 0.9999999586369661f,
        1.0000000000000000f, 0.9999999988510268f, 0.9999999954041073f, 0.9999999896592415f
    };

    static const float XFFT_A_IM[] __lsp_aligned16 =
    {
        0.0000000000000000f, 0.7071067811865475f, 1.0000000000000000f, 0.7071067811865476f,
        0.0000000000000000f, 0.3826834323650898f, 0.7071067811865475f, 0.9238795325112867f,
        0.0000000000000000f, 0.1950903220161283f, 0.3826834323650898f, 0.5555702330196022f,
        0.0000000000000000f, 0.0980171403295606f, 0.1950903220161283f, 0.2902846772544624f,
        0.0000000000000000f, 0.0490676743274180f, 0.0980171403295606f, 0.1467304744553617f,
        0.0000000000000000f, 0.0245412285229123f, 0.0490676743274180f, 0.0735645635996674f,
        0.0000000000000000f, 0.0122715382857199f, 0.0245412285229123f, 0.0368072229413588f,
        0.0000000000000000f, 0.0061358846491545f, 0.0122715382857199f, 0.0184067299058048f,
        0.0000000000000000f, 0.0030679567629660f, 0.0061358846491545f, 0.0092037547820598f,
        0.0000000000000000f, 0.0015339801862848f, 0.0030679567629660f, 0.0046019261204486f,
        0.0000000000000000f, 0.0007669903187427f, 0.0015339801862848f, 0.0023009691514258f,
        0.0000000000000000f, 0.0003834951875714f, 0.0007669903187427f, 0.0011504853371138f,
        0.0000000000000000f, 0.0001917475973107f, 0.0003834951875714f, 0.0005752427637321f,
        0.0000000000000000f, 0.0000958737990960f, 0.0001917475973107f, 0.0002876213937629f,
        0.0000000000000000f, 0.0000479368996031f, 0.0000958737990960f, 0.0001438106983686f
    };

    static const float XFFT_A[] __lsp_aligned16 =
    {
        1.0000000000000000f, 0.7071067811865475f, 0.0000000000000000f, -0.7071067811865475f,0.0000000000000000f, 0.7071067811865475f, 1.0000000000000000f, 0.7071067811865476f,
        1.0000000000000000f, 0.9238795325112868f, 0.7071067811865475f, 0.3826834323650898f, 0.0000000000000000f, 0.3826834323650898f, 0.7071067811865475f, 0.9238795325112867f,
        1.0000000000000000f, 0.9807852804032305f, 0.9238795325112868f, 0.8314696123025452f, 0.0000000000000000f, 0.1950903220161283f, 0.3826834323650898f, 0.5555702330196022f,
        1.0000000000000000f, 0.9951847266721969f, 0.9807852804032305f, 0.9569403357322089f, 0.0000000000000000f, 0.0980171403295606f, 0.1950903220161283f, 0.2902846772544624f,
        1.0000000000000000f, 0.9987954562051724f, 0.9951847266721969f, 0.9891765099647810f, 0.0000000000000000f, 0.0490676743274180f, 0.0980171403295606f, 0.1467304744553617f,
        1.0000000000000000f, 0.9996988186962042f, 0.9987954562051724f, 0.9972904566786902f, 0.0000000000000000f, 0.0245412285229123f, 0.0490676743274180f, 0.0735645635996674f,
        1.0000000000000000f, 0.9999247018391445f, 0.9996988186962042f, 0.9993223845883495f, 0.0000000000000000f, 0.0122715382857199f, 0.0245412285229123f, 0.0368072229413588f,
        1.0000000000000000f, 0.9999811752826011f, 0.9999247018391445f, 0.9998305817958234f, 0.0000000000000000f, 0.0061358846491545f, 0.0122715382857199f, 0.0184067299058048f,
        1.0000000000000000f, 0.9999952938095762f, 0.9999811752826011f, 0.9999576445519639f, 0.0000000000000000f, 0.0030679567629660f, 0.0061358846491545f, 0.0092037547820598f,
        1.0000000000000000f, 0.9999988234517019f, 0.9999952938095762f, 0.9999894110819284f, 0.0000000000000000f, 0.0015339801862848f, 0.0030679567629660f, 0.0046019261204486f,
        1.0000000000000000f, 0.9999997058628822f, 0.9999988234517019f, 0.9999973527669782f, 0.0000000000000000f, 0.0007669903187427f, 0.0015339801862848f, 0.0023009691514258f,
        1.0000000000000000f, 0.9999999264657179f, 0.9999997058628822f, 0.9999993381915255f, 0.0000000000000000f, 0.0003834951875714f, 0.0007669903187427f, 0.0011504853371138f,
        1.0000000000000000f, 0.9999999816164293f, 0.9999999264657179f, 0.9999998345478677f, 0.0000000000000000f, 0.0001917475973107f, 0.0003834951875714f, 0.0005752427637321f,
        1.0000000000000000f, 0.9999999954041073f, 0.9999999816164293f, 0.9999999586369661f, 0.0000000000000000f, 0.0000958737990960f, 0.0001917475973107f, 0.0002876213937629f,
        1.0000000000000000f, 0.9999999988510268f, 0.9999999954041073f, 0.9999999896592415f, 0.0000000000000000f, 0.0000479368996031f, 0.0000958737990960f, 0.0001438106983686f
    };
}

namespace sse
{
    // Make set of butterfly implementations
    #define FFT_BUTTERFLY_DIRECT_NAME       butterfly_direct_aa
    #define FFT_BUTTERFLY_REVERSE_NAME      butterfly_reverse_aa
    #define LS_RE                           "movaps"
    #define LS_IM                           "movaps"
    #include <dsp/arch/x86/sse/fft/butterfly.h>

    #define FFT_BUTTERFLY_DIRECT_NAME       butterfly_direct_au
    #define FFT_BUTTERFLY_REVERSE_NAME      butterfly_reverse_au
    #define LS_RE                           "movaps"
    #define LS_IM                           "movups"
    #include <dsp/arch/x86/sse/fft/butterfly.h>

    #define FFT_BUTTERFLY_DIRECT_NAME       butterfly_direct_ua
    #define FFT_BUTTERFLY_REVERSE_NAME      butterfly_reverse_ua
    #define LS_RE                           "movups"
    #define LS_IM                           "movaps"
    #include <dsp/arch/x86/sse/fft/butterfly.h>

    #define FFT_BUTTERFLY_DIRECT_NAME       butterfly_direct_uu
    #define FFT_BUTTERFLY_REVERSE_NAME      butterfly_reverse_uu
    #define LS_RE                           "movups"
    #define LS_IM                           "movups"
    #include <dsp/arch/x86/sse/fft/butterfly.h>

    #define FFT_BUTTERFLY_DIRECT_NAME       packed_butterfly_direct_a
    #define FFT_BUTTERFLY_REVERSE_NAME      packed_butterfly_reverse_a
    #define LS_RE                           "movaps"
    #include <dsp/arch/x86/sse/fft/p_butterfly.h>

    #define FFT_BUTTERFLY_DIRECT_NAME       packed_butterfly_direct_u
    #define FFT_BUTTERFLY_REVERSE_NAME      packed_butterfly_reverse_u
    #define LS_RE                           "movups"
    #include <dsp/arch/x86/sse/fft/p_butterfly.h>

    // Make set of scramble implementations
    // Use 8-bit reversive algorithm
    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct8_aa
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse8_aa
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct8_aa
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse8_aa
    #define FFT_TYPE                        uint8_t
    #define LS_RE                           "movaps"
    #define LS_IM                           "movaps"
    #include <dsp/arch/x86/sse/fft/scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct8_au
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse8_au
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct8_au
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse8_au
    #define FFT_TYPE                        uint8_t
    #define LS_RE                           "movaps"
    #define LS_IM                           "movups"
    #include <dsp/arch/x86/sse/fft/scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct8_ua
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse8_ua
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct8_ua
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse8_ua
    #define FFT_TYPE                        uint8_t
    #define LS_RE                           "movups"
    #define LS_IM                           "movaps"
    #include <dsp/arch/x86/sse/fft/scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct8_uu
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse8_uu
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct8_uu
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse8_uu
    #define FFT_TYPE                        uint8_t
    #define LS_RE                           "movups"
    #define LS_IM                           "movups"
    #include <dsp/arch/x86/sse/fft/scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   packed_scramble_self_direct8_a
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  packed_scramble_self_reverse8_a
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   packed_scramble_copy_direct8_a
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  packed_scramble_copy_reverse8_a
    #define FFT_TYPE                        uint8_t
    #define LS_RE                           "movaps"
    #include <dsp/arch/x86/sse/fft/p_scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   packed_scramble_self_direct8_u
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  packed_scramble_self_reverse8_u
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   packed_scramble_copy_direct8_u
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  packed_scramble_copy_reverse8_u
    #define FFT_TYPE                        uint8_t
    #define LS_RE                           "movups"
    #include <dsp/arch/x86/sse/fft/p_scramble.h>

    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   conv_scramble_copy_direct8_a
    #define FFT_TYPE                        uint8_t
    #define LS_RE                           "movaps"
    #include <dsp/arch/x86/sse/fft/c_scramble.h>

    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   conv_scramble_copy_direct8_u
    #define FFT_TYPE                        uint8_t
    #define LS_RE                           "movups"
    #include <dsp/arch/x86/sse/fft/c_scramble.h>

    // Use 16-bit reversive algorithm
    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct16_aa
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse16_aa
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct16_aa
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse16_aa
    #define FFT_TYPE                        uint16_t
    #define LS_RE                           "movaps"
    #define LS_IM                           "movaps"
    #include <dsp/arch/x86/sse/fft/scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct16_au
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse16_au
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct16_au
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse16_au
    #define FFT_TYPE                        uint16_t
    #define LS_RE                           "movaps"
    #define LS_IM                           "movups"
    #include <dsp/arch/x86/sse/fft/scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct16_ua
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse16_ua
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct16_ua
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse16_ua
    #define FFT_TYPE                        uint16_t
    #define LS_RE                           "movups"
    #define LS_IM                           "movaps"
    #include <dsp/arch/x86/sse/fft/scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct16_uu
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse16_uu
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct16_uu
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse16_uu
    #define FFT_TYPE                        uint16_t
    #define LS_RE                           "movups"
    #define LS_IM                           "movups"
    #include <dsp/arch/x86/sse/fft/scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   packed_scramble_self_direct16_a
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  packed_scramble_self_reverse16_a
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   packed_scramble_copy_direct16_a
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  packed_scramble_copy_reverse16_a
    #define FFT_TYPE                        uint16_t
    #define LS_RE                           "movaps"
    #include <dsp/arch/x86/sse/fft/p_scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   packed_scramble_self_direct16_u
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  packed_scramble_self_reverse16_u
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   packed_scramble_copy_direct16_u
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  packed_scramble_copy_reverse16_u
    #define FFT_TYPE                        uint16_t
    #define LS_RE                           "movups"
    #include <dsp/arch/x86/sse/fft/p_scramble.h>

    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   conv_scramble_copy_direct16_a
    #define FFT_TYPE                        uint16_t
    #define LS_RE                           "movaps"
    #include <dsp/arch/x86/sse/fft/c_scramble.h>

    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   conv_scramble_copy_direct16_u
    #define FFT_TYPE                        uint16_t
    #define LS_RE                           "movups"
    #include <dsp/arch/x86/sse/fft/c_scramble.h>

    //// Use 32-bit reversive algorithm
    //#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct32_aa
    //#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse32_aa
    //#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct32_aa
    //#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse32_aa
    //#define FFT_TYPE                        uint32_t
    //#define LS_RE                           "movaps"
    //#define LS_IM                           "movaps"
    //#include <dsp/arch/x86/sse/fft/scramble.h>
    //
    //#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct32_au
    //#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse32_au
    //#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct32_au
    //#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse32_au
    //#define FFT_TYPE                        uint32_t
    //#define LS_RE                           "movaps"
    //#define LS_IM                           "movups"
    //#include <dsp/arch/x86/sse/fft/scramble.h>
    //
    //#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct32_ua
    //#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse32_ua
    //#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct32_ua
    //#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse32_ua
    //#define FFT_TYPE                        uint32_t
    //#define LS_RE                           "movups"
    //#define LS_IM                           "movaps"
    //#include <dsp/arch/x86/sse/fft/scramble.h>
    //
    //#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct32_uu
    //#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse32_uu
    //#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct32_uu
    //#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse32_uu
    //#define FFT_TYPE                        uint32_t
    //#define LS_RE                           "movups"
    //#define LS_IM                           "movups"
    //#include <dsp/arch/x86/sse/fft/scramble.h>
    //
    //// Use 64-bit reversive algorithm
    //#ifdef __x86_64__
    //    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct64_aa
    //    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse64_aa
    //    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct64_aa
    //    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse64_aa
    //    #define FFT_TYPE                        uint64_t
    //    #define LS_RE                           "movaps"
    //    #define LS_IM                           "movaps"
    //    #include <dsp/arch/x86/sse/fft/scramble.h>
    //
    //    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct64_au
    //    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse64_au
    //    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct64_au
    //    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse64_au
    //    #define FFT_TYPE                        uint64_t
    //    #define LS_RE                           "movaps"
    //    #define LS_IM                           "movups"
    //    #include <dsp/arch/x86/sse/fft/scramble.h>
    //
    //    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct64_ua
    //    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse64_ua
    //    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct64_ua
    //    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse64_ua
    //    #define FFT_TYPE                        uint64_t
    //    #define LS_RE                           "movups"
    //    #define LS_IM                           "movaps"
    //    #include <dsp/arch/x86/sse/fft/scramble.h>
    //
    //    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct64_uu
    //    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse64_uu
    //    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct64_uu
    //    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse64_uu
    //    #define FFT_TYPE                        uint64_t
    //    #define LS_RE                           "movups"
    //    #define LS_IM                           "movups"
    //    #include <dsp/arch/x86/sse/fft/scramble.h>
    //#endif /* __x86_64__ */

    // Make set of scramble-switch implementations
    #define FFT_SCRAMBLE_SELF_DIRECT_NAME       scramble_self_direct
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME       scramble_copy_direct
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME      scramble_self_reverse
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME      scramble_copy_reverse
    #define FFT_SCRAMBLE_DIRECT_NAME            scramble_direct
    #define FFT_SCRAMBLE_REVERSE_NAME           scramble_reverse
    #define FFT_MODE                            aa
    #include <dsp/arch/x86/sse/fft/switch.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME       scramble_self_direct
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME       scramble_copy_direct
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME      scramble_self_reverse
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME      scramble_copy_reverse
    #define FFT_SCRAMBLE_DIRECT_NAME            scramble_direct
    #define FFT_SCRAMBLE_REVERSE_NAME           scramble_reverse
    #define FFT_MODE                            au
    #include <dsp/arch/x86/sse/fft/switch.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME       scramble_self_direct
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME       scramble_copy_direct
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME      scramble_self_reverse
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME      scramble_copy_reverse
    #define FFT_SCRAMBLE_DIRECT_NAME            scramble_direct
    #define FFT_SCRAMBLE_REVERSE_NAME           scramble_reverse
    #define FFT_MODE                            ua
    #include <dsp/arch/x86/sse/fft/switch.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME       scramble_self_direct
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME       scramble_copy_direct
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME      scramble_self_reverse
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME      scramble_copy_reverse
    #define FFT_SCRAMBLE_DIRECT_NAME            scramble_direct
    #define FFT_SCRAMBLE_REVERSE_NAME           scramble_reverse
    #define FFT_MODE                            uu
    #include <dsp/arch/x86/sse/fft/switch.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME       packed_scramble_self_direct
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME       packed_scramble_copy_direct
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME      packed_scramble_self_reverse
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME      packed_scramble_copy_reverse
    #define FFT_SCRAMBLE_DIRECT_NAME            packed_scramble_direct
    #define FFT_SCRAMBLE_REVERSE_NAME           packed_scramble_reverse
    #define FFT_REPACK                          packed_fft_repack
    #define FFT_REPACK_NORMALIZE                packed_fft_repack_normalize
    #define LS_RE                               "movaps"
    #define FFT_MODE                            a
    #include <dsp/arch/x86/sse/fft/p_switch.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME       packed_scramble_self_direct
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME       packed_scramble_copy_direct
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME      packed_scramble_self_reverse
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME      packed_scramble_copy_reverse
    #define FFT_SCRAMBLE_DIRECT_NAME            packed_scramble_direct
    #define FFT_SCRAMBLE_REVERSE_NAME           packed_scramble_reverse
    #define FFT_REPACK                          packed_fft_repack
    #define FFT_REPACK_NORMALIZE                packed_fft_repack_normalize
    #define LS_RE                               "movups"
    #define FFT_MODE                            u
    #include <dsp/arch/x86/sse/fft/p_switch.h>


    void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
    {
        // Check bounds
        if (rank <= 2)
        {
            if (rank == 2)
            {
                float s0_re     = src_re[0] + src_re[1];
                float s1_re     = src_re[0] - src_re[1];
                float s2_re     = src_re[2] + src_re[3];
                float s3_re     = src_re[2] - src_re[3];

                float s0_im     = src_im[0] + src_im[1];
                float s1_im     = src_im[0] - src_im[1];
                float s2_im     = src_im[2] + src_im[3];
                float s3_im     = src_im[2] - src_im[3];

                dst_re[0]       = s0_re + s2_re;
                dst_re[1]       = s1_re + s3_im;
                dst_re[2]       = s0_re - s2_re;
                dst_re[3]       = s1_re - s3_im;

                dst_im[0]       = s0_im + s2_im;
                dst_im[1]       = s1_im - s3_re;
                dst_im[2]       = s0_im - s2_im;
                dst_im[3]       = s1_im + s3_re;
            }
            else if (rank == 1)
            {
                // s0' = s0 + s1
                // s1' = s0 - s1
                float s1_re     = src_re[1];
                float s1_im     = src_im[1];
                dst_re[1]       = src_re[0] - s1_re;
                dst_im[1]       = src_im[0] - s1_im;
                dst_re[0]       = src_re[0] + s1_re;
                dst_im[0]       = src_im[0] + s1_im;
            }
            else
            {
                dst_re[0]       = src_re[0];
                dst_im[0]       = src_im[0];
            }
            return;
        }

        // Iterate butterflies
        if (sse_aligned(dst_re))
        {
            if (sse_aligned(dst_im))
            {
                scramble_direct_aa(dst_re, dst_im, src_re, src_im, rank);

                for (size_t i=2; i < rank; ++i)
                    butterfly_direct_aa(dst_re, dst_im, i /*1 << i*/, 1 << (rank - i - 1));
            }
            else
            {
                scramble_direct_au(dst_re, dst_im, src_re, src_im, rank);

                for (size_t i=2; i < rank; ++i)
                    butterfly_direct_au(dst_re, dst_im, i /*1 << i*/, 1 << (rank - i - 1));
            }
        }
        else
        {
            if (sse_aligned(dst_im))
            {
                scramble_direct_ua(dst_re, dst_im, src_re, src_im, rank);

                for (size_t i=2; i < rank; ++i)
                    butterfly_direct_ua(dst_re, dst_im, i /*1 << i*/, 1 << (rank - i - 1));
            }
            else
            {
                scramble_direct_uu(dst_re, dst_im, src_re, src_im, rank);

                for (size_t i=2; i < rank; ++i)
                    butterfly_direct_uu(dst_re, dst_im, i /*1 << i*/, 1 << (rank - i - 1));
            }
        }
    }

    void packed_direct_fft(float *dst, const float *src, size_t rank)
    {
        // Check bounds
        if (rank <= 2)
        {
            if (rank == 2)
            {
                float s0_re     = dst[0] + dst[2];
                float s1_re     = dst[0] - dst[2];
                float s0_im     = dst[1] + dst[3];
                float s1_im     = dst[1] - dst[3];

                float s2_re     = dst[4] + dst[6];
                float s3_re     = dst[4] - dst[6];
                float s2_im     = dst[5] + dst[7];
                float s3_im     = dst[5] - dst[7];

                dst[0]          = s0_re + s2_re;
                dst[1]          = s0_im + s2_im;
                dst[2]          = s1_re + s3_im;
                dst[3]          = s1_im - s3_re;

                dst[4]          = s0_re - s2_re;
                dst[5]          = s0_im - s2_im;
                dst[6]          = s1_re - s3_im;
                dst[7]          = s1_im + s3_re;
            }
            else if (rank == 1)
            {
                // s0' = s0 + s1
                // s1' = s0 - s1
                float s1_re     = src[2];
                float s1_im     = src[3];
                dst[2]          = src[0] - s1_re;
                dst[3]          = src[1] - s1_im;
                dst[0]          = src[0] + s1_re;
                dst[1]          = src[1] + s1_im;
            }
            else
            {
                dst[0]          = src[0];
                dst[1]          = src[1];
            }
            return;
        }

        // Iterate butterflies
        if (sse_aligned(dst))
        {
            packed_scramble_direct_a(dst, src, rank);

            for (size_t i=2; i < rank; ++i)
                packed_butterfly_direct_a(dst, i /*1 << i*/, 1 << (rank - i - 1));

            packed_fft_repack_a(dst, rank);
        }
        else
        {
            packed_scramble_direct_u(dst, src, rank);

            for (size_t i=2; i < rank; ++i)
                packed_butterfly_direct_u(dst, i /*1 << i*/, 1 << (rank - i - 1));

            packed_fft_repack_u(dst, rank);
        }
    }

    void conv_direct_fft(float *dst, const float *src, size_t rank)
    {
        // Check bounds
        if (rank <= 2)
        {
            if (rank == 2)
            {
                float r0        = src[0];
                float r1        = src[2];

                // Re-shuffle output to store [re0, re1, re2, re3, im0, im1, im2, im3]
                dst[0]          = r0 + r1;
                dst[1]          = 0.0f;
                dst[2]          = r0;
                dst[3]          = - r1;
                dst[4]          = r0 - r1;
                dst[5]          = 0.0f;
                dst[6]          = r0;
                dst[7]          = + r1;
            }
            else if (rank == 1)
            {
                // s0' = s0 + s1
                // s1' = s0 - s1
                dst[0]          = src[0] + src[1];
                dst[1]          = 0.0f;
                dst[2]          = src[0] - src[1];
                dst[3]          = 0.0f;
            }
            else
            {
                dst[0]          = src[0];
                dst[1]          = 0.0f;
            }
            return;
        }

        // Iterate butterflies
        if (sse_aligned(dst))
        {
            if (rank <= 8)
                conv_scramble_copy_direct8_a(dst, src, rank-3);
            else //if (rank <= 16)
                conv_scramble_copy_direct16_a(dst, src, rank-3);

            for (size_t i=2; i < rank; ++i)
                packed_butterfly_direct_a(dst, i /*1 << i*/, 1 << (rank - i - 1));

            packed_fft_repack_a(dst, rank);
        }
        else
        {
            if (rank <= 8)
                conv_scramble_copy_direct8_u(dst, src, rank-3);
            else //if (rank <= 16)
                conv_scramble_copy_direct16_u(dst, src, rank-3);

            for (size_t i=2; i < rank; ++i)
                packed_butterfly_direct_u(dst, i /*1 << i*/, 1 << (rank - i - 1));

            packed_fft_repack_u(dst, rank);
        }
    }

    void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
    {
        // Check bounds
        if (rank <= 2)
        {
            if (rank == 2)
            {
                float s0_re     = src_re[0] + src_re[1];
                float s1_re     = src_re[0] - src_re[1];
                float s2_re     = src_re[2] + src_re[3];
                float s3_re     = src_re[2] - src_re[3];

                float s0_im     = src_im[0] + src_im[1];
                float s1_im     = src_im[0] - src_im[1];
                float s2_im     = src_im[2] + src_im[3];
                float s3_im     = src_im[2] - src_im[3];

                dst_re[0]       = (s0_re + s2_re)*0.25f;
                dst_re[1]       = (s1_re - s3_im)*0.25f;
                dst_re[2]       = (s0_re - s2_re)*0.25f;
                dst_re[3]       = (s1_re + s3_im)*0.25f;

                dst_im[0]       = (s0_im + s2_im)*0.25f;
                dst_im[1]       = (s1_im + s3_re)*0.25f;
                dst_im[2]       = (s0_im - s2_im)*0.25f;
                dst_im[3]       = (s1_im - s3_re)*0.25f;
            }
            else if (rank == 1)
            {
                // s0' = s0 + s1
                // s1' = s0 - s1
                float s1_re     = src_re[1];
                float s1_im     = src_im[1];
                dst_re[1]       = (src_re[0] - s1_re) * 0.5f;
                dst_im[1]       = (src_im[0] - s1_im) * 0.5f;
                dst_re[0]       = (src_re[0] + s1_re) * 0.5f;
                dst_im[0]       = (src_im[0] + s1_im) * 0.5f;
            }
            else
            {
                dst_re[0]       = src_re[0];
                dst_im[0]       = src_im[0];
            }
            return;
        }

        // Iterate butterflies
        if (sse_aligned(dst_re))
        {
            if (sse_aligned(dst_im))
            {
                scramble_reverse_aa(dst_re, dst_im, src_re, src_im, rank);

                for (size_t i=2; i < rank; ++i)
                    butterfly_reverse_aa(dst_re, dst_im, i, 1 << (rank - i - 1));
            }
            else
            {
                scramble_reverse_au(dst_re, dst_im, src_re, src_im, rank);

                for (size_t i=2; i < rank; ++i)
                    butterfly_reverse_au(dst_re, dst_im, i, 1 << (rank - i - 1));
            }
        }
        else
        {
            if (sse_aligned(dst_im))
            {
                scramble_reverse_ua(dst_re, dst_im, src_re, src_im, rank);

                for (size_t i=2; i < rank; ++i)
                    butterfly_reverse_ua(dst_re, dst_im, i, 1 << (rank - i - 1));
            }
            else
            {
                scramble_reverse_uu(dst_re, dst_im, src_re, src_im, rank);

                for (size_t i=2; i < rank; ++i)
                    butterfly_reverse_uu(dst_re, dst_im, i, 1 << (rank - i - 1));
            }
        }

        dsp::normalize_fft(dst_re, dst_im, dst_re, dst_im, rank);
    }

    void packed_reverse_fft(float *dst, const float *src, size_t rank)
    {
        // Check bounds
        if (rank <= 2)
        {
            if (rank == 2)
            {
                float s0_re     = src[0] + src[2];
                float s1_re     = src[0] - src[2];
                float s2_re     = src[4] + src[6];
                float s3_re     = src[4] - src[6];

                float s0_im     = src[1] + src[3];
                float s1_im     = src[1] - src[3];
                float s2_im     = src[5] + src[7];
                float s3_im     = src[5] - src[7];

                dst[0]          = (s0_re + s2_re)*0.25f;
                dst[1]          = (s0_im + s2_im)*0.25f;
                dst[2]          = (s1_re - s3_im)*0.25f;
                dst[3]          = (s1_im + s3_re)*0.25f;

                dst[4]          = (s0_re - s2_re)*0.25f;
                dst[5]          = (s0_im - s2_im)*0.25f;
                dst[6]          = (s1_re + s3_im)*0.25f;
                dst[7]          = (s1_im - s3_re)*0.25f;
            }
            else if (rank == 1)
            {
                // s0' = s0 + s1
                // s1' = s0 - s1
                float s1_re     = src[2];
                float s1_im     = src[3];
                dst[2]          = src[0] - s1_re;
                dst[3]          = src[1] - s1_im;
                dst[0]          = src[0] + s1_re;
                dst[1]          = src[1] + s1_im;
            }
            else
            {
                dst[0]          = src[0];
                dst[1]          = src[1];
            }
            return;
        }

        // Iterate butterflies
        if (sse_aligned(dst))
        {
            packed_scramble_reverse_a(dst, src, rank);

            for (size_t i=2; i < rank; ++i)
                packed_butterfly_reverse_a(dst, i /*1 << i*/, 1 << (rank - i - 1));

            packed_fft_repack_normalize_a(dst, rank);
        }
        else
        {
            packed_scramble_reverse_u(dst, src, rank);

            for (size_t i=2; i < rank; ++i)
                packed_butterfly_reverse_u(dst, i /*1 << i*/, 1 << (rank - i - 1));

            packed_fft_repack_normalize_u(dst, rank);
        }
    }
}

#endif /* DSP_ARCH_X86_SSE_FFT_H_ */
