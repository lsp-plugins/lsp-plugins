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

            protected:
                enum pmode_t
                {
                    WRITE_ROOT,
                    WRITE_ARRAY,
                    WRITE_OBJECT
                };

                enum serialize_flags_t
                {
                    SF_PROPERTY     = 1 << 0,
                    SF_VALUE        = 1 << 1,
                    SF_COMMA        = 1 << 2,
                    SF_CONTENT      = 1 << 3
                };

                typedef struct state_t
                {
                    pmode_t         mode;
                    size_t          flags;
                    size_t          ident;
                } state_t;

            protected:
                io::IOutSequence   *pOut;
                size_t              nWFlags;
                state_t             sState;
                cstorage<state_t>   sStack;
                serial_flags_t      sSettings;

            protected:
                inline status_t     push_state(pmode_t state);
                status_t            pop_state();
                inline void         copy_settings(const serial_flags_t *flags);

                inline char         hex(int x);
                status_t            write_raw(const char *buf, int len);
                status_t            write_literal(const LSPString *value);
                inline status_t     emit_comma();
                inline status_t     emit_separator();

            public:
                explicit Serializer();
                virtual ~Serializer();

            public:
                /**
                 * Open parser
                 * @param path UTF-8 path to the file
                 * @param settings serialization flags
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    open(const char *path, const serial_flags_t *settings, const char *charset = NULL);

                /**
                 * Open parser
                 * @param path string representation of path to the file
                 * @param settings serialization flags
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    open(const LSPString *path, const serial_flags_t *settings, const char *charset = NULL);

                /**
                 * Open parser
                 * @param path path to the file
                 * @param settings serialization flags
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    open(const io::Path *path, const serial_flags_t *settings, const char *charset = NULL);

                /**
                 * Wrap string with parser
                 * @param str string to wrap
                 * @param settings serialization flags
                 * @return status of operation
                 */
                status_t    wrap(LSPString *str, const serial_flags_t *settings);

                /**
                 * Wrap input sequence with parser
                 * @param seq sequence to use for reads
                 * @param settings serialization flags
                 * @param flags wrapping flags
                 * @return status of operation
                 */
                status_t    wrap(io::IOutSequence *seq, const serial_flags_t *settings, size_t flags = WRAP_NONE);

                /**
                 * Wrap input stream with parser
                 * @param os output stream
                 * @param version JSON version
                 * @param settings serialization flags
                 * @param flags wrapping flags
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    wrap(io::IOutStream *os, const serial_flags_t *settings, size_t flags = WRAP_NONE, const char *charset = NULL);

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
                 * Write comma
                 * @return status of operation
                 */
                status_t    write_comma();

                /**
                 * Write new line
                 * @return status of operation
                 */
                status_t    writeln();

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
                 * Write double
                 * @param value double value
                 * @return status of operation
                 */
                status_t    write_double(double value);

                /**
                 * Write double
                 * @param value double value
                 * @param fmt format specifier (same as for sprintf)
                 * @return status of operation
                 */
                status_t    write_double(double value, const char *fmt);

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
                status_t    write_string(const char *value, const char *charset);

                /**
                 * Write string in UTF-8
                 * @param value value to write, NULL will be interpreted as NULL value
                 * @return status of operation
                 */
                status_t    write_string(const char *value);

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
                status_t    write_comment(const char *value, const char *charset);

                /**
                 * Write comment in UTF-8 encoding
                 * @param value comment to write
                 * @return status of operation
                 */
                status_t    write_comment(const char *value);

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
                status_t    write_property(const char *name, const char *charset);

                /**
                 * Write property in UTF-8 encoding
                 * @param name property name
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    write_property(const char *name);

                /**
                 * Write property
                 * @param name property name
                 * @return status of operation
                 */
                status_t    write_property(const LSPString *name);

                /**
                 * Write beginning of the object
                 * @return status of operation
                 */
                status_t    start_object();

                /**
                 * Write the end of the object
                 * @return status of operation
                 */
                status_t    end_object();

                /**
                 * Write beginning of the array
                 * @return status of operation
                 */
                status_t    start_array();

                /**
                 * Write the end of the array
                 * @return status of operation
                 */
                status_t    end_array();

            public:
                /**
                 * Get multiline flag
                 * @return multiline flag
                 */
                inline bool get_multiline() const { return sSettings.multiline; }

                /**
                 * Get multiline flag
                 * @return multiline flag
                 */
                inline bool set_multiline(bool multiline)
                {
                    bool prev = sSettings.multiline;
                    sSettings.multiline = multiline;
                    return prev;
                }
        };
    
    } /* namespace json */
} /* namespace lsp */

#endif /* CORE_FILES_JSON_SERIALIZER_H_ */
