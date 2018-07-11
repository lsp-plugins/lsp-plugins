/*
 * fade.h
 *
 *  Created on: 24 марта 2016 г.
 *      Author: sadko
 */

#ifndef CORE_FADE_H_
#define CORE_FADE_H_

#include <core/types.h>

namespace lsp
{
    /** Fade-in (with range check)
     *
     * @param dst destination buffer
     * @param src source buffer
     * @param fade_len length of fade (in elements)
     * @param buf_len length of the buffer
     */
    void fade_in(float *dst, const float *src, size_t fade_len, size_t buf_len);

    /** Fade-out (with range check)
     *
     * @param dst destination buffer
     * @param src source buffer
     * @param fade_len length of fade (in elements)
     * @param buf_len length of the buffer
     */
    void fade_out(float *dst, const float *src, size_t fade_len, size_t buf_len);
}

#endif /* CORE_FADE_H_ */
