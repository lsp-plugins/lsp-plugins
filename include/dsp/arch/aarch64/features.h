/*
 * features.h
 *
 *  Created on: 18 мар. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_FEATURES_H_
#define DSP_ARCH_AARCH64_FEATURES_H_

#include <common/types.h>

#ifdef PLATFORM_POSIX
    #include <sys/auxv.h>
#endif /* PLATFORM_UNIX */

// Override some constants if they were not defined previously
#ifndef HWCAP_FP
    #define HWCAP_FP                0
#endif /* HWCAP_FP */

#ifndef HWCAP_ASIMD
    #define HWCAP_ASIMD             0
#endif /* HWCAP_ASIMD */

#ifndef HWCAP_EVTSTRM
    #define HWCAP_EVTSTRM           0
#endif /* HWCAP_EVTSTRM */

#ifndef HWCAP_AES
    #define HWCAP_AES               0
#endif /* HWCAP_AES */

#ifndef HWCAP_PMULL
    #define HWCAP_PMULL             0
#endif /* HWCAP_PMULL */

#ifndef HWCAP_SHA1
    #define HWCAP_SHA1              0
#endif /* HWCAP_SHA1 */

#ifndef HWCAP_SHA2
    #define HWCAP_SHA2              0
#endif /* HWCAP_SHA2 */

#ifndef HWCAP_CRC32
    #define HWCAP_CRC32             0
#endif /* HWCAP_CRC32 */

#ifndef HWCAP_ATOMICS
    #define HWCAP_ATOMICS           0
#endif /* HWCAP_ATOMICS */

#ifndef HWCAP_FPHP
    #define HWCAP_FPHP              0
#endif /* HWCAP_FPHP */

#ifndef HWCAP_ASIMDHP
    #define HWCAP_ASIMDHP           0
#endif /* HWCAP_ASIMDHP */

#ifndef HWCAP_CPUID
    #define HWCAP_CPUID             0
#endif /* HWCAP_CPUID */

#ifndef HWCAP_ASIMDRDM
    #define HWCAP_ASIMDRDM          0
#endif /* HWCAP_ASIMDRDM */

#ifndef HWCAP_JSCVT
    #define HWCAP_JSCVT             0
#endif /* HWCAP_JSCVT */

#ifndef HWCAP_FCMA
    #define HWCAP_FCMA              0
#endif /* HWCAP_FCMA */

#ifndef HWCAP_LRCPC
    #define HWCAP_LRCPC             0
#endif /* HWCAP_LRCPC */

#ifndef HWCAP_DCPOP
    #define HWCAP_DCPOP             0
#endif /* HWCAP_DCPOP */

#ifndef HWCAP_SHA3
    #define HWCAP_SHA3              0
#endif /* HWCAP_SHA3 */

#ifndef HWCAP_SM3
    #define HWCAP_SM3               0
#endif /* HWCAP_SM3 */

#ifndef HWCAP_SM4
    #define HWCAP_SM4               0
#endif /* HWCAP_SM4 */

#ifndef HWCAP_ASIMDDP
    #define HWCAP_ASIMDDP           0
#endif /* HWCAP_ASIMDDP */

#ifndef HWCAP_SHA512
    #define HWCAP_SHA512            0
#endif /* HWCAP_SHA512 */

#ifndef HWCAP_SVE
    #define HWCAP_SVE               0
#endif /* HWCAP_SVE */

#ifndef HWCAP_ASIMDFHM
    #define HWCAP_ASIMDFHM          0
#endif /* HWCAP_ASIMDFHM */

#ifndef HWCAP_DIT
    #define HWCAP_DIT               0
#endif /* HWCAP_DIT */

#ifndef HWCAP_USCAT
    #define HWCAP_USCAT             0
#endif /* HWCAP_USCAT */

#ifndef HWCAP_ILRCPC
    #define HWCAP_ILRCPC            0
#endif /* HWCAP_ILRCPC */

#ifndef HWCAP_FLAGM
    #define HWCAP_FLAGM             0
#endif /* HWCAP_FLAGM */

// Define LSP-defined AARCH-64 specific macros
#define HWCAP_AARCH64_FP                HWCAP_FP
#define HWCAP_AARCH64_ASIMD             HWCAP_ASIMD
#define HWCAP_AARCH64_EVTSTRM           HWCAP_EVTSTRM
#define HWCAP_AARCH64_AES               HWCAP_AES
#define HWCAP_AARCH64_PMULL             HWCAP_PMULL
#define HWCAP_AARCH64_SHA1              HWCAP_SHA1
#define HWCAP_AARCH64_SHA2              HWCAP_SHA2
#define HWCAP_AARCH64_CRC32             HWCAP_CRC32
#define HWCAP_AARCH64_ATOMICS           HWCAP_ATOMICS
#define HWCAP_AARCH64_FPHP              HWCAP_FPHP
#define HWCAP_AARCH64_ASIMDHP           HWCAP_ASIMDHP
#define HWCAP_AARCH64_CPUID             HWCAP_CPUID
#define HWCAP_AARCH64_ASIMDRDM          HWCAP_ASIMDRDM
#define HWCAP_AARCH64_JSCVT             HWCAP_JSCVT
#define HWCAP_AARCH64_FCMA              HWCAP_FCMA
#define HWCAP_AARCH64_LRCPC             HWCAP_LRCPC
#define HWCAP_AARCH64_DCPOP             HWCAP_DCPOP
#define HWCAP_AARCH64_SHA3              HWCAP_SHA3
#define HWCAP_AARCH64_SM3               HWCAP_SM3
#define HWCAP_AARCH64_SM4               HWCAP_SM4
#define HWCAP_AARCH64_ASIMDDP           HWCAP_ASIMDDP
#define HWCAP_AARCH64_SHA512            HWCAP_SHA512
#define HWCAP_AARCH64_SVE               HWCAP_SVE
#define HWCAP_AARCH64_ASIMDFHM          HWCAP_ASIMDFHM
#define HWCAP_AARCH64_DIT               HWCAP_DIT
#define HWCAP_AARCH64_USCAT             HWCAP_USCAT
#define HWCAP_AARCH64_ILRCPC            HWCAP_ILRCPC
#define HWCAP_AARCH64_FLAGM             HWCAP_FLAGM

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
