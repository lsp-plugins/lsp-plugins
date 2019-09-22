/*
 * headers.h
 *
 *  Created on: 28 авг. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_JAVA_HEADERS_H_
#define CORE_FILES_JAVA_HEADERS_H_

#include <common/types.h>
#include <core/files/java/const.h>

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

        enum obj_stream_const_t
        {
            /**
             * Bit mask for ObjectStreamClass flag. Indicates a Serializable class
             * defines its own writeObject method.
             */
            SC_WRITE_METHOD = 0x01,

            /**
             * Bit mask for ObjectStreamClass flag. Indicates class is Serializable.
             */
            SC_SERIALIZABLE = 0x02,

            /**
             * Bit mask for ObjectStreamClass flag. Indicates class is Externalizable.
             */
            SC_EXTERNALIZABLE = 0x04,

            /**
             * Bit mask for ObjectStreamClass flag. Indicates Externalizable data
             * written in Block Data mode.
             * Added for PROTOCOL_VERSION_2.
             */
            SC_BLOCK_DATA = 0x08,

            /**
             * Bit mask for ObjectStreamClass flag. Indicates class is an enum type.
             * @since 1.5
             */
            SC_ENUM = 0x10
        };

        enum prim_type_t
        {
            PTC_BYTE    = 'B',      ///< byte
            PTC_CHAR    = 'C',      ///< char
            PTC_DOUBLE  = 'D',      ///< double
            PTC_FLOAT   = 'F',      ///< float
            PTC_INTEGER = 'I',      ///< integer
            PTC_LONG    = 'J',      ///< long
            PTC_SHORT   = 'S',      ///< short
            PTC_BOOL    = 'Z',      ///< boolean
            PTC_ARRAY   = '[',      ///< array
            PTC_OBJECT  = 'L'       ///< object
        };


        class Object;

        #pragma pack(push, 1)
            typedef struct obj_stream_hdr_t
            {
                uint16_t    magic;
                uint16_t    version;
            } obj_stream_hdr_t;

            typedef union prim_ptr_t
            {
                int8_t         *p_byte      __lsp_packed;
                uint8_t        *p_ubyte     __lsp_packed;
                int16_t        *p_short     __lsp_packed;
                uint16_t       *p_ushort    __lsp_packed;
                int32_t        *p_int       __lsp_packed;
                uint32_t       *p_uint      __lsp_packed;
                int64_t        *p_long      __lsp_packed;
                uint64_t       *p_ulong     __lsp_packed;
                lsp_utf16_t    *p_char      __lsp_packed;
                double_t       *p_double    __lsp_packed;
                float_t        *p_float     __lsp_packed;
                bool_t         *p_bool      __lsp_packed;
                Object        **p_object    __lsp_packed;
                void           *p_void      __lsp_packed;
            } prim_ptr_t;
        #pragma pack(pop)

        struct obj_ptr_t
        {
            private:
                Object    **obj;

            public:
                inline obj_ptr_t(Object **p) { obj   = p; }
                inline obj_ptr_t & operator = (Object **p)  { obj   = p; return *this; }
                inline operator Object **()     { return obj; }


                template <typename type_t>
                    inline obj_ptr_t(type_t **p)
                {
                    union { Object **o; type_t **t; } x;
                    x.t     = p;
                    obj     = x.o;
                }

                template <typename type_t>
                    inline obj_ptr_t & operator = (type_t **p)
                    {
                        union { Object **o; type_t **t; } x;
                        x.t     = p;
                        obj     = x.o;
                        return *this;
                    }

                template <typename type_t>
                    inline operator type_t **()
                    {
                        union { Object **o; type_t **t; } x;
                        x.o     = obj;
                        return x.t;
                    }
        };

        /**
         * Decode primitive type
         * @param primitive type
         * @return decoded result
         */
        ftype_t     decode_primitive_type(size_t tcode);

        /**
         * Get primitive type name
         * @param tcode primitive type code
         * @return primitive type name or NULL if type is non-primitive
         */
        const char *primitive_type_name(ftype_t tcode);
    }
}

#endif /* CORE_FILES_JAVA_HEADERS_H_ */
