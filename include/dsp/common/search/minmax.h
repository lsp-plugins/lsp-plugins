/*
 * minmax.h
 *
 *  Created on: 4 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_SEARCH_MINMAX_H_
#define DSP_COMMON_SEARCH_MINMAX_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

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
}

#endif /* DSP_COMMON_SEARCH_MINMAX_H_ */
