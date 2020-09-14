/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 9 авг. 2017 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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
