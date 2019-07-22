/*
 * CtlExpression.h
 *
 *  Created on: 23 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLEXPRESSION_H_
#define UI_CTL_CTLEXPRESSION_H_

namespace lsp
{
    namespace ctl
    {
        #define CTL_EXPR_TOKEN_LENGTH   128
        
        #define EXPR_FLAGS_MULTIPLE             (1 << 0)

        class CtlExpression: public CtlPortListener
        {
            private:
                CtlExpression & operator = (const CtlExpression &);

            protected:
                enum operation_t
                {
                    OP_LOAD,
                    OP_TERNARY,

                    // Floating-point operations
                    OP_ADD,
                    OP_SUB,
                    OP_SIGN,
                    OP_MUL,
                    OP_DIV,

                    // Integer operations
                    OP_IADD,
                    OP_ISUB,
                    OP_IMUL,
                    OP_POWER,
                    OP_IDIV,
                    OP_MOD,

                    // Logical operations
                    OP_AND,
                    OP_OR,
                    OP_NOT,
                    OP_XOR,

                    // Bitwise operations
                    OP_BAND,
                    OP_BOR,
                    OP_BNOT,
                    OP_BXOR,

                    // Floating-point comparisons
                    OP_LESS,
                    OP_GREATER,
                    OP_LESS_EQ,
                    OP_GREATER_EQ,
                    OP_NOT_EQ,
                    OP_EQ,

                    // Integer comparisons
                    OP_ILESS,
                    OP_IGREATER,
                    OP_ILESS_EQ,
                    OP_IGREATER_EQ,
                    OP_INOT_EQ,
                    OP_IEQ
                };

                enum token_t
                {
                    TT_UNKNOWN,

                    TT_IDENTIFIER,
                    TT_VALUE,

                    TT_LBRACE,
                    TT_RBRACE,

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
                    TT_DOTS,
                    TT_SEMICOLON,

                    TT_EOF
                };

                enum flags_t
                {
                    F_NONE      = 0,
                    F_GET       = 1 << 0,
                    F_XSIGN     = 1 << 1
                };

                typedef struct binding_t
                {
                    operation_t enOp;
                    union
                    {
                        struct
                        {
                            binding_t  *pLeft;
                            binding_t  *pRight;
                            binding_t  *pCond;
                        } sCalc;
                        struct
                        {
                            CtlPort    *pPort;
                            float       fValue;
                        } sLoad;
                    };
                } binding_t;

                typedef struct root_t
                {
                    binding_t      *binding;
                    float           result;
                } root_t;

                typedef struct tokenizer_t
                {
                    char        sText[CTL_EXPR_TOKEN_LENGTH];
                    float       fValue;
                    token_t     enType;
                    char        sUnget;
                    const char *pStr;
                } tokenizer_t;

            protected:
                cstorage<root_t>    vRoots;
                CtlRegistry        *pCtl;
                CtlPortListener    *pListener;

            protected:
                void        destroy_data(binding_t *ptr);
                void        destroy_all_data();
                static token_t get_token(tokenizer_t *t, size_t flags);
                static float execute(binding_t *expr);

                binding_t  *parse_ternary(tokenizer_t *t, size_t flags);
                binding_t  *parse_bit_xor(tokenizer_t *t, size_t flags);
                binding_t  *parse_bit_or(tokenizer_t *t, size_t flags);
                binding_t  *parse_bit_and(tokenizer_t *t, size_t flags);
                binding_t  *parse_xor(tokenizer_t *t, size_t flags);
                binding_t  *parse_or(tokenizer_t *t, size_t flags);
                binding_t  *parse_and(tokenizer_t *t, size_t flags);
                binding_t  *parse_cmp(tokenizer_t *t, size_t flags);
                binding_t  *parse_addsub(tokenizer_t *t, size_t flags);
                binding_t  *parse_muldiv(tokenizer_t *t, size_t flags);
                binding_t  *parse_power(tokenizer_t *t, size_t flags);
                binding_t  *parse_not(tokenizer_t *t, size_t flags);
                binding_t  *parse_sign(tokenizer_t *t, size_t flags);
                binding_t  *parse_exists(tokenizer_t *t, size_t flags);
                binding_t  *parse_primary(tokenizer_t *t, size_t flags);
                inline binding_t  *parse_expression(tokenizer_t *t, size_t flags) { return parse_ternary(t, flags); }

            public:
                explicit CtlExpression();
                virtual ~CtlExpression();

            public:
                virtual void notify(CtlPort *port);

            public:
                void    init(CtlRegistry *ctl, CtlPortListener *listener);
                void    destroy();

                float   evaluate();
                float   evaluate(size_t idx);
                inline size_t  results() { return vRoots.size(); }
                float   result(size_t idx);
                bool    parse(const char *expr, size_t flags = 0);
                inline bool valid() const { return vRoots.size() > 0; };
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_CTL_CTLEXPRESSION_H_ */
