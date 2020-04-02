/*
 * filters.h
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_FILTERS_H_
#define DSP_COMMON_FILTERS_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

//-----------------------------------------------------------------------
// Digital filtering
/*
  DIGITAL FILTER BASICS

     Analog filter cascade:
                t0 + t1*s + t2*s^2
       H[s] = ──────────────────────
                b0 + b1*s + b2*s^2

     Normalized biquad filter:
               b0 + b1*z^-1 + b2*z^-2
       h[z] = ────────────────────────
               1 - a1*z^-1 - a2*z^-2

     The structure of biquad filter:

       s  = input sample
       s' = output sample
       d0, d1 = filter memory

        s   ┌─────┐     ┌───┐                s'
       ──┬─►│x b0 │────►│ + │──────────────┬────►
         │  └─────┘     └───┘              │
         │                ▲                │
         │                │                │
         │             ┌─────┐             │
         │             │  d0 │             │
         │             └─────┘             │
         │                ▲                │
         │                │                │
         │  ┌─────┐     ┌───┐     ┌─────┐  │
         ├─►│x b1 │────►│ + │◄────│x a1 │◄─┤
         │  └─────┘     └───┘     └─────┘  │
         │                ▲                │
         │                │                │
         │             ┌─────┐             │
         │             │  d1 │             │
         │             └─────┘             │
         │                ▲                │
         │                │                │
         │  ┌─────┐     ┌───┐     ┌─────┐  │
         └─►│x b2 │────►│ + │◄────│x a2 │◄─┘
            └─────┘     └───┘     └─────┘

    Static filters consist of biquad filter banks organized in optimal
    for SIMD processing mode. For example, x4 static filter bank will contain
    four biquad cascades:
        ┌───────┬───────┬───────┬───────┐
        │  f[0] │  f[1] │  f[2] │  f[3] │
        └───────┴───────┴───────┴───────┘

    Each biquad filter cascade contains:
        - top coefficients of equation a0..a2;
        - bottom coefficients of equation b0..b1.

    For optimal SIMD processing each coefficient should be packed into distinct
    SIMD register, so here's the data structure of the optimized x4 biquad filter bank:

      Index      +0x00     +0x01     +0x02     +0x03
              ┌─────────┬─────────┬─────────┬─────────┐
       +0x00  │ f[0].b0 │ f[1].b0 │ f[2].b0 │ f[3].b0 │
              ├─────────┼─────────┼─────────┼─────────┤
       +0x04  │ f[0].b1 │ f[1].b1 │ f[2].b1 │ f[3].b1 │
              ├─────────┼─────────┼─────────┼─────────┤
       +0x08  │ f[0].b2 │ f[1].b2 │ f[2].b2 │ f[3].b2 │
              ├─────────┼─────────┼─────────┼─────────┤
       +0x0c  │ f[0].a0 │ f[1].a0 │ f[2].a0 │ f[3].a0 │
              ├─────────┼─────────┼─────────┼─────────┤
       +0x10  │ f[0].a1 │ f[1].a1 │ f[2].a1 │ f[3].a1 │
              └─────────┴─────────┴─────────┴─────────┘

    Dynamic filters consist of shifted biquad filter banks. For example,
    bank of x4 filters for 5 input samples looks like:

      Index     +0x00   +0x01   +0x02   +0x03
              ┌───────┬───────┬───────┬───────┐
       +0x00  │ f[0:0]│   1   │   1   │   1   │
              ├───────┼───────┼───────┼───────┤
       +0x04  │ f[0:1]│ f[1:0]│   1   │   1   │
              ├───────┼───────┼───────┼───────┤
       +0x08  │ f[0:2]│ f[1:1]│ f[2:0]│   1   │
              ├───────┼───────┼───────┼───────┤
       +0x0c  │ f[0:3]│ f[1:2]│ f[2:1]│ f[3:0]│
              ├───────┼───────┼───────┼───────┤
       +0x10  │ f[0:4]│ f[1:3]│ f[2:2]│ f[3:1]│
              ├───────┼───────┼───────┼───────┤
       +0x14  │   1   │ f[1:4]│ f[2:3]│ f[3:2]│
              ├───────┼───────┼───────┼───────┤
       +0x18  │   1   │   1   │ f[2:4]│ f[3:3]│
              ├───────┼───────┼───────┼───────┤
       +0x1c  │   1   │   1   │   1   │ f[3:4]│
              └───────┴───────┴───────┴───────┘

       Each cell is one biquad filter f[i:j] where:
         - i is number of the filter cascade
         - j is the number of filter state for the j'th sample
         - 1 is the cascade that does nothing with input signal

   Then each row can be combined into single x4 filter bank same as for
   static filters and applied to the input sample in a pipeline mode.
 */

