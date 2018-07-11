/*
 * Expression.cpp
 *
 *  Created on: 28 окт. 2016 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <ctype.h>

namespace lsp
{
    Expression::Expression(): IUIPortListener()
    {
        pRoot       = NULL;
        pUI         = NULL;
        pListener   = NULL;
    }

    Expression::~Expression()
    {
        destroy_data(pRoot);
        pRoot       = NULL;
        pUI         = NULL;
        pListener   = NULL;
    }

    void Expression::init(plugin_ui *ui, IUIPortListener *listener)
    {
        pRoot       = NULL;
        pUI         = ui;
        pListener   = listener;
    }

    void Expression::destroy_data(binding_t *ptr)
    {
        if (ptr == NULL)
            return;

        if (ptr->enOp == OP_LOAD)
        {
            if (ptr->sLoad.pPort != NULL)
            {
                ptr->sLoad.pPort->unbind(this);
                ptr->sLoad.pPort = NULL;
            }
        }
        else
        {
            destroy_data(ptr->sCalc.pLeft);
            ptr->sCalc.pLeft = NULL;
            destroy_data(ptr->sCalc.pRight);
            ptr->sCalc.pRight = NULL;
        }

        // Delete binding
        delete ptr;
    }

    void Expression::destroy()
    {
        destroy_data(pRoot);
        pRoot = NULL;
        pListener = NULL;
    }

    void Expression::notify(IUIPort *port)
    {
        if (pListener != NULL)
            pListener->notify(port);
    }

    Expression::token_t Expression::get_token(tokenizer_t *t, bool get)
    {
        // Pre-checks
        if (!get)
            return t->enType;

        // Skip whitespace
        char c = t->sUnget;
        t->sUnget = '\0';

        while (c == '\0')
        {
            c = *(t->pStr);
            if (c == '\0')
            {
                t->sText[0] = '\0';
                return t->enType   = TT_EOF;
            }

            // Update pointer
            t->pStr++;

            // Leave cycle if not a whitespace
            if ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'))
                c = '\0';
        }

        // Now analyze first character
        switch (c)
        {
            case '(': // TT_LBRACE
                return t->enType   = TT_LBRACE;
            case ')': // TT_RBRACE
                return t->enType   = TT_RBRACE;
            case '+': // TT_ADD
                return t->enType   = TT_ADD;
            case '-': // TT_SUB
                return t->enType   = TT_SUB;
            case '*': // TT_MUL
                return t->enType   = TT_MUL;
            case '/': // TT_DIV
                return t->enType   = TT_DIV;
            case '%': // TT_MOD
                return t->enType   = TT_MOD;
            case '?': // TT_QUESTION
                return t->enType   = TT_QUESTION;
            case '&': // TT_AND
                if (*(t->pStr) == '&') // Accept both variants: & and &&
                    t->pStr ++;
                return t->enType   = TT_AND;

            case '|': // TT_OR
                if (*(t->pStr) == '|') // Accept both variants: | and ||
                    t->pStr ++;
                return t->enType   = TT_OR;

            case '!': // TT_NOT, TT_NOT_EQ
                if (*(t->pStr) == '=') // Check for !=
                {
                    t->pStr ++;
                    return t->enType   = TT_NOT_EQ;
                }
                return t->enType   = TT_NOT;

            case '^': // TT_XOR
                if (*(t->pStr) == '^') // Accept both variants: ^ and ^^
                    t->pStr ++;
                return t->enType   = TT_XOR;

            case '<': // TT_LESS, TT_LESS_EQ, TT_NOT_EQ
                c = *(t->pStr);
                if (c == '=') // <=
                {
                    t->pStr ++;
                    return t->enType    = TT_LESS_EQ;
                }
                else if (c == '>') // <>
                {
                    t->pStr ++;
                    return t->enType    = TT_NOT_EQ;
                }
                return t->enType    = TT_LESS;

            case '>': // TT_GREATER, TT_GREATER_EQ
                if (*(t->pStr) == '=') // Check for !=
                {
                    t->pStr ++;
                    return t->enType   = TT_GREATER_EQ;
                }
                return t->enType   = TT_GREATER;

            case '=': // TT_EQ
                if (*(t->pStr) == '=') // Accept both variants: = and ==
                    t->pStr ++;
                return t->enType   = TT_EQ;

            case ':': // TT_DOTS, TT_IDENTIFIER
            {
                // Check for next character
                c = *(t->pStr);
                if ((!isalpha(c)) && (c != '_'))
                    return t->enType    = TT_DOTS;

                size_t i=0;
                do
                {
                    if (i >= (EXPR_TOKEN_LENGTH-1))
                    {
                        t->sText[i] = '\0';
                        return t->enType    = TT_UNKNOWN;
                    }
                    t->sText[i++] = c;

                    c = *(++t->pStr);
                } while ((isalnum(c)) || (c == '_'));

                t->sText[i] = '\0';
                return t->enType    = TT_IDENTIFIER;
            }

            default: // TT_VALUE
            {
                const char *p = &t->pStr[-1];
                if (((p[0] == 'T') || (p[0] == 't')) &&
                    ((p[1] == 'R') || (p[1] == 'r')) &&
                    ((p[2] == 'U') || (p[2] == 'u')) &&
                    ((p[3] == 'E') || (p[3] == 'e')))
                {
                    t->fValue   = 1.0f;
                    return t->enType    = TT_VALUE;
                }
                else if (((p[0] == 'F') || (p[0] == 'f')) &&
                    ((p[1] == 'A') || (p[1] == 'a')) &&
                    ((p[2] == 'L') || (p[2] == 'l')) &&
                    ((p[3] == 'S') || (p[3] == 's')) &&
                    ((p[4] == 'E') || (p[4] == 'e')))
                {
                    t->fValue   = 0.0f;
                    return t->enType    = TT_VALUE;
                }

                // Parse float value
                char *endptr    = NULL;
                errno           = 0;
                t->fValue       = strtof(p, &endptr);
                if (errno != 0)
                    return t->enType    = TT_UNKNOWN;

                t->pStr     = endptr;
                return t->enType    = TT_VALUE;
            }
        }

        return t->enType    = TT_UNKNOWN;
    }

    float Expression::execute(binding_t *expr)
    {
        #define BINARY(key, op) case key: return execute(expr->sCalc.pLeft) op execute(expr->sCalc.pRight);
        #define UNARY(key, op) case key: return op execute(expr->sCalc.pLeft);

        if (expr == NULL)
            return 0.0f;
        switch (expr->enOp)
        {
            case OP_LOAD:
                return (expr->sLoad.pPort != NULL) ?
                    expr->sLoad.pPort->getValue() : expr->sLoad.fValue;

            BINARY(OP_ADD, +)
            BINARY(OP_SUB, -)
            UNARY(OP_SIGN, -)
            BINARY(OP_MUL, *)
            BINARY(OP_DIV, /)

            case OP_MOD:
                return long(execute(expr->sCalc.pLeft)) % long(execute(expr->sCalc.pRight));

            case OP_AND:
                if (execute(expr->sCalc.pLeft) < 0.5f)
                    return 0.0f;
                return (execute(expr->sCalc.pRight) >= 0.5f) ? 1.0f : 0.0f;

            case OP_OR:
                if (execute(expr->sCalc.pLeft) >= 0.5f)
                    return 1.0f;
                return (execute(expr->sCalc.pRight) >= 0.5f) ? 1.0f : 0.0f;

            case OP_NOT:
                return execute(expr->sCalc.pLeft) < 0.5f;

            case OP_TERNARY:
                return (execute(expr->sCalc.pCond) >= 0.5f) ?
                    execute(expr->sCalc.pLeft) :
                    execute(expr->sCalc.pRight);

            case OP_XOR:
            {
                bool a = execute(expr->sCalc.pLeft) >= 0.5f;
                bool b = execute(expr->sCalc.pRight) >= 0.5f;
                return (a ^ b) ? 1.0f : 0.0f;
            }

            BINARY(OP_LESS, <)
            BINARY(OP_GREATER, >)
            BINARY(OP_LESS_EQ, <=)
            BINARY(OP_GREATER_EQ, >=)
            BINARY(OP_NOT_EQ, !=)
            BINARY(OP_EQ, ==)

            default:
                return 0.0f;
        }
        #undef BINARY
        #undef UNARY

        return 0.0f;
    }

    float Expression::evaluate()
    {
        return (pRoot != NULL) ? execute(pRoot) : 0.0f;
    }

    Expression::binding_t *Expression::parse_ternary(tokenizer_t *t, bool get)
    {
        // Parse condition part
        binding_t *cond = parse_xor(t, get);
        if (cond == NULL)
            return NULL;

        // Check token
        token_t tok = get_token(t, false);
        if (tok != TT_QUESTION)
            return cond;

        // Parse left part
        binding_t *left = parse_ternary(t, true);
        if (left == NULL)
        {
            destroy_data(cond);
            return NULL;
        }

        // Check token
        tok = get_token(t, false);
        if (tok != TT_DOTS)
            return cond;

        // Parse right part
        binding_t *right = parse_ternary(t, true);
        if (right == NULL)
        {
            destroy_data(cond);
            destroy_data(left);
            return NULL;
        }

        // Create binding between left and right
        binding_t *bind     = new binding_t;
        if (bind == NULL)
        {
            destroy_data(cond);
            destroy_data(left);
            destroy_data(right);
            return NULL;
        }
        bind->enOp          = OP_TERNARY;
        bind->sCalc.pLeft   = left;
        bind->sCalc.pRight  = right;
        bind->sCalc.pCond   = cond;
        return bind;
    }

    Expression::binding_t *Expression::parse_xor(tokenizer_t *t, bool get)
    {
        // Parse left part
        binding_t *left = parse_or(t, get);
        if (left == NULL)
            return NULL;

        // Check token
        token_t tok = get_token(t, false);
        if (tok != TT_XOR)
            return left;

        // Parse right part
        binding_t *right = parse_xor(t, true);
        if (right == NULL)
        {
            destroy_data(left);
            return NULL;
        }

        // Create binding between left and right
        binding_t *bind     = new binding_t;
        if (bind == NULL)
        {
            destroy_data(left);
            destroy_data(right);
            return NULL;
        }
        bind->enOp          = OP_XOR;
        bind->sCalc.pLeft   = left;
        bind->sCalc.pRight  = right;
        return bind;
    }

    Expression::binding_t  *Expression::parse_or(tokenizer_t *t, bool get)
    {
        // Parse left part
        binding_t *left = parse_and(t, get);
        if (left == NULL)
            return NULL;

        // Check token
        token_t tok = get_token(t, false);
        if (tok != TT_OR)
            return left;

        // Parse right part
        binding_t *right = parse_or(t, true);
        if (right == NULL)
        {
            destroy_data(left);
            return NULL;
        }

        // Create binding between left and right
        binding_t *bind     = new binding_t;
        if (bind == NULL)
        {
            destroy_data(left);
            destroy_data(right);
            return NULL;
        }
        bind->enOp          = OP_OR;
        bind->sCalc.pLeft   = left;
        bind->sCalc.pRight  = right;
        return bind;
    }

    Expression::binding_t  *Expression::parse_and(tokenizer_t *t, bool get)
    {
        // Parse left part
        binding_t *left = parse_cmp(t, get);
        if (left == NULL)
            return NULL;

        // Check token
        token_t tok = get_token(t, false);
        if (tok != TT_AND)
            return left;

        // Parse right part
        binding_t *right = parse_and(t, true);
        if (right == NULL)
        {
            destroy_data(left);
            return NULL;
        }

        // Create binding between left and right
        binding_t *bind     = new binding_t;
        if (bind == NULL)
        {
            destroy_data(left);
            destroy_data(right);
            return NULL;
        }
        bind->enOp          = OP_AND;
        bind->sCalc.pLeft   = left;
        bind->sCalc.pRight  = right;
        return bind;
    }

    Expression::binding_t  *Expression::parse_cmp(tokenizer_t *t, bool get)
    {
        // Parse left part
        binding_t *left = parse_addsub(t, get);
        if (left == NULL)
            return NULL;

        // Check token
        token_t tok = get_token(t, false);
        switch (tok)
        {
            case TT_LESS:
            case TT_GREATER:
            case TT_LESS_EQ:
            case TT_GREATER_EQ:
            case TT_NOT_EQ:
            case TT_EQ:
                break;
            default:
                return left;
        }

        // Parse right part
        binding_t *right = parse_cmp(t, true);
        if (right == NULL)
        {
            destroy_data(left);
            return NULL;
        }

        // Create binding between left and right
        binding_t *bind     = new binding_t;
        if (bind == NULL)
        {
            destroy_data(left);
            destroy_data(right);
            return NULL;
        }

        switch (tok)
        {
            case TT_LESS: bind->enOp = OP_LESS; break;
            case TT_GREATER: bind->enOp = OP_GREATER; break;
            case TT_LESS_EQ: bind->enOp = OP_LESS_EQ; break;
            case TT_GREATER_EQ: bind->enOp = OP_GREATER_EQ; break;
            case TT_NOT_EQ: bind->enOp = OP_NOT_EQ; break;
            case TT_EQ: bind->enOp = OP_EQ; break;
            default:
            {
                destroy_data(left);
                destroy_data(right);
                destroy_data(bind);
                return NULL;
            }
        }
        bind->sCalc.pLeft   = left;
        bind->sCalc.pRight  = right;
        return bind;
    }

    Expression::binding_t  *Expression::parse_addsub(tokenizer_t *t, bool get)
    {
        // Parse left part
        binding_t *left = parse_muldiv(t, get);
        if (left == NULL)
            return NULL;

        // Check token
        token_t tok = get_token(t, false);
        if ((tok != TT_ADD) && (tok != TT_SUB))
            return left;

        // Parse right part
        binding_t *right = parse_addsub(t, true);
        if (right == NULL)
        {
            destroy_data(left);
            return NULL;
        }

        // Create binding between left and right
        binding_t *bind     = new binding_t;
        if (bind == NULL)
        {
            destroy_data(left);
            destroy_data(right);
            return NULL;
        }
        bind->enOp          = (tok == TT_ADD) ? OP_ADD : OP_SUB;
        bind->sCalc.pLeft   = left;
        bind->sCalc.pRight  = right;
        return bind;
    }

    Expression::binding_t  *Expression::parse_muldiv(tokenizer_t *t, bool get)
    {
        // Parse left part
        binding_t *left = parse_not(t, get);
        if (left == NULL)
            return NULL;

        // Check token
        token_t tok = get_token(t, false);
        if ((tok != TT_MUL) && (tok != TT_DIV) && (tok != TT_MOD))
            return left;

        // Parse right part
        binding_t *right = parse_muldiv(t, true);
        if (right == NULL)
        {
            destroy_data(left);
            return NULL;
        }

        // Create binding between left and right
        binding_t *bind     = new binding_t;
        if (bind == NULL)
        {
            destroy_data(left);
            destroy_data(right);
            return NULL;
        }
        bind->enOp          = (tok == TT_MUL) ? OP_MUL : (tok == TT_DIV) ? OP_DIV : OP_MOD;
        bind->sCalc.pLeft   = left;
        bind->sCalc.pRight  = right;
        return bind;
    }

    Expression::binding_t  *Expression::parse_not(tokenizer_t *t, bool get)
    {
        // Check token
        token_t tok = get_token(t, get);

        // Parse right part
        binding_t *right = (tok == TT_NOT) ?
            parse_not(t, true) :
            parse_sign(t, false);
        if ((right == NULL) || (tok != TT_NOT))
            return right;

        // Create binding between left and right
        binding_t *bind     = new binding_t;
        if (bind == NULL)
        {
            destroy_data(right);
            return NULL;
        }
        bind->enOp          = OP_NOT;
        bind->sCalc.pLeft   = right;
        bind->sCalc.pRight  = NULL;
        return bind;
    }

    Expression::binding_t  *Expression::parse_sign(tokenizer_t *t, bool get)
    {
        // Check token
        token_t tok = get_token(t, get);

        // Parse right part
        binding_t *right = (tok == TT_ADD) || (tok == TT_SUB) ?
            parse_sign(t, true) :
            parse_primary(t, false);
        if ((right == NULL) || (tok != TT_SUB))
            return right;

        // Create binding between left and right
        binding_t *bind     = new binding_t;
        if (bind == NULL)
        {
            destroy_data(right);
            return NULL;
        }
        bind->enOp          = OP_SIGN;
        bind->sCalc.pLeft   = right;
        bind->sCalc.pRight  = NULL;
        return bind;
    }

    Expression::binding_t  *Expression::parse_primary(tokenizer_t *t, bool get)
    {
        token_t tok = get_token(t, get);
        switch (tok)
        {
            case TT_IDENTIFIER:
            {
                binding_t *bind     = new binding_t;
                bind->enOp          = OP_LOAD;
                bind->sLoad.pPort   = pUI->port(t->sText);
                if (bind->sLoad.pPort != NULL)
                {
                    bind->sLoad.pPort->bind(this);
                    bind->sLoad.fValue  = bind->sLoad.pPort->getValue();
                }
                else
                    bind->sLoad.fValue  = 0.0f;
                get_token(t, true);
                return bind;
            }

            case TT_VALUE:
            {
                binding_t *bind     = new binding_t;
                bind->enOp          = OP_LOAD;
                bind->sLoad.pPort   = NULL;
                bind->sLoad.fValue  = t->fValue;
                get_token(t, true);
                return bind;
            }

            case TT_LBRACE:
            {
                binding_t *bind     = parse_expression(t, true);
                if (bind == NULL)
                    return bind;
                tok = get_token(t, false);
                if (tok == TT_RBRACE)
                {
                    get_token(t, true); // TODO: check that really needed
                    return bind;
                }

                destroy_data(bind);
                return NULL;
            }

            case TT_EOF:
                return NULL;

            default:
            case TT_UNKNOWN:
                return NULL;
        }
        return NULL;
    }

    bool Expression::parse(const char *expr)
    {
        // Drop previous data
        destroy_data(pRoot);
        pRoot       = NULL;
        if (expr == NULL)
            return true;

        char *saved_locale = setlocale(LC_NUMERIC, "C");

        // Initialize tokenizer
        tokenizer_t t;
        t.sText[0]  = '\0';
        t.fValue    = 0.0f;
        t.enType    = TT_UNKNOWN;
        t.sUnget    = '\0';
        t.pStr      = expr;

        // Parse expression
        pRoot       = parse_expression(&t, true);
        setlocale(LC_NUMERIC, saved_locale);
        if (pRoot == NULL)
            return false;

        if (get_token(&t, false) != TT_EOF)
        {
            destroy_data(pRoot);
            pRoot = NULL;
            return false;
        }

        return true;
    }
} /* namespace lsp */
