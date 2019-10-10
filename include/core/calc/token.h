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
            TT_IDENTIFIER,  // Identifier
            TT_BAREWORD,    // Bareword
            TT_IVALUE,      // Integer value
            TT_FVALUE,      // Floating-point value
            TT_STRING,      // String literal

            TT_LBRACE,      // (
            TT_RBRACE,      // )
            TT_LQBRACE,     // [
            TT_RQBRACE,     // ]
            TT_LCBRACE,     // {
            TT_RCBRACE,     // }

            // Constants
            TT_TRUE,        // true
            TT_FALSE,       // false
            TT_NULL,        // null
            TT_UNDEF,       // undef

            // Logical operations
            TT_AND,         // &, &&, and
            TT_OR,          // |, ||, or
            TT_NOT,         // !, not
            TT_XOR,         // ^, ^^, xor

            // Binary operations
            TT_BAND,        // band
            TT_BOR,         // bor
            TT_BNOT,        // ~, bnot
            TT_BXOR,        // bxor

            // Floating-point operations
            TT_ADD,         // +
            TT_SUB,         // -
            TT_ADDSYM,      // add
            TT_SUBSYM,      // sub
            TT_MUL,         // *, mul
            TT_POW,         // **, pow
            TT_DIV,         // /, div
            TT_FMOD,        // fmod

            // Integer operations
            TT_IADD,        // iadd
            TT_ISUB,        // isub
            TT_IMUL,        // imul
            TT_IDIV,        // idiv
            TT_IMOD,        // %, mod, imod

            // Floating-point comparison
            TT_LESS,        // <, lt, nge
            TT_GREATER,     // >, gt, nle
            TT_LESS_EQ,     // <=, le, ngt
            TT_GREATER_EQ,  // >=, ge, nlt
            TT_NOT_EQ,      // !=, <>, ne
            TT_EQ,          // =, ==, eq
            TT_CMP,         // <=>, cmp

            // Integer comparison
            TT_ILESS,       // ilt, inge
            TT_IGREATER,    // igt, inle
            TT_ILESS_EQ,    // ile, ingt
            TT_IGREATER_EQ, // ige, ingt
            TT_INOT_EQ,     // ine
            TT_IEQ,         // ie, ieq
            TT_ICMP,        // icmp

            // String operations
            TT_SCAT,        // String concat
            TT_SREP,        // String repeat
            TT_SLEN,        // String length
            TT_SREV,        // String reverse
            TT_SUPR,        // String upper
            TT_SLWR,        // String lower

            // Casts
            TT_INT,         // Cast to integer
            TT_FLOAT,       // Cast to float
            TT_STR,         // Cast to string
            TT_BOOL,        // Cast to bool

            // Misc operations
            TT_EX,          // ex

            // Special measuring units
            TT_DB,          // db

            // Different operators
            TT_QUESTION,    // ?
            TT_COLON,       // :
            TT_SEMICOLON,   // ;

            // Control tokens
            TT_UNKNOWN,     // Unknown token
            TT_EOF,         // End of file reached
            TT_ERROR        // error
        };

        enum token_flags_t
        {
            TF_NONE     = 0,
            TF_GET      = 1 << 0,
            TF_XSIGN    = 1 << 1,
            TF_BAREWORD = 1 << 2
        };
    }
}

#endif /* CORE_CALC_TOKEN_H_ */
