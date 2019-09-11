/*
 * defs.cpp
 *
 *  Created on: 29 авг. 2019 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/files/java/defs.h>
#include <core/files/java/Object.h>

namespace lsp
{
    namespace java
    {
        ftype_t decode_primitive_type(size_t tcode)
        {
            switch (tcode)
            {
                #define XDEC(a, b) case a: return b;
                XDEC(PTC_BYTE, JFT_BYTE)
                XDEC(PTC_CHAR, JFT_CHAR)
                XDEC(PTC_DOUBLE, JFT_DOUBLE)
                XDEC(PTC_FLOAT, JFT_FLOAT)
                XDEC(PTC_INTEGER, JFT_INTEGER)
                XDEC(PTC_LONG, JFT_LONG)
                XDEC(PTC_SHORT, JFT_SHORT)
                XDEC(PTC_BOOL, JFT_BOOL)
                XDEC(PTC_ARRAY, JFT_ARRAY)
                XDEC(PTC_OBJECT, JFT_OBJECT)
                #undef XDEC
                default: break;
            }

            return JFT_UNKNOWN;
        }

        const char *primitive_type_name(ftype_t tcode)
        {
            switch (tcode)
            {
                #define XDEC(a, b) case a: return #b;
                XDEC(JFT_BYTE, byte)
                XDEC(JFT_CHAR, char)
                XDEC(JFT_DOUBLE, double)
                XDEC(JFT_FLOAT, float)
                XDEC(JFT_INTEGER, int)
                XDEC(JFT_LONG, long)
                XDEC(JFT_SHORT, short)
                XDEC(JFT_BOOL, bool)
                #undef XDEC
                default: break;
            }

            return NULL;
        }
    }
}

