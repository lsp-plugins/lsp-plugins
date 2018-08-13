/*
 * const.h
 *
 *  Created on: 13 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_CONST_H_
#define DSP_COMMON_CONST_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

#define DSP_F32VEC4(name, v)            extern const float name[]
#define DSP_U32VEC4(name, v)            extern const uint32_t name[]

#define DSP_F32VECX4(name, a, b, c, d)  extern const float name[]
#define DSP_U32VECX4(name, a, b, c, d)  extern const uint32_t name[]

#include <dsp/common/const/const16.h>

#undef DSP_U32VECX4
#undef DSP_F32VECX4

#undef DSP_U32VEC4
#undef DSP_F32VEC4

#endif /* DSP_COMMON_CONST_H_ */
