/*
 * neon-d32.cpp
 *
 *  Created on: 2 сент. 2018 г.
 *      Author: sadko
 */


#include <dsp/dsp.h>
#include <test/test.h>
#include <dsp/arch/arm/features.h>

#define DSP_ARCH_ARM_NEON_32_IMPL

#include <dsp/arch/arm/neon-d32/copy.h>
#include <dsp/arch/arm/neon-d32/complex.h>
#include <dsp/arch/arm/neon-d32/pcomplex.h>
#include <dsp/arch/arm/neon-d32/convolve.h>

#undef DSP_ARCH_ARM_NEON_32_IMPL

#define EXPORT2(function, export)           dsp::function = neon_d32::export; TEST_EXPORT(neon_d32::export);
#define EXPORT1(function)                   EXPORT2(function, function)

namespace neon_d32
{
    void dsp_init(const arm::cpu_features_t *f)
    {
        if ((f->hwcap & (HWCAP_ARM_NEON | HWCAP_ARM_VFPD32)) != (HWCAP_ARM_NEON | HWCAP_ARM_VFPD32))
            return;

        lsp_trace("Optimizing DSP for NEON-D32 instruction set");
        EXPORT1(copy);
        EXPORT1(move);

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

        // Misc for testing
        TEST_EXPORT(neon_d32::complex_mul3_x12);
    }
}

//#endif /* ARCH_ARM */
