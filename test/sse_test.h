#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <core/dsp.h>
#include <core/windows.h>
#include <core/envelope.h>

#include "common.h"

namespace lsp
{
    namespace native
    {
        void scale2(float *dst, float k, size_t count);
        void scale3(float *dst, const float *src, float k, size_t count);

        void add2(float *dst, const float *src, size_t count);
        void sub2(float *dst, const float *src, size_t count);
        void mul2(float *dst, const float *src, size_t count);
        void div2(float *dst, const float *src, size_t count);

        void add3(float *dst, const float *src1, const float *src2, size_t count);
        void sub3(float *dst, const float *src1, const float *src2, size_t count);
        void mul3(float *dst, const float *src1, const float *src2, size_t count);
        void div3(float *dst, const float *src1, const float *src2, size_t count);

        void scale_add3(float *dst, const float *src, float k, size_t count);
        void scale_sub3(float *dst, const float *src, float k, size_t count);
        void scale_mul3(float *dst, const float *src, float k, size_t count);
        void scale_div3(float *dst, const float *src, float k, size_t count);

        void mix2(float *dst, const float *src, float k1, float k2, size_t count);
        void mix3(float *dst, const float *src1, const float *src2, float k1, float k2, float k3, size_t count);
        void mix4(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count);

        void mix_copy2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
        void mix_copy3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count);
        void mix_copy4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);

        void mix_add2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
        void mix_add3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count);
        void mix_add4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);

        void fill(float *dst, float value, size_t count);

        float h_sum(const float *src, size_t count);
        float h_sqr_sum(const float *src, size_t count);
        float h_abs_sum(const float *src, size_t count);

        void abs1(float *src, size_t count);
        void abs2(float *dst, const float *src, size_t count);
        void abs_add2(float *dst, const float *src, size_t count);
        void abs_sub2(float *dst, const float *src, size_t count);
        void abs_mul2(float *dst, const float *src, size_t count);
        void abs_div2(float *dst, const float *src, size_t count);

        void reverse1(float *dst, size_t count);
        void reverse2(float *dst, const float *src, size_t count);

        float min(const float *src, size_t count);
        float max(const float *src, size_t count);
        void minmax(const float *src, size_t count, float *min, float *max);

        float abs_min(const float *src, size_t count);
        float abs_max(const float *src, size_t count);
        void abs_minmax(const float *src, size_t count, float *min, float *max);
    }

    namespace sse
    {
        void scale2(float *dst, float k, size_t count);
        void scale3(float *dst, const float *src, float k, size_t count);

        void add2(float *dst, const float *src, size_t count);
        void sub2(float *dst, const float *src, size_t count);
        void mul2(float *dst, const float *src, size_t count);
        void div2(float *dst, const float *src, size_t count);

        void add3(float *dst, const float *src1, const float *src2, size_t count);
        void sub3(float *dst, const float *src1, const float *src2, size_t count);
        void mul3(float *dst, const float *src1, const float *src2, size_t count);
        void div3(float *dst, const float *src1, const float *src2, size_t count);

        void scale_add3(float *dst, const float *src, float k, size_t count);
        void scale_sub3(float *dst, const float *src, float k, size_t count);
        void scale_mul3(float *dst, const float *src, float k, size_t count);
        void scale_div3(float *dst, const float *src, float k, size_t count);

        void mix2(float *dst, const float *src, float k1, float k2, size_t count);
        void mix3(float *dst, const float *src1, const float *src2, float k1, float k2, float k3, size_t count);
        void mix4(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count);

        void mix_copy2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
        void mix_copy3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count);
        void mix_copy4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);

        void mix_add2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
        void mix_add3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count);
        void mix_add4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);

        void move(float *dst, const float *src, size_t count);
        void fill(float *dst, float value, size_t count);

        float h_sum(const float *src, size_t count);
        float h_sqr_sum(const float *src, size_t count);
        float h_abs_sum(const float *src, size_t count);

        void abs1(float *src, size_t count);
        void abs2(float *dst, const float *src, size_t count);
        void abs_add2(float *dst, const float *src, size_t count);
        void abs_sub2(float *dst, const float *src, size_t count);
        void abs_mul2(float *dst, const float *src, size_t count);
        void abs_div2(float *dst, const float *src, size_t count);

        void reverse1(float *dst, size_t count);
        void reverse2(float *dst, const float *src, size_t count);

        float min(const float *src, size_t count);
        float max(const float *src, size_t count);
        void minmax(const float *src, size_t count, float *min, float *max);

        float abs_min(const float *src, size_t count);
        float abs_max(const float *src, size_t count);
        void abs_minmax(const float *src, size_t count, float *min, float *max);
    }
}

