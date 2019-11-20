/*
 * asimd.cpp
 *
 *  Created on: 11 апр. 2019 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <core/types.h>

#ifdef ARCH_AARCH64

#include <test/test.h>
#include <dsp/arch/aarch64/features.h>

#define DSP_ARCH_AARCH64_ASIMD_IMPL

namespace asimd // TODO: make constants common for all architectures
{
    //-------------------------------------------------------------------------
    // Constants definition
    #define DSP_F32VEC4(name, v)        static const float name[] __lsp_aligned16          = { v, v, v, v }
    #define DSP_U32VEC4(name, v)        static const uint32_t name[] __lsp_aligned16       = { uint32_t(v), uint32_t(v), uint32_t(v), uint32_t(v) }

    #define DSP_F32VECX4(name, a, b, c, d)  static const float name[] __lsp_aligned16      = { a, b, c, d }
    #define DSP_U32VECX4(name, a, b, c, d)  static const uint32_t name[] __lsp_aligned16   = { uint32_t(a), uint32_t(b), uint32_t(c), uint32_t(d) }

    #define DSP_F32REP4(v)              v, v, v, v
    #define DSP_U32REP4(v)              uint32_t(v), uint32_t(v), uint32_t(v), uint32_t(v)

    #define DSP_F32ARRAY(name, ...)     static const float name[] __lsp_aligned16          = { __VA_ARGS__ }
    #define DSP_U32ARRAY(name, ...)     static const uint32_t name[] __lsp_aligned16       = { __VA_ARGS__ }

    #include <dsp/common/const/const16.h>

    #undef DSP_F32ARRAY_IMPL
    #undef DSP_F32ARRAY

    #undef DSP_U32REP4
    #undef DSP_F32REP4

    #undef DSP_U32VECX4
    #undef DSP_F32VECX4

    #undef DSP_U32VEC4
    #undef DSP_F32VEC4
}

#include <dsp/arch/aarch64/asimd/copy.h>
#include <dsp/arch/aarch64/asimd/pmath/op_kx.h>
#include <dsp/arch/aarch64/asimd/pmath/op_vv.h>
#include <dsp/arch/aarch64/asimd/pmath/fmop_kx.h>
#include <dsp/arch/aarch64/asimd/complex.h>
#include <dsp/arch/aarch64/asimd/pcomplex.h>

#define EXPORT2(function, export)           dsp::function = asimd::export; TEST_EXPORT(asimd::export);
#define EXPORT1(function)                   EXPORT2(function, function)

#undef DSP_ARCH_AARCH64_ASIMD_IMPL

namespace asimd
{
    void dsp_init(const aarch64::cpu_features_t *f)
    {
        if ((f->hwcap & (HWCAP_AARCH64_ASIMD)) != (HWCAP_AARCH64_ASIMD))
            return;

        lsp_trace("Optimizing DSP for ASIMD instruction set");
        EXPORT1(copy);
        EXPORT1(move);

        EXPORT1(add_k2);
        EXPORT1(sub_k2);
        EXPORT1(rsub_k2);
        EXPORT1(mul_k2);
        EXPORT1(div_k2);
        EXPORT1(rdiv_k2);

        EXPORT1(add_k3);
        EXPORT1(sub_k3);
        EXPORT1(rsub_k3);
        EXPORT1(mul_k3);
        EXPORT1(div_k3);
        EXPORT1(rdiv_k3);

        EXPORT1(add2);
        EXPORT1(sub2);
        EXPORT1(rsub2);
        EXPORT1(mul2);
        EXPORT1(div2);
        EXPORT1(rdiv2);

        EXPORT1(fmadd_k3);
        EXPORT1(fmsub_k3);
        EXPORT1(fmrsub_k3);
        EXPORT1(fmmul_k3);
        EXPORT1(fmdiv_k3);
        EXPORT1(fmrdiv_k3);

        EXPORT1(add3);
        EXPORT1(sub3);
        EXPORT1(mul3);
        EXPORT1(div3);

        EXPORT1(complex_mul2);
        EXPORT1(complex_mul3);
        EXPORT1(complex_div2);
        EXPORT1(complex_rdiv2);
        EXPORT1(complex_div3);
        EXPORT1(complex_mod);
        EXPORT1(complex_rcp1);
        EXPORT1(complex_rcp2);

        EXPORT1(pcomplex_mul2);
        EXPORT1(pcomplex_mul3);
        EXPORT1(pcomplex_div2);
        EXPORT1(pcomplex_rdiv2);
        EXPORT1(pcomplex_div3);
        EXPORT1(pcomplex_mod);
        EXPORT1(pcomplex_rcp1);
        EXPORT1(pcomplex_rcp2);
        EXPORT1(pcomplex_r2c);
        EXPORT1(pcomplex_c2r);
        EXPORT1(pcomplex_add_r);
    }
}

#endif /* ARCH_AARCH64 */

