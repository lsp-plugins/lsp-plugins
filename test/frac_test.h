#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <core/dsp.h>

namespace frac_test
{
    using namespace lsp;

    const float sse_pi2[] __lsp_aligned16 =
    {
        M_PI_2, M_PI_2, M_PI_2, M_PI_2,
        2, 2, 2, 2
    };

    const float sse_1_pi2[] __lsp_aligned16 =
    {
//        M_PI_2, M_PI_2, M_PI_2, M_PI_2,
        0.5, 0.5, 0.5, 0.5
    };

    /*
        1/1! = 1.00000000000000000000, 1.00000000000000000000, 1.00000000000000000000, 1.00000000000000000000
        1/2! = 0.50000000000000000000, 0.50000000000000000000, 0.50000000000000000000, 0.50000000000000000000
        1/3! = 0.16666666666666665741, 0.16666666666666665741, 0.16666666666666665741, 0.16666666666666665741
        1/4! = 0.04166666666666666435, 0.04166666666666666435, 0.04166666666666666435, 0.04166666666666666435
        1/5! = 0.00833333333333333322, 0.00833333333333333322, 0.00833333333333333322, 0.00833333333333333322
        1/6! = 0.00138888888888888894, 0.00138888888888888894, 0.00138888888888888894, 0.00138888888888888894
        1/7! = 0.00019841269841269841, 0.00019841269841269841, 0.00019841269841269841, 0.00019841269841269841
        1/8! = 0.00002480158730158730, 0.00002480158730158730, 0.00002480158730158730, 0.00002480158730158730
        1/9! = 0.00000275573192239859, 0.00000275573192239859, 0.00000275573192239859, 0.00000275573192239859
        1/10! = 0.00000027557319223986, 0.00000027557319223986, 0.00000027557319223986, 0.00000027557319223986
        1/11! = 0.00000002505210838544, 0.00000002505210838544, 0.00000002505210838544, 0.00000002505210838544
        1/12! = 0.00000000208767569879, 0.00000000208767569879, 0.00000000208767569879, 0.00000000208767569879
        1/13! = 0.00000000016059043837, 0.00000000016059043837, 0.00000000016059043837, 0.00000000016059043837
        1/14! = 0.00000000001147074560, 0.00000000001147074560, 0.00000000001147074560, 0.00000000001147074560
        1/15! = 0.00000000000076471637, 0.00000000000076471637, 0.00000000000076471637, 0.00000000000076471637
     */

