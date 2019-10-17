/*
 * Serializer.h
 *
 *  Created on: 17 окт. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_JSON_SERIALIZER_H_
#define CORE_FILES_JSON_SERIALIZER_H_

#include <common/types.h>
#include <core/io/IOutStream.h>
#include <core/io/IOutSequence.h>
#include <core/io/Path.h>
#include <core/files/json/token.h>
#include <data/cstorage.h>

namespace lsp
{
    namespace json
    {
        
        class Serializer
        {
            private:
                Serializer & operator = (const Serializer &);

                enum pmode_t
                {
                    WRITE_ROOT,
                    WRITE_ARRAY,
                    WRITE_OBJECT
                };

                enum serialize_flags_t
                {
                    SF_COMMA        = 1 << 0,
                    SF_PROPERTY     = 1 << 1,
                    SF_VALUE        = 1 << 2,

                    SF_ARRAY_ALL    = SF_COMMA | SF_VALUE,
                    SF_OBJECT_ALL   = SF_COMMA | SF_PROPERTY | SF_VALUE
                };

                typedef struct state_t
                {
                    pmode_t         mode;
                    size_t          flags;
                } state_t;

            protected:
                inline void         init_serial_flags(const serial_flags_t *flags);

            public:
                explicit Serializer();
                virtual ~Serializer();

            public:
                /**
                 * Open parser
                 * @param path UTF-8 path to the file
                 * @param flags serialization flags
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    open(const char *path, const serial_flags_t *flags, const char *charset = NULL);

                /**
                 * Open parser
                 * @param path string representation of path to the file
                 * @param flags serialization flags
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    open(const LSPString *path, const serial_flags_t *flags, const char *charset = NULL);

                /**
                 * Open parser
                 * @param path path to the file
                 * @param flags serialization flags
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    open(const io::Path *path, const serial_flags_t *flags, const char *charset = NULL);

                /**
                 * Wrap string with parser
                 * @param str string to wrap
                 * @param flags serialization flags
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    wrap(const char *str, const serial_flags_t *flags, const char *charset = NULL);

                /**
                 * Wrap string with parser
                 * @param str string to wrap
                 * @param flags serialization flags
                 * @return status of operation
                 */
                status_t    wrap(const LSPString *str, const serial_flags_t *flags);

                /**
                 * Wrap input sequence with parser
                 * @param seq sequence to use for reads
                 * @param flags serialization flags
                 * @return status of operation
                 */
                status_t    wrap(io::IOutSequence *seq, const serial_flags_t *flags, size_t flags = WRAP_NONE);

                /**
                 * Wrap input stream with parser
                 * @param is input stream
                 * @param version JSON version
                 * @param flags serialization flags
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    wrap(io::IOutStream *is, const serial_flags_t *flags, size_t flags = WRAP_NONE, const char *charset = NULL);

                /**
                 * Close parser
                 * @return status of operation
                 */
                status_t    close();

            public:

                /**
                 * Write JSON event to output
                 * @param event event to write
                 * @return status of operation
                 */
                status_t    write(const event_t *event);

                /**
                 * Write integer value
                 * @param value value to write
                 * @return status of operation
                 */
                status_t    write_int(ssize_t value);

                /**
                 * Write hexadecimal value
                 * @param value value to write
                 * @return status of operation
                 */
                status_t    write_hex(ssize_t value);

                /**
                 * Write boolean value
                 * @param value value to write
                 * @return status of operation
                 */
                status_t    write_bool(bool value);

                /**
                 * Write string value
                 * @param value value to write, NULL will be interpreted as NULL value
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    write_string(const char *value, const char *charset=NULL);

                /**
                 * Write string value
                 * @param value value to write, NULL will be interpreted as NULL value
                 * @return status of operation
                 */
                status_t    write_string(const LSPString *value);

                /**
                 * Write null value
                 * @return status of operation
                 */
                status_t    write_null();

                /**
                 * Write comment
                 * @param value comment to write
                 * @return status of operation
                 */
                status_t    write_comment(const char *value, const char *charset=NULL);

                /**
                 * Write comment
                 * @param value comment to write
                 * @return status of operation
                 */
                status_t    write_comment(const LSPString *value);

                /**
                 * Write property
                 * @param name property name
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    write_property(const char *name, const char *charset=NULL);

                /**
                 * Write property
                 * @param name property name
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    write_property(const LSPString *name, const char *charset=NULL);

                /**
                 * Write beginning of the object
                 * @return status of operation
                 */
                status_t    begin_object();

                /**
                 * Write the end of the object
                 * @return status of operation
                 */
                status_t    end_object();

                /**
                 * Write beginning of the array
                 * @return status of operation
                 */
                status_t    begin_array();

                /**
                 * Write the end of the array
                 * @return status of operation
                 */
                status_t    end_array();
        };
    
    } /* namespace json */
} /* namespace lsp */

#endif /* CORE_FILES_JSON_SERIALIZER_H_ */
