/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 23 дек. 2017 г.
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

#ifndef UI_CTL_PARSE_H_
#define UI_CTL_PARSE_H_

#include <core/parse.h>

#define BIND_PORT(ctl, field, id) \
    { \
        field   = ctl->port(id); \
        if (field != NULL) \
            field->bind(this); \
    }

#define BIND_EXPR(field, expr) \
    (field).parse(expr);



namespace lsp
{
    namespace ctl
    {
        bool parse_file_formats(const char *variable, LSPFileFilter *flt);

        bool set_port_value(CtlPort *port, const char *value, size_t flags, const io::Path *base);

        status_t format_port_value(CtlPort *port, LSPString *name, LSPString *value, LSPString *comment, int *flags, const io::Path *base);
    }
}

#endif /* UI_CTL_PARSE_H_ */
