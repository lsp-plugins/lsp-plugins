/*
 * features.h
 *
 *  Created on: 3 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_FEATURES_H_
#define DSP_ARCH_ARM_FEATURES_H_

#include <common/types.h>

#ifdef PLATFORM_POSIX
    #include <sys/auxv.h>
#endif /* PLATFORM_UNIX */

#ifdef ARCH_ARM
    /*
        Redefine constants to be compatible with GCC 6.x (Raspbian) and GCC 8.x (FreeBSD)
    */
    #if defined(HWCAP_SWP) && !defined(HWCAP_ARM_SWP)
        #define HWCAP_ARM_SWP HWCAP_SWP
    #endif
    #if defined(HWCAP_HALF) && !defined(HWCAP_ARM_HALF)
        #define HWCAP_ARM_HALF HWCAP_HALF
    #endif
    #if defined(HWCAP_THUMB) && !defined(HWCAP_ARM_THUMB)
        #define HWCAP_ARM_THUMB HWCAP_THUMB
    #endif
    #if defined(HWCAP_26BIT) && !defined(HWCAP_ARM_26BIT)
        #define HWCAP_ARM_26BIT HWCAP_26BIT
    #endif
    #if defined(HWCAP_FAST_MULT) && !defined(HWCAP_ARM_FAST_MULT)
        #define HWCAP_ARM_FAST_MULT HWCAP_FAST_MULT
    #endif
    #if defined(HWCAP_FPA) && !defined(HWCAP_ARM_FPA)
        #define HWCAP_ARM_FPA HWCAP_FPA
    #endif
    #if defined(HWCAP_VFP) && !defined(HWCAP_ARM_VFP)
        #define HWCAP_ARM_VFP HWCAP_VFP
    #endif
    #if defined(HWCAP_EDSP) && !defined(HWCAP_ARM_EDSP)
        #define HWCAP_ARM_EDSP HWCAP_EDSP
    #endif
    #if defined(HWCAP_JAVA) && !defined(HWCAP_ARM_JAVA)
        #define HWCAP_ARM_JAVA HWCAP_JAVA
    #endif
    #if defined(HWCAP_IWMMXT) && !defined(HWCAP_ARM_IWMMXT)
        #define HWCAP_ARM_IWMMXT HWCAP_IWMMXT
    #endif
    #if defined(HWCAP_CRUNCH) && !defined(HWCAP_ARM_CRUNCH)
        #define HWCAP_ARM_CRUNCH HWCAP_CRUNCH
    #endif
    #if defined(HWCAP_THUMBEE) && !defined(HWCAP_ARM_THUMBEE)
        #define HWCAP_ARM_THUMBEE HWCAP_THUMBEE
    #endif
    #if defined(HWCAP_NEON) && !defined(HWCAP_ARM_NEON)
        #define HWCAP_ARM_NEON HWCAP_NEON
    #endif
    #if defined(HWCAP_VFPv3) && !defined(HWCAP_ARM_VFPv3)
        #define HWCAP_ARM_VFPv3 HWCAP_VFPv3
    #endif
    #if defined(HWCAP_VFPv3D16) && !defined(HWCAP_ARM_VFPv3D16)
        #define HWCAP_ARM_VFPv3D16 HWCAP_VFPv3D16
    #endif
    #if defined(HWCAP_TLS) && !defined(HWCAP_ARM_TLS)
        #define HWCAP_ARM_TLS HWCAP_TLS
    #endif
    #if defined(HWCAP_VFPv4) && !defined(HWCAP_ARM_VFPv4)
        #define HWCAP_ARM_VFPv4 HWCAP_VFPv4
    #endif
    #if defined(HWCAP_IDIVA) && !defined(HWCAP_ARM_IDIVA)
        #define HWCAP_ARM_IDIVA HWCAP_IDIVA
    #endif
    #if defined(HWCAP_IDIVT) && !defined(HWCAP_ARM_IDIVT)
        #define HWCAP_ARM_IDIVT HWCAP_IDIVT
    #endif
    #if defined(HWCAP_VFPD32) && !defined(HWCAP_ARM_VFPD32)
        #define HWCAP_ARM_VFPD32 HWCAP_VFPD32
    #endif
    #if defined(HWCAP_LPAE) && !defined(HWCAP_ARM_LPAE)
        #define HWCAP_ARM_LPAE HWCAP_LPAE
    #endif
    #if defined(HWCAP_EVTSTRM) && !defined(HWCAP_ARM_EVTSTRM)
        #define HWCAP_ARM_EVTSTRM HWCAP_EVTSTRM
    #endif
#else
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
