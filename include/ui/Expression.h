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

                OP_ADD,
                OP_SUB,
                OP_SIGN,
                OP_MUL,
                OP_DIV,

                OP_AND,
                OP_OR,
                OP_NOT,
                OP_XOR,

                OP_LESS,
                OP_GREATER,
                OP_LESS_EQ,
                OP_GREATER_EQ,
                OP_NOT_EQ,
                OP_EQ
            };

            enum token_t
            {
                TT_UNKNOWN,

                TT_IDENTIFIER,
                TT_VALUE,

                TT_LBRACE,
                TT_RBRACE,
                TT_ADD,
                TT_SUB,
                TT_MUL,
                TT_DIV,
                TT_AND,
                TT_OR,
                TT_NOT,
                TT_XOR,
                TT_LESS,
                TT_GREATER,
                TT_LESS_EQ,
                TT_GREATER_EQ,
                TT_NOT_EQ,
                TT_EQ,
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

            binding_t  *parse_ternary(tokenizer_t *t, bool get);
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
