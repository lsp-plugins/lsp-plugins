/*
 * const.h
 *
 *  Created on: 13 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_FASTCONV_CONST_H_
#define DSP_ARCH_AARCH64_ASIMD_FASTCONV_CONST_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    static const uint32_t fastconv_swp_mask[8] =
    {
        0, 0xffffffff, 0, 0xffffffff,
        0, 0xffffffff, 0, 0xffffffff
    };
}


#endif /* DSP_ARCH_AARCH64_ASIMD_FASTCONV_CONST_H_ */
