/*
 * parse.h
 *
 *  Created on: 23 дек. 2017 г.
 *      Author: sadko
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

        bool set_port_value(CtlPort *port, const char *value, size_t flags);

        status_t format_port_value(CtlPort *port, LSPString *name, LSPString *value, LSPString *comment, int *flags);
    }
}

#endif /* UI_CTL_PARSE_H_ */
