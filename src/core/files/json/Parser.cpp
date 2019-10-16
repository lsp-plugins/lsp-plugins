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
            sState.mode     = READ_ROOT;
            sState.flags    = 0;
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

            pTokenizer      = tok;
            pSequence       = seq;
            nWFlags         = flags;
            enVersion       = version;
            sState.mode     = READ_ROOT;
            sState.flags    = 0;

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
            sStack.flush();

            return res;
        }

        status_t Parser::push_state(pmode_t mode)
        {
            if (!sStack.add(&sState))
                return STATUS_NO_MEM;

            sState.mode     = mode;
            sState.flags    = 0;
            return STATUS_OK;
        }

        status_t Parser::pop_state()
        {
            state_t *st = sStack.last();
            if (st == NULL)
                return STATUS_BAD_STATE;
            sState          = *st;
            return (sStack.remove_last()) ? STATUS_OK : STATUS_BAD_STATE;
        }

        token_t Parser::lookup_token()
        {
            return pTokenizer->get_token(true);
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
                        if (enVersion <= JSON_JSON5) // Only JSON5 allows comments
                            return STATUS_BAD_TOKEN;
                        break;

                    case JT_LQ_BRACE:       // Start of array
                        if (sState.flags & PF_VALUE)
                            return STATUS_BAD_TOKEN;
                        sCurrent.type       = JE_ARRAY_START;
                        sState.flags       |= PF_VALUE;
                        return push_state(READ_ARRAY);

                    case JT_LC_BRACE:       // Start of object
                        if (sState.flags & PF_VALUE)
                            return STATUS_BAD_TOKEN;
                        sCurrent.type       = JE_OBJECT_START;
                        sState.flags       |= PF_VALUE;
                        return push_state(READ_OBJECT);

                    case JT_SQ_STRING:
                    case JT_DQ_STRING:
                    case JT_TRUE:
                    case JT_FALSE:
                    case JT_NULL:
                    case JT_DECIMAL:
                    case JT_DOUBLE:
                    case JT_HEXADECIMAL:
                        if (sState.flags & PF_VALUE)
                            return STATUS_BAD_TOKEN;
                        sState.flags       |= PF_VALUE;
                        return read_primitive(tok);

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
                        if (enVersion <= JSON_JSON5) // Only JSON5 allows comments
                            return STATUS_BAD_TOKEN;
                        break;

                    case JT_COMMA:          // Comma
                        if ((sState.flags & PF_ARRAY_ALL) != PF_VALUE)
                            return STATUS_BAD_TOKEN;
                        sState.flags    |= PF_COMMA;    // Add comma flag
                        break;

                    case JT_RQ_BRACE:       // End of current array
                        // Closing brace after comma is allowed only since JSON5
                        if ((sState.flags & PF_COMMA) && (enVersion <= JSON_JSON5))
                            return STATUS_BAD_TOKEN;
                        sCurrent.type       = JE_ARRAY_END;
                        return pop_state();

                    case JT_LQ_BRACE:       // Start of sub-array
                    {
                        size_t flags        = sState.flags & PF_ARRAY_ALL;
                        if ((flags != 0) && (flags != (PF_VALUE | PF_COMMA)))
                            return STATUS_BAD_TOKEN;
                        sState.flags        = PF_VALUE;
                        sCurrent.type       = JE_ARRAY_START;
                        return push_state(READ_ARRAY);
                    }

                    case JT_LC_BRACE:       // Start of nested object
                    {
                        size_t flags        = sState.flags & PF_ARRAY_ALL;
                        if ((flags != 0) && (flags != (PF_VALUE | PF_COMMA)))
                            return STATUS_BAD_TOKEN;
                        sState.flags        = PF_VALUE;
                        sCurrent.type       = JE_OBJECT_START;
                        return push_state(READ_OBJECT);
                    }

                    case JT_SQ_STRING:
                    case JT_DQ_STRING:
                    case JT_TRUE:
                    case JT_FALSE:
                    case JT_NULL:
                    case JT_DECIMAL:
                    case JT_DOUBLE:
                    case JT_HEXADECIMAL:    // Read primitive type
                    {
                        size_t flags        = sState.flags & PF_ARRAY_ALL;
                        if ((flags != 0) && (flags != (PF_VALUE | PF_COMMA)))
                            return STATUS_BAD_TOKEN;
                        sState.flags        = PF_VALUE;
                        return read_primitive(tok);
                    }

                    default:
                        return STATUS_BAD_TOKEN;
                }
            }
        }

        status_t Parser::read_object()
        {
            status_t res;

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
                        if (enVersion <= JSON_JSON5) // Only JSON5 allows comments
                            return STATUS_BAD_TOKEN;
                        break;

                    case JT_COMMA:          // Comma
                        if ((sState.flags & PF_OBJECT_ALL) != (PF_PROPERTY | PF_COLON | PF_VALUE))
                            return STATUS_BAD_TOKEN;
                        sState.flags     |= PF_COMMA;    // Add comma flag
                        break;

                    case JT_COLON:          // Colon
                        if ((sState.flags & PF_OBJECT_ALL) != PF_PROPERTY)
                            return STATUS_BAD_TOKEN;
                        sState.flags     |= PF_COLON;
                        break;

                    case JT_RC_BRACE:       // End of current object
                    {
                        size_t flags = sState.flags & PF_OBJECT_ALL;

                        // JSON5 allows a comma before closing brace
                        if ((flags == PF_OBJECT_ALL) && (enVersion <= JSON_JSON5))
                            return STATUS_BAD_TOKEN;

                        // If property is present, there should be a corresponding value
                        if ((flags != 0) && (flags != (PF_PROPERTY | PF_COLON | PF_VALUE)))
                            return STATUS_BAD_TOKEN;

                        sCurrent.type       = JE_OBJECT_END;
                        return pop_state();
                    }

                    case JT_LQ_BRACE:       // Start of array value
                        // Property should be defined first
                        if ((sState.flags & PF_OBJECT_ALL) != (PF_PROPERTY | PF_COLON))
                            return STATUS_BAD_TOKEN;
                        sCurrent.type       = JE_ARRAY_START;
                        sState.flags       |= PF_VALUE;
                        return push_state(READ_ARRAY);

                    case JT_LC_BRACE:       // Start of object value
                        // Property should be defined first
                        if ((sState.flags & PF_OBJECT_ALL) != (PF_PROPERTY | PF_COLON))
                             return STATUS_BAD_TOKEN;
                        sCurrent.type       = JE_OBJECT_START;
                        sState.flags       |= PF_VALUE;
                        return push_state(READ_OBJECT);

                    case JT_SQ_STRING:
                    case JT_DQ_STRING:
                    case JT_IDENTIFIER:     // Property name
                    {
                        size_t flags = sState.flags & PF_OBJECT_ALL;
                        if ((flags == 0) || (flags == PF_OBJECT_ALL)) // Property name?
                        {
                            if ((res = read_primitive(tok)) == STATUS_OK)
                            {
                                sState.flags        = PF_PROPERTY;
                                sCurrent.type       = JE_PROPERTY;  // Override type of event
                            }
                        }
                        else if (flags == (PF_PROPERTY | PF_COLON)) // Value?
                        {
                            if ((res = read_primitive(tok)) == STATUS_OK)
                                sState.flags       |= PF_VALUE;
                        }
                        else
                            res = STATUS_BAD_STATE;

                        return res;
                    }

                    case JT_TRUE:
                    case JT_FALSE:
                    case JT_NULL:
                    case JT_DECIMAL:
                    case JT_DOUBLE:
                    case JT_HEXADECIMAL:    // Read primitive type
                    {
                        if ((sState.flags & PF_OBJECT_ALL) != (PF_PROPERTY | PF_COLON))
                             return STATUS_BAD_TOKEN;
                        sState.flags    |= PF_VALUE;
                        return read_primitive(tok);
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
            switch (sState.mode)
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
