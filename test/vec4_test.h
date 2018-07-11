#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <core/dsp.h>

namespace vec4_test
{
    using namespace lsp;

    #define print_vec(s, v)     printf("%s: { %.3f, %.3f, %.3f, %.3f }\n", s, v[0], v[1], v[2], v[3])
    #define print_vec2(s, v)    printf("%s: { %.3f, %.3f, %.3f, %.3f, %.3f }\n", s, v[0], v[1], v[2], v[3], v[4])

    int test(int argc, const char **argv)
    {
        dsp::init();

/*        float v1[4]     = { 1.0f, 2.0f, 3.0f, 4.0f };
        float v2[4]     = { 1.0f, 10.0f, 100.0f, 1000.0f };
        float r         = 0.0f;

        // Issue multiplication
        print_vec("v1", v1);
        print_vec("v2", v2);
        r               = dsp::vec4_scalar_mul(v1, v2);
        printf("result = %.3f\n", r);

        // Push data to vector
        print_vec("v1", v1);
        r               = dsp::vec4_push(v1, 5.0f);
        printf("result = %.3f\n", r);

        // Unshift data to vector
        print_vec("v1", v1);
        r               = dsp::vec4_unshift(v1, 1.0f);
        printf("result = %.3f\n", r);

        // Output result
        print_vec("v1", v1);*/

        float buf[4]    = { 1, 2, 3, 4 };
        float ir[8]     = { 0.5, 0.4, 0.3, 0.2, 0.1, 0.0, 0.0, 0.0 };

        print_vec("buf", buf);
        print_vec2("ir", ir);

        float result    = dsp::biquad_process(buf, ir, 5.0);
        printf("result = %.3f\n", result);

        print_vec("buf", buf);
        print_vec2("ir", ir);

        /*
            // Native
            buf: { 1.000, 2.000, 3.000, 4.000 }
            ir: { 0.500, 0.400, 0.300, 0.200, 0.100 }
            result = 3.500
            buf: { 3.500, 5.000, 1.000, 2.000 }
            ir: { 0.500, 0.400, 0.300, 0.200, 0.100 }

            // SSE
            buf: { 1.000, 2.000, 3.000, 4.000 }
            ir: { 0.500, 0.400, 0.300, 0.200, 0.100 }
            result = 3.500
            buf: { 3.500, 5.000, 1.000, 2.000 }
            ir: { 0.500, 0.400, 0.300, 0.200, 0.100 }
         */

        return 0;
    }

    #undef print_vec
}
