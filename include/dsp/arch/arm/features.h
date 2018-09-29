/*
 * features.h
 *
 *  Created on: 3 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_FEATURES_H_
#define DSP_ARCH_ARM_FEATURES_H_

#include <dsp/types.h>
#include <sys/auxv.h>

#ifndef ARCH_ARM
    /* This is feature definition taken from ARM headers for non-ARM architecture
     * to simply compile from IDE
     */
    #define HWCAP_ARM_SWP           1
    #define HWCAP_ARM_HALF          2
    #define HWCAP_ARM_THUMB         4
    #define HWCAP_ARM_26BIT         8
    #define HWCAP_ARM_FAST_MULT     16
    #define HWCAP_ARM_FPA           32
    #define HWCAP_ARM_VFP           64
    #define HWCAP_ARM_EDSP          128
    #define HWCAP_ARM_JAVA          256
    #define HWCAP_ARM_IWMMXT        512
    #define HWCAP_ARM_CRUNCH        1024
    #define HWCAP_ARM_THUMBEE       2048
    #define HWCAP_ARM_NEON          4096
    #define HWCAP_ARM_VFPv3         8192
    #define HWCAP_ARM_VFPv3D16      16384
    #define HWCAP_ARM_TLS           32768
    #define HWCAP_ARM_VFPv4         65536
    #define HWCAP_ARM_IDIVA         131072
    #define HWCAP_ARM_IDIVT         262144
    #define HWCAP_ARM_VFPD32        524288
    #define HWCAP_ARM_LPAE          1048576
    #define HWCAP_ARM_EVTSTRM       2097152
#endif /* ARCH_ARM */

namespace arm
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

#endif /* DSP_ARCH_ARM_FEATURES_H_ */
