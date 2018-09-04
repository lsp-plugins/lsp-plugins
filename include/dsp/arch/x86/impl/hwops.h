/*
 * hwops.h
 *
 *  Created on: 4 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_IMPL_HWOPS_H_
#define DSP_ARCH_X86_IMPL_HWOPS_H_

#include <dsp/arch/x86/features.h>
#include <dsp/arch/x86/hwops.h>

namespace x86
{
    bool hwops_check(const cpu_features_t *f, hwops_t ops)
    {
        if (ops == HWOPS_FAST_MOVS)
        {
            return (f->vendor == CPU_VENDOR_INTEL) && (f->family == 0x6) && (f->model >= 0x5e);
        }

        return false;
    }
}

#endif /* INCLUDE_DSP_ARCH_X86_IMPL_HWOPS_H_ */
