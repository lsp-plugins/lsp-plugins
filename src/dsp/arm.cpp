/*
 * arm.cpp
 *
 *  Created on: 20 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/types.h>

#include <sys/auxv.h>
#include <stdio.h>
#include <malloc.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

namespace arm
{
    //    processor       : 0
    //    model name      : ARMv7 Processor rev 4 (v7l)
    //    BogoMIPS        : 38.40
    //    Features        : half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm crc32
    //    CPU implementer : 0x41
    //    CPU architecture: 7
    //    CPU variant     : 0x0
    //    CPU part        : 0xd03
    //    CPU revision    : 4

    typedef struct cpu_features_t
    {
        size_t      implementer;
        size_t      architecture;
        size_t      variant;
        size_t      part;
        size_t      revision;
        uint64_t    hwcap;
    } cpu_features_t;

    void detect_cpu_features(cpu_features_t *f)  // must be at least 13 bytes
    {
        f->implementer      = 0;
        f->architecture     = 0;
        IF_ARCH_ARM6(f->architecture = 6);
        IF_ARCH_ARM7(f->architecture = 7);
        IF_ARCH_ARM8(f->architecture = 8);
        f->variant          = 0;
        f->part             = 0;
        f->revision         = 0;
        f->hwcap            = getauxval(AT_HWCAP);

        // Read /proc/cpuinfo
        FILE *cpuinfo = fopen("/proc/cpuinfo", "r");
        if (cpuinfo == NULL)
            return;  // handle error

        size_t size = 0;
        char *line  = NULL;

        while (getline(&line, &size, cpuinfo) >= 0)
        {
            size_t *field = NULL;

            // Find field
            if (strncasecmp(line, "CPU implementer", 15) == 0)
                field = &f->implementer;
            else if (strncasecmp(line, "CPU architecture", 16) == 0)
                field = &f->architecture;
            else if (strncasecmp(line, "CPU variant", 11) == 0)
                field = &f->variant;
            else if (strncasecmp(line, "CPU part", 8) == 0)
                field = &f->part;
            else if (strncasecmp(line, "CPU revision", 12) == 0)
                field = &f->revision;

            if (field == NULL) // Field not found ?
                continue;
            char *colon = strchr(line, ':'); // Colon not found ?
            if (colon++ == NULL)
                continue;

            while ((*colon) == ' ')
                colon++;
            if ((*colon) == '\0') // No data ?
                continue;

            // Detect number base
            size_t base = 10;
            if (strncasecmp(colon, "0x", 2) == 0)
            {
                colon  += 2;
                base    = 16;
            }

            // Parse value
            errno       = 0;
            long value  = strtol(colon, &colon, base);
            if (errno != 0) // Failed parse ?
                continue;
            if (((*colon) != '\0') && (*colon) != '\n') // Additional data?
                continue;

            // Store parsed value
            *field      = value;
        }

        // if we got here, handle error
        if (line != NULL)
            free(line);
        fclose(cpuinfo);
    }

    void dsp_init()
    {
// ARM-specific constants
//        #define HWCAP_ARM_SWP           1
//        #define HWCAP_ARM_HALF          2
//        #define HWCAP_ARM_THUMB         4
//        #define HWCAP_ARM_26BIT         8
//        #define HWCAP_ARM_FAST_MULT     16
//        #define HWCAP_ARM_FPA           32
//        #define HWCAP_ARM_VFP           64
//        #define HWCAP_ARM_EDSP          128
//        #define HWCAP_ARM_JAVA          256
//        #define HWCAP_ARM_IWMMXT        512
//        #define HWCAP_ARM_CRUNCH        1024
//        #define HWCAP_ARM_THUMBEE       2048
//        #define HWCAP_ARM_NEON          4096
//        #define HWCAP_ARM_VFPv3         8192
//        #define HWCAP_ARM_VFPv3D16      16384
//        #define HWCAP_ARM_TLS           32768
//        #define HWCAP_ARM_VFPv4         65536
//        #define HWCAP_ARM_IDIVA         131072
//        #define HWCAP_ARM_IDIVT         262144
//        #define HWCAP_ARM_VFPD32        524288
//        #define HWCAP_ARM_LPAE          1048576
//        #define HWCAP_ARM_EVTSTRM       2097152

    }
}
