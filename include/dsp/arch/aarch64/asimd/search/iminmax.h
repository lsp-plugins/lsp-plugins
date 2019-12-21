/*
 * search.h
 *
 *  Created on: 3 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_SEARCH_H_
#define DSP_ARCH_AARCH64_ASIMD_SEARCH_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    IF_ARCH_AARCH64(
        static uint32_t search_indexes[] __lsp_aligned16 =
        {
            // indexes
            0, 1, 2, 3,
            4, 5, 6, 7,
            8, 9, 10, 11,
            12, 13, 14, 15,
            // steps
            16, 16, 16, 16,
            8, 8, 8, 8,
            4, 4, 4, 4,
            1, 1, 1, 1
        };
    )

    /* Register allocation:
       v0       = cind[0..3]        // current index
       v1       = cind[4..7]
       v2       = cind[8..11]
       v3       = cind[12..15]
       v4       = cval[0..3]        // current value
       v5       = cval[4..7]
       v6       = cval[8..11]
       v7       = cval[12..15]
       v8       = nind[0..3]        // new index
       v9       = nind[4..7]
       v10      = nind[8..11]
       v11      = nind[12..15]
       v12      = nval[0..3]        // new value
       v13      = nval[4..7]
       v14      = nval[8..11]
       v15      = nval[12..15]
       v16      = xcmp[0..3]        // comparison result
       v17      = xcmp[4..7]
       v18      = xcmp[8..11]
       v19      = xcmp[12..15]
       v30      = step[0..3]        // bulk step value
       v31      = step[4..7]
     */

