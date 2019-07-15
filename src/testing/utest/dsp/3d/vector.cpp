/*
 * vector.cpp
 *
 *  Created on: 30 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <testing/utest/dsp/3d/helpers.h>

namespace native
{
    void init_vector_dxyz(vector3d_t *v, float dx, float dy, float dz);
    void init_vector(vector3d_t *p, const vector3d_t *s);
    void normalize_vector(vector3d_t *v);
}

IF_ARCH_X86(
    namespace sse
    {
        void init_vector_dxyz(vector3d_t *v, float dx, float dy, float dz);
        void init_vector(vector3d_t *p, const vector3d_t *s);
        void normalize_vector(vector3d_t *v);
    }
)

typedef void (* init_vector_dxyz_t)(vector3d_t *v, float dx, float dy, float dz);
typedef void (* init_vector_t)(vector3d_t *p, const vector3d_t *s);
typedef void (* normalize_vector_t)(vector3d_t *v);

UTEST_BEGIN("dsp.3d", vector)

    void call(
            const char *label,
            init_vector_dxyz_t init_dxyz,
            init_vector_t init,
            normalize_vector_t norm
        )
    {
        if ((!UTEST_SUPPORTED(init_dxyz)) || (!UTEST_SUPPORTED(init)) || (!UTEST_SUPPORTED(norm)))
            return;

        printf("Testing %s\n", label);

        vector3d_t  v1, v2, v3;

        native::init_vector_dxyz(&v1, 1.0f, 2.0f, 3.0f);
        init_dxyz(&v2, 1.0f, 2.0f, 3.0f);
        UTEST_ASSERT_MSG(vector3d_sck(&v1, &v2), "Failed init_vector_dxyz");

        native::init_vector(&v2, &v1);
        init(&v3, &v1);
        UTEST_ASSERT_MSG(vector3d_sck(&v1, &v2), "Failed native init_vector");
        UTEST_ASSERT_MSG(vector3d_sck(&v1, &v3), "Failed optimized init_vector");

        native::normalize_vector(&v2);
        norm(&v3);

        UTEST_ASSERT_MSG(vector3d_sck(&v2, &v3), "Failed normalize vector");
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse_vector", sse::init_vector_dxyz, sse::init_vector, sse::normalize_vector));
    }
UTEST_END;



