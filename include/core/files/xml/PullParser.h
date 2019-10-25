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
#include <data/cvector.h>

namespace lsp
{
    namespace xml
    {
        class PullParser
        {
            protected:
                enum parse_state_t
                {
                    PS_READ_MISC,
                    PS_READ_ELEMENTS,
                    PS_READ_ATTRIBUTES,
                    PS_READ_ELEMENT_DATA,
                    PS_END_DOCUMENT
                };

                enum xml_flags_t
                {
                    XF_STANDALONE   = 1 << 0,
                    XF_CHARSET      = 1 << 1,
                    XF_VERSION      = 1 << 2,
                    XF_ROOT         = 1 << 3,
                    XF_HEADER       = 1 << 4
                };

            protected:
                io::IInSequence    *pIn;
                size_t              nWFlags;
                status_t            nToken;
                parse_state_t       nState;
                xml_version_t       enVersion;

                lsp_swchar_t        cLast;

                // Misc data
                size_t              nFlags;         // Field presence
                LSPString           sVersion;       // Document version
                LSPString           sCharset;       // Document character set
                LSPString           sName;          // Property/Tag name
                LSPString           sValue;         // Property value
                cvector<LSPString>  vTags;

            protected:
                inline lsp_swchar_t getch();
                inline void         ungetch(lsp_swchar_t c);

                bool                is_valid_char(lsp_swchar_t c);
                bool                is_name_start(lsp_swchar_t c);
                bool                is_name_char(lsp_swchar_t c);
                bool                is_whitespace(lsp_swchar_t c);
                bool                is_restricted_char(lsp_swchar_t c);

                bool                skip_spaces();

                status_t            parse_version(const LSPString *version);
                status_t            parse_encoding(const LSPString *encoding);
                status_t            parse_standalone(const LSPString *standalone);

                status_t            preprocess_value(LSPString *value);

                status_t            read_text(const char *text);
                status_t            read_start_document();
                status_t            read_end_document();
                status_t            read_name(LSPString *name);
                status_t            read_value(LSPString *value);
                status_t            read_attribute(LSPString *name, LSPString *value);
                status_t            read_header();
                status_t            read_misc();
                status_t            read_comment();
                status_t            read_doctype();
                status_t            read_tag_open();
                status_t            read_tag_close(bool copy);
                status_t            read_tag_attribute();
                status_t            read_tag_content();
                status_t            read_processing_instruction();
                status_t            read_cdata();

                status_t            read_token();

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
