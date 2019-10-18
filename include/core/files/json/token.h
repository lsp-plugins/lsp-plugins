/*
 * token.h
 *
 *  Created on: 14 окт. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_JSON_TOKEN_H_
#define CORE_FILES_JSON_TOKEN_H_

#include <core/LSPString.h>

namespace lsp
{
    namespace json
    {
        /**
         * Low-level JSON tokens
         */
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
            JT_TRUE,            // true
            JT_FALSE,           // false
            JT_NULL,            // null
            JT_RESERVED,        // reserved word
            JT_SL_COMMENT,      // //comment
            JT_ML_COMMENT,      // /* comment */
            JT_DECIMAL,         // 1234
            JT_HEXADECIMAL,     // 0x1234
            JT_DOUBLE,          // 12.34, 1.234e+1
        };

        /**
         * JSON versions
         */
        enum json_version_t
        {
            JSON_LEGACY     = 0,    //!< JSON_LEGACY legacy JSON, very strict verison
            JSON_VERSION5   = 5000  //!< JSON_VERSION5 more user-friendly version
        };

        /**
         * High-level JSON event types
         */
        enum event_type_t
        {
            JE_OBJECT_START,    // Start of object - {
            JE_OBJECT_END,      // End of object - }
            JE_ARRAY_START,     // Start of array - [
            JE_ARRAY_END,       // End of array - ]
            JE_PROPERTY,        // Property - "key":
            JE_STRING,          // "String value"
            JE_INTEGER,         // Integer value - 123, 0x123
            JE_DOUBLE,          // Floating-point value
            JE_BOOL,            // Boolean value
            JE_NULL,            // Null value

            JE_UNKNOWN = -1     // Unknown event
        };

        /**
         * JSON event
         */
        typedef struct event_t
        {
            event_type_t    type;
            LSPString       sValue;
            union
            {
                bool            bValue;
                ssize_t         iValue;
                double          fValue;
            };
        } event_t;

        /**
         * JSON serialization flags
         */
        typedef struct serial_flags_t
        {
            json_version_t  version;            // Json version
            bool            identifiers;        // Prefer identifiers over strings (at least JSON5)
            lsp_wchar_t     ident;              // Identation character
            size_t          padding;            // Number of characters for identation
            bool            separator;          // Add space separator after colons and commas in objects
            bool            multiline;          // Use newline characters (multiline output)
        } serial_flags_t;
    }
}

#endif /* CORE_FILES_JSON_TOKEN_H_ */
