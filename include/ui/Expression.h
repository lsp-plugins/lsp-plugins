/*
 * Expression.h
 *
 *  Created on: 28 окт. 2016 г.
 *      Author: sadko
 */

#ifndef UI_EXPRESSION_H_
#define UI_EXPRESSION_H_

namespace lsp
{
    #define EXPR_TOKEN_LENGTH       128

    class Expression: public IUIPortListener
    {
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

                // Different operators
                TT_QUESTION,
                TT_DOTS,

                TT_EOF
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
                        IUIPort    *pPort;
                        float       fValue;
                    } sLoad;
                };
            } binding_t;

            typedef struct tokenizer_t
            {
                char        sText[EXPR_TOKEN_LENGTH];
                float       fValue;
                token_t     enType;
                char        sUnget;
                const char *pStr;
            } tokenizer_t;

        protected:
            binding_t          *pRoot;
            plugin_ui          *pUI;
            IUIPortListener    *pListener;

        protected:
            void        destroy_data(binding_t *ptr);
            static token_t get_token(tokenizer_t *t, bool get);
            static float execute(binding_t *expr);

            static inline bool isalpha(char ch);
            static inline bool isnum(char ch);
            static inline bool isalnum(char ch);

            binding_t  *parse_ternary(tokenizer_t *t, bool get);
            binding_t  *parse_bit_xor(tokenizer_t *t, bool get);
            binding_t  *parse_bit_or(tokenizer_t *t, bool get);
            binding_t  *parse_bit_and(tokenizer_t *t, bool get);
            binding_t  *parse_xor(tokenizer_t *t, bool get);
            binding_t  *parse_or(tokenizer_t *t, bool get);
            binding_t  *parse_and(tokenizer_t *t, bool get);
            binding_t  *parse_cmp(tokenizer_t *t, bool get);
            binding_t  *parse_addsub(tokenizer_t *t, bool get);
            binding_t  *parse_muldiv(tokenizer_t *t, bool get);
            binding_t  *parse_not(tokenizer_t *t, bool get);
            binding_t  *parse_sign(tokenizer_t *t, bool get);
            binding_t  *parse_primary(tokenizer_t *t, bool get);
            inline binding_t  *parse_expression(tokenizer_t *t, bool get) { return parse_ternary(t, get); }

        public:
            Expression();
            virtual ~Expression();

        public:
            virtual void notify(IUIPort *port);

        public:
            void    init(plugin_ui *ui, IUIPortListener *listener);
            void    destroy();

            float   evaluate();
            bool    parse(const char *expr);
            inline bool valid() const { return pRoot != NULL; };
    };

} /* namespace lsp */

#endif /* UI_EXPRESSION_H_ */
