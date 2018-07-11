/*
 * types.h
 *
 *  Created on: 26 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _CORE_TYPES_H_
#define _CORE_TYPES_H_

#include <sys/types.h>
#include <stddef.h>

#ifdef __linux__
    #include <linux/limits.h>
#endif /* __linux__ */

#define __ASM_EMIT(code)                    code "\n\t"

// Some physical constants
#define MAX_SAMPLE_RATE                     192000              /* Maximum supported sample rate [samples / s]      */
#define DECIBEL_INF_THRESH                  1.19209289551e-40   /* Float value that is equal -INF dB                */
#define AIR_ADIABATIC_INDEX                 1.4                 /* Adiabatic index for the Air                      */
#define AIR_MOLAR_MASS                      28.98               /* Molar mass of the air [g/mol]                    */
#define GAS_CONSTANT                        8.3144598           /* Gas constant [ j/(mol * K) }                     */
#define TEMP_ABS_ZERO                       -273.15             /* Temperature of the absolute zero [ C ]           */
#define MAX_SOUND_SPEED                     500                 /* Maximum speed of the sound [ m/s ]               */
#define CMP_TOLERANCE                       1e-5                /* Float comparison tolerance                       */

// Special symbols
#define STR_A_UMLAUT_SMALL                  "\x84"
#define STR_A_UMLAUT_LARGE                  "\x8e"
#define STR_U_UMLAUT_SMALL                  "\x81"
#define STR_U_UMLAUT_LARGE                  "\x9a"
#define STR_O_UMLAUT_SMALL                  "\x94"
#define STR_O_UMLAUT_LARGE                  "\x99"
#define STR_ESZETT                          "\xe1"

namespace lsp
{
    typedef __uint32_t      uint32_t;

    // Mesh port structure
    typedef struct mesh_t
    {
        size_t          nBuffers;
        size_t          nItems;
        float          *pvData[];
    } mesh_t;
}

#endif /* TYPES_H_ */
