/*
 * filters.h
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_DSP_FILTERS_H_
#define INCLUDE_CORE_DSP_FILTERS_H_

#ifndef __CORE_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __CORE_DSP_DEFS */

namespace lsp
{
    //-----------------------------------------------------------------------
    // Digital filtering
    #pragma pack(push, 1)
    /*
         Normalized biquad filter:
                   a0 + a1*z^-1 + a2*z^-2
           h[z] = ------------------------
                   1 - b1*z^-1 - b2*z^-2
     */

    // These constants should be redefined if structure of biquad_t changes
    #define BIQUAD_X8_A0_OFF    0x40
    #define BIQUAD_X8_A0_SOFF   "0x40"
    #define BIQUAD_X8_I0_OFF    0x50
    #define BIQUAD_X8_I0_SOFF   "0x50"
    #define BIQUAD_X8_A1_OFF    0x60
    #define BIQUAD_X8_A1_SOFF   "0x60"
    #define BIQUAD_X8_I1_OFF    0x70
    #define BIQUAD_X8_I1_SOFF   "0x70"
    #define BIQUAD_X8_A2_OFF    0x80
    #define BIQUAD_X8_A2_SOFF   "0x80"
    #define BIQUAD_X8_I2_OFF    0x90
    #define BIQUAD_X8_I2_SOFF   "0x90"
    #define BIQUAD_X8_B1_OFF    0xa0
    #define BIQUAD_X8_B1_SOFF   "0xa0"
    #define BIQUAD_X8_J1_OFF    0xb0
    #define BIQUAD_X8_J1_SOFF   "0xb0"
    #define BIQUAD_X8_B2_OFF    0xc0
    #define BIQUAD_X8_B2_SOFF   "0xc0"
    #define BIQUAD_X8_J2_OFF    0xd0
    #define BIQUAD_X8_J2_SOFF   "0xd0"

    #define BIQUAD_X4_A0_OFF    0x40
    #define BIQUAD_X4_A0_SOFF   "0x40"
    #define BIQUAD_X4_A1_OFF    0x50
    #define BIQUAD_X4_A1_SOFF   "0x50"
    #define BIQUAD_X4_A2_OFF    0x60
    #define BIQUAD_X4_A2_SOFF   "0x60"
    #define BIQUAD_X4_B1_OFF    0x70
    #define BIQUAD_X4_B1_SOFF   "0x70"
    #define BIQUAD_X4_B2_OFF    0x80
    #define BIQUAD_X4_B2_SOFF   "0x80"

    #define BIQUAD_X2_A_OFF     BIQUAD_X4_A0_OFF
    #define BIQUAD_X2_A_SOFF    BIQUAD_X4_A0_SOFF
    #define BIQUAD_X2_I_OFF     BIQUAD_X4_A1_OFF
    #define BIQUAD_X2_I_SOFF    BIQUAD_X4_A1_SOFF
    #define BIQUAD_X2_B_OFF     BIQUAD_X4_A2_OFF
    #define BIQUAD_X2_B_SOFF    BIQUAD_X4_A2_SOFF
    #define BIQUAD_X2_J_OFF     BIQUAD_X4_B1_OFF
    #define BIQUAD_X2_J_SOFF    BIQUAD_X4_B1_SOFF

    #define BIQUAD_X1_A_OFF     BIQUAD_X4_A0_OFF
    #define BIQUAD_X1_A_SOFF    BIQUAD_X4_A0_SOFF
    #define BIQUAD_X1_B_OFF     BIQUAD_X4_A1_OFF
    #define BIQUAD_X1_B_SOFF    BIQUAD_X4_A1_SOFF

    #define BIQUAD_D0_OFF       0x00
    #define BIQUAD_D0_SOFF      "0x00"
    #define BIQUAD_D1_OFF       0x10
    #define BIQUAD_D1_SOFF      "0x10"
    #define BIQUAD_D2_OFF       0x20
    #define BIQUAD_D2_SOFF      "0x20"
    #define BIQUAD_D3_OFF       0x30
    #define BIQUAD_D3_SOFF      "0x30"

    #define BIQUAD_D_ITEMS      16
    #define BIQUAD_ALIGN        0x40

    typedef struct biquad_x8_t
    {
        float   a0[8];
        float   a1[8];
        float   a2[8];
        float   b1[8];
        float   b2[8];
    } biquad_x8_t;

    typedef struct biquad_x4_t
    {
        float   a0[4];
        float   a1[4];
        float   a2[4];
        float   b1[4];
        float   b2[4];
    } biquad_x4_t;

    typedef struct biquad_x2_t
    {
        float   a[8];  //  a0 a0 a1 a2 i0 i1 i2 i3
        float   b[8];  //  b1 b2 0  0  j0 j1 j2 j3
    } biquad_x2_t;

    typedef struct biquad_x1_t
    {
        float   a[4];  //  a0 a0 a1 a2
        float   b[4];  //  b1 b2 0  0
    } biquad_x1_t;

    // This is main filter structure
    // It should be aligned at least to 16-byte boundary
    // For best purposes it should be aligned to 64-byte boundary
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

    namespace dsp
    {
        /** Process bi-quadratic filter for multiple samples
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
    } // dsp
} // lsp

#endif /* INCLUDE_CORE_DSP_FILTERS_H_ */
