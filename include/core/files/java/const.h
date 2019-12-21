/*
 * const.h
 *
 *  Created on: 31 авг. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_JAVA_CONST_H_
#define CORE_FILES_JAVA_CONST_H_

#include <common/types.h>
#include <core/types.h>

namespace lsp
{
    namespace java
    {
        typedef float               float_t;
        typedef double              double_t;
        typedef uint8_t             bool_t;
        typedef lsp_utf16_t         char_t;
        typedef int8_t              byte_t;
        typedef int16_t             short_t;
        typedef int32_t             int_t;
        typedef int64_t             long_t;

        enum stream_token_t
        {
            JST_NULL,               ///< Null object reference.
            JST_REFERENCE,          ///< Reference to an object already written into the stream.
            JST_CLASS_DESC,         ///< new Class Descriptor.
            JST_OBJECT,             ///< Object.
            JST_STRING,             ///< String.
            JST_ARRAY,              ///< Array.
            JST_CLASS,              ///< Reference to Class.
            JST_BLOCK_DATA,         ///< Block of optional data. Byte following tag indicates number of bytes in this block data.
            JST_END_BLOCK_DATA,     ///< End of block of optional data.
            JST_RESET,              ///< Reset stream context. All handles written into stream are reset.
            JST_EXCEPTION,          ///< Exception during write.
            JST_PROXY_CLASS_DESC,   ///< new Proxy Class Descriptor.
            JST_ENUM,               ///< new Enum constant, since java 1.5

            JST_UNDEFINED  = -1
        };

        enum cflags_t
        {
            JCF_PROXY           = 1 << 0,
            JCF_WRITE_METHOD    = 1 << 1,
            JCF_BLOCK_DATA      = 1 << 2,
            JCF_EXTERNALIZABLE  = 1 << 3,
            JCF_SERIALIZABLE    = 1 << 4,
            JCF_ENUM            = 1 << 5
        };

        enum ftype_t
        {
            JFT_BYTE,
            JFT_CHAR,
            JFT_DOUBLE,
            JFT_FLOAT,
            JFT_INTEGER,
            JFT_LONG,
            JFT_SHORT,
            JFT_BOOL,
            JFT_ARRAY,
            JFT_OBJECT,

            JFT_TOTAL,
            JFT_UNKNOWN = -1
        };

        /**
         * Determine size of primitive type
         * @param type of primitive type
         * @return size of primitive type
         */
        size_t      size_of(ftype_t type);

        /**
         * Compute aligned offset of the typed data
         * @param offset the current offset
         * @param type data type
         * @return aligned offset, greater or equal than passed value
         */
        size_t      aligned_offset(size_t offset, ftype_t type);

        /**
         * Check that type is primitive
         * @param type type
         * @return true if type is primitive
         */
        bool        is_primitive(ftype_t type);

        /**
         * Check that type is reference
         * @param type type
         * @return true if type is reference
         */
        bool        is_reference(ftype_t type);
    }
}

#endif /* INCLUDE_CORE_FILES_JAVA_CONST_H_ */
