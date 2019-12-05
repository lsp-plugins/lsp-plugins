/*
 * search.h
 *
 *  Created on: 9 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_SEARCH_IMINMAX_H_
#define DSP_ARCH_ARM_NEON_D32_SEARCH_IMINMAX_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    IF_ARCH_ARM(
        static uint32_t indexes[] __lsp_aligned16 =
        {
            0, 1, 2, 3, 4, 5, 6, 7,
            8, 8, 8, 8, 8, 8, 8, 8,
            4, 4, 4, 4,
            1, 1, 1, 1
        };
    );

    /*
     * Register allocation:
     * q0       imin0
     * q1       imin1
     * q2       vmin0
     * q3       vmin1
     * q8       cind0
     * q9       cind1
     * q10      cval0
     * q11      cval1
     * q12      xcmp0
     * q13      xcmp1
     * q14      step0
     * q15      step1
     */
    #define IDX_COND_SEARCH(kmin) \
        __ASM_EMIT("veor        q0, q0")                    /* q0   = imin0 */ \
        __ASM_EMIT("cmp         %[count], $1") \
        __ASM_EMIT("blo         6f") \
        /* 8x blocks */ \
        __ASM_EMIT("vldm        %[IDXS]!, {q8-q9}")         /* q8   = cind0, q9 = cind1 */ \
        __ASM_EMIT("vld1.32     {d4[], d5[]}, [%[src]]")    /* q2   = vmin0 */ \
        __ASM_EMIT("subs        %[count], $8") \
        __ASM_EMIT("vldm        %[IDXS]!, {q14-q15}")       /* q14  = step0, q15 = step1 */ \
        __ASM_EMIT("blo         2f") \
        __ASM_EMIT("vmov        q1, q0")                    /* q1   = imin1 */ \
        __ASM_EMIT("vmov        q3, q2")                    /* q3   = vmin1 */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm        %[src]!, {q10-q11}")        /* q10  = cval0, q11 = cval1 */ \
        __ASM_EMIT(kmin "       q12, q2, q10")              /* q12  = (vmin0 <= samp0) */ \
        __ASM_EMIT(kmin "       q13, q3, q11") \
        __ASM_EMIT("vbif        q0, q8, q12")               /* q0  = imin0 & (vmin0 <= samp0) | inew0 & (vmin0 > samp0) */ \
        __ASM_EMIT("vbif        q1, q9, q13") \
        __ASM_EMIT("vbif        q2, q10, q12")              /* q6  = vmin0 & (vmin0 <= samp0) | samp0 & (vmin0 > samp0) */ \
        __ASM_EMIT("vbif        q3, q11, q13") \
        __ASM_EMIT("vadd.u32    q8, q14")                   /* inew0 += 8 */ \
        __ASM_EMIT("vadd.u32    q9, q15")                   /* inew1 += 8 */ \
        __ASM_EMIT("subs        %[count], $8")              /* count -= 8 */ \
        __ASM_EMIT("bhs         1b") \
        /* 8 -> 4 reduce */ \
        __ASM_EMIT(kmin "       q12, q2, q3") \
        __ASM_EMIT("vbif        q0, q1, q12") \
        __ASM_EMIT("vbif        q2, q3, q12") \
        /* 4x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("vldm        %[IDXS]!, {q14-q15}")       /* q14 = incr = 4, q15 = incr = 1 */ \
        __ASM_EMIT("adds        %[count], $4") \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vldm        %[src]!, {q10}")            /* q10 = samp0 */ \
        __ASM_EMIT(kmin "       q12, q2, q10")              /* q12 = (vmin0 <= samp0) */ \
        __ASM_EMIT("vbif        q0, q8, q12")               /* q0  = imin0 & (vmin0 <= samp0) | inew0 & (vmin0 > samp0) */ \
        __ASM_EMIT("vbif        q2, q10, q12")              /* q6  = vmin0 & (vmin0 <= samp0) | samp0 & (vmin0 > samp0) */ \
        __ASM_EMIT("vadd.u32    q8, q14")                   /* inew0 += 4 */ \
        __ASM_EMIT("sub         %[count], $4")              /* count -= 4 */ \
        __ASM_EMIT("4:") \
        /* reduce 4->1, step 1 */ \
        __ASM_EMIT("vext.32     q10, q2, q2, #3") \
        __ASM_EMIT("vext.32     q11, q0, q0, #3") \
        __ASM_EMIT(kmin "       q12, q2, q10") \
        __ASM_EMIT("vbif        q2, q10, q12") \
        __ASM_EMIT("vbif        q0, q11, q12") \
        /* 4x post-process, step 2 */ \
        __ASM_EMIT("vext.32     q10, q2, q2, #2") \
        __ASM_EMIT("vext.32     q11, q0, q0, #2") \
        __ASM_EMIT(kmin "       q12, q2, q10") \
        __ASM_EMIT("vbif        q2, q10, q12") \
        __ASM_EMIT("vbif        q0, q11, q12") \
        /* 1x block */ \
        __ASM_EMIT("adds        %[count], $3") \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("5:") \
        __ASM_EMIT("vld1.32     {d20[], d21[]}, [%[src]]!") /* q10 = samp0 */ \
        __ASM_EMIT(kmin "       q12, q2, q10")              /* q12 = (vmin0 <= samp0) */ \
        __ASM_EMIT("vbif        q0, q8, q12")               /* q0  = imin0 & (vmin0 <= samp0) | inew0 & (vmin0 > samp0) */ \
        __ASM_EMIT("vbif        q2, q10, q12")              /* q6  = vmin0 & (vmin0 <= samp0) | samp0 & (vmin0 > samp0) */ \
        __ASM_EMIT("vadd.u32    q8, q15")                   /* inew1 += 1 */ \
        __ASM_EMIT("subs        %[count], $1")              /* count-- */ \
        __ASM_EMIT("bge         5b") \
        __ASM_EMIT("6:") \
        /* end */ \
        __ASM_EMIT("vmov        %[res], d0[0]")

    size_t min_index(const float *src, size_t count)
    {
        uint32_t res = 0;
        IF_ARCH_ARM(uint32_t *pindexes = indexes);
        ARCH_ARM_ASM(
            IDX_COND_SEARCH("vcle.f32")
            : [src] "+r" (src), [count] "+r" (count),
              [IDXS] "+r" (pindexes),
              [res] "+r" (res)
            :
            : "cc", "memory",
              "q1", "q2", "q3",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );

        return res;
    }

    size_t max_index(const float *src, size_t count)
    {
        uint32_t res = 0;
        IF_ARCH_ARM(uint32_t *pindexes = indexes);
        ARCH_ARM_ASM(
            IDX_COND_SEARCH("vcge.f32")
            : [src] "+r" (src), [count] "+r" (count),
              [IDXS] "+r" (pindexes),
              [res] "+r" (res)
            :
            : "cc", "memory",
              "q1", "q2", "q3",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );

        return res;
    }

    size_t abs_min_index(const float *src, size_t count)
    {
        uint32_t res = 0;
        IF_ARCH_ARM(uint32_t *pindexes = indexes;);
        ARCH_ARM_ASM(
            IDX_COND_SEARCH("vacle.f32")
            : [src] "+r" (src), [count] "+r" (count),
              [IDXS] "+r" (pindexes),
              [res] "+r" (res)
            :
            : "cc", "memory",
              "q1", "q2", "q3",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );

        return res;
    }

    size_t abs_max_index(const float *src, size_t count)
    {
        uint32_t res = 0;
        IF_ARCH_ARM(uint32_t *pindexes = indexes;);
        ARCH_ARM_ASM(
            IDX_COND_SEARCH("vacge.f32")
            : [src] "+r" (src), [count] "+r" (count),
              [IDXS] "+r" (pindexes),
              [res] "+r" (res)
            :
            : "cc", "memory",
              "q1", "q2", "q3",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );

        return res;
    }

    #undef IDX_COND_SEARCH

    /*
     * Register allocation:
     * q0       imin0
     * q1       imin1
     * q2       vmin0
     * q3       vmin1
     * q4       imax0
     * q5       imax1
     * q6       vmax0
     * q7       vmax1
     * q8       cind0
     * q9       cind1
     * q10      cval0
     * q11      cval1
     * q12      xcmp0
     * q13      xcmp1
     * q14      step0
     * q15      step1
     */
    #define MINMAX_COND_SEARCH(kmin, kmax) \
        __ASM_EMIT("veor        q0, q0")                    /* q0   = imin0 */ \
        __ASM_EMIT("veor        q4, q4")                    /* q4   = imax0 */ \
        __ASM_EMIT("cmp         %[count], $1") \
        __ASM_EMIT("blo         6f") \
        /* 8x blocks */ \
        __ASM_EMIT("vldm        %[IDXS]!, {q8-q9}")         /* q8   = cind0, q9 = cind1 */ \
        __ASM_EMIT("vld1.32     {d4[], d5[]}, [%[src]]")    /* q2   = vmin0 */ \
        __ASM_EMIT("vmov        q6, q2")                    /* q6   = vmax */ \
        __ASM_EMIT("subs        %[count], $8") \
        __ASM_EMIT("vldm        %[IDXS]!, {q14-q15}")       /* q14  = step0, q15 = step1 */ \
        __ASM_EMIT("blo         2f") \
        __ASM_EMIT("vmov        q1, q0")                    /* q1   = imin1 */ \
        __ASM_EMIT("vmov        q3, q2")                    /* q3   = vmin1 */ \
        __ASM_EMIT("vmov        q5, q4")                    /* q5   = imax1 */ \
        __ASM_EMIT("vmov        q7, q6")                    /* q7   = vmax1 */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm        %[src]!, {q10-q11}")        /* q10  = cval0, q11 = cval1 */ \
        __ASM_EMIT(kmin "       q12, q2, q10")              /* q12  = (vmin0 <= samp0) */ \
        __ASM_EMIT(kmin "       q13, q3, q11") \
        __ASM_EMIT("vbif        q0, q8, q12")               /* q0  = imin0 & (vmin0 <= samp0) | inew0 & (vmin0 > samp0) */ \
        __ASM_EMIT("vbif        q1, q9, q13") \
        __ASM_EMIT("vbif        q2, q10, q12")              /* q6  = vmin0 & (vmin0 <= samp0) | samp0 & (vmin0 > samp0) */ \
        __ASM_EMIT("vbif        q3, q11, q13") \
        __ASM_EMIT(kmax "       q12, q6, q10")              /* q12 = vmax0 >= samp0 */ \
        __ASM_EMIT(kmax "       q13, q7, q11") \
        __ASM_EMIT("vbif        q4, q8, q12")               /* q2  = imax0 & (vmax0 >= samp0) | q12 = inew0 & (vmax0 < samp0) */ \
        __ASM_EMIT("vbif        q5, q9, q13") \
        __ASM_EMIT("vbif        q6, q10, q12")              /* q8  = vmax0 & (vmax0 >= samp0) | samp0 & (vmax0 < samp0) */ \
        __ASM_EMIT("vbif        q7, q11, q13") \
        __ASM_EMIT("vadd.u32    q8, q14")                   /* inew0 += 8 */ \
        __ASM_EMIT("vadd.u32    q9, q15")                   /* inew1 += 8 */ \
        __ASM_EMIT("subs        %[count], $8")              /* count -= 8 */ \
        __ASM_EMIT("bhs         1b") \
        /* 8 -> 4 reduce */ \
        __ASM_EMIT(kmin "       q12, q2, q3") \
        __ASM_EMIT(kmax "       q13, q6, q7") \
        __ASM_EMIT("vbif        q0, q1, q12") \
        __ASM_EMIT("vbif        q4, q5, q13") \
        __ASM_EMIT("vbif        q2, q3, q12") \
        __ASM_EMIT("vbif        q6, q7, q13") \
        /* 4x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("vldm        %[IDXS]!, {q14-q15}")       /* q14 = incr = 4, q15 = incr = 1 */ \
        __ASM_EMIT("adds        %[count], $4") \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vldm        %[src]!, {q10}")            /* q10 = samp0 */ \
        __ASM_EMIT(kmin "       q12, q2, q10")              /* q12 = (vmin0 <= samp0) */ \
        __ASM_EMIT(kmax "       q13, q6, q10")              /* q13 = vmax0 >= samp0 */ \
        __ASM_EMIT("vbif        q0, q8, q12")               /* q0  = imin0 & (vmin0 <= samp0) | inew0 & (vmin0 > samp0) */ \
        __ASM_EMIT("vbif        q4, q8, q13")               /* q2  = imax0 & (vmax0 >= samp0) | q12 = inew0 & (vmax0 < samp0) */ \
        __ASM_EMIT("vbif        q2, q10, q12")              /* q6  = vmin0 & (vmin0 <= samp0) | samp0 & (vmin0 > samp0) */ \
        __ASM_EMIT("vbif        q6, q10, q13")              /* q8  = vmax0 & (vmax0 >= samp0) | samp0 & (vmax0 < samp0) */ \
        __ASM_EMIT("vadd.u32    q8, q14")                   /* inew0 += 4 */ \
        __ASM_EMIT("sub         %[count], $4")              /* count -= 4 */ \
        __ASM_EMIT("4:") \
        /* reduce 4->1, step 1 */ \
        __ASM_EMIT("vext.32     q10, q2, q2, #3") \
        __ASM_EMIT("vext.32     q11, q0, q0, #3") \
        __ASM_EMIT(kmin "       q12, q2, q10") \
        __ASM_EMIT("vbif        q2, q10, q12") \
        __ASM_EMIT("vbif        q0, q11, q12") \
        __ASM_EMIT("vext.32     q10, q6, q6, #3") \
        __ASM_EMIT("vext.32     q11, q4, q4, #3") \
        __ASM_EMIT(kmax "       q12, q6, q10") \
        __ASM_EMIT("vbif        q6, q10, q12") \
        __ASM_EMIT("vbif        q4, q11, q12") \
        /* 4x post-process, step 2 */ \
        __ASM_EMIT("vext.32     q10, q2, q2, #2") \
        __ASM_EMIT("vext.32     q11, q0, q0, #2") \
        __ASM_EMIT(kmin "       q12, q2, q10") \
        __ASM_EMIT("vbif        q2, q10, q12") \
        __ASM_EMIT("vbif        q0, q11, q12") \
        __ASM_EMIT("vext.32     q10, q6, q6, #2") \
        __ASM_EMIT("vext.32     q11, q4, q4, #2") \
        __ASM_EMIT(kmax "       q12, q6, q10") \
        __ASM_EMIT("vbif        q6, q10, q12") \
        __ASM_EMIT("vbif        q4, q11, q12") \
        /* 1x block */ \
        __ASM_EMIT("adds        %[count], $3") \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("5:") \
        __ASM_EMIT("vld1.32     {d20[], d21[]}, [%[src]]!") /* q10 = samp0 */ \
        __ASM_EMIT(kmin "       q12, q2, q10")              /* q12 = (vmin0 <= samp0) */ \
        __ASM_EMIT(kmax "       q13, q6, q10")              /* q13 = vmax0 >= samp0 */ \
        __ASM_EMIT("vbif        q0, q8, q12")               /* q0  = imin0 & (vmin0 <= samp0) | inew0 & (vmin0 > samp0) */ \
        __ASM_EMIT("vbif        q4, q8, q13")               /* q2  = imax0 & (vmax0 >= samp0) | q12 = inew0 & (vmax0 < samp0) */ \
        __ASM_EMIT("vbif        q2, q10, q12")              /* q6  = vmin0 & (vmin0 <= samp0) | samp0 & (vmin0 > samp0) */ \
        __ASM_EMIT("vbif        q6, q10, q13")              /* q8  = vmax0 & (vmax0 >= samp0) | samp0 & (vmax0 < samp0) */ \
        __ASM_EMIT("vadd.u32    q8, q15")                   /* inew1 += 1 */ \
        __ASM_EMIT("subs        %[count], $1")              /* count-- */ \
        __ASM_EMIT("bge         5b") \
        __ASM_EMIT("6:") \
        /* end */ \
        __ASM_EMIT("vst1.32     {d0[0]}, [%[min]]") \
        __ASM_EMIT("vst1.32     {d8[0]}, [%[max]]")

    void minmax_index(const float *src, size_t count, size_t *min, size_t *max)
    {
        IF_ARCH_ARM(uint32_t *pindexes = indexes);
        ARCH_ARM_ASM(
            MINMAX_COND_SEARCH("vcle.f32", "vcge.f32")
            : [src] "+r" (src), [count] "+r" (count),
              [IDXS] "+r" (pindexes)
            : [min] "r" (min), [max] "r" (max)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void abs_minmax_index(const float *src, size_t count, size_t *min, size_t *max)
    {
        IF_ARCH_ARM(uint32_t *pindexes = indexes);
        ARCH_ARM_ASM(
            MINMAX_COND_SEARCH("vacle.f32", "vacge.f32")
            : [src] "+r" (src), [count] "+r" (count),
              [IDXS] "+r" (pindexes)
            : [min] "r" (min), [max] "r" (max)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    #undef MINMAX_COND_SEARCH
}

#endif /* DSP_ARCH_ARM_NEON_D32_SEARCH_IMINMAX_H_ */
