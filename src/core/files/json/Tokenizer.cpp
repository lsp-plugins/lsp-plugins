/*
 * Tokenizer.cpp
 *
 *  Created on: 14 окт. 2019 г.
 *      Author: sadko
 */

#include <core/files/json/Tokenizer.h>

namespace lsp
{
    namespace json
    {
        
        Tokenizer::Tokenizer(io::IInSequence *in)
        {
            pIn         = in;
            cCurrent    = -1;
            enToken     = JT_UNKNOWN;
            nError      = STATUS_OK;
            fValue      = 0;
            iValue      = 0;
            nUnget      = 0;
        }
        
        Tokenizer::~Tokenizer()
        {
            pIn         = NULL;
        }

        token_t Tokenizer::get_token(bool get)
        {
            // Pre-checks
            if (!get)
                return enToken;
            else if (nUnget > 0)
            {
                --nUnget;
                return enToken;
            }

            return JT_UNKNOWN;
        }
    
    } /* namespace json */
} /* namespace lsp */
