/*
 * headers.h
 *
 *  Created on: 28 авг. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_JAVA_HEADERS_H_
#define CORE_FILES_JAVA_HEADERS_H_

#include <common/types.h>

#define JAVA_STREAM_MAGIC               0xACED
#define JAVA_BASE_WIRE_HANDLE           0x7e0000
#define JAVA_MAX_BLOCK_SIZE             1024

namespace lsp
{
    namespace java
    {
        enum obj_stream_tag_t
        {
            TC_NULL             = 0x70, ///< Null object reference.
            TC_REFERENCE        = 0x71, ///< Reference to an object already written into the stream.
            TC_CLASSDESC        = 0x72, ///< new Class Descriptor.
            TC_OBJECT           = 0x73, ///< new Object.
            TC_STRING           = 0x74, ///< new String.
            TC_ARRAY            = 0x75, ///< new Array.
            TC_CLASS            = 0x76, ///< Reference to Class.
            TC_BLOCKDATA        = 0x77, ///< Block of optional data. Byte following tag indicates number of bytes in this block data.
            TC_ENDBLOCKDATA     = 0x78, ///< End of optional block data blocks for an object.
            TC_RESET            = 0x79, ///< Reset stream context. All handles written into stream are reset.
            TC_BLOCKDATALONG    = 0x7A, ///< long Block data. The long following the tag indicates the number of bytes in this block data.
            TC_EXCEPTION        = 0x7B, ///< Exception during write.
            TC_LONGSTRING       = 0x7C, ///< Long string.
            TC_PROXYCLASSDESC   = 0x7D, ///< new Proxy Class Descriptor.
            TC_ENUM             = 0x7E, ///< new Enum constant, since java 1.5

            TC_BASE             = TC_NULL, ///< First tag value
            TC_MAX              = TC_ENUM ///< Last tag value.
        };

        extern const char *CLASSNAME_STRING;

        #pragma pack(push, 1)
            typedef struct obj_stream_hdr_t
            {
                uint16_t    magic;
                uint16_t    version;
            } obj_stream_hdr_t;
        #pragma pack(pop)
    }
}

#endif /* CORE_FILES_JAVA_HEADERS_H_ */
