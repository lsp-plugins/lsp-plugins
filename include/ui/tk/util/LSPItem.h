/*
 * LSPItem.h
 *
 *  Created on: 9 авг. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPITEM_H_
#define UI_TK_LSPITEM_H_

namespace lsp
{
    namespace tk
    {
        class LSPItem
        {
            protected:
                char       *sText;
                float       fValue;

            protected:
                void    construct(const char *text, float value);

            public:
                explicit LSPItem(const LSPItem *src);
                explicit LSPItem(float value = 0.0f);
                explicit LSPItem(const char *text, float value = 0.0f);
                virtual ~LSPItem();

            protected:
                virtual void        on_text_change();
                virtual void        on_value_change();
                virtual void        on_change();

            public:
                inline const char  *text() const    { return sText;     }
                inline float        value() const   { return fValue;    }

            public:
                status_t            set(const char *text, float value);
                status_t            set(const LSPItem *src);
                status_t            set_text(const char *text);
                void                set_value(float value);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPITEM_H_ */
