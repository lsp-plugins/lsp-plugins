/*
 * flags.h
 *
 *  Created on: 13 июл. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_CONFIG_FLAGS_H_
#define CORE_FILES_CONFIG_FLAGS_H_

namespace lsp
{
    namespace config
    {
        enum serialize_flags_t
        {
            SF_NONE         = 0,

            SF_TYPE_NATIVE  = 0x00,
            SF_TYPE_I32     = 0x01,
            SF_TYPE_U32     = 0x02,
            SF_TYPE_I64     = 0x03,
            SF_TYPE_U64     = 0x04,
            SF_TYPE_F32     = 0x05,
            SF_TYPE_F64     = 0x06,
            SF_TYPE_STR     = 0x07,
            SF_TYPE_BLOB    = 0x08,

            SF_TYPE_MASK    = 0x0f,

            SF_QUOTED       = 1 << 4,
        };
    }
}


#endif /* CORE_FILES_CONFIG_FLAGS_H_ */
