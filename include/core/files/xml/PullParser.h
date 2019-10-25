/*
 * PullParser.h
 *
 *  Created on: 24 окт. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_XML_PULLPARSER_H_
#define CORE_FILES_XML_PULLPARSER_H_

#include <common/types.h>
#include <core/io/IInStream.h>
#include <core/io/IInSequence.h>
#include <core/io/Path.h>
#include <core/files/xml/const.h>

namespace lsp
{
    namespace xml
    {
        class PullParser
        {
            protected:
                enum parse_state_t
                {
                    PS_READ_HEADER,
                    PS_READ_MISC,
                    PS_ELEMENT,
                    PS_ELEMENT_DATA,
                    PS_END_DOCUMENT
                };

                enum xml_flags_t
                {
                    XF_STANDALONE   = 1 << 0,
                    XF_CHARSET      = 1 << 1,
                    XF_VERSION      = 1 << 2
                };

            protected:
                io::IInSequence    *pIn;
                size_t              nWFlags;
                xml_token_t         nToken;
                parse_state_t       nState;
                xml_version_t       enVersion;

                lsp_wchar_t        *vBuffer;
                ssize_t             nBufMark;
                size_t              nBufTail;
                size_t              nBufSize;
                size_t              nBufCap;

                // Misc data
                size_t              nFlags;         // Field presence
                LSPString           sVersion;       // Document version
                LSPString           sCharset;       // Document character set
                LSPString           sName;          // Property/Tag name
                LSPString           sValue;         // Property value

            protected:
                status_t            fill_buf();
                lsp_swchar_t        get_char();
                lsp_swchar_t        lookup_char();
                inline lsp_swchar_t lookup_next_char();
                status_t            lookup(const char *text);
                status_t            lookup_nocase(const char *text);

                inline void         next_char();
                inline void         mark_buf();
                inline void         commit_buf();
                inline void         rollback_buf();

                bool                is_valid_char(lsp_swchar_t c);
                bool                is_name_start(lsp_swchar_t c);
                bool                is_name_char(lsp_swchar_t c);
                bool                is_whitespace(lsp_swchar_t c);
                bool                is_restricted_char(lsp_swchar_t c);

                bool                skip_spaces();

                status_t            parse_version(const LSPString *version);
                status_t            parse_encoding(const LSPString *encoding);
                status_t            parse_standalone(const LSPString *standalone);

                status_t            read_character(lsp_wchar_t ch);
                status_t            read_name(LSPString *name);
                status_t            read_value(LSPString *value);
                status_t            read_attribute(LSPString *name, LSPString *value);

                status_t            read_header();
                status_t            read_misc();
                status_t            read_comment();
                status_t            read_processing_instruction();

            public:
                explicit PullParser();
                virtual ~PullParser();

            public:
                /**
                 * Open parser
                 * @param path UTF-8 path to the file
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    open(const char *path, const char *charset = NULL);

                /**
                 * Open parser
                 * @param path string representation of path to the file
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    open(const LSPString *path, const char *charset = NULL);

                /**
                 * Open parser
                 * @param path path to the file
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    open(const io::Path *path, const char *charset = NULL);

                /**
                 * Wrap string with parser
                 * @param str string to wrap
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    wrap(const char *str, const char *charset = NULL);

                /**
                 * Wrap string with parser
                 * @param str string to wrap
                 * @return status of operation
                 */
                status_t    wrap(const LSPString *str);

                /**
                 * Wrap input sequence with parser
                 * @param seq sequence to use for reads
                 * @return status of operation
                 */
                status_t    wrap(io::IInSequence *seq, size_t flags = WRAP_NONE);

                /**
                 * Wrap input stream with parser
                 * @param is input stream
                 * @param flags wrap flags
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    wrap(io::IInStream *is, size_t flags = WRAP_NONE, const char *charset = NULL);

                /**
                 * Close parser
                 * @return status of operation
                 */
                status_t    close();

            public:
                /**
                 * Read next element
                 * @return XT_ element code or negative status of operation
                 */
                status_t    read_next();

                /**
                 * Get current element
                 * @return XT_ element code or negative status of operation
                 */
                status_t    get_current();
        };
    
    } /* namespace xml */
} /* namespace lsp */

#endif /* CORE_FILES_XML_PULLPARSER_H_ */
