/*
 * aarch64.cpp
 *
 *  Created on: 18 мар. 2019 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>

#ifdef ARCH_AARCH64

#include <test/test.h>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <dsp/arch/aarch64/features.h>
#include <dsp/arch/aarch64/fpcr.h>

namespace asimd
{
    extern void dsp_init(const aarch64::cpu_features_t *f);
}

namespace aarch64
{
    typedef struct cpu_part_t
    {
        uint32_t    id;
        const char *name;
    } cpu_part_t;

    typedef struct feature_t
    {
        uint32_t    mask;
        const char *name;
    } feature_t;

    static const cpu_part_t cpu_parts[] =
    {
        { 0xb02, "ARM11 MPCore" },
        { 0xb36, "ARM1136" },
        { 0xb56, "ARM1156" },
        { 0xb76, "ARM1176" },

        { 0xc05, "Cortex-A5" },
        { 0xc07, "Cortex-A7" },
        { 0xc08, "Cortex-A8" },
        { 0xc09, "Cortex-A9" },
        { 0xc0d, "Cortex-A12" },
        { 0xc0e, "Cortex-A17" },
        { 0xc0f, "Cortex-A15" },
        { 0xc14, "Cortex-R4" },
        { 0xc15, "Cortex-R5" },
        { 0xc17, "Cortex-R7" },
        { 0xc18, "Cortex-R8" },

        { 0xc20, "Cortex-M0" },
        { 0xc21, "Cortex-M1" },
        { 0xc23, "Cortex-M3" },
        { 0xc24, "Cortex-M4" },
        { 0xc27, "Cortex-M7" },
        { 0xc60, "Cortex-M0+" },

        { 0xd01, "Cortex-A32" },
        { 0xd03, "Cortex-A53" },
        { 0xd04, "Cortex-A35" },
        { 0xd05, "Cortex-A55" },
        { 0xd07, "Cortex-A57" },
        { 0xd08, "Cortex-A72" },
        { 0xd09, "Cortex-A73" },
        { 0xd0a, "Cortex-A75" },
        { 0xd13, "Cortex-R52" },

        { 0xd20, "Cortex-M23" },
        { 0xd21, "Cortex-M33" }
    };

    static const feature_t cpu_features[] =
    {
IF_ARCH_AARCH64(
        { HWCAP_AARCH64_FP, "FP" },
        { HWCAP_AARCH64_ASIMD, "ASIMD" },
        { HWCAP_AARCH64_EVTSTRM, "EVTSTRM" },
        { HWCAP_AARCH64_AES, "AES" },
        { HWCAP_AARCH64_PMULL, "PMULL" },
        { HWCAP_AARCH64_SHA1, "SHA1" },
        { HWCAP_AARCH64_SHA2, "SHA2" },
        { HWCAP_AARCH64_CRC32, "CRC32" },
        { HWCAP_AARCH64_ATOMICS, "ATOMICS" },
        { HWCAP_AARCH64_FPHP, "FPHP" },
        { HWCAP_AARCH64_ASIMDHP, "ASIMDHP" },
        { HWCAP_AARCH64_CPUID, "CPUID" },
        { HWCAP_AARCH64_ASIMDRDM, "ASIMDRDM" },
        { HWCAP_AARCH64_JSCVT, "JSCVT" },
        { HWCAP_AARCH64_FCMA, "FCMA" },
        { HWCAP_AARCH64_LRCPC, "LSCPC" },
        { HWCAP_AARCH64_DCPOP, "DCPOP" },
        { HWCAP_AARCH64_SHA3, "SHA3" },
        { HWCAP_AARCH64_SM3, "SM3" },
        { HWCAP_AARCH64_SM4, "SM4" },
        { HWCAP_AARCH64_ASIMDDP, "ASMIDDP" },
        { HWCAP_AARCH64_SHA512, "SHA512" },
        { HWCAP_AARCH64_SVE, "SVE" },
        { HWCAP_AARCH64_ASIMDFHM, "ASIMDFHM" },
        { HWCAP_AARCH64_DIT, "DIT" },
        { HWCAP_AARCH64_USCAT, "USCAT" },
        { HWCAP_AARCH64_ILRCPC, "ILRCPC" },
        { HWCAP_AARCH64_FLAGM, "FLAGM" }
)
    };

    const char *find_cpu_name(uint32_t id)
    {
        ssize_t first = 0, last = (sizeof(cpu_parts) / sizeof(cpu_part_t)) - 1;
        while (first <= last)
        {
            ssize_t mid     = (first + last) >> 1;
            uint32_t xmid   = cpu_parts[mid].id;
            if (id < xmid)
                last = mid - 1;
            else if (id > xmid)
                first = mid + 1;
            else
                return cpu_parts[mid].name;
        }
        return "Generic ARM processor";
    }

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

//        processor       : 0
//        BogoMIPS        : 38.40
//        Features        : fp asimd evtstrm crc32 cpuid
//        CPU implementer : 0x41
//        CPU architecture: 8
//        CPU variant     : 0x0
//        CPU part        : 0xd03
//        CPU revision    : 4

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

    static size_t estimate_features_size(const cpu_features_t *f)
    {
        // Estimate the string length
        size_t estimate = 1; // End of string character
        for (size_t i = 0, n=sizeof(cpu_features)/sizeof(feature_t); i < n; i++)
        {
            if (!(f->hwcap & cpu_features[i].mask))
                continue;

            if (estimate > 0)
                estimate++;
            estimate += strlen(cpu_features[i].name);
        }
        return estimate;
    }

    static char *build_features_list(char *dst, const cpu_features_t *f)
    {
        // Build string
        char *s = dst;

        for (size_t i = 0, n=sizeof(cpu_features)/sizeof(feature_t); i < n; i++)
        {
            if (!(f->hwcap & cpu_features[i].mask))
                continue;
            if (s != dst)
                s = stpcpy(s, " ");
            s = stpcpy(s, cpu_features[i].name);
        }
        *s = '\0';

        return s;
    }

    dsp::info_t *info()
    {
        cpu_features_t f;
        detect_cpu_features(&f);

        const char *cpu = find_cpu_name(f.part);
        char *model     = NULL;
        int n = asprintf(&model, "vendor=0x%x, architecture=%d, variant=%d, part=0x%x, revision=%d",
                int(f.implementer), int(f.architecture), int(f.variant), int(f.part), int(f.revision));
        if ((n < 0) || (model == NULL))
            return NULL;

        size_t size     = sizeof(dsp::info_t);
        size           += strlen(ARCH_STRING) + 1;
        size           += strlen(cpu) + 1;
        size           += strlen(model) + 1;
        size           += estimate_features_size(&f);

        dsp::info_t *res = reinterpret_cast<dsp::info_t *>(malloc(size));
        if (res == NULL)
        {
            free(model);
            return res;
        }

        char *text      = reinterpret_cast<char *>(&res[1]);
        res->arch       = text;
        text            = stpcpy(text, ARCH_STRING) + 1;
        res->cpu        = text;
        text            = stpcpy(text, cpu) + 1;
        res->model      = text;
        text            = stpcpy(text, model) + 1;
        res->features   = text;
        build_features_list(text, &f);

        free(model);
        return res;
    }

    static dsp::start_t     dsp_start       = NULL;
    static dsp::finish_t    dsp_finish      = NULL;

    void start(dsp::context_t *ctx)
    {
        dsp_start(ctx);
        uint64_t fpcr           = read_fpcr();
        ctx->data[ctx->top++]   = uint32_t(fpcr);
        ctx->data[ctx->top++]   = uint32_t(fpcr >> 32);
        write_fpcr(fpcr | FPCR_FZ | FPCR_DN | FPCR_FZ16);
    }

    void finish(dsp::context_t *ctx)
    {
        uint64_t lo, hi;
        hi = ctx->data[--ctx->top];
        lo = ctx->data[--ctx->top];
        write_fpcr(lo | (hi << 32));
        dsp_finish(ctx);
    }

#define EXPORT2(function, export)           dsp::function = aarch64::export; TEST_EXPORT(aarch64::export);
#define EXPORT1(function)                   EXPORT2(function, function)

    void dsp_init()
    {
        cpu_features_t f;
        detect_cpu_features(&f);

        if (f.hwcap & HWCAP_AARCH64_ASIMD)
        {
            // Save previous entry points
            dsp_start                       = dsp::start;
            dsp_finish                      = dsp::finish;

            // Export routines
            EXPORT1(start);
            EXPORT1(finish);
        }

        // Export functions
        EXPORT1(info);

        // Initialize Advanced SIMD support
        asimd::dsp_init(&f);
    }
}

#endif /* ARCH_AARCH64 */
