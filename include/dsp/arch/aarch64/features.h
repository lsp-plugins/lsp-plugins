/*
 * features.h
 *
 *  Created on: 18 мар. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_FEATURES_H_
#define DSP_ARCH_AARCH64_FEATURES_H_

#include <dsp/types.h>

#ifdef PLATFORM_POSIX
    #include <sys/auxv.h>
#endif /* PLATFORM_UNIX */

#ifdef ARCH_AARCH64
    /* This is feature definition taken from ARM headers for non-ARM architecture
     * to simply compile from IDE
     */
    #define HWCAP_AARCH64_FP                (1 << 0)
    #define HWCAP_AARCH64_ASIMD             (1 << 1)
    #define HWCAP_AARCH64_EVTSTRM           (1 << 2)
    #define HWCAP_AARCH64_AES               (1 << 3)
    #define HWCAP_AARCH64_PMULL             (1 << 4)
    #define HWCAP_AARCH64_SHA1              (1 << 5)
    #define HWCAP_AARCH64_SHA2              (1 << 6)
    #define HWCAP_AARCH64_CRC32             (1 << 7)
    #define HWCAP_AARCH64_ATOMICS           (1 << 8)
    #define HWCAP_AARCH64_FPHP              (1 << 9)
    #define HWCAP_AARCH64_ASIMDHP           (1 << 10)
    #define HWCAP_AARCH64_CPUID             (1 << 11)
    #define HWCAP_AARCH64_ASIMDRDM          (1 << 12)
    #define HWCAP_AARCH64_JSCVT             (1 << 13)
    #define HWCAP_AARCH64_FCMA              (1 << 14)
    #define HWCAP_AARCH64_LRCPC             (1 << 15)
    #define HWCAP_AARCH64_DCPOP             (1 << 16)
    #define HWCAP_AARCH64_SHA3              (1 << 17)
    #define HWCAP_AARCH64_SM3               (1 << 18)
    #define HWCAP_AARCH64_SM4               (1 << 19)
    #define HWCAP_AARCH64_ASIMDDP           (1 << 20)
    #define HWCAP_AARCH64_SHA512            (1 << 21)
    #define HWCAP_AARCH64_SVE               (1 << 22)
    #define HWCAP_AARCH64_ASIMDFHM          (1 << 23)
    #define HWCAP_AARCH64_DIT               (1 << 24)
    #define HWCAP_AARCH64_USCAT             (1 << 25)
    #define HWCAP_AARCH64_ILRCPC            (1 << 26)
    #define HWCAP_AARCH64_FLAGM             (1 << 27)
#endif /* ARCH_AARCH64 */

namespace aarch64
{
    typedef struct cpu_features_t
    {
        size_t      implementer;
        size_t      architecture;
        size_t      variant;
        size_t      part;
        size_t      revision;
        uint64_t    hwcap;
    } cpu_features_t;
}


#endif /* DSP_ARCH_AARCH64_FEATURES_H_ */
