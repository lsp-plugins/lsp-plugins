/*
 * StreamDataReader.h
 *
 *  Created on: 28 авг. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_JAVA_STREAMDATAREADER_H_
#define CORE_FILES_JAVA_STREAMDATAREADER_H_

#include <common/types.h>
#include <core/status.h>
#include <core/LSPString.h>
#include <core/io/Path.h>
#include <core/io/IInStream.h>

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
        };

        class StreamDataReader
        {
            protected:
                io::IInStream      *pIS;
                size_t              nFlags;
                ssize_t             nToken;
                ssize_t             enToken;
                ssize_t             nVersion;
                size_t              nHandle;

            private:
                status_t            set_error(status_t res);

            protected:
                status_t    initial_read(io::IInStream *is);
                status_t    lookup_token();

            public:
                explicit StreamDataReader();
                virtual ~StreamDataReader();

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
                 * Get current token
                 * @param force force token signature read from underlying data stream
                 * @return token type or negative error code, may be:
                 *   STATUS_UNSPECIFIED - if token was not read
                 */
                ssize_t get_token(bool force = true);

                /**
                 * Get current object stream version
                 * @return current object stream version
                 */
                inline ssize_t version() const { return nVersion; }

                status_t read_simple(uint8_t *dst);
                status_t read_simple(int8_t *dst);
                status_t read_simple(uint16_t *dst);
                status_t read_simple(int16_t *dst);
                status_t read_simple(uint32_t *dst);
                status_t read_simple(int32_t *dst);
                status_t read_simple(uint64_t *dst);
                status_t read_simple(int64_t *dst);
                status_t read_simple(float *dst);
                status_t read_simple(double *dst);

                /**
                 * Read string object, may be null for skipping
                 * @param handle current handle number
                 * @param dst pointer to string to store result, can be NULL for skipping
                 * @return status of operation
                 */
                status_t read_string(size_t *handle, LSPString *dst);

                /**
                 * Read block data
                 * @param dst pointer to store block data, should be free()'d after use, can be NULL for skipping
                 * @param size block size in bytes, can be NULL
                 * @return status of operation
                 */
                status_t read_block(void **dst, size_t *size);

                /**
                 * Read reset primitive
                 * @return status of operation
                 */
                status_t read_reset();

                /**
                 * Read null primitive
                 * @return status of operation
                 */
                status_t read_null();
        };
    
    } /* namespace java */
} /* namespace lsp */

#endif /* CORE_FILES_JAVA_STREAMDATAREADER_H_ */