/*
  BILINEAR TRANSFORMATION

    Original filter chain:

               t[0] + t[1] * p + t[2] * p^2
      H(p) =  ------------------------------
               b[0] + b[1] * p + b[2] * p^2

    Bilinear transform:

      x    = z^-1

      kf   = 1 / tan(pi * frequency / sample_rate) - frequency shift factor

                   1 - x
      p    = kf * -------   - analog -> digital bilinear transform expression
                   1 + x

    Applied bilinear transform:

               (t[0] + t[1]*kf + t[2]*kf^2) + 2*(t[0] - t[2]*kf^2)*x + (t[0] - t[1]*kf + t[2]*kf^2)*x^2
      H[x] =  -----------------------------------------------------------------------------------------
               (b[0] + b[1]*kf + b[2]*kf^2) + 2*(b[0] - b[2]*kf^2)*x + (b[0] - b[1]*kf + b[2]*kf^2)*x^2

    Finally:

      T    =   { t[0], t[1]*kf, t[2]*kf*kf }
      B    =   { b[0], b[1]*kf, b[2]*kf*kf }

               (T[0] + T[1] + T[2]) + 2*(T[0] - T[2])*z^-1 + (T[0] - T[1] + T[2])*z^-2
      H[z] =  -------------------------------------------------------------------------
               (B[0] + B[1] + B[2]) + 2*(B[0] - B[2])*z^-1 + (B[0] - B[1] + B[2])*z^-2
 */

/*
  MATCHED Z TRANSFORMATION

    Original filter chain:

               t[0] + t[1] * p + t[2] * p^2     k1 * (p + a[0]) * (p + a[1])
      H(p) =  ------------------------------ = -----------------------------
               b[0] + b[1] * p + b[2] * p^2     k2 * (p + b[0]) * (p + b[1])

      a[0], a[1], b[0], b[1] may not exist, so there are series of solutions

    Matched Z-transform:

      T    = discretization period

      x    = z^-1

      p + a = 1 - x*exp(-a * T)

      kf   = 1 / f, f = filter frequency

    After the Matched Z-transform the Frequency Response of the filter has to be normalized!

*/

/**
 * These constants define the offset of filter constants relative to the memory in biquad_t structure,
 * filter alignment and maximum number of memory elements
 */
#define BIQUAD_XN_OFF           0x40
#define BIQUAD_XN_SOFF          "0x40"
#define BIQUAD_ALIGN            0x40
#define BIQUAD_D_ITEMS          16

#pragma pack(push, 1)

/**
 * Analog filter cascade transfer function:
 *
 *              t0 + t1*s + t2*s^2
 *     H[s] = ──────────────────────
 *              b0 + b1*s + b2*s^2
 */
typedef struct f_cascade_t
{
    float       t[4];       // Top part of polynom (zeros): T[p] = t[0] + t[1] * p + t[2] * p^2
    float       b[4];       // Bottom part of polynom (poles): B[p] = b[0] + b[1] * p + b[2] * p^2
} f_cascade_t;

/**
 * Biquad filter bank for 1 digital biquad filter
 * Non-used elements should be filled with zeros
 */
typedef struct biquad_x1_t
{
    float   b0, b1, b2;     //  b0 b1 b2
    float   a1, a2;         //  a1 a2
    float   p0, p1, p2;     //  padding (not used), SHOULD be zero
} biquad_x1_t;

/**
 * Biquad filter bank for 2 digital biquad filters
 * Non-used elements should be filled with zeros
 */
typedef struct biquad_x2_t
{
    float   b0[2];
    float   b1[2];
    float   b2[2];
    float   a1[2];
    float   a2[2];
    float   p[2];           // padding (not used), SHOULD be zero
} biquad_x2_t;

/**
 * Biquad filter bank for 4 digital biquad filters
 */
typedef struct biquad_x4_t
{
    float   b0[4];
    float   b1[4];
    float   b2[4];
    float   a1[4];
    float   a2[4];
} biquad_x4_t;

/**
 * Biquad filter bank for 8 digital biquad filters
 */
typedef struct biquad_x8_t
{
    float   b0[8];
    float   b1[8];
    float   b2[8];
    float   a1[8];
    float   a2[8];
} biquad_x8_t;

/**
 * This is main filter structure with memory elements
 * It should be aligned at least to 16-byte boundary due to
 * alignment restrictions of some different hardware architectures
 * For best purpose it should be aligned to 64-byte boundary
 */
