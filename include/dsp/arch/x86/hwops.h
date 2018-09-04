/*
 * hwops.h
 *
 *  Created on: 4 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_HWOPS_H_
#define DSP_ARCH_X86_HWOPS_H_

#include <dsp/arch/x86/features.h>

namespace x86
{
    enum hwops_t
    {
        HWOPS_FAST_MOVS
    };

    bool hwops_check(const cpu_features_t *f, hwops_t ops);
}

#endif /* DSP_ARCH_X86_HWOPS_H_ */
