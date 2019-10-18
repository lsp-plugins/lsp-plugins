/*
 * Tokenizer.h
 *
 *  Created on: 14 окт. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_JSON_TOKENIZER_H_
#define CORE_FILES_JSON_TOKENIZER_H_

#include <core/LSPString.h>
#include <core/io/IInSequence.h>
#include <core/files/json/token.h>

namespace lsp
{
    namespace json
    {
        
        class Tokenizer
        {
            private:
                Tokenizer & operator = (const Tokenizer &);

                friend class Serializer;

            protected:
                io::IInSequence        *pIn;
                lsp_swchar_t            cCurrent;
                token_t                 enToken;
                LSPString               sValue;
                union
                {
                    double              fValue;
                    ssize_t             iValue;
                };
                status_t                nError;
                size_t                  nUnget;
                lsp_utf16_t            *vPending;
                size_t                  nPending;
                size_t                  nCapacity;

            protected:
                lsp_swchar_t        skip_whitespace();
                inline lsp_swchar_t lookup();
                token_t             commit(token_t token);
                inline token_t      skip(token_t token)            { cCurrent = -1; return enToken = token; }
                lsp_swchar_t        commit_lookup(token_t token);
                inline token_t      set_error(status_t code);

                static bool         is_reserved_word(const LSPString *text);
                static bool         is_valid_identifier(const LSPString *text);
                static bool         is_identifier_start(lsp_wchar_t ch);
                static bool         is_identifier(lsp_wchar_t ch);
                static bool         parse_digit(int *digit, lsp_wchar_t ch, int radix);

                status_t            add_pending_character(lsp_utf16_t ch);
                status_t            commit_pending_characters();
                token_t             parse_unicode_escape_sequence(token_t type);
                token_t             parse_hexadecimal_escape_sequence(token_t type);

                token_t             parse_string(token_t type);
                token_t             parse_identifier();
                token_t             parse_single_line_comment();
                token_t             parse_multiline_comment();
                token_t             parse_number();

            public:
                explicit Tokenizer(io::IInSequence *in);
                virtual ~Tokenizer();

            public:
                /**
                 * Get token
                 * @param get force token lookup
                 * @return current token value
                 */
                token_t                 get_token(bool get);

                /**
                 * Unget token
                 */
                inline void             unget()     { ++nUnget; };

                /**
                 * Get current token
                 * @return current token
                 */
                inline token_t          current() const { return enToken; }

                /**
                 * Get current token value for tokens that consist of characters
                 * @return current token value
                 */
                inline const LSPString *text_value() const  { return &sValue; }

                /**
                 * Get floating-point value of the token
                 * @return floating-point value of the token
                 */
                inline const double     float_value() const { return fValue; }

                /**
                 * Get integer value of the token
                 * @return integer value of the token
                 */
                inline ssize_t          int_value() const   { return iValue; }

                /**
                 * Get last error code
                 * @return last error code
                 */
                inline status_t         error() const { return nError; }
        };
    
    } /* namespace json */
} /* namespace lsp */

#endif /* CORE_FILES_JSON_TOKENIZER_H_ */
