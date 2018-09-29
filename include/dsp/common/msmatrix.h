/*
 * msmatrix.h
 *
 *  Created on: 10 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_MSMATRIX_H_
#define DSP_COMMON_MSMATRIX_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

//-----------------------------------------------------------------------
// DSP stereo to mid-side conversion and back
namespace dsp
{
    /** Convert stereo signal to mid-side signal
     *
     * @param m mid signal
     * @param s side signal
     * @param l left signal
     * @param r right signal
     * @param count number of samples to process
     */
    extern void (* lr_to_ms)(float *m, float *s, const float *l, const float *r, size_t count);

    /** Convert stereo signal to middle signal
     *
     * @param m mid signal
     * @param l left channel
     * @param r right channel
     * @param count number of samples to process
     */
    extern void (* lr_to_mid)(float *m, const float *l, const float *r, size_t count);

    /** Convert stereo signal to side signal
     *
     * @param s side signal
     * @param l left channel
     * @param r right channel
     * @param count number of samples to process
     */
    extern void (* lr_to_side)(float *s, const float *l, const float *r, size_t count);

    /** Convert mid-side signal to left-right signal
     *
     * @param l left signal
     * @param r right signal
     * @param m mid signal
     * @param s side signal
     * @param count number of samples to process
     */
    extern void (* ms_to_lr)(float *l, float *r, const float *m, const float *s, size_t count);

    /** Convert mid-side signal to left signal
     *
     * @param l left signal
     * @param m mid signal
     * @param s side signal
     * @param count number of samples to process
     */
    extern void (* ms_to_left)(float *l, const float *m, const float *s, size_t count);

    /** Convert mid-side signal to right signal
     *
     * @param r right signal
     * @param m mid signal
     * @param s side signal
     * @param count number of samples to process
     */
    extern void (* ms_to_right)(float *r, const float *m, const float *s, size_t count);

}

#endif /* DSP_COMMON_MSMATRIX_H_ */
