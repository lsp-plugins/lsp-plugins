#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <dsp/dsp.h>
#include <core/windows.h>
#include <core/envelope.h>

//#define __AVX__

namespace avx_test
{
    using namespace lsp;

    float data[] =
    {
        1, 2, 3, 4, 5, 6, 7, 8,
        9, 10, 11, 12, 13, 14, 15, 16
    };

#ifdef __AVX__
    float push(float *v, float s)
    {
        __asm__ __volatile__
        (
            __ASM_EMIT("vmovaps         (%[v]), %%ymm1")                    // ymm1 = v0 v1 v2 v3 v4 v5 v6 v7
            __ASM_EMIT("vpermilps       $0x93, %%ymm1, %%ymm1")             // ymm1 = v3 v0 v1 v2 v7 v4 v5 v6
            __ASM_EMIT("vextractf128    $0x01, %%ymm1, %%xmm2")             // ymm2 = v7 v4 v5 v6 0 0 0 0
            __ASM_EMIT("vinsertf128     $0x01, %%xmm1, %%ymm0, %%ymm0")     // ymm0 = s ? ? ? v3 v0 v1 v2
            __ASM_EMIT("vblendps        $0x11, %%ymm0, %%ymm1, %%ymm1")     // ymm1 = s v0 v1 v2 v3 v4 v5 v6
            __ASM_EMIT("vmovss          %%xmm2, %%xmm0, %%xmm0")            // xmm0 = v7 ? ? ? 0 0 0 0
            __ASM_EMIT("vmovaps         %%ymm1, (%[v])")                    // *v   = s v0 v1 v2 v3 v4 v5 v6
            : [s] "+Yz" (s)
            : [v] "r" (v)
            : "memory"
        );
        return s;
    }

    int test(int argc, const char **argv)
    {
        float v[8] __lsp_aligned64;
        for (size_t i=0; i<8; ++i)
            v[i] = -ssize_t(i + 1);

        for (size_t i=0; i<sizeof(data)/sizeof(float); ++i)
        {
            printf("push %.0f -> [%.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f]\n", data[i], v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
            float x = push(v, data[i]);
            printf("pop %.0f <- [%.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f]\n", x, v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
        }

        return 0;
    }
#else
    int test(int argc, const char **argv)
    {
        printf("AVX IS OFF, can not perform test\n");
        return 0;
    }
#endif
}

