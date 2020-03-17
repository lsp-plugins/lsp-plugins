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
                class LocalString: public LSPLocalString
                {
                    private:
                        friend class LSPItem;

                    protected:
                        LSPItem *pItem;

                    protected:
                        virtual void        sync();

                    public:
                        inline LocalString(LSPItem *item) { pItem = item; }
                };

            protected:
                LocalString         sText;
                float               fValue;

            public:
                explicit LSPItem();
                explicit LSPItem(const LSPItem *src);
                virtual ~LSPItem();

            protected:
                virtual void        on_change();

            public:
                inline LSPLocalString          *text()          { return &sText;    }
                inline const LSPLocalString    *text() const    { return &sText;    }
                inline float                    value() const   { return fValue;    }

            public:
                status_t            set(const LSPItem *src);
                void                set_value(float value);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPITEM_H_ */
