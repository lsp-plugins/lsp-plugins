/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 21 авг. 2019 г.
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

#ifndef UI_TK_UTIL_LSPTEXTDATASOURCE_H_
#define UI_TK_UTIL_LSPTEXTDATASOURCE_H_

namespace lsp
{
    namespace tk
    {
        class LSPTextDataSource: public ws::IDataSource
        {
            protected:
                LSPString   sText;

            public:
                explicit LSPTextDataSource();
                virtual ~LSPTextDataSource();

            public:
                /**
                 * Set UTF-8 encoded text
                 * @param text UTF-8 encoded text
                 * @return status of operation
                 */
                status_t set_text(const char *text);

                /**
                 * Set text from the LSPString object
                 * @param text text
                 * @return status of operation
                 */
                status_t set_text(const LSPString *text);

                /**
                 * Set text from the LSPString object
                 * @param text text
                 * @param first the first character of the source string to use
                 * @return status of operation
                 */
                status_t set_text(const LSPString *text, ssize_t first);

                /**
                 * Set text from the LSPString object
                 * @param text text
                 * @param first the first character to start from
                 * @param last the last character to end
                 * @return status of operation
                 */
                status_t set_text(const LSPString *text, ssize_t first, ssize_t last);

            public:
                virtual io::IInStream   *open(const char *mime);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPTEXTDATASOURCE_H_ */