typedef struct biquad_t
{
    float   d[BIQUAD_D_ITEMS];
    union
    {
        biquad_x1_t x1;
        biquad_x2_t x2;
        biquad_x4_t x4;
        biquad_x8_t x8;
    };
    float   __pad[8];
} __lsp_aligned(BIQUAD_ALIGN) biquad_t;

#pragma pack(pop)

//-----------------------------------------------------------------------
// DSP filter functions
namespace dsp
{
    //---------------------------------------------------------------------------------------
    // Static filters
    //---------------------------------------------------------------------------------------
    /** Process single bi-quadratic filter for multiple samples
     *
     * @param dst destination samples
     * @param src source samples
     * @param count number of samples to process
     * @param f bi-quadratic filter structure
     */
    extern void (* biquad_process_x1)(float *dst, const float *src, size_t count, biquad_t *f);

    /** Process two bi-quadratic filters for multiple samples simultaneously
     *
     * @param dst destination samples
     * @param src source samples
     * @param count number of samples to process
     * @param f bi-quadratic filter structure
     */
    extern void (* biquad_process_x2)(float *dst, const float *src, size_t count, biquad_t *f);

    /** Process four bi-quadratic filters for multiple samples simultaneously
     *
     * @param dst destination samples
     * @param src source samples
     * @param count number of samples to process
     * @param f bi-quadratic filter structure
     */
    extern void (* biquad_process_x4)(float *dst, const float *src, size_t count, biquad_t *f);

    /** Process eight bi-quadratic filters for multiple samples simultaneously
     *
     * @param dst destination samples
     * @param src source samples
     * @param count number of samples to process
     * @param f bi-quadratic filter structure
     */
    extern void (* biquad_process_x8)(float *dst, const float *src, size_t count, biquad_t *f);

    //---------------------------------------------------------------------------------------
    // Dynamic filters
    //---------------------------------------------------------------------------------------
    /** Process single dynamic bi-quadratic filter for multiple samples
     *
     * @param dst array of count destination samples to emit
     * @param src array of count source samples to process
     * @param d pointer to filter memory (2 floats)
     * @param count number of samples to process
     * @param f array of count memory-aligned bi-quadratic filters
     */
    extern void (* dyn_biquad_process_x1)(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f);

    /** Process two dynamic bi-quadratic filters for multiple samples
     *
     * @param dst array of count destination samples to emit
     * @param src array of count source samples to process
     * @param d pointer to filter memory (2 floats)
     * @param count number of samples to process
     * @param f array matrix of (count+1)*2 memory-aligned bi-quadratic filters
     */
    extern void (* dyn_biquad_process_x2)(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f);

    /** Process four dynamic bi-quadratic filters for multiple samples
     *
     * @param dst array of count destination samples to emit
     * @param src array of count source samples to process
     * @param d pointer to filter memory (8 floats)
     * @param count number of samples to process
     * @param f array matrix of (count+3)*4 memory-aligned bi-quadratic filters
     */
    extern void (* dyn_biquad_process_x4)(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f);

    /** Process eight dynamic bi-quadratic filters for multiple samples
     *
     * @param dst array of count destination samples to emit
     * @param src array of count source samples to process
     * @param d pointer to filter memory (16 floats)
     * @param count number of samples to process
     * @param f array matrix of (count+7)*8 memory-aligned bi-quadratic filters
     */
    extern void (* dyn_biquad_process_x8)(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f);

    //---------------------------------------------------------------------------------------
    // Transfer function calculation
    //---------------------------------------------------------------------------------------
    /**
     * Compute filter transfer function, computes complex dst = H(f)
     * @param re destination to store transfer function (real value)
     * @param im destination to store transfer function (imaginary value)
     * @param c filter cascade
     * @param freq normalized frequency array
     * @param count size of frequency array
     */
    extern void (* filter_transfer_calc_ri)(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count);

    /**
     * Apply filter transfer function, computes complex dst = dst * H(f)
     * @param re destination to apply transfer function (real value)
     * @param im destination to apply transfer function (imaginary value)
     * @param c filter cascade
     * @param freq normalized frequency array
     * @param count size of frequency array
     */
    extern void (* filter_transfer_apply_ri)(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count);

    /**
     * Compute filter transfer function, computes complex dst = H(f)
     * @param dst destination to store transfer function (packed complex value)
     * @param c filter cascade
     * @param freq normalized frequency array
     * @param count size of frequency array
     */
    extern void (* filter_transfer_calc_pc)(float *dst, const f_cascade_t *c, const float *freq, size_t count);

