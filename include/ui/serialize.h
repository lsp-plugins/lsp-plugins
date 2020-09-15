/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 21 июн. 2017 г.
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

#ifndef UI_SERIALIZE_H_
#define UI_SERIALIZE_H_

#include <ui/tk/tk.h>

namespace lsp
{
    /** Load theme from XML
     *
     * @param theme theme to store data
     * @param uri uri of the resource
     * @return true if theme was loaded
     */
    status_t    load_theme(tk::LSPTheme *theme, const LSPString *uri);

    /** Load theme from XML
     *
     * @param theme theme to store data
     * @param uri uri of the resource
     * @return true if theme was loaded
     */
    status_t    load_theme(tk::LSPTheme *theme, const char *uri);
}

#endif /* UI_SERIALIZE_H_ */
