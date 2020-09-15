/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 16 сент. 2019 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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
                union
                {
                    double              fValue;
                    ssize_t             iValue;
                };
                status_t            nError;
                size_t              nUnget;

            protected:
                lsp_swchar_t        lookup();
                lsp_swchar_t        commit_lookup(token_t token);
                lsp_swchar_t        skip_whitespace();
                token_t             lookup_identifier(token_t type);
                token_t             lookup_string();
                token_t             lookup_number();
                token_t             commit(token_t token);
                token_t             set_error(status_t code);
                token_t             decode_bareword();

                static bool         parse_digit(int *digit, lsp_wchar_t ch, int radix);

            public:
                explicit Tokenizer(io::IInSequence *in);
                virtual ~Tokenizer();

            public:
                /**
                 * Check that character matches first identifier letter
                 * @param ch character
                 * @return true if character matchers
                 */
                static bool             is_identifier_first(lsp_wchar_t ch);

                /**
                 * Check that character matches next identifier letter
                 * @param ch character
                 * @return true if character matchers
                 */
                static bool             is_identifier_next(lsp_wchar_t ch);

                /**
                 * Get token
                 * @param flags tokenizing flags
                 * @return current token value
                 */
                token_t                 get_token(size_t flags = TF_NONE);

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
    
    } /* namespace calc */
} /* namespace lsp */

#endif /* CORE_CALC_TOKENIZER_H_ */
