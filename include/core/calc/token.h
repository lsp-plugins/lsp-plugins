/*
 * token.h
 *
 *  Created on: 16 сент. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_CALC_TOKEN_H_
#define CORE_CALC_TOKEN_H_

#include <core/LSPString.h>

namespace lsp
{
    namespace calc
    {
        enum token_t
        {
            TT_UNKNOWN,

            TT_IDENTIFIER,  // Identifier
            TT_IVALUE,      // Integer value
            TT_FVALUE,      // Floating-point value
            TT_STRING,      // String literal

            TT_LBRACE,
            TT_RBRACE,
            TT_LQBRACE,
            TT_RQBRACE,

            // Constants
            TT_TRUE,
            TT_FALSE,

            // Logical operations
            TT_AND,
            TT_OR,
            TT_NOT,
            TT_XOR,

            // Binary operations
            TT_BAND,
            TT_BOR,
            TT_BNOT,
            TT_BXOR,

            // Floating-point operations
            TT_ADD,
            TT_SUB,
            TT_MUL,
            TT_POW,
            TT_DIV,

            // Integer operations
            TT_IADD,
            TT_ISUB,
            TT_IMUL,
            TT_IDIV,
            TT_MOD,

            // Floating-point comparison
            TT_LESS,
            TT_GREATER,
            TT_LESS_EQ,
            TT_GREATER_EQ,
            TT_NOT_EQ,
            TT_EQ,
            TT_CMP,

            // Integer comparison
            TT_ILESS,
            TT_IGREATER,
            TT_ILESS_EQ,
            TT_IGREATER_EQ,
            TT_INOT_EQ,
            TT_IEQ,

            // Misc operations
            TT_EX,

            // Different operators
            TT_QUESTION,
            TT_COLON,
            TT_SEMICOLON,

            // Special measuring units
            TT_DB,

            TT_EOF,
            TT_ERROR
        };

        enum token_flags_t
        {
            TF_NONE     = 0,
            TF_GET      = 1 << 0,
            TF_XSIGN    = 1 << 1
        };
    }
}

#endif /* CORE_CALC_TOKEN_H_ */
