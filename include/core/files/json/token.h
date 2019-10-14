/*
 * token.h
 *
 *  Created on: 14 окт. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_JSON_TOKEN_H_
#define CORE_FILES_JSON_TOKEN_H_

namespace lsp
{
    namespace json
    {
        enum token_t
        {
            JT_UNKNOWN,         // Unknown token
            JT_ERROR,           // Error occurred
            JT_EOF,             // End of input

            JT_LQ_BRACE,        // [
            JT_RQ_BRACE,        // ]
            JT_LC_BRACE,        // {
            JT_RC_BRACE,        // }
            JT_COLON,           // :
            JT_COMMA,           // ,
            JT_SQ_STRING,       // 'string'
            JT_DQ_STRING,       // "string"
            JT_IDENTIFIER,      // identifier
            JT_SL_COMMENT,      // //comment
            JT_ML_COMMENT,      // /* comment */
            JT_DECIMAL,         // 1234
            JT_HEXADECIMAL,     // 0x1234
            JT_DOUBLE,          // 12.34, 1.234e+1
        };
    }
}

#endif /* CORE_FILES_JSON_TOKEN_H_ */
