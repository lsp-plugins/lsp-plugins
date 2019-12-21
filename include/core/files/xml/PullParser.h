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
                    PS_READ_REFERENCE,
                    PS_READ_CHARACTERS,
                    PS_READ_SQ_ATTRIBUTE,
                    PS_READ_DQ_ATTRIBUTE,
                    PS_END_DOCUMENT
                };

                enum xml_flags_t
                {
                    XF_STANDALONE   = 1 << 0,
                    XF_ENCODING     = 1 << 1,
                    XF_VERSION      = 1 << 2,
                    XF_ROOT         = 1 << 3,
                    XF_HEADER       = 1 << 4,
                    XF_DOCTYPE      = 1 << 5,
                    XF_DOCTYPE_SYS  = 1 << 6,
                    XF_DOCTYPE_PUB  = 1 << 7,
                };

            protected:
                io::IInSequence    *pIn;
                size_t              nWFlags;
                status_t            nToken;
                parse_state_t       nState;
                xml_version_t       enVersion;

                lsp_swchar_t        vUngetch[4];
                size_t              nUngetch;
                parse_state_t       vStates[4];
                size_t              nStates;

                // Misc data
                size_t              nFlags;         // Field presence
                LSPString           sVersion;       // Document version
                LSPString           sEncoding;      // Document encoding
                LSPString           sName;          // Property/Tag name
                LSPString           sValue;         // Property value
                LSPString           sRefName;       // Reference name
                LSPString           sDoctype;       // Document type
                LSPString           sSystem;        // System literal
                LSPString           sPublic;        // Public literal
                cvector<LSPString>  vTags;
                cvector<LSPString>  vAtts;

            protected:
                static void         drop_list(cvector<LSPString> *list);

                inline lsp_swchar_t getch();
                inline void         ungetch(lsp_swchar_t c);
                inline void         push_state(parse_state_t override);
                inline void         pop_state();

                bool                skip_spaces();
                status_t            check_duplicate_attribute();

                status_t            read_version();
                status_t            read_encoding();
                status_t            read_standalone();

                status_t            read_text(const char *text);
                status_t            read_start_document();
                status_t            read_end_document();
                status_t            read_name(LSPString *name);
                status_t            read_attribute_value(lsp_swchar_t qc);
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
                status_t            read_characters();
                status_t            read_entity_reference(LSPString *cdata);
                status_t            read_system_literal(LSPString *dst);
                status_t            read_pubid_literal(LSPString *dst);

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
                status_t            open(const char *path, const char *charset = NULL);

                /**
                 * Open parser
                 * @param path string representation of path to the file
                 * @param charset character set
                 * @return status of operation
                 */
                status_t            open(const LSPString *path, const char *charset = NULL);

                /**
                 * Open parser
                 * @param path path to the file
                 * @param charset character set
                 * @return status of operation
                 */
                status_t            open(const io::Path *path, const char *charset = NULL);

                /**
                 * Wrap string with parser
                 * @param str string to wrap
                 * @param charset character set
                 * @return status of operation
                 */
                status_t            wrap(const char *str, const char *charset = NULL);

                /**
                 * Wrap string with parser
                 * @param str string to wrap
                 * @return status of operation
                 */
                status_t            wrap(const LSPString *str);

                /**
                 * Wrap input sequence with parser
                 * @param seq sequence to use for reads
                 * @return status of operation
                 */
                status_t            wrap(io::IInSequence *seq, size_t flags = WRAP_NONE);

                /**
                 * Wrap input stream with parser
                 * @param is input stream
                 * @param flags wrap flags
                 * @param charset character set
                 * @return status of operation
                 */
                status_t            wrap(io::IInStream *is, size_t flags = WRAP_NONE, const char *charset = NULL);

                /**
                 * Close parser
                 * @return status of operation
                 */
                status_t            close();

            public:
                /**
                 * Read next element
                 * @return XT_ element code or negative status of operation
                 */
                status_t            read_next();

                /**
                 * Get current element
                 * @return XT_ element code or negative status of operation
                 */
                status_t            get_current();

                /**
                 * Resolve entity with specified value
                 * @param value value to resolve
                 * @return status of operation
                 */
                status_t            set_value(const LSPString *value);

                /**
                 * Resolve entity with specified value
                 * @param value value to resolve
                 * @param charset character set encoding
                 * @return status of operation
                 */
                status_t            resolve_entity(const char *value, const char *charset = NULL);

            public:
                /**
                 * Check whethere document is standalone
                 * @return true if document is standalone
                 */
                inline bool             is_standalone() const   { return nFlags & XF_STANDALONE; }

                /**
                 * Get version string
                 * @return version string or NULL is version string is not present in header
                 */
                inline const LSPString *version() const         { return (nFlags & XF_VERSION) ? &sVersion : NULL; }

                /**
                 * Get actual parser version
                 * @return actual parser version
                 */
                inline xml_version_t    xml_version() const     { return enVersion; }

                /**
                 * Get document encoding
                 * @return document encoding or NULL if encoding string is not present in header
                 */
                inline const LSPString *encoding() const        { return (nFlags & XF_ENCODING) ? &sEncoding : NULL; }

                /**
                 * Return name of current property, tag or processing instruction
                 * @return name name
                 */
                const LSPString        *name() const;

                /**
                 * Return value of current property, comment or processing instruction
                 * @return value value
                 */
                const LSPString        *value() const;

                /**
                 * Get current element nesting level
                 * @return current element nesting level
                 */
                inline size_t           level() const           { return vTags.size(); }

                /**
                 * Get document type name
                 * @return document type name or NULL if not present
                 */
                inline const            LSPString *doctype() const { return (nFlags & XF_DOCTYPE) ? &sDoctype : NULL; }

                /**
                 * Get document type public literal
                 * @return document type public literal or NULL if not present
                 */
                inline const            LSPString *pub_literal() const { return (nFlags & XF_DOCTYPE_PUB) ? &sPublic: NULL; }

                /**
                 * Get document type system literal
                 * @return document type system literal or NULL if not present
                 */
                inline const            LSPString *sys_literal() const { return (nFlags & XF_DOCTYPE_SYS) ? &sSystem: NULL; }
        };
    
    } /* namespace xml */
} /* namespace lsp */

#endif /* CORE_FILES_XML_PULLPARSER_H_ */
