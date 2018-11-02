/*
 * parse.h
 *
 *  Created on: 23 дек. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_PARSE_H_
#define UI_CTL_PARSE_H_

// Data parsing
#define PARSE_INT(var, code) \
    { \
        errno = 0; \
        long __ = strtol(var, NULL, 10); \
        if (errno == 0) \
            { code; } \
    }

#define PARSE_BOOL(var, code) \
    { \
        bool __ = !strcasecmp(var, "true"); \
        if (! __ ) \
            __ = !strcasecmp(var, "1"); \
        { code; } \
    }

#define PARSE_FLAG(var, dst, flag) PARSE_BOOL(var, if (__) dst |= flag; else dst &= ~flag)
#define UPDATE_LOCALE(out_var, lc, value) \
       char *out_var = setlocale(lc, NULL); \
       if (out_var != NULL) \
       { \
           size_t ___len = strlen(out_var) + 1; \
           char *___copy = static_cast<char *>(alloca(___len)); \
           memcpy(___copy, out_var, ___len); \
           out_var = ___copy; \
       } \
       setlocale(lc, value);

#define PARSE_FLOAT(var, code) \
    { \
        float __; \
        if (parse_float(var, &__)) \
            { code; } \
    }

#define PARSE_DOUBLE(var, code) \
    { \
        double __; \
        if (parse_double(var, &__)) \
            { code; } \
    }

#define BIND_PORT(ctl, field, id) \
    { \
        field   = ctl->port(id); \
        if (field != NULL) \
            field->bind(this); \
    }

#define BIND_EXPR(field, expr) \
    (field).parse(expr);

#define PARSE_STRING(var, store) \
    { \
        char *__ = strdup(var);\
        if (__ != NULL) \
        { \
            if ((store) != NULL) \
                free(store); \
            store = __; \
        } \
    }

namespace lsp
{
    namespace ctl
    {
        bool parse_float(const char *variable, float *res);

        bool parse_double(const char *variable, double *res);

        bool parse_file_formats(const char *variable, LSPFileFilter *flt);

        bool set_port_value(CtlPort *port, const char *value);

        status_t format_port_value(CtlPort *port, LSPString *name, LSPString *value, LSPString *comment, int *flags);
    }
}

#endif /* UI_CTL_PARSE_H_ */
