/*
 * Parser.cpp
 *
 *  Created on: 16 окт. 2019 г.
 *      Author: sadko
 */

#include <core/io/InStringSequence.h>
#include <core/io/InSequence.h>
#include <core/files/json/Parser.h>

namespace lsp
{
    namespace json
    {
        
        Parser::Parser()
        {
            pTokenizer      = NULL;
            pSequence       = NULL;
            nWFlags         = 0;
            enVersion       = JSON_LEGACY;
            enState         = READ_ROOT;
            nPFlags         = PF_GET;

            sCurrent.type   = JE_UNKNOWN;
        }
        
        Parser::~Parser()
        {
            close();
        }

        status_t Parser::open(const char *str, json_version_t version, const char *charset)
        {
            if (pTokenizer != NULL)
                return STATUS_BAD_STATE;
            else if (str == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InStringSequence *seq = new io::InStringSequence();
            if (seq == NULL)
                return STATUS_NO_MEM;

            status_t res = seq->wrap(str, charset);
            if (res == STATUS_OK)
            {
                if ((res = open(seq, version, WRAP_CLOSE | WRAP_DELETE)) == STATUS_OK)
                    return res;
                seq->close();
            }

            delete seq;
            return res;
        }

        status_t Parser::open(const LSPString *str, json_version_t version)
        {
            if (pTokenizer != NULL)
                return STATUS_BAD_STATE;
            else if (str == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InStringSequence *seq = new io::InStringSequence();
            if (seq == NULL)
                return STATUS_NO_MEM;

            status_t res = seq->wrap(str);
            if (res == STATUS_OK)
            {
                if ((res = open(seq, version, WRAP_CLOSE | WRAP_DELETE)) == STATUS_OK)
                    return res;
                seq->close();
            }

            delete seq;
            return res;
        }

        status_t Parser::open(io::IInStream *is, json_version_t version, size_t flags, const char *charset)
        {
            if (pTokenizer != NULL)
                return STATUS_BAD_STATE;
            else if (is == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InSequence *seq = new io::InSequence();
            if (seq == NULL)
                return STATUS_NO_MEM;

            status_t res = seq->wrap(is, flags, charset);
            if (res == STATUS_OK)
            {
                if ((res = open(seq, version, WRAP_CLOSE | WRAP_DELETE)) == STATUS_OK)
                    return res;
                seq->close();
            }

            delete seq;
            return res;
        }

        status_t Parser::open(io::IInSequence *seq, json_version_t version, size_t flags)
        {
            if (pTokenizer != NULL)
                return STATUS_BAD_STATE;
            else if (seq == NULL)
                return STATUS_BAD_ARGUMENTS;

            Tokenizer *tok = new Tokenizer(seq);
            if (tok == NULL)
                return STATUS_NO_MEM;

            pTokenizer  = tok;
            pSequence   = seq;
            nWFlags     = flags;
            enVersion   = version;
            enState     = READ_ROOT;
            nPFlags     = PF_GET;

            return STATUS_OK;
        }

        status_t Parser::close()
        {
            status_t res = STATUS_OK;

            if (pTokenizer != NULL)
            {
                delete pTokenizer;
                pTokenizer = NULL;
            }

            if (pSequence != NULL)
            {
                if (nWFlags & WRAP_CLOSE)
                {
                    status_t xres = pSequence->close();
                    if (res == STATUS_OK)
                        res = xres;
                }

                if (nWFlags & WRAP_DELETE)
                    delete pSequence;

                pSequence = NULL;
            }

            sCurrent.type   = JE_UNKNOWN;

            return res;
        }

        bool Parser::push_state(state_t state)
        {
            if (!sStack.append(state))
                return false;

            enState = state;
            nPFlags    &=   PF_GET; // Reset all flags except PF_GET
            return true;
        }

        bool Parser::pop_state()
        {
            if (!sStack.remove_last())
                return false;

            if (sStack.length() <= 0)
                enState     = READ_REJECT;
            else
                enState     = state_t(sStack.last());

            nPFlags    &=   PF_GET; // Reset all flags except PF_GET
            return true;
        }

        token_t Parser::lookup_token()
        {
            token_t tok     = pTokenizer->get_token(nPFlags & PF_GET);
            nPFlags        |= PF_GET;
            return tok;
        }

        void Parser::unget_token()
        {
            nPFlags        &= ~PF_GET;
        }

        status_t Parser::get_current(event_t *ev)
        {
            if (pTokenizer == NULL)
                return STATUS_BAD_STATE;
            if (ev == NULL)
                return STATUS_BAD_ARGUMENTS;

            switch (sCurrent.type)
            {
                case JE_OBJECT_START:
                case JE_OBJECT_END:
                case JE_ARRAY_START:
                case JE_ARRAY_END:
                case JE_UNKNOWN:
                case JE_NULL:
                    break;

                case JE_PROPERTY:
                case JE_STRING:
                    if (!ev->sValue.set(&sCurrent.sValue))
                        return STATUS_NO_MEM;
                    break;
                case JE_INTEGER:
                    ev->iValue      = sCurrent.iValue;
                    break;
                case JE_DOUBLE:
                    ev->fValue      = sCurrent.fValue;
                    break;
                case JE_BOOL:
                    ev->bValue      = sCurrent.bValue;
                    break;

                default:
                    return STATUS_BAD_STATE;
            }

            ev->type = sCurrent.type;
            nPFlags    &=   PF_GET; // Reset all flags except PF_GET
            return STATUS_OK;
        }

        status_t Parser::read_primitive(token_t tok)
        {
            switch (tok)
            {
                case JT_DQ_STRING:
                    if (!sCurrent.sValue.set(pTokenizer->text_value()))
                        return STATUS_NO_MEM;
                    sCurrent.type = JE_STRING;
                    break;

                case JT_TRUE:
                case JT_FALSE:
                    sCurrent.bValue = (tok == JT_TRUE);
                    sCurrent.type   = JE_BOOL;
                    break;

                case JT_NULL:
                    sCurrent.type   = JE_NULL;
                    break;

                case JT_DECIMAL:
                    sCurrent.iValue = pTokenizer->int_value();
                    sCurrent.type   = JE_INTEGER;
                    break;

                case JT_DOUBLE:
                    sCurrent.fValue = pTokenizer->float_value();
                    sCurrent.type   = JE_DOUBLE;
                    break;

                case JT_SQ_STRING:   // Single-quoted strings are allowed since JSON5
                case JT_IDENTIFIER:
                    if (enVersion < JSON_JSON5)
                        return STATUS_BAD_TOKEN;
                    if (!sCurrent.sValue.set(pTokenizer->text_value()))
                        return STATUS_NO_MEM;
                    sCurrent.type = JE_STRING;
                    break;

                case JT_HEXADECIMAL: // Hexadecimals are allowed since JSON5
                    if (enVersion < JSON_JSON5)
                        return STATUS_BAD_TOKEN;
                    sCurrent.iValue = pTokenizer->int_value();
                    sCurrent.type   = JE_INTEGER;
                    break;

                default:
                    return STATUS_BAD_TOKEN;
            }

            return STATUS_OK;
        }

        status_t Parser::read_root()
        {
            while (true)
            {
                // Get token
                token_t tok     = lookup_token();

                // Analyze token
                switch (tok)
                {
                    case JT_EOF:            // End of input
                        return STATUS_EOF;
                    case JT_ERROR:          // Error input
                        return pTokenizer->error();

                    case JT_SL_COMMENT:     // Skip comments
                    case JT_ML_COMMENT:
                        break;

                    case JT_LQ_BRACE:       // Start of array
                        if (!push_state(READ_ARRAY))
                            return STATUS_NO_MEM;
                        sCurrent.type       = JE_ARRAY_START;
                        return STATUS_OK;

                    case JT_LC_BRACE:       // Start of object
                        if (!push_state(READ_OBJECT))
                            return STATUS_NO_MEM;
                        sCurrent.type       = JE_OBJECT_START;
                        return STATUS_OK;

                    case JT_SQ_STRING:
                    case JT_DQ_STRING:
                    case JT_TRUE:
                    case JT_FALSE:
                    case JT_NULL:
                    case JT_DECIMAL:
                    case JT_DOUBLE:
                    case JT_HEXADECIMAL:
                        return read_primitive(tok);

                    default:
                        return STATUS_BAD_TOKEN;
                }
            }
        }

        status_t Parser::read_reject()
        {
            while (true)
            {
                // Get token
                token_t tok     = lookup_token();

                // Analyze token
                switch (tok)
                {
                    case JT_EOF:            // End of input
                        return STATUS_OK;
                    case JT_ERROR:          // Error input
                        return pTokenizer->error();

                    case JT_SL_COMMENT:     // Skip comments
                    case JT_ML_COMMENT:
                        break;

                    default:
                        return STATUS_BAD_TOKEN;
                }
            }
        }

        status_t Parser::read_array()
        {
            while (true)
            {
                // Get token
                token_t tok     = lookup_token();

                // Analyze token
                switch (tok)
                {
                    case JT_EOF:            // End of input
                        return STATUS_CORRUPTED;
                    case JT_ERROR:          // Error input
                        return pTokenizer->error();

                    case JT_SL_COMMENT:     // Skip comments
                    case JT_ML_COMMENT:
                        break;

                    case JT_COMMA:          // Comma
                        if (nPFlags & PF_COMMA)
                            return STATUS_BAD_TOKEN;
                        nPFlags     |= PF_COMMA;    // Add comma flag
                        break;

                    case JT_RQ_BRACE:       // End of current array
                        // Closing brace after comma is allowed only since JSON5
                        if ((nPFlags & PF_COMMA) && (enVersion <= JSON_JSON5))
                            return STATUS_BAD_TOKEN;
                        if (!pop_state())
                            return STATUS_BAD_STATE;
                        sCurrent.type       = JE_ARRAY_END;
                        return STATUS_OK;

                    case JT_LQ_BRACE:       // Start of sub-array
                        if (!push_state(READ_ARRAY))
                            return STATUS_NO_MEM;
                        sCurrent.type       = JE_ARRAY_START;
                        return STATUS_OK;

                    case JT_LC_BRACE:       // Start of nested object
                        if (!push_state(READ_OBJECT))
                            return STATUS_NO_MEM;
                        sCurrent.type       = JE_OBJECT_START;
                        return STATUS_OK;

                    case JT_SQ_STRING:
                    case JT_DQ_STRING:
                    case JT_TRUE:
                    case JT_FALSE:
                    case JT_NULL:
                    case JT_DECIMAL:
                    case JT_DOUBLE:
                    case JT_HEXADECIMAL:    // Read primitive type
                        return read_primitive(tok);

                    default:
                        return STATUS_BAD_TOKEN;
                }
            }
        }

        status_t Parser::read_object()
        {
            while (true)
            {
                // Get token
                token_t tok     = lookup_token();

                // Analyze token
                switch (tok)
                {
                    case JT_EOF:            // End of input
                        return STATUS_CORRUPTED;
                    case JT_ERROR:          // Error input
                        return pTokenizer->error();

                    case JT_SL_COMMENT:     // Skip comments
                    case JT_ML_COMMENT:
                        break;

                    case JT_COMMA:          // Comma
                        if (nPFlags & PF_COMMA)
                            return STATUS_BAD_TOKEN;
                        nPFlags     |= PF_COMMA;    // Add comma flag
                        break;

                    case JT_COLON:          // Colon
                        if (!(nPFlags & PF_PROPERTY))
                            return STATUS_BAD_TOKEN;
                        if (nPFlags & PF_COLON)
                            return STATUS_BAD_TOKEN;
                        nPFlags     |= PF_COLON;
                        break;

                    case JT_RC_BRACE:       // End of current object
                    {
                        // Closing brace after comma is allowed only since JSON5
                        if ((nPFlags & PF_COMMA) && (enVersion <= JSON_JSON5))
                            return STATUS_BAD_TOKEN;

                        // If property is present, there should be a corresponding value
                        size_t flags = (nPFlags & (PF_PROPERTY | PF_VALUE));
                        if ((flags != 0) && (flags != (PF_PROPERTY | PF_VALUE)))
                            return STATUS_CORRUPTED;

                        if (!pop_state())
                            return STATUS_BAD_STATE;
                        sCurrent.type       = JE_ARRAY_END;
                        return STATUS_OK;
                    }

                    case JT_LQ_BRACE:       // Start of array value
                        // Property should be defined first
                        if ((nPFlags & (PF_PROPERTY | PF_COLON)) != (PF_PROPERTY | PF_COLON))
                            return STATUS_BAD_TOKEN;
                        if (!push_state(READ_ARRAY))
                            return STATUS_NO_MEM;
                        sCurrent.type       = JE_ARRAY_START;
                        return STATUS_OK;

                    case JT_LC_BRACE:       // Start of object value
                        // Property should be defined first
                        if ((nPFlags & (PF_PROPERTY | PF_COLON)) != (PF_PROPERTY | PF_COLON))
                             return STATUS_BAD_TOKEN;
                        if (!push_state(READ_OBJECT))
                            return STATUS_NO_MEM;
                        sCurrent.type       = JE_OBJECT_START;
                        return STATUS_OK;

                    case JT_SQ_STRING:
                    case JT_DQ_STRING:
                    case JT_IDENTIFIER:     // Property name
                    {
                        if (nPFlags & PF_PROPERTY)  // Property is not set?
                            return STATUS_BAD_TOKEN;
                        status_t res        = read_primitive(tok);
                        if (res != STATUS_OK)
                            return res;
                        sCurrent.type       = JE_PROPERTY;
                        return STATUS_OK;
                    }

                    default:
                        return STATUS_BAD_TOKEN;
                }
            }
        }

        status_t Parser::get_next(event_t *ev)
        {
            if (pTokenizer == NULL)
                return STATUS_BAD_STATE;

            status_t res;

            // Root position?
            switch (enState)
            {
                case READ_ROOT:
                    res = read_root();
                    break;

                case READ_ARRAY:
                    res = read_array();
                    break;

                case READ_OBJECT:
                    res = read_object();
                    break;

                case READ_REJECT:
                    res = read_reject();
                    break;

                default:
                    return STATUS_BAD_STATE;
            }

            // Deploy result if there is place to deploy
            if ((res == STATUS_OK) && (ev != NULL))
                res = get_current(ev);

            return res;
        }
    
    } /* namespace json */
} /* namespace lsp */