#define IOP_SEARCH_CORE(DST, SRC, CMP, UPD) \
    __ASM_EMIT("cmp         %[count], #1") \
    __ASM_EMIT("b.lo        200f") \
    __ASM_EMIT("ld1r        {v4.4s}, [%[" SRC "]]")             /* v4       = cval[0..3] */ \
    __ASM_EMIT("ldp         q8, q9, [%[mask], #0x00]")          /* v8, v9   = nind[0..7] */ \
    __ASM_EMIT("eor         v0.16b, v0.16b, v0.16b")            /* v0       = cind[0..3] */ \
    __ASM_EMIT("subs        %[count], %[count], #8") \
    __ASM_EMIT("b.lt        8f") \
    __ASM_EMIT("ldp         q30, q31, [%[mask], #0x40]")        /* v30, v31 = step[0..7] */ \
    __ASM_EMIT("eor         v1.16b, v1.16b, v1.16b")            /* v1       = cind[4..7] */ \
    __ASM_EMIT("mov         v5.16b, v4.16b")                    /* v5       = cval[4..7] */ \
    /* 16x blocks */ \
    __ASM_EMIT("subs        %[count], %[count], #8") \
    __ASM_EMIT("b.lt        2f") \
    __ASM_EMIT("ldp         q10, q11, [%[mask], #0x20]")        /* v10, v11 = nind[8..15] */ \
    __ASM_EMIT("eor         v2.16b, v2.16b, v2.16b")            /* v2       = cind[8..11] */ \
    __ASM_EMIT("mov         v6.16b, v4.16b")                    /* v6       = cval[8..11] */ \
    __ASM_EMIT("eor         v3.16b, v3.16b, v3.16b")            /* v3       = cind[12..15] */ \
    __ASM_EMIT("mov         v7.16b, v5.16b")                    /* v7       = cval[12..15] */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp         q12, q13, [%[" SRC "], #0x00]")     /* v12, v13 = nval[0..7] */ \
    __ASM_EMIT("ldp         q14, q15, [%[" SRC "], #0x20]") \
    __ASM_EMIT(CMP "        v16.4s, v12.4s, v4.4s")             /* v16      = xcmp[0..3] */ \
    __ASM_EMIT(CMP "        v17.4s, v13.4s, v5.4s") \
    __ASM_EMIT(CMP "        v18.4s, v14.4s, v6.4s") \
    __ASM_EMIT(CMP "        v19.4s, v15.4s, v7.4s") \
    __ASM_EMIT(UPD "        v0.16b, v8.16b, v16.16b")           /* v0       = upd(cind[0..3], nind[0..3], xcmp[0..3]) */ \
    __ASM_EMIT(UPD "        v1.16b, v9.16b, v17.16b") \
    __ASM_EMIT(UPD "        v2.16b, v10.16b, v18.16b") \
    __ASM_EMIT(UPD "        v3.16b, v11.16b, v19.16b") \
    __ASM_EMIT(UPD "        v4.16b, v12.16b, v16.16b")          /* v4       = upd(cval[0..3], nval[0..3], xcmp[0..3]) */ \
    __ASM_EMIT(UPD "        v5.16b, v13.16b, v17.16b") \
    __ASM_EMIT(UPD "        v6.16b, v14.16b, v18.16b") \
    __ASM_EMIT(UPD "        v7.16b, v15.16b, v19.16b") \
    __ASM_EMIT("add         v8.4s, v8.4s, v30.4s")              /* v8       = nind[0..3] += (step = 16) */ \
    __ASM_EMIT("add         v9.4s, v9.4s, v30.4s") \
    __ASM_EMIT("add         v10.4s, v10.4s, v30.4s") \
    __ASM_EMIT("add         v11.4s, v11.4s, v30.4s") \
    __ASM_EMIT("subs        %[count], %[count], #16") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x40") \
    __ASM_EMIT("b.hs        1b") \
    /* reduce 16 -> 8 */ \
    __ASM_EMIT(CMP "        v16.4s, v5.4s, v4.4s") \
    __ASM_EMIT(CMP "        v17.4s, v7.4s, v6.4s") \
    __ASM_EMIT(UPD "        v0.16b, v1.16b, v16.16b") \
    __ASM_EMIT(UPD "        v2.16b, v3.16b, v17.16b") \
    __ASM_EMIT(UPD "        v4.16b, v5.16b, v16.16b") \
    __ASM_EMIT(UPD "        v6.16b, v7.16b, v17.16b") \
    __ASM_EMIT("mov         v1.16b, v2.16b") \
    __ASM_EMIT("mov         v5.16b, v6.16b") \
    __ASM_EMIT("2:") \
    /* 8x block */ \
    __ASM_EMIT("adds        %[count], %[count], #8") \
    __ASM_EMIT("b.lt        6f") \
    __ASM_EMIT("ldp         q12, q13, [%[" SRC "], #0x00]")     /* v12, v13 = nval[0..7] */ \
    __ASM_EMIT(CMP "        v16.4s, v12.4s, v4.4s")             /* v16      = xcmp[0..3] */ \
    __ASM_EMIT(CMP "        v17.4s, v13.4s, v5.4s") \
    __ASM_EMIT(UPD "        v0.16b, v8.16b, v16.16b")           /* v0       = upd(cind[0..3], nind[0..3], xcmp[0..3]) */ \
    __ASM_EMIT(UPD "        v1.16b, v9.16b, v17.16b") \
    __ASM_EMIT(UPD "        v4.16b, v12.16b, v16.16b")          /* v4       = upd(cval[0..3], nval[0..3], xcmp[0..3]) */ \
    __ASM_EMIT(UPD "        v5.16b, v13.16b, v17.16b") \
    __ASM_EMIT("add         v8.4s, v8.4s, v31.4s")              /* v8       = nind[0..3] += (step = 8) */ \
    __ASM_EMIT("sub         %[count], %[count], #8") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x20") \
    __ASM_EMIT("6:") \
    /* reduce 8 -> 4 */ \
    __ASM_EMIT(CMP "        v16.4s, v5.4s, v4.4s") \
    __ASM_EMIT(UPD "        v0.16b, v1.16b, v16.16b") \
    __ASM_EMIT(UPD "        v4.16b, v5.16b, v16.16b") \
    /* 4x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("ldp         q30, q31, [%[mask], #0x60]")        /* v30, v31 = step[8..15]    */ \
    __ASM_EMIT("adds        %[count], %[count], #4") \
    __ASM_EMIT("b.lt        8f") \
    __ASM_EMIT("ldr         q12, [%[" SRC "], #0x00]")          /* v12, v13 = nval[0..7] */ \
    __ASM_EMIT(CMP "        v16.4s, v12.4s, v4.4s")             /* v16      = xcmp[0..3] */ \
    __ASM_EMIT(UPD "        v0.16b, v8.16b, v16.16b")           /* v0       = upd(cind[0..3], nind[0..3], xcmp[0..3]) */ \
    __ASM_EMIT(UPD "        v4.16b, v12.16b, v16.16b")          /* v4       = upd(cval[0..3], nval[0..3], xcmp[0..3]) */ \
    __ASM_EMIT("add         v8.4s, v8.4s, v30.4s")              /* v8       = nind[0..3] += (step = 4) */ \
    __ASM_EMIT("sub         %[count], %[count], #4") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x10") \
    __ASM_EMIT("8:") \
    /* reduce 4->1 , step 1 */ \
    __ASM_EMIT("ext         v1.16b, v0.16b, v0.16b, #12")       /* v0 = i0 i1 i2 i3, v1 = i1 i2 i3 i0 */ \
    __ASM_EMIT("ext         v5.16b, v4.16b, v4.16b, #12")       /* v4 = x0 x1 x2 x3, v5 = x1 x2 x3 x0 */ \
    __ASM_EMIT(CMP "        v16.4s, v5.4s, v4.4s") \
    __ASM_EMIT(UPD "        v0.16b, v1.16b, v16.16b") \
    __ASM_EMIT(UPD "        v4.16b, v5.16b, v16.16b") \
    /* reduce 4->1 , step 2 */ \
    __ASM_EMIT("ext         v1.16b, v0.16b, v0.16b, #8")        /* v0 = i0 i1 i2 i3, v1 = i2 i3 i0 i1 */ \
    __ASM_EMIT("ext         v5.16b, v4.16b, v4.16b, #8")        /* v4 = x0 x1 x2 x3, v5 = x2 x3 x0 x1 */ \
    __ASM_EMIT(CMP "        v16.4s, v5.4s, v4.4s") \
    __ASM_EMIT(UPD "        v0.16b, v1.16b, v16.16b") \
    __ASM_EMIT(UPD "        v4.16b, v5.16b, v16.16b") \
    /* 1x blocks */ \
    __ASM_EMIT("adds        %[count], %[count], #3") \
    __ASM_EMIT("b.lt        10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("ld1r        {v12.4s}, [%[" SRC "]]") \
    __ASM_EMIT(CMP "        v16.4s, v12.4s, v4.4s")             /* v16      = xcmp[0..3] */ \
    __ASM_EMIT(UPD "        v0.16b, v8.16b, v16.16b")           /* v0       = upd(cind[0..3], nind[0..3], xcmp[0..3]) */ \
    __ASM_EMIT(UPD "        v4.16b, v12.16b, v16.16b")          /* v4       = upd(cval[0..3], nval[0..3], xcmp[0..3]) */ \
    __ASM_EMIT("add         v8.4s, v8.4s, v31.4s")              /* v8       = nind[0..3] += (step = 1) */ \
    __ASM_EMIT("subs        %[count], %[count], #1") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x04") \
    __ASM_EMIT("b.ge        9b") \
    __ASM_EMIT("10:") \
    /* end */ \
    __ASM_EMIT("mov         %w[" DST "], v0.s[0]") \
    __ASM_EMIT("200:")

    size_t min_index(const float *src, size_t count)
    {
        uint32_t res = 0;

        ARCH_AARCH64_ASM(
            IOP_SEARCH_CORE("res", "src", "fcmge", "bif")
            : [src] "+r" (src), [count] "+r" (count),
              [res] "+r" (res)
            : [mask] "r" (&search_indexes[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15",
              "v16", "v17", "v18", "v19",
              "v30", "v31"
        );

        return res;
    }

    size_t max_index(const float *src, size_t count)
    {
        uint32_t res = 0;

        ARCH_AARCH64_ASM(
            IOP_SEARCH_CORE("res", "src", "fcmgt", "bit")
            : [src] "+r" (src), [count] "+r" (count),
              [res] "+r" (res)
            : [mask] "r" (&search_indexes[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15",
              "v16", "v17", "v18", "v19",
              "v30", "v31"
        );

        return res;
    }

    size_t abs_min_index(const float *src, size_t count)
    {
        uint32_t res = 0;

        ARCH_AARCH64_ASM(
            IOP_SEARCH_CORE("res", "src", "facge", "bif")
            : [src] "+r" (src), [count] "+r" (count),
              [res] "+r" (res)
            : [mask] "r" (&search_indexes[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15",
              "v16", "v17", "v18", "v19",
              "v30", "v31"
        );

        return res;
    }

    size_t abs_max_index(const float *src, size_t count)
    {
        uint32_t res = 0;

        ARCH_AARCH64_ASM(
            IOP_SEARCH_CORE("res", "src", "facgt", "bit")
            : [src] "+r" (src), [count] "+r" (count),
              [res] "+r" (res)
            : [mask] "r" (&search_indexes[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15",
              "v16", "v17", "v18", "v19",
              "v30", "v31"
        );

        return res;
    }

#undef IOP_SEARCH_CORE

    /* Register allocation:
       v0       = imin[0..3]        // index of minimum
       v1       = imin[4..7]
       v2       = imin[8..11]
       v3       = imin[12..15]
       v4       = vmin[0..3]        // value of minimum
       v5       = vmin[4..7]
       v6       = vmin[8..11]
       v7       = vmin[12..15]
       v8       = imax[0..3]        // index of maximum
       v9       = imax[4..7]
       v10      = imax[8..11]
       v11      = imax[12..15]
       v12      = vmax[0..3]        // value of maximum
       v13      = vmax[4..7]
       v14      = vmax[8..11]
       v15      = vmax[12..15]
       v16      = cind[0..3]        // current index
       v17      = cind[4..7]
       v18      = cind[8..11]
       v19      = cind[12..15]
       v20      = cval[0..3]        // current value
       v21      = cval[4..7]
       v22      = cval[8..11]
       v23      = cval[12..15]
       v24      = xcmp[0..3]        // comparison mask
       v25      = xcmp[4..7]
       v26      = xcmp[8..11]
       v27      = xcmp[12..15]
       v30      = step[0..3]        // bulk step value
       v31      = step[4..7]
     */

#define IOP_MINMAX_CORE(DST1, DST2, SRC, CMP1, UPD1, CMP2, UPD2) \
    __ASM_EMIT("eor         v0.16b, v0.16b, v0.16b")            /* v0       = imin[0..3] */ \
    __ASM_EMIT("eor         v8.16b, v8.16b, v8.16b")            /* v8       = imax[0..3] */ \
    __ASM_EMIT("cmp         %[count], #1") \
    __ASM_EMIT("b.lo        200f") \
    __ASM_EMIT("ld1r        {v4.4s}, [%[" SRC "]]")             /* v4       = vmin[0..3] */ \
    __ASM_EMIT("ldp         q16, q17, [%[mask], #0x00]")        /* v16, v17 = cind[0..7] */ \
    __ASM_EMIT("mov         v12.16b, v4.16b")                   /* v12      = vmax[0..3] */ \
    __ASM_EMIT("subs        %[count], %[count], #8") \
    __ASM_EMIT("b.lt        8f") \
    __ASM_EMIT("ldp         q30, q31, [%[mask], #0x40]")        /* v30, v31 = step[0..7] */ \
    __ASM_EMIT("mov         v5.16b, v4.16b")                    /* v5       = vmin[4..7] */ \
    __ASM_EMIT("mov         v13.16b, v12.16b")                  /* v13      = vmax[4..7] */ \
    __ASM_EMIT("eor         v1.16b, v1.16b, v1.16b")            /* v1       = imin[4..7] */ \
    __ASM_EMIT("eor         v9.16b, v9.16b, v9.16b")            /* v9       = imax[4..7] */ \
    /* 16x blocks */ \
    __ASM_EMIT("subs        %[count], %[count], #8") \
    __ASM_EMIT("b.lt        2f") \
    __ASM_EMIT("ldp         q18, q19, [%[mask], #0x20]")        /* v18, v19 = cind[8..15] */ \
    __ASM_EMIT("eor         v2.16b, v2.16b, v2.16b")            /* v2       = imin[8..11] */ \
    __ASM_EMIT("eor         v3.16b, v3.16b, v3.16b")            /* v3       = imin[12..15] */ \
    __ASM_EMIT("eor         v10.16b, v10.16b, v10.16b")         /* v10      = imax[8..11] */ \
    __ASM_EMIT("eor         v11.16b, v11.16b, v11.16b")         /* v11      = imax[12..15] */ \
    __ASM_EMIT("mov         v6.16b, v4.16b")                    /* v6       = vmin[8..11] */ \
    __ASM_EMIT("mov         v7.16b, v5.16b")                    /* v7       = vmin[12..15] */ \
    __ASM_EMIT("mov         v14.16b, v12.16b")                  /* v14      = vmax[8..11] */ \
    __ASM_EMIT("mov         v15.16b, v13.16b")                  /* v15      = vmax[12..15] */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp         q20, q21, [%[" SRC "], #0x00]")     /* v20, v21 = cval[0..7] */ \
    __ASM_EMIT("ldp         q22, q23, [%[" SRC "], #0x20]") \
    __ASM_EMIT(CMP1 "       v24.4s, v20.4s, v4.4s")             /* v24      = xcmp[0..3] = (cval[0..3] <=> vmin[0..3]) */ \
    __ASM_EMIT(CMP1 "       v25.4s, v21.4s, v5.4s") \
    __ASM_EMIT(UPD1 "       v0.16b, v16.16b, v24.16b")          /* v0       = upd(imin[0..3], cind[0..3], xcmp[0..3]) */ \
    __ASM_EMIT(CMP1 "       v26.4s, v22.4s, v6.4s") \
    __ASM_EMIT(UPD1 "       v1.16b, v17.16b, v25.16b") \
    __ASM_EMIT(CMP1 "       v27.4s, v23.4s, v7.4s") \
    __ASM_EMIT(UPD1 "       v2.16b, v18.16b, v26.16b") \
    __ASM_EMIT(UPD1 "       v3.16b, v19.16b, v27.16b") \
    __ASM_EMIT(UPD1 "       v4.16b, v20.16b, v24.16b")          /* vv       = upd(vmin[0..3], cval[0..3], xcmp[0..3]) */ \
    __ASM_EMIT(UPD1 "       v5.16b, v21.16b, v25.16b") \
    __ASM_EMIT(CMP2 "       v24.4s, v20.4s, v12.4s")            /* v24      = xcmp[0..3] = (cval[0..3] <=> vmax[0..3]) */ \
    __ASM_EMIT(UPD1 "       v6.16b, v22.16b, v26.16b") \
    __ASM_EMIT(CMP2 "       v25.4s, v21.4s, v13.4s") \
    __ASM_EMIT(UPD1 "       v7.16b, v23.16b, v27.16b") \
    __ASM_EMIT(CMP2 "       v26.4s, v22.4s, v14.4s") \
    __ASM_EMIT(UPD2 "       v8.16b, v16.16b, v24.16b")          /* v0       = upd(imin[0..3], cind[0..3], xcmp[0..3]) */ \
    __ASM_EMIT(CMP2 "       v27.4s, v23.4s, v15.4s") \
    __ASM_EMIT(UPD2 "       v9.16b, v17.16b, v25.16b") \
    __ASM_EMIT(UPD2 "       v10.16b, v18.16b, v26.16b") \
    __ASM_EMIT(UPD2 "       v11.16b, v19.16b, v27.16b") \
    __ASM_EMIT(UPD2 "       v12.16b, v20.16b, v24.16b")         /* v12      = upd(vmin[0..3], cval[0..3], xcmp[0..3]) */ \
    __ASM_EMIT("add         v16.4s, v16.4s, v30.4s")            /* v16      = nind[0..3] += (step = 16) */ \
    __ASM_EMIT(UPD2 "       v13.16b, v21.16b, v25.16b") \
    __ASM_EMIT("add         v17.4s, v17.4s, v30.4s") \
    __ASM_EMIT(UPD2 "       v14.16b, v22.16b, v26.16b") \
    __ASM_EMIT("add         v18.4s, v18.4s, v30.4s") \
    __ASM_EMIT(UPD2 "       v15.16b, v23.16b, v27.16b") \
    __ASM_EMIT("subs        %[count], %[count], #16") \
    __ASM_EMIT("add         v19.4s, v19.4s, v30.4s") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x40") \
    __ASM_EMIT("b.hs        1b") \
    /* reduce 16 -> 8 */ \
    __ASM_EMIT(CMP1 "       v24.4s, v5.4s, v4.4s") \
    __ASM_EMIT(CMP1 "       v25.4s, v7.4s, v6.4s") \
    __ASM_EMIT(CMP2 "       v26.4s, v13.4s, v12.4s") \
    __ASM_EMIT(CMP2 "       v27.4s, v15.4s, v14.4s") \
    __ASM_EMIT(UPD1 "       v0.16b, v1.16b, v24.16b") \
    __ASM_EMIT(UPD1 "       v2.16b, v3.16b, v25.16b") \
    __ASM_EMIT(UPD2 "       v8.16b, v9.16b, v26.16b") \
    __ASM_EMIT(UPD2 "       v10.16b, v11.16b, v27.16b") \
    __ASM_EMIT(UPD1 "       v4.16b, v5.16b, v24.16b") \
    __ASM_EMIT(UPD1 "       v6.16b, v7.16b, v25.16b") \
    __ASM_EMIT(UPD2 "       v12.16b, v13.16b, v26.16b") \
    __ASM_EMIT(UPD2 "       v14.16b, v15.16b, v27.16b") \
    __ASM_EMIT("mov         v1.16b, v2.16b") \
    __ASM_EMIT("mov         v5.16b, v6.16b") \
    __ASM_EMIT("mov         v9.16b, v10.16b") \
    __ASM_EMIT("mov         v13.16b, v14.16b") \
    __ASM_EMIT("2:") \
    /* 8x block */ \
    __ASM_EMIT("adds        %[count], %[count], #8") \
    __ASM_EMIT("b.lt        6f") \
    __ASM_EMIT("ldp         q20, q21, [%[" SRC "], #0x00]")     /* v12, v13 = nval[0..7] */ \
    __ASM_EMIT(CMP1 "       v24.4s, v20.4s, v4.4s")             /* v24      = xcmp[0..3] = (cval[0..3] <=> vmin[0..3]) */ \
    __ASM_EMIT(CMP1 "       v25.4s, v21.4s, v5.4s") \
    __ASM_EMIT(UPD1 "       v0.16b, v16.16b, v24.16b")          /* v0       = upd(imin[0..3], cind[0..3], xcmp[0..3]) */ \
    __ASM_EMIT(UPD1 "       v1.16b, v17.16b, v25.16b") \
    __ASM_EMIT(UPD1 "       v4.16b, v20.16b, v24.16b")          /* vv       = upd(vmin[0..3], cval[0..3], xcmp[0..3]) */ \
    __ASM_EMIT(UPD1 "       v5.16b, v21.16b, v25.16b") \
    __ASM_EMIT(CMP2 "       v24.4s, v20.4s, v12.4s")            /* v24      = xcmp[0..3] = (cval[0..3] <=> vmax[0..3]) */ \
    __ASM_EMIT(CMP2 "       v25.4s, v21.4s, v13.4s") \
    __ASM_EMIT(UPD2 "       v8.16b, v16.16b, v24.16b")          /* v0       = upd(imin[0..3], cind[0..3], xcmp[0..3]) */ \
    __ASM_EMIT(UPD2 "       v9.16b, v17.16b, v25.16b") \
    __ASM_EMIT(UPD2 "       v12.16b, v20.16b, v24.16b")         /* v12      = upd(vmin[0..3], cval[0..3], xcmp[0..3]) */ \
    __ASM_EMIT(UPD2 "       v13.16b, v21.16b, v25.16b") \
    __ASM_EMIT("add         v16.4s, v16.4s, v31.4s")            /* v16      = nind[0..3] += (step = 16) */ \
    __ASM_EMIT("sub         %[count], %[count], #8") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x20") \
    __ASM_EMIT("6:") \
    /* reduce 8 -> 4 */ \
    __ASM_EMIT(CMP1 "       v24.4s, v5.4s, v4.4s") \
    __ASM_EMIT(CMP2 "       v26.4s, v13.4s, v12.4s") \
    __ASM_EMIT(UPD1 "       v0.16b, v1.16b, v24.16b") \
    __ASM_EMIT(UPD2 "       v8.16b, v9.16b, v26.16b") \
    __ASM_EMIT(UPD1 "       v4.16b, v5.16b, v24.16b") \
    __ASM_EMIT(UPD2 "       v12.16b, v13.16b, v26.16b") \
    /* 4x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("ldp         q30, q31, [%[mask], #0x60]")        /* v30, v31 = step[8..15]    */ \
    __ASM_EMIT("adds        %[count], %[count], #4") \
    __ASM_EMIT("b.lt        8f") \
    __ASM_EMIT("ldr         q20, [%[" SRC "], #0x00]")          /* v12, v13 = nval[0..7] */ \
    __ASM_EMIT(CMP1 "       v24.4s, v20.4s, v4.4s")             /* v24      = xcmp[0..3] = (cval[0..3] <=> vmin[0..3]) */ \
    __ASM_EMIT(CMP2 "       v25.4s, v20.4s, v12.4s")            /* v25      = xcmp[0..3] = (cval[0..3] <=> vmax[0..3]) */ \
    __ASM_EMIT(UPD1 "       v0.16b, v16.16b, v24.16b")          /* v0       = upd(imin[0..3], cind[0..3], xcmp[0..3]) */ \
    __ASM_EMIT(UPD2 "       v8.16b, v16.16b, v25.16b")          /* v0       = upd(imin[0..3], cind[0..3], xcmp[0..3]) */ \
    __ASM_EMIT(UPD1 "       v4.16b, v20.16b, v24.16b")          /* vv       = upd(vmin[0..3], cval[0..3], xcmp[0..3]) */ \
    __ASM_EMIT(UPD2 "       v12.16b, v20.16b, v25.16b")         /* v12      = upd(vmin[0..3], cval[0..3], xcmp[0..3]) */ \
    __ASM_EMIT("add         v16.4s, v16.4s, v30.4s")            /* v16      = nind[0..3] += (step = 4) */ \
    __ASM_EMIT("sub         %[count], %[count], #4") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x10") \
    __ASM_EMIT("8:") \
    /* reduce 4->1 , step 1 */ \
    __ASM_EMIT("ext         v1.16b, v0.16b, v0.16b, #12")       /* v0 = i0 i1 i2 i3, v1 = i1 i2 i3 i0 */ \
    __ASM_EMIT("ext         v9.16b, v8.16b, v8.16b, #12") \
    __ASM_EMIT("ext         v5.16b, v4.16b, v4.16b, #12")       /* v4 = x0 x1 x2 x3, v5 = x1 x2 x3 x0 */ \
    __ASM_EMIT("ext         v13.16b, v12.16b, v12.16b, #12") \
    __ASM_EMIT(CMP1 "       v24.4s, v5.4s, v4.4s") \
    __ASM_EMIT(CMP2 "       v25.4s, v13.4s, v12.4s") \
    __ASM_EMIT(UPD1 "       v0.16b, v1.16b, v24.16b") \
    __ASM_EMIT(UPD2 "       v8.16b, v9.16b, v25.16b") \
    __ASM_EMIT(UPD1 "       v4.16b, v5.16b, v24.16b") \
    __ASM_EMIT(UPD2 "       v12.16b, v13.16b, v25.16b") \
    /* reduce 4->1 , step 2 */ \
    __ASM_EMIT("ext         v1.16b, v0.16b, v0.16b, #8")        /* v0 = i0 i1 i2 i3, v1 = i1 i2 i3 i0 */ \
    __ASM_EMIT("ext         v9.16b, v8.16b, v8.16b, #8") \
    __ASM_EMIT("ext         v5.16b, v4.16b, v4.16b, #8")        /* v4 = x0 x1 x2 x3, v5 = x1 x2 x3 x0 */ \
    __ASM_EMIT("ext         v13.16b, v12.16b, v12.16b, #8") \
    __ASM_EMIT(CMP1 "       v24.4s, v5.4s, v4.4s") \
    __ASM_EMIT(CMP2 "       v25.4s, v13.4s, v12.4s") \
    __ASM_EMIT(UPD1 "       v0.16b, v1.16b, v24.16b") \
    __ASM_EMIT(UPD2 "       v8.16b, v9.16b, v25.16b") \
    __ASM_EMIT(UPD1 "       v4.16b, v5.16b, v24.16b") \
    __ASM_EMIT(UPD2 "       v12.16b, v13.16b, v25.16b") \
    /* 1x blocks */ \
    __ASM_EMIT("adds        %[count], %[count], #3") \
    __ASM_EMIT("b.lt        10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("ld1r        {v20.4s}, [%[" SRC "]]") \
    __ASM_EMIT(CMP1 "       v24.4s, v20.4s, v4.4s")             /* v24      = xcmp[0..3] = (cval[0..3] <=> vmin[0..3]) */ \
    __ASM_EMIT(CMP2 "       v25.4s, v20.4s, v12.4s")            /* v25      = xcmp[0..3] = (cval[0..3] <=> vmax[0..3]) */ \
    __ASM_EMIT(UPD1 "       v0.16b, v16.16b, v24.16b")          /* v0       = upd(imin[0..3], cind[0..3], xcmp[0..3]) */ \
    __ASM_EMIT(UPD2 "       v8.16b, v16.16b, v25.16b")          /* v0       = upd(imin[0..3], cind[0..3], xcmp[0..3]) */ \
    __ASM_EMIT(UPD1 "       v4.16b, v20.16b, v24.16b")          /* vv       = upd(vmin[0..3], cval[0..3], xcmp[0..3]) */ \
    __ASM_EMIT(UPD2 "       v12.16b, v20.16b, v25.16b")         /* v12      = upd(vmin[0..3], cval[0..3], xcmp[0..3]) */ \
    __ASM_EMIT("add         v16.4s, v16.4s, v31.4s")            /* v16      = nind[0..3] += (step = 1) */ \
    __ASM_EMIT("subs        %[count], %[count], #1") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x04") \
    __ASM_EMIT("b.ge        9b") \
    __ASM_EMIT("10:") \
    /* end */ \
    __ASM_EMIT("200:") \
    __ASM_EMIT("st1         {v0.s}[0], [%[" DST1 "]]") \
    __ASM_EMIT("st1         {v8.s}[0], [%[" DST2 "]]")

    void minmax_index(const float *src, size_t count, size_t *min, size_t *max)
    {
        *min    = 0;
        *max    = 0;
        ARCH_AARCH64_ASM(
            IOP_MINMAX_CORE("min", "max", "src", "fcmge", "bif", "fcmgt", "bit")
            : [src] "+r" (src), [count] "+r" (count)
            : [min] "r" (min), [max] "r" (max),
              [mask] "r" (&search_indexes[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23",
              "v24", "v25", "v26", "v27",
              "v30", "v31"
        );
    }

    void abs_minmax_index(const float *src, size_t count, size_t *min, size_t *max)
    {
        *min    = 0;
        *max    = 0;
        ARCH_AARCH64_ASM(
            IOP_MINMAX_CORE("min", "max", "src", "facge", "bif", "facgt", "bit")
            : [src] "+r" (src), [count] "+r" (count)
            : [min] "r" (min), [max] "r" (max),
              [mask] "r" (&search_indexes[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23",
              "v24", "v25", "v26", "v27",
              "v30", "v31"
        );
    }
}

#endif /* DSP_ARCH_AARCH64_ASIMD_SEARCH_H_ */
