/*
 * Parser.h
 *
 *  Created on: 16 окт. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_JSON_PARSER_H_
#define CORE_FILES_JSON_PARSER_H_

#include <common/types.h>
#include <core/io/IInStream.h>
#include <core/files/json/token.h>
#include <core/files/json/Tokenizer.h>
#include <data/cstorage.h>

namespace lsp
{
    namespace json
    {
        
        class Parser
        {
            private:
                Parser & operator = (const Parser &);

                enum pmode_t
                {
                    READ_ROOT,
                    READ_ARRAY,
                    READ_OBJECT
                };

                enum parse_flags_t
                {
                    PF_COMMA        = 1 << 0,
                    PF_COLON        = 1 << 1,
                    PF_PROPERTY     = 1 << 2,
                    PF_VALUE        = 1 << 3,

                    PF_ARRAY_ALL    = PF_COMMA | PF_VALUE,
                    PF_OBJECT_ALL   = PF_COMMA | PF_COLON | PF_PROPERTY | PF_VALUE
                };

                typedef struct state_t
                {
                    pmode_t         mode;
                    size_t          flags;
                } state_t;

            protected:
                Tokenizer          *pTokenizer;
                io::IInSequence    *pSequence;
                size_t              nWFlags;
                json_version_t      enVersion;
                state_t             sState;
                event_t             sCurrent;
                cstorage<state_t>   sStack;

            protected:
                status_t            read_root();
                status_t            read_array();
                status_t            read_object();
                status_t            read_primitive(token_t tok);

                inline status_t     push_state(pmode_t state);
                inline status_t     pop_state();

                inline token_t      lookup_token();

            public:
                explicit Parser();
                virtual ~Parser();

            public:
                /**
                 * Open parser
                 * @param str string to wrap
                 * @param version JSON version
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    open(const char *str, json_version_t version, const char *charset = NULL);

                /**
                 * Open parser
                 * @param str string to wrap
                 * @param version JSON version
                 * @return status of operation
                 */
                status_t    open(const LSPString *str, json_version_t version);

                /**
                 * Open parser
                 * @param seq sequence to use for reads
                 * @param version JSON version
                 * @return status of operation
                 */
                status_t    open(io::IInSequence *seq, json_version_t version, size_t flags = WRAP_NONE);

                /**
                 * Open parser
                 * @param is input stream
                 * @param version JSON version
                 * @param flags wrap flags
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    open(io::IInStream *is, json_version_t version, size_t flags = WRAP_NONE, const char *charset = NULL);

                /**
                 * Close parser
                 * @return status of operation
                 */
                status_t    close();

            public:
                /**
                 * Read next event
                 * @param ev event to read
                 * @return status of operation
                 */
                status_t    get_next(event_t *ev);

                /**
                 * Get current event
                 * @param ev pointer to save the event
                 * @return status of operation
                 */
                status_t    get_current(event_t *ev);

                /**
                 * Skip nested data structures after current event
                 * @return status of operation
                 */
                status_t    skip_next();

                /**
                 * Skip nested data structures associated with current event.
                 * If current event is JE_OBJECT_START
                 * @return status of operation
                 */
                status_t    skip_current();
        };
    
    } /* namespace json */
} /* namespace lsp */

#endif /* CORE_FILES_JSON_PARSER_H_ */
