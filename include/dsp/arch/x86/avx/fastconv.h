/*
 * fastconv.h
 *
 *  Created on: 13 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_FASTCONV_H_
#define DSP_ARCH_X86_AVX_FASTCONV_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

#include <dsp/arch/x86/avx/fastconv/prepare.h>
#include <dsp/arch/x86/avx/fastconv/butterfly.h>
#include <dsp/arch/x86/avx/fastconv/apply.h>

namespace avx
{
    void fastconv_parse(float *dst, const float *src, size_t rank)
    {
        const float *ak = &FFT_A[(rank - 3) << 4];
        const float *wk = &FFT_DW[(rank - 3) << 4];
        size_t np       = 1 << (rank - 1);
        size_t nb       = 1;

        if (np > 4)
        {
            fastconv_direct_prepare(dst, src, ak, wk, np);
            ak         -= 16;
            wk         -= 16;
            np        >>= 1;
            nb        <<= 1;
        }
        else
            fastconv_direct_unpack(dst, src);

        while (np > 4)
        {
            fastconv_direct_butterfly(dst, ak, wk, np, nb);
            ak         -= 16;
            wk         -= 16;
            np        >>= 1;
            nb        <<= 1;
        }

        fastconv_direct_butterfly_last(dst, nb);
    }

    void fastconv_parse_fma3(float *dst, const float *src, size_t rank)
    {
        const float *ak = &FFT_A[(rank - 3) << 4];
        const float *wk = &FFT_DW[(rank - 3) << 4];
        size_t np       = 1 << (rank - 1);
        size_t nb       = 1;

        if (np > 4)
        {
            fastconv_direct_prepare_fma3(dst, src, ak, wk, np);
            ak         -= 16;
            wk         -= 16;
            np        >>= 1;
            nb        <<= 1;
        }
        else
            fastconv_direct_unpack(dst, src);

        while (np > 4)
        {
            fastconv_direct_butterfly_fma3(dst, ak, wk, np, nb);
            ak         -= 16;
            wk         -= 16;
            np        >>= 1;
            nb        <<= 1;
        }

        fastconv_direct_butterfly_last_fma3(dst, nb);
    }

    void fastconv_restore(float *dst, float *tmp, size_t rank)
    {
        size_t nb = 1 << (rank - 3), np = 4;
        const float *ak = FFT_A;
        const float *wk = FFT_DW;

        fastconv_reverse_prepare(tmp, nb);
        if ((nb >>= 1) <= 0)
        {
            fastconv_reverse_unpack(dst, tmp, rank);
            return;
        }
        ak     += 16;
        wk     += 16;
        np    <<= 1;

        while (nb > 1)
        {
            fastconv_reverse_butterfly(tmp, ak, wk, np, nb);
            ak     += 16;
            wk     += 16;
            np    <<= 1;
            nb    >>= 1;
        }

        fastconv_reverse_butterfly_last(dst, tmp, ak, wk, np);
    }

    void fastconv_restore_fma3(float *dst, float *tmp, size_t rank)
    {
        size_t nb = 1 << (rank - 3), np = 4;
        const float *ak = FFT_A;
        const float *wk = FFT_DW;

        fastconv_reverse_prepare_fma3(tmp, nb);
        if ((nb >>= 1) <= 0)
        {
            fastconv_reverse_unpack(dst, tmp, rank);
            return;
        }
        ak     += 16;
        wk     += 16;
        np    <<= 1;

        while (nb > 1)
        {
            fastconv_reverse_butterfly_fma3(tmp, ak, wk, np, nb);
            ak     += 16;
            wk     += 16;
            np    <<= 1;
            nb    >>= 1;
        }

        fastconv_reverse_butterfly_last_fma3(dst, tmp, ak, wk, np);
    }

    void fastconv_apply(float *dst, float *tmp, const float *c1, const float *c2, size_t rank)
    {
        size_t nb = 1 << (rank - 3), np = 4;
        const float *ak = FFT_A;
        const float *wk = FFT_DW;

        fastconv_apply_prepare(tmp, c1, c2, nb);
        if ((nb >>= 1) <= 0)
        {
            fastconv_reverse_unpack_adding(dst, tmp, rank);
            return;
        }
        ak     += 16;
        wk     += 16;
        np    <<= 1;

        while (nb > 1)
        {
            fastconv_reverse_butterfly(tmp, ak, wk, np, nb);
            ak     += 16;
            wk     += 16;
            np    <<= 1;
            nb    >>= 1;
        }

        fastconv_reverse_butterfly_last_adding(dst, tmp, ak, wk, np);
    }

    void fastconv_apply_fma3(float *dst, float *tmp, const float *c1, const float *c2, size_t rank)
    {
        size_t nb = 1 << (rank - 3), np = 4;
        const float *ak = FFT_A;
        const float *wk = FFT_DW;

        fastconv_apply_prepare_fma3(tmp, c1, c2, nb);
        if ((nb >>= 1) <= 0)
        {
            fastconv_reverse_unpack_adding(dst, tmp, rank);
            return;
        }
        ak     += 16;
        wk     += 16;
        np    <<= 1;

        while (nb > 1)
        {
            fastconv_reverse_butterfly_fma3(tmp, ak, wk, np, nb);
            ak     += 16;
            wk     += 16;
            np    <<= 1;
            nb    >>= 1;
        }

        fastconv_reverse_butterfly_last_adding_fma3(dst, tmp, ak, wk, np);
    }

    void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank)
    {
        const float *ak = &FFT_A[(rank - 3) << 4];
        const float *wk = &FFT_DW[(rank - 3) << 4];
        size_t np       = 1 << (rank - 1);
        size_t nb       = 1;

        if (np > 4)
        {
            fastconv_direct_prepare(tmp, src, ak, wk, np);
            ak         -= 16;
            wk         -= 16;
            np        >>= 1;
            nb        <<= 1;
        }
        else
            fastconv_direct_unpack(tmp, src);

        while (np > 4)
        {
            fastconv_direct_butterfly(tmp, ak, wk, np, nb);
            ak         -= 16;
            wk         -= 16;
            np        >>= 1;
            nb        <<= 1;
        }

        fastconv_apply_internal(tmp, c, nb);

        if ((nb >>= 1) <= 0)
        {
            fastconv_reverse_unpack_adding(dst, tmp, rank);
            return;
        }
        ak     += 16;
        wk     += 16;
        np    <<= 1;

        while (nb > 1)
        {
            fastconv_reverse_butterfly(tmp, ak, wk, np, nb);
            ak     += 16;
            wk     += 16;
            np    <<= 1;
            nb    >>= 1;
        }

        fastconv_reverse_butterfly_last_adding(dst, tmp, ak, wk, np);
    }

    void fastconv_parse_apply_fma3(float *dst, float *tmp, const float *c, const float *src, size_t rank)
    {
        const float *ak = &FFT_A[(rank - 3) << 4];
        const float *wk = &FFT_DW[(rank - 3) << 4];
        size_t np       = 1 << (rank - 1);
        size_t nb       = 1;

        if (np > 4)
        {
            fastconv_direct_prepare_fma3(tmp, src, ak, wk, np);
            ak         -= 16;
            wk         -= 16;
            np        >>= 1;
            nb        <<= 1;
        }
        else
            fastconv_direct_unpack(tmp, src);

        while (np > 4)
        {
            fastconv_direct_butterfly_fma3(tmp, ak, wk, np, nb);
            ak         -= 16;
            wk         -= 16;
            np        >>= 1;
            nb        <<= 1;
        }

        fastconv_apply_internal_fma3(tmp, c, nb);

        if ((nb >>= 1) <= 0)
        {
            fastconv_reverse_unpack_adding(dst, tmp, rank);
            return;
        }
        ak     += 16;
        wk     += 16;
        np    <<= 1;

        while (nb > 1)
        {
            fastconv_reverse_butterfly_fma3(tmp, ak, wk, np, nb);
            ak     += 16;
            wk     += 16;
            np    <<= 1;
            nb    >>= 1;
        }

        fastconv_reverse_butterfly_last_adding_fma3(dst, tmp, ak, wk, np);
    }
}

#endif /* DSP_ARCH_X86_AVX_FASTCONV_H_ */
