/*
 * ObjectStream.h
 *
 *  Created on: 28 авг. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_JAVA_OBJECTSTREAM_H_
#define CORE_FILES_JAVA_OBJECTSTREAM_H_

#include <common/types.h>
#include <core/status.h>
#include <core/LSPString.h>
#include <core/io/Path.h>
#include <core/io/IInStream.h>

#include <core/files/java/Object.h>
#include <core/files/java/String.h>
#include <core/files/java/ClassDescriptor.h>
#include <core/files/java/Handles.h>

namespace lsp
{
    namespace java
    {
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
            JST_RESET,              ///< Reset stream context. All handles written into stream are reset.
            JST_EXCEPTION,          ///< Exception during write.
            JST_PROXY_CLASS_DESC,   ///< new Proxy Class Descriptor.
            JST_ENUM,               ///< new Enum constant, since java 1.5

            JST_UNDEFINED  = -1
        };

        class ObjectStream
        {
            protected:
                typedef struct block_t
                {
                    uint8_t            *data;
                    size_t              size;
                    size_t              offset;
                    size_t              unread;
                    bool                enabled;
                } block_t;

            protected:
                io::IInStream      *pIS;
                size_t              nFlags;
                ssize_t             nToken;
                stream_token_t      enToken;
                size_t              nDepth;
                ssize_t             nVersion;
                Handles            *pHandles;
                block_t             sBlock;

            private:
                status_t            do_close();
                status_t            set_block_mode(bool enabled, bool *old = NULL);

            protected:
                status_t    initial_read(io::IInStream *is);
                ssize_t     get_token();
                status_t    lookup_token();

                status_t    fill_block();
                status_t    read_fully(void *dst, size_t count);

                status_t    read_handle(Object **dst);
                status_t    read_string_internal(String **dst);
                status_t    handle_reset();
                status_t    read_null();
                status_t    read_class_descriptor(ClassDescriptor **dst);
                status_t    read_utf(LSPString *dst, size_t len);

            public:
                explicit ObjectStream(Handles *handles);
                virtual ~ObjectStream();

            public:
                /**
                 * Open stream reader
                 * @param file location of the file in UTF-8 encoding
                 * @return status of operation
                 */
                status_t    open(const char *file);

                /**
                 * Open stream reader
                 * @param file location of the file
                 * @return status of operation
                 */
                status_t    open(const LSPString *file);

                /**
                 * Open stream reader
                 * @param file location of the file
                 * @return status of operation
                 */
                status_t    open(const io::Path *file);

                /**
                 * Wrap memory buffer with stream reader
                 * @param buf data contents
                 * @param size size of data
                 * @return status of operation
                 */
                status_t    wrap(const void *buf, size_t count);

                /**
                 * Wrap memory buffer with stream reader
                 * @param buf data contents
                 * @param size size of data
                 * @param drop drop operation on close()
                 * @return status of operation
                 */
                status_t    wrap(void *buf, size_t count, lsp_memdrop_t drop);

                /**
                 * Wrap input stream
                 * @param is input stream
                 * @param flags actions on the input stream after close()
                 * @return status of operation
                 */
                status_t    wrap(io::IInStream *is, size_t flags);

                /**
                 * Close data reader
                 * @return status of operation
                 */
                status_t    close();

                /**
                 * Get current object stream version
                 * @return current object stream version
                 */
                inline ssize_t version() const { return nVersion; }

                status_t    current_token();

                status_t    read_byte(uint8_t *dst);
                status_t    read_byte(int8_t *dst);
                status_t    read_short(uint16_t *dst);
                status_t    read_short(int16_t *dst);
                status_t    read_int(uint32_t *dst);
                status_t    read_int(int32_t *dst);
                status_t    read_long(uint64_t *dst);
                status_t    read_long(int64_t *dst);
                status_t    read_float(float *dst);
                status_t    read_double(double *dst);

                status_t    read_utf(LSPString *dst);

                status_t    read_string(String **dst);
                status_t    read_object(Object **dst);
        };
    
    } /* namespace java */
} /* namespace lsp */

#endif /* CORE_FILES_JAVA_OBJECTSTREAM_H_ */