namespace sse_test
{
    using namespace test;
    using namespace lsp;

    typedef void (* unary_math_t) (float *dst, size_t count);
    typedef void (* binary_math_t) (float *dst, const float *src, size_t count);
    typedef void (* ternary_math_t) (float *dst, const float *src1, const float *src2, size_t count);
    typedef void (* scale_op_t) (float *dst, const float *src, float k, size_t count);
    typedef float (* hfunc_t) (const float *src, size_t count);
    typedef void (* minmax_t) (const float *src, size_t count, float *a, float *b);

    #define TEST_FOREACH(var, ...)    \
        const size_t ___sizes[] = { __VA_ARGS__ }; \
        for (size_t ___i=0, var=___sizes[0]; ___i<(sizeof(___sizes)/sizeof(size_t)); ++___i, var=___sizes[___i])

    bool test_scale3()
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0xfff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                FBuffer src(sz, mask & 0x01);
                FBuffer dst1(sz, mask & 0x02);
                FBuffer dst2(sz, mask & 0x02);

                native::scale3(dst1, src, 2.0f, sz);
                sse::scale3(dst2, src, 2.0f, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_scale2()
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0xfff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                FBuffer dst1(sz, mask & 0x01);
                FBuffer dst2(dst1);

                native::scale2(dst1, 2.0f, sz);
                sse::scale2(dst2, 2.0f, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_scale_op(scale_op_t native, scale_op_t sse)
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0xfff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                FBuffer dst1(sz, mask & 0x01);
                FBuffer dst2(dst1);
                FBuffer src(sz, mask & 0x02);

                native(dst1, src, 2.0f, sz);
                sse(dst2, src, 2.0f, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_mix2()
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0xfff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                FBuffer dst1(sz, mask & 0x01);
                FBuffer dst2(dst1);
                FBuffer src(sz, mask & 0x02);

                native::mix2(dst1, src, 2.0f, 4.0f, sz);
                sse::mix2(dst2, src, 2.0f, 4.0f, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_mix_copy2()
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0xfff)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                FBuffer dst1(sz, mask & 0x01);
                FBuffer dst2(sz, mask & 0x01);
                FBuffer src1(sz, mask & 0x02);
                FBuffer src2(sz, mask & 0x04);

                native::mix_copy2(dst1, src1, src2, 2.0f, 4.0f, sz);
                sse::mix_copy2(dst2, src1, src2, 2.0f, 4.0f, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_mix_add2()
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0xfff)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                FBuffer dst1(sz, mask & 0x01);
                FBuffer dst2(dst1);
                FBuffer src1(sz, mask & 0x02);
                FBuffer src2(sz, mask & 0x04);

                native::mix_add2(dst1, src1, src2, 2.0f, 4.0f, sz);
                sse::mix_add2(dst2, src1, src2, 2.0f, 4.0f, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_mix3()
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0xfff)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                FBuffer dst1(sz, mask & 0x01);
                FBuffer dst2(dst1);
                FBuffer src1(sz, mask & 0x02);
                FBuffer src2(sz, mask & 0x04);

                native::mix3(dst1, src1, src2, 2.0f, 4.0f, 8.0f, sz);
                sse::mix3(dst2, src1, src2, 2.0f, 4.0f, 8.0f, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_mix_copy3()
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0xfff)
        {
            for (size_t mask=0; mask <= 0x0f; ++mask)
            {
                FBuffer dst1(sz, mask & 0x01);
                FBuffer dst2(sz, mask & 0x01);
                FBuffer src1(sz, mask & 0x02);
                FBuffer src2(sz, mask & 0x04);
                FBuffer src3(sz, mask & 0x08);

                native::mix_copy3(dst1, src1, src2, src3, 2.0f, 4.0f, 8.0f, sz);
                sse::mix_copy3(dst2, src1, src2, src3, 2.0f, 4.0f, 8.0f, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_mix_add3()
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0xfff)
        {
            for (size_t mask=0; mask <= 0x0f; ++mask)
            {
                FBuffer dst1(sz, mask & 0x01);
                FBuffer dst2(dst1);
                FBuffer src1(sz, mask & 0x02);
                FBuffer src2(sz, mask & 0x04);
                FBuffer src3(sz, mask & 0x08);

                native::mix_add3(dst1, src1, src2, src3, 2.0f, 4.0f, 8.0f, sz);
                sse::mix_add3(dst2, src1, src2, src3, 2.0f, 4.0f, 8.0f, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_mix4()
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0xfff)
        {
            for (size_t mask=0; mask <= 0x0f; ++mask)
            {
                FBuffer dst1(sz, mask & 0x01);
                FBuffer dst2(dst1);
                FBuffer src1(sz, mask & 0x02);
                FBuffer src2(sz, mask & 0x04);
                FBuffer src3(sz, mask & 0x08);

                native::mix4(dst1, src1, src2, src3, 2.0f, 4.0f, 8.0f, 16.0f, sz);
                sse::mix4(dst2, src1, src2, src3, 2.0f, 4.0f, 8.0f, 16.0f, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_mix_copy4()
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0xfff)
        {
            for (size_t mask=0; mask <= 0x1f; ++mask)
            {
                FBuffer dst1(sz, mask & 0x01);
                FBuffer dst2(sz, mask & 0x01);
                FBuffer src1(sz, mask & 0x02);
                FBuffer src2(sz, mask & 0x04);
                FBuffer src3(sz, mask & 0x08);
                FBuffer src4(sz, mask & 0x08);

                native::mix_copy4(dst1, src1, src2, src3, src4, 2.0f, 4.0f, 8.0f, 16.0f, sz);
                sse::mix_copy4(dst2, src1, src2, src3, src4, 2.0f, 4.0f, 8.0f, 16.0f, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_mix_add4()
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0xfff)
        {
            for (size_t mask=0; mask <= 0x1f; ++mask)
            {
                FBuffer dst1(sz, mask & 0x01);
                FBuffer dst2(dst1);
                FBuffer src1(sz, mask & 0x02);
                FBuffer src2(sz, mask & 0x04);
                FBuffer src3(sz, mask & 0x08);
                FBuffer src4(sz, mask & 0x08);

                native::mix_add4(dst1, src1, src2, src3, src4, 2.0f, 4.0f, 8.0f, 16.0f, sz);
                sse::mix_add4(dst2, src1, src2, src3, src4, 2.0f, 4.0f, 8.0f, 16.0f, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_fill()
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0x100, 0x1ff, 0xfff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                FBuffer dst1(sz, mask & 0x01);
                FBuffer dst2(sz, mask & 0x01);

                native::fill(dst1, 3.14f, sz);
                sse::fill(dst2, 3.14f, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_unary_abs(unary_math_t native, unary_math_t sse)
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0xfff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                FBuffer dst1(sz, mask & 0x01);
                FBuffer dst2(dst1);

                native(dst1, sz);
                sse(dst2, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_binary(binary_math_t native, binary_math_t sse)
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0xfff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                FBuffer src(sz, mask & 0x01);
                FBuffer dst1(sz, mask & 0x02);
                FBuffer dst2(dst1);

                native(dst1, src, sz);
                sse(dst2, src, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_binary_abs(binary_math_t native, binary_math_t sse)
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0xfff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                FBuffer src(sz, mask & 0x01);
                src.randomize_negative();
                FBuffer dst1(sz, mask & 0x02);
                FBuffer dst2(dst1);

                native(dst1, src, sz);
                sse(dst2, src, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_ternary(ternary_math_t native, ternary_math_t sse)
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0xfff)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                FBuffer src1(sz, mask & 0x01);
                FBuffer src2(sz, mask & 0x02);
                FBuffer dst1(sz, mask & 0x03);
                FBuffer dst2(sz, mask & 0x03);

                native(dst1, src1, src2, sz);
                sse(dst2, src1, src2, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_move()
    {
        TEST_FOREACH(sz, 0x00, 0x01, 0x03, 0x0f, 0x1f, 0x3f, 0x7f, 0xff, 0xfff, 0x1000)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                FBuffer dst1(sz, mask & 0x01);
                FBuffer dst2(sz, mask & 0x02);

                // Test both forward and backward algorithms
                sse::move(dst1, dst2, sz);
                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed move dst2 -> dst1 size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst1.validate()) ? "false" : "true");
                    return false;
                }

                dst2.randomize();
                sse::move(dst2, dst1, sz);

                if (!dst2.compare(dst1))
                {
                    lsp_error("  Failed move dst1 -> dst2 size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_horizontal(hfunc_t native, hfunc_t sse)
    {
        TEST_FOREACH(sz, 0x00, 0x01, 0x03, 0x08, 0x09, 0x0f, 0x1f, 0x3f, 0x7f, 0xff, 0xfff, 0x1000)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                FBuffer src(sz, mask & 0x01);
                float *p = src.data();
                for (size_t i=0; i<sz; ++i)
                    p[i]        = (i + 1) * -0.1f;

                float s1 = native(src, sz);
                float s2 = sse(src, sz);
//                lsp_trace("sz=%d, s1=%f, s2=%f", int(sz), s1, s2);

                if (fabs(1.0f - s1/s2) >= 1e-5)
                {
                    lsp_error("  Failed hsum: size=%d, mask=0x%x, s1=%f, s2=%f",
                        int(sz), int(mask), s1, s2);
                    return false;
                }
            }
        }

        return true;
    }

    bool test_minmax(hfunc_t native, hfunc_t sse)
    {
        TEST_FOREACH(sz, 0x00, 0x01, 0x03, 0x08, 0x09, 0x0f, 0x1f, 0x3f, 0x7f, 0xff, 0xfff, 0x1000)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                FBuffer src(sz, mask & 0x01);
                src.randomize_sign();

                float s1 = native(src, sz);
                float s2 = sse(src, sz);

                if (fabs(1.0f - s1/s2) >= 1e-5)
                {
                    lsp_error("  Failed hsum: size=%d, mask=0x%x, s1=%f, s2=%f",
                        int(sz), int(mask), s1, s2);
                    return false;
                }
            }
        }

        return true;
    }

    bool test_minmax(minmax_t native, minmax_t sse)
    {
        TEST_FOREACH(sz, 0x00, 0x01, 0x03, 0x08, 0x09, 0x0f, 0x1f, 0x3f, 0x7f, 0xff, 0xfff, 0x1000)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                FBuffer src(sz, mask & 0x01);
                src.randomize_sign();

                float min1, min2, max1, max2;
                native(src, sz, &min1, &max1);
                sse(src, sz, &min2, &max2);

                if ((max1 < min1) || (max2 < min2))
                {
                    lsp_error("  Failed minmax: size=%d, mask=0x%x, min1=%f, min2=%f, max1=%f, max2=%f",
                        int(sz), int(mask), min1, min2, max1, max2);
                    return false;
                }
                else if (fabs(1.0f - min1/min2) >= 1e-5)
                {
                    lsp_error("  Failed minmax: size=%d, mask=0x%x, min1=%f, min2=%f",
                        int(sz), int(mask), min1, min2);
                    return false;
                }
                else if (fabs(1.0f - max1/max2) >= 1e-5)
                {
                    lsp_error("  Failed minmax: size=%d, mask=0x%x, max1=%f, max2=%f",
                        int(sz), int(mask), max1, max2);
                    return false;
                }
            }
        }

        return true;
    }

    bool test_reverse1()
    {
        TEST_FOREACH(sz, 0x00, 0x01, 0x03, 0x08, 0x09, 0x0f, 0x1f, 0x3f, 0x7f, 0xff, 0xfff, 0x1000)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                FBuffer dst(sz, mask & 0x01);
                FBuffer dst1(dst);
                FBuffer dst2(dst);

                native::reverse1(dst1, sz);
                sse::reverse1(dst2, sz);

                if (!dst2.compare(dst1))
                {
                    lsp_error("  Failed test_reverse1: size=%d, mask=0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }

                native::reverse1(dst1, sz);
                sse::reverse1(dst2, sz);

                if (!dst2.compare(dst1))
                {
                    lsp_error("  Failed test_reverse1: size=%d, mask=0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
                else if (!dst2.compare(dst))
                {
                    lsp_error("  Failed test_reverse1: size=%d, mask=0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_reverse2()
    {
        TEST_FOREACH(sz, 0x00, 0x01, 0x03, 0x08, 0x09, 0x0f, 0x1f, 0x3f, 0x7f, 0xff, 0xfff, 0x1000)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                FBuffer src(sz, mask & 0x01);
                FBuffer dst1(sz, mask & 0x02);
                FBuffer dst2(sz, mask & 0x02);
                FBuffer dst3(sz, mask & 0x04);
                FBuffer dst4(sz, mask & 0x04);

                native::reverse2(dst1, src, sz);
                sse::reverse2(dst2, src, sz);

                if (!dst2.compare(dst1))
                {
                    lsp_error("  Failed test_reverse1: size=%d, mask=0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }

                native::reverse2(dst3, dst1, sz);
                sse::reverse2(dst4, dst2, sz);

                if (!dst4.compare(dst3))
                {
                    lsp_error("  Failed test_reverse1: size=%d, mask=0x%x, overflow=%s",
                        int(sz), int(mask), (dst4.validate()) ? "false" : "true");
                    return false;
                }
                else if (!dst4.compare(src))
                {
                    lsp_error("  Failed test_reverse1: size=%d, mask=0x%x, overflow=%s",
                        int(sz), int(mask), (dst4.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    int test(int argc, const char **argv)
    {
        dsp_context_t ctx;
        dsp::init();
        dsp::start(&ctx);

        int code = 0;
        #define LAUNCH(x, ...) --code; lsp_trace("Launching %s(%s)...", #x, #__VA_ARGS__); if (!x(__VA_ARGS__)) return code;

        LAUNCH(test_scale2)
        LAUNCH(test_scale3)

        LAUNCH(test_mix2)
        LAUNCH(test_mix3)
        LAUNCH(test_mix4)

        LAUNCH(test_mix_copy2)
        LAUNCH(test_mix_copy3)
        LAUNCH(test_mix_copy4)

        LAUNCH(test_mix_add2)
        LAUNCH(test_mix_add3)
        LAUNCH(test_mix_add4)

        LAUNCH(test_binary, native::add2, sse::add2)
        LAUNCH(test_binary, native::sub2, sse::sub2)
        LAUNCH(test_binary, native::mul2, sse::mul2)
        LAUNCH(test_binary, native::div2, sse::div2)

        LAUNCH(test_unary_abs, native::abs1, sse::abs1)
        LAUNCH(test_binary_abs, native::abs2, sse::abs2)
        LAUNCH(test_binary_abs, native::abs_add2, sse::abs_add2)
        LAUNCH(test_binary_abs, native::abs_sub2, sse::abs_sub2)
        LAUNCH(test_binary_abs, native::abs_mul2, sse::abs_mul2)
        LAUNCH(test_binary_abs, native::abs_div2, sse::abs_div2)

        LAUNCH(test_ternary, native::add3, sse::add3)
        LAUNCH(test_ternary, native::sub3, sse::sub3)
        LAUNCH(test_ternary, native::mul3, sse::mul3)
        LAUNCH(test_ternary, native::div3, sse::div3)

        LAUNCH(test_scale_op, native::scale_add3, sse::scale_add3)
        LAUNCH(test_scale_op, native::scale_sub3, sse::scale_sub3)
        LAUNCH(test_scale_op, native::scale_mul3, sse::scale_mul3)
        LAUNCH(test_scale_op, native::scale_div3, sse::scale_div3)

        LAUNCH(test_move)
        LAUNCH(test_fill)
        LAUNCH(test_reverse1)
        LAUNCH(test_reverse2)

        LAUNCH(test_horizontal, native::h_sum, sse::h_sum)
        LAUNCH(test_horizontal, native::h_sqr_sum, sse::h_sqr_sum)
        LAUNCH(test_horizontal, native::h_abs_sum, sse::h_abs_sum)

        LAUNCH(test_minmax, native::min, sse::min);
        LAUNCH(test_minmax, native::max, sse::max);
        LAUNCH(test_minmax, native::abs_min, sse::abs_min);
        LAUNCH(test_minmax, native::abs_max, sse::abs_max);
        LAUNCH(test_minmax, native::minmax, sse::minmax);
        LAUNCH(test_minmax, native::abs_minmax, sse::abs_minmax);

        lsp_info("All tests have been successfully passed");

        dsp::finish(&ctx);

        return 0;
    }
}

