/*
 * Tokenizer.h
 *
 *  Created on: 16 сент. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_CALC_TOKENIZER_H_
#define CORE_CALC_TOKENIZER_H_

#include <core/LSPString.h>
#include <core/calc/token.h>
#include <core/io/IInSequence.h>

namespace lsp
{
    namespace calc
    {
        
        class Tokenizer
        {
            private:
                io::IInSequence    *pIn;
                lsp_swchar_t        cCurrent;
                token_t             enToken;
                LSPString           sValue;
                status_t            nError;

            protected:
                lsp_swchar_t        lookup();
                lsp_swchar_t        commit_lookup(token_t token);
                lsp_swchar_t        skip_whitespace();
                token_t             lookup_identifier();
                token_t             lookup_string();
                token_t             commit(token_t token);
                token_t             commit_word(lsp_wchar_t ch);
                token_t             set_error(status_t code);

                static bool         is_identifier_first(lsp_wchar_t ch);
                static bool         is_identifier_next(lsp_wchar_t ch);

            public:
                explicit Tokenizer(io::IInSequence *in);
                virtual ~Tokenizer();

            public:
                /**
                 * Get token
                 * @param flags tokenizing flags
                 * @return current token value
                 */
                token_t                 get_token(size_t flags = TF_NONE);

                /**
                 * Get current token
                 * @return current token
                 */
                inline token_t          current() const { return enToken; }

                /**
                 * Get current token value for tokens that consist of characters
                 * @return current token value
                 */
                inline const LSPString *text() const { return &sValue; }



                /**
                 * Get last error code
                 * @return last error code
                 */
                inline status_t         error() const { return nError; }
        };
    
    } /* namespace calc */
} /* namespace lsp */

#endif /* CORE_CALC_TOKENIZER_H_ */
