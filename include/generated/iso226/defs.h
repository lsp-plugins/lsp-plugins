/*
 * defs.h
 *
 *  Created on: 29 июн. 2020 г.
 *      Author: sadko
 */

#ifndef GENERATED_ISO226_DEFS_H_
#define GENERATED_ISO226_DEFS_H_

#include <core/types.h>

namespace lsp
{
    typedef struct freq_curve_t
    {
        float   fmin;       // The minimum frequency per curve
        float   fmax;       // The maximum frequency per curve
        float   amin;       // The minimum amplitude per curve
        float   amax;       // The maximum amplitude per curve
        size_t  hdots;      // Number of dots per each curve
        size_t  curves;     // The overall number of curves

        const float * const data[]; // The array of curve data
    } freq_curve_t;
}

#endif /* GENERATED_ISO226_DEFS_H_ */
