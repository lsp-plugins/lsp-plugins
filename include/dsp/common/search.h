/*
 * search.h
 *
 *  Created on: 10 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_SEARCH_H_
#define DSP_COMMON_SEARCH_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

//-----------------------------------------------------------------------
// DSP search functions
namespace dsp
{
    /** Calculate min { src }
     *
     * @param src source vector
     * @param count number of elements
     * @return minimum value
     */
    extern float (* min)(const float *src, size_t count);

    /** Calculate max { src }
     *
     * @param src source vector
     * @param count number of elements
     * @return maximum value
     */
    extern float (* max)(const float *src, size_t count);

    /** Get absolute maximum: result = max { abs(src[i]) }
     *
     * @param src source array
     * @param count number of elements
     * @return result
     */
    extern float (* abs_max)(const float *src, size_t count);

    /** Get absolute minimum: result = min { abs(src[i]) }
     *
     * @param src source array
     * @param count number of elements
     * @return result
     */
    extern float (* abs_min)(const float *src, size_t count);

    /** Calculate min { src }, max { src }
     *
     * @param src source vector
     * @param count number of elements
     * @return maximum value
     */
    extern void (* minmax)(const float *src, size_t count, float *min, float *max);

    /** Calculate min { abs(src) }, max { abs(src) }
     *
     * @param src source vector
     * @param count number of elements
     * @return maximum value
     */
    extern void (* abs_minmax)(const float *src, size_t count, float *min, float *max);

    /** Calculate @ min { src }
     *
     * @param src source vector
     * @param count number of elements
     * @return minimum value index
     */
    extern size_t (* min_index)(const float *src, size_t count);

    /** Calculate @ max { src }
     *
     * @param src source vector
     * @param count number of elements
     * @return maximum value
     */
    extern size_t (* max_index)(const float *src, size_t count);

    /** Calculate @ minmax { src }
     *
     * @param src source vector
     * @param count number of elements
     * @param min pointer to store minimum value index
     * @param max pointer to store maximum value index
     */
    extern void (* minmax_index)(const float *src, size_t count, size_t *min, size_t *max);

    /** Calculate @ max { abs(src) }
     *
     * @param src source
     * @param count number of samples
     * @return index of maximum element
     */
    extern size_t  (* abs_max_index)(const float *src, size_t count);

    /** Calculate @ min { abs(src) }
     *
     * @param src source
     * @param count number of samples
     * @return index of maximum element
     */
    extern size_t  (* abs_min_index)(const float *src, size_t count);

    /** Calculate @ minmax { abs(src) }
     *
     * @param src source vector
     * @param count number of elements
     * @param min pointer to store absolute minimum value index
     * @param max pointer to store absolute maximum value index
     */
    extern void (* abs_minmax_index)(const float *src, size_t count, size_t *min, size_t *max);

}

#endif /* DSP_COMMON_SEARCH_H_ */