    void dump_data(const float *data, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            if (i > 0)
                printf(", ");
            printf("%.7f", data[i]);
        }
        printf("\n");
    }

    namespace sincos
    {
        const float S0[] __lsp_aligned16 = {  1.00000000000000000000,  1.00000000000000000000,  1.00000000000000000000,  1.00000000000000000000  };
        const float S1[] __lsp_aligned16 = { -0.16666666666666665741, -0.16666666666666665741, -0.16666666666666665741, -0.16666666666666665741  };
        const float S2[] __lsp_aligned16 = {  0.00833333333333333322,  0.00833333333333333322,  0.00833333333333333322,  0.00833333333333333322  };
        const float S3[] __lsp_aligned16 = { -0.00019841269841269841, -0.00019841269841269841, -0.00019841269841269841, -0.00019841269841269841  };
        const float S4[] __lsp_aligned16 = {  0.00000275573192239859,  0.00000275573192239859,  0.00000275573192239859,  0.00000275573192239859  };
        const float S5[] __lsp_aligned16 = { -0.00000002505210838544, -0.00000002505210838544, -0.00000002505210838544, -0.00000002505210838544  };

        const float C0[] __lsp_aligned16 = {  1.00000000000000000000,  1.00000000000000000000,  1.00000000000000000000,  1.00000000000000000000  };
        const float C1[] __lsp_aligned16 = { -0.50000000000000000000, -0.50000000000000000000, -0.50000000000000000000, -0.50000000000000000000  };
        const float C2[] __lsp_aligned16 = {  0.04166666666666666435,  0.04166666666666666435,  0.04166666666666666435,  0.04166666666666666435  };
        const float C3[] __lsp_aligned16 = { -0.00138888888888888894, -0.00138888888888888894, -0.00138888888888888894, -0.00138888888888888894  };
        const float C4[] __lsp_aligned16 = {  0.00002480158730158730,  0.00002480158730158730,  0.00002480158730158730,  0.00002480158730158730  };
        const float C5[] __lsp_aligned16 = { -0.00000027557319223986, -0.00000027557319223986, -0.00000027557319223986, -0.00000027557319223986  };

        const float PI[] __lsp_aligned16 = { M_PI, M_PI, M_PI, M_PI  };
        const float PI_2[] __lsp_aligned16 = { M_PI_2, M_PI_2, M_PI_2, M_PI_2  };
    };

    void sincos4(float *sin, float *cos, const float *src)
    {
        using namespace sincos;

        __asm__ __volatile__
        (
            /* Prepare */
            __ASM_EMIT("movaps      (%2), %%xmm0")              /* xmm0 = x */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = x */
            /* We consider that the values are positive */
            __ASM_EMIT("movaps      %[PI_2], %%xmm4")           /* xmm4 = pi/2 */
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")            /* xmm2 = pi/2 */
            __ASM_EMIT("movaps      %[PI], %%xmm2")             /* xmm2 = PI */
            __ASM_EMIT("movaps      %%xmm2, %%xmm3")            /* xmm3 = PI */
            __ASM_EMIT("cmpltps     %%xmm0, %%xmm4")
            __ASM_EMIT("cmpltps     %%xmm1, %%xmm5")
            __ASM_EMIT("subps       %%xmm0, %%xmm2")            /* xmm2 = PI - x */
            __ASM_EMIT("subps       %%xmm1, %%xmm3")            /* xmm3 = PI - x */
            __ASM_EMIT("movaps      %%xmm5, %%xmm7")
            __ASM_EMIT("andps       %%xmm4, %%xmm2")
            __ASM_EMIT("andps       %%xmm5, %%xmm3")
            __ASM_EMIT("andnps      %%xmm0, %%xmm4")
            __ASM_EMIT("andnps      %%xmm1, %%xmm5")
            __ASM_EMIT("orps        %%xmm4, %%xmm2")
            __ASM_EMIT("orps        %%xmm5, %%xmm3")
            __ASM_EMIT("pslld       $31, %%xmm7")               /* xmm7 = s */
            __ASM_EMIT("movaps      %%xmm2, %%xmm0")
            __ASM_EMIT("movaps      %%xmm3, %%xmm1")

            /* Update arguments to match -pi/2 .. pi/2 interval */

            /* Calculate */
            __ASM_EMIT("movaps      %[S5], %%xmm2")             /* xmm2 = S5 */
            __ASM_EMIT("movaps      %[C5], %%xmm3")             /* xmm3 = C5 */
            __ASM_EMIT("mulps       %%xmm1, %%xmm1")            /* xmm1 = x^2 */
            __ASM_EMIT("movaps      %[S4], %%xmm4")             /* xmm4 = S4 */
            __ASM_EMIT("movaps      %[C4], %%xmm5")             /* xmm5 = C4 */
            __ASM_EMIT("mulps       %%xmm1, %%xmm2")            /* xmm2 = S5 * x^2 */
            __ASM_EMIT("mulps       %%xmm1, %%xmm3")            /* xmm3 = C5 * x^2 */
            __ASM_EMIT("addps       %%xmm4, %%xmm2")            /* xmm2 = S4 + S5 * x^2 */
            __ASM_EMIT("addps       %%xmm5, %%xmm3")            /* xmm3 = C4 + C5 * x^2 */
            __ASM_EMIT("movaps      %[S3], %%xmm4")             /* xmm4 = S3 */
            __ASM_EMIT("movaps      %[C3], %%xmm5")             /* xmm5 = C3 */
            __ASM_EMIT("mulps       %%xmm1, %%xmm2")            /* xmm2 = x^2 * (S4 + S5 * x^2) */
            __ASM_EMIT("mulps       %%xmm1, %%xmm3")            /* xmm3 = x^2 * (C4 + C5 * x^2) */
            __ASM_EMIT("addps       %%xmm4, %%xmm2")            /* xmm2 = S3 + x^2 * (S4 + S5 * x^2) */
            __ASM_EMIT("addps       %%xmm5, %%xmm3")            /* xmm3 = C3 + x^2 * (C4 + C5 * x^2) */
            __ASM_EMIT("movaps      %[S2], %%xmm4")             /* xmm4 = S2 */
            __ASM_EMIT("movaps      %[C2], %%xmm5")             /* xmm5 = C2 */
            __ASM_EMIT("mulps       %%xmm1, %%xmm2")            /* xmm2 = x^2 * (S3 + x^2 * (S4 + S5 * x^2)) */
            __ASM_EMIT("mulps       %%xmm1, %%xmm3")            /* xmm3 = x^2 * (C3 + x^2 * (C4 + C5 * x^2)) */
            __ASM_EMIT("addps       %%xmm4, %%xmm2")            /* xmm2 = S2 + x^2 * (S3 + x^2 * (S4 + S5 * x^2)) */
            __ASM_EMIT("addps       %%xmm5, %%xmm3")            /* xmm3 = C2 + x^2 * (C3 + x^2 * (C4 + C5 * x^2)) */
            __ASM_EMIT("movaps      %[S1], %%xmm4")             /* xmm4 = S1 */
            __ASM_EMIT("movaps      %[C1], %%xmm5")             /* xmm5 = C1 */
            __ASM_EMIT("mulps       %%xmm1, %%xmm2")            /* xmm2 = x^2 * (S2 + x^2 * (S3 + x^2 * (S4 + S5 * x^2))) */
            __ASM_EMIT("mulps       %%xmm1, %%xmm3")            /* xmm3 = x^2 * (C2 + x^2 * (C3 + x^2 * (C4 + C5 * x^2))) */
            __ASM_EMIT("addps       %%xmm4, %%xmm2")            /* xmm2 = S1 + x^2 * (S2 + x^2 * (S3 + x^2 * (S4 + S5 * x^2))) */
            __ASM_EMIT("addps       %%xmm5, %%xmm3")            /* xmm3 = C1 + x^2 * (C2 + x^2 * (C3 + x^2 * (C4 + C5 * x^2))) */
            __ASM_EMIT("movaps      %[S0], %%xmm4")             /* xmm4 = S0 */
            __ASM_EMIT("movaps      %[C0], %%xmm5")             /* xmm5 = C0 */
            __ASM_EMIT("mulps       %%xmm1, %%xmm2")            /* xmm2 = x^2 * (S1 + x^2 * (S2 + x^2 * (S3 + x^2 * (S4 + S5 * x^2)))) */
            __ASM_EMIT("mulps       %%xmm1, %%xmm3")            /* xmm3 = x^2 * (C1 + x^2 * (C2 + x^2 * (C3 + x^2 * (C4 + C5 * x^2)))) */
            __ASM_EMIT("addps       %%xmm4, %%xmm2")            /* xmm2 = S0 + x^2 * (S1 + x^2 * (S2 + x^2 * (S3 + x^2 * (S4 + S5 * x^2)))) */
            __ASM_EMIT("addps       %%xmm5, %%xmm3")            /* xmm3 = C0 + x^2 * (C1 + x^2 * (C2 + x^2 * (C3 + x^2 * (C4 + C5 * x^2)))) */
            __ASM_EMIT("mulps       %%xmm0, %%xmm2")            /* xmm2 = x * (S0 + x^2 * (S1 + x^2 * (S2 + x^2 * (S3 + x^2 * (S4 + S5 * x^2))))) */
            __ASM_EMIT("xorps       %%xmm7, %%xmm3")            /* xmm3 = s * x * (S0 + x^2 * (S1 + x^2 * (S2 + x^2 * (S3 + x^2 * (S4 + S5 * x^2))))) */

            /* Store results */
            __ASM_EMIT("movaps      %%xmm2, (%0)")              /* sin = xmm2 */
            __ASM_EMIT("movaps      %%xmm3, (%1)")              /* cos = xmm3 */
            :
            : "r"(sin), "r"(cos), "r"(src),
                [S0] "m"(S0),
                [S1] "m"(S1),
                [S2] "m"(S2),
                [S3] "m"(S3),
                [S4] "m"(S4),
                [S5] "m"(S5),
                [C0] "m"(C0),
                [C1] "m"(C1),
                [C2] "m"(C2),
                [C3] "m"(C3),
                [C4] "m"(C4),
                [C5] "m"(C5),
                [PI] "m"(PI),
                [PI_2] "m"(PI_2)
            : "%xmm0", "%xmm1", "%xmm2",
                "%mm0", "%mm1"
        );

//
//            /* k = 2 * PI */
//            __ASM_EMIT("emms")
//
//            /* Load values */
//            __ASM_EMIT("movaps      (%2), %%xmm0")          /* xmm0 = src [0..3] */
//            __ASM_EMIT("movaps      %3, %%xmm1")            /* xmm1 = k [0..3] */
//            __ASM_EMIT("movaps      %4, %%xmm2")            /* xmm2 = 1/k [0..3] */
//            __ASM_EMIT("movaps      %%xmm1, %%xmm4")        /* xmm4 = k [0..3] */
//
//            /* Calculate */
//            __ASM_EMIT("mulps       %%xmm0, %%xmm2")        /* xmm2 = src / k [0..3] */
//            __ASM_EMIT("movhlps     %%xmm2, %%xmm3")        /* xmm3 = src / k [2..3] */
//            __ASM_EMIT("cvttps2pi   %%xmm2, %%mm0")         /* mm0  = int(src / k) [0..1] */
//            __ASM_EMIT("cvttps2pi   %%xmm3, %%mm1")         /* mm1  = int(src / k) [2..3] */
//            __ASM_EMIT("cvtpi2ps    %%mm0, %%xmm2")         /* xmm1 = int(src / k) [0..1] */
//            __ASM_EMIT("cvtpi2ps    %%mm1, %%xmm3")         /* xmm3 = int(src / k) [2..3] */
//            __ASM_EMIT("movlhps     %%xmm3, %%xmm2")        /* xmm2 = int(src / k) [0..3] */
//
//            __ASM_EMIT("mulps       %%xmm1, %%xmm2")        /* xmm1 = int(src / k) * k [0..3] */
//            __ASM_EMIT("subps       %%xmm2, %%xmm0")        /* xmm0 = src - int(src / k) * k [0..3] */
//
//            __ASM_EMIT("xorps       %%xmm2, %%xmm2")        /* xmm2 = 0 [0..3] */
//            __ASM_EMIT("movaps      %%xmm0, %%xmm1")        /* xmm1 = src - int(src / k) * k [0..3] */
//            __ASM_EMIT("cmpltps     %%xmm0, %%xmm2")        /* xmm2 = sign mask [0..3] */
//            __ASM_EMIT("addps       %%xmm4, %%xmm1")        /* xmm1 = k + negative values [0..3] */
//            __ASM_EMIT("andps       %%xmm2, %%xmm0")        /* xmm0 = negative values [0..3] */
//            __ASM_EMIT("andnps      %%xmm1, %%xmm2")        /* xmm1 = positive values [0..3] */
//            __ASM_EMIT("orps        %%xmm2, %%xmm0")        /* xmm0 = values 0..k [0..3] */
//
//            /* Store result */
//            __ASM_EMIT("movaps      %%xmm0, (%0)")          /* xmm0 = src - int (src / k) * k [0..3] */
//            __ASM_EMIT("movaps      %%xmm0, (%1)")          /* xmm0 = src - int (src / k) * k [0..3] */
    }

    int test(int argc, const char **argv)
    {
        dsp::init();

        float test[4] __lsp_aligned16;
        float sin[4] __lsp_aligned16;
        float cos[4] __lsp_aligned16;

        size_t base = 1;
        for (size_t i=1; i < 16; ++i)
        {
            base *= i;
            double factor = (1.0 / base);

            printf("1/%d! = %.20f, %.20f, %.20f, %.20f\n", int(i), factor, factor, factor, factor);
        }

//        float x = 0.1;//M_PI / 64;

//        for (float i= -40; i<=40; )
        {
            test[0] = 0.5;
            test[1] = 1.5707964;
            test[2] = 1.9634955;
            test[3] = 2.7488935;

/*            w[0] = 1.5707964 {91.4532242 1.0000036}
w[1] = 1.9634955 {786.5607910 640.9477539}
w[2] = 2.3561945 {3728.3395996 9658.6279297}
w[3] = 2.7488935 {-80113.2265625 353237.6875000}*/
//            for (size_t n=0; n<4; ++n)
//            {
//                test[n] = i;
//                i += x;
//            }

            printf("arg: "); dump_data(test, 4);
            sincos4(sin, cos, test);
            printf("sin: "); dump_data(sin, 4);
            printf("cos: "); dump_data(cos, 4);
        }

        return 0;
    }
    
}