    /**
     * Apply filter transfer function, computes complex dst = dst * H(f)
     * @param dst destination to apply transfer function (packed complex value)
     * @param c filter cascade
     * @param freq normalized frequency array
     * @param count size of frequency array
     */
    extern void (* filter_transfer_apply_pc)(float *dst, const f_cascade_t *c, const float *freq, size_t count);

    //---------------------------------------------------------------------------------------
    // Bilinear transformation of dynamic filters
    //---------------------------------------------------------------------------------------
    /** Perform bilinear transformation of one filter bank
     *
     * @param bf memory-aligned target transformed biquad x1 filters
     * @param bc memory-aligned source analog bilinear filter cascades
     * @param kf frequency shift coefficient
     * @param count number of cascades  to process
     */
    extern void (* bilinear_transform_x1)(biquad_x1_t *bf, const f_cascade_t *bc, float kf, size_t count);

    /** Perform bilinear transformation of two filter banks
     *
     * @param bf memory-aligned target transformed biquad x2 filters
     * @param bc memory-aligned source analog bilinear filter cascades matrix
     * @param kf frequency shift coefficient
     * @param count number of matrix rows to process
     */
    extern void (* bilinear_transform_x2)(biquad_x2_t *bf, const f_cascade_t *bc, float kf, size_t count);

    /** Perform bilinear transformation of four filter banks
     *
     * @param bf memory-aligned target transformed biquad x4 filters
     * @param bc memory-aligned source analog bilinear filter cascades matrix
     * @param kf frequency shift coefficient
     * @param count number of matrix rows to process
     */
    extern void (* bilinear_transform_x4)(biquad_x4_t *bf, const f_cascade_t *bc, float kf, size_t count);

    /** Perform bilinear transformation of eight filter banks
     *
     * @param bf memory-aligned target transformed biquad x8 filters
     * @param bc memory-aligned source analog bilinear filter cascades matrix
     * @param kf frequency shift coefficient
     * @param count number of matrix rows to process
     */
    extern void (* bilinear_transform_x8)(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count);

    //---------------------------------------------------------------------------------------
    // Matched Z transformation of dynamic filters
    //---------------------------------------------------------------------------------------
    /** Perform matched Z transformation of one filter bank,
     * function modifies contents of the bc memory chunk
     *
     * @param bf memory-aligned target transformed biquad x1 filters
     * @param bc memory-aligned source analog bilinear filter cascades, modified after execution
     * @param kf frequency shift coefficient (filter frequency)
     * @param td tranformation coefficient (2 * pi / sample rate)
     * @param count number of cascades  to process
     */
    extern void (* matched_transform_x1)(biquad_x1_t *bf, f_cascade_t *bc, float kf, float td, size_t count);

    /** Perform matched Z transformation of two filter banks,
     * function modifies contents of the bc memory chunk
     *
     * @param bf memory-aligned target transformed biquad x2 filters
     * @param bc memory-aligned source analog bilinear filter cascades, modified after execution
     * @param kf frequency shift coefficient (filter frequency)
     * @param td tranformation coefficient (2 * pi / sample rate)
     * @param count number of cascades  to process
     */
    extern void (* matched_transform_x2)(biquad_x2_t *bf, f_cascade_t *bc, float kf, float td, size_t count);

    /** Perform matched Z transformation of four filter banks,
     * function modifies contents of the bc memory chunk
     *
     * @param bf memory-aligned target transformed biquad x4 filters
     * @param bc memory-aligned source analog bilinear filter cascades, modified after execution
     * @param kf frequency shift coefficient (filter frequency)
     * @param td tranformation coefficient (2 * pi / sample rate)
     * @param count number of cascades  to process
     */
    extern void (* matched_transform_x4)(biquad_x4_t *bf, f_cascade_t *bc, float kf, float td, size_t count);

    /** Perform matched Z transformation of eight filter banks,
     * function modifies contents of the bc memory chunk
     *
     * @param bf memory-aligned target transformed biquad x8 filters
     * @param bc memory-aligned source analog bilinear filter cascades, modified after execution
     * @param kf frequency shift coefficient (filter frequency)
     * @param td tranformation coefficient (2 * pi / sample rate)
     * @param count number of cascades  to process
     */
    extern void (* matched_transform_x8)(biquad_x8_t *bf, f_cascade_t *bc, float kf, float td, size_t count);

} // dsp

#endif /* DSP_COMMON_FILTERS_H_ */
