/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 12 июл. 2017 г.
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

#ifndef UI_CTL_CTLPADDING_H_
#define UI_CTL_CTLPADDING_H_

namespace lsp
{
    namespace ctl
    {
        class CtlPadding
        {
            protected:
                LSPPadding     *pPadding;
                ssize_t         vAttributes[5];

            public:
                explicit CtlPadding();
                virtual ~CtlPadding();

            public:
                void init(LSPPadding *padding, ssize_t l, ssize_t r, ssize_t t, ssize_t b, ssize_t c);

                inline void init(LSPPadding *padding)
                {
                    init(padding, A_PAD_LEFT, A_PAD_RIGHT, A_PAD_TOP, A_PAD_BOTTOM, A_PADDING);
                }

                bool set(widget_attribute_t att, const char *value);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLPADDING_H_ */
