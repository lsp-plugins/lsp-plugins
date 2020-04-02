/*
 * metadata.cpp
 *
 *  Created on: 08 окт. 2015 г.
 *      Author: sadko
 */

#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <core/debug.h>
#include <core/alloc.h>

#include <stdio.h>
#include <math.h>
#include <locale.h>
#include <errno.h>
#include <stdlib.h>

#define UPDATE_LOCALE(out_var, lc, value) \
       char *out_var = ::setlocale(lc, NULL); \
       if (out_var != NULL) \
       { \
           size_t ___len = ::strlen(out_var) + 1; \
           char *___copy = static_cast<char *>(::alloca(___len)); \
           ::memcpy(___copy, out_var, ___len); \
           out_var = ___copy; \
       } \
       ::setlocale(lc, value);

namespace lsp
{
    const port_t lv2_atom_ports[] =
    {
        // Input audio ports
        { LSP_LV2_ATOM_PORT_IN,     "UI Input",     U_NONE,         R_UI_SYNC, F_IN, 0, 0, 0, 0, NULL       },
        { LSP_LV2_ATOM_PORT_OUT,    "UI Output",    U_NONE,         R_UI_SYNC, F_OUT, 0, 0, 0, 0, NULL      },

        { NULL, NULL }
    };

    const port_t lv2_latency_port =
    {
        LSP_LV2_LATENCY_PORT,   "Latency OUT",          U_NONE,         R_CONTROL, F_OUT | F_INT | F_LOWER | F_UPPER, 0, MAX_SAMPLE_RATE, 0, 0, NULL
    };

    typedef struct unit_desc_t
    {
        const char *name;
        const char *lc_key;
    } unit_desc_t;

    const unit_desc_t unit_desc[] =
    {
        { NULL,     NULL },
        { NULL,     NULL },
        { NULL,     NULL },
        { "%",      "units.pc" },

        { "mm",     "units.mm" },
        { "cm",     "units.cm" },
        { "m",      "units.m" },
        { "\"",     "units.inch" },
        { "km",     "units.km" },

        { "m/s",    "units.mps" },
        { "km/h",   "units.kmph" },

        { "samp",   "units.samp" },

        { "Hz",     "units.hz" },
        { "kHz",    "units.khz" },
        { "MHz",    "units.mhz" },
        { "bpm",    "units.bpm" },

        { "cent",   "units.cent" },
        { "oct",    "units.octave" },
        { "st",     "units.st" },

        { "bar",    "units.bar" },
        { "beat",   "units.beat" },
        { "min",    "units.min" },
        { "s",      "units.s" },
        { "ms",     "units.ms" },

        { "dB",     "units.db" },
        { "G",      "units.gain" },
        { "G",      "units.gain" },

        { "°",      "units.deg" },
        { "°C",     "units.degc" },
        { "°F",     "units.degf" },
        { "°K",     "units.degk" },
        { "°R",     "units.degr" },

        NULL
    };

    static port_item_t default_bool[] =
    {
        { "off",    "bool.off" },
        { "on",     "bool.on" },
        { NULL, NULL }
    };

    const char *encode_unit(size_t unit)
    {
        return ((unit >= 0) && (unit <= U_ENUM)) ?
                unit_desc[unit].name : NULL;
    }

    const char *unit_lc_key(size_t unit)
    {
        return ((unit >= 0) && (unit <= U_ENUM)) ?
                unit_desc[unit].lc_key : NULL;
    }

    unit_t decode_unit(const char *name)
    {
        for (ssize_t i=0; i<= U_ENUM; ++i)
        {
            const char *uname = unit_desc[i].name;
            if ((uname != NULL) && (!::strcmp(name, uname)))
                return unit_t(i);
        }
        return U_NONE;
    }

    bool is_discrete_unit(size_t unit)
    {
        switch (unit)
        {
            case U_BOOL:
            case U_SAMPLES:
            case U_ENUM:
                return true;
            default:
                break;
        }
        return false;
    }

    bool is_decibel_unit(size_t unit)
    {
        switch (unit)
        {
            case U_DB:
            case U_GAIN_AMP:
            case U_GAIN_POW:
                return true;
            default:
                break;
        }
        return false;
    }

    bool is_degree_unit(size_t unit)
    {
        switch (unit)
        {
            case U_DEG:
            case U_DEG_CEL:
            case U_DEG_FAR:
            case U_DEG_K:
            case U_DEG_R:
                return true;
            default:
                break;
        }
        return false;
    }

    bool is_log_rule(const port_t *port)
    {
        if (port->flags & F_LOG)
            return true;
        return is_decibel_unit(port->unit);
    }

    size_t list_size(const port_item_t *list)
    {
        size_t size = 0;
        for ( ; (list != NULL) && (list->text != NULL); ++list)
            ++size;
        return size;
    }

    float limit_value(const port_t *port, float value)
    {
        if ((port->flags & (F_CYCLIC | F_UPPER | F_LOWER)) == (F_CYCLIC | F_UPPER | F_LOWER))
        {
            if (port->max > port->min)
            {
                value = port->min + fmodf(value - port->min, port->max - port->min);
                if (value < port->min)
                    value  += port->max - port->min;
            }
            else if (port->min > port->max)
            {
                value = port->max + fmodf(value - port->max, port->min - port->max);
                if (value < port->max)
                    value  += port->min - port->max;
            }
        }

        if (port->flags & F_UPPER)
        {
            if (value > port->max)
                value = port->max;
        }
        if (port->flags & F_LOWER)
        {
            if (value < port->min)
                value = port->min;
        }
        return value;
    }

    void format_float(char *buf, size_t len, const port_t *meta, float value, ssize_t precision)
    {
        float v = (value < 0.0f) ? - value : value;
        size_t tolerance    = 0;

        // Select the tolerance of output value
        if (precision < 0)
        {
            // Determine regular tolerance
            if (v < 0.1f)
                tolerance   = 4;
            else if (v < 1.0f)
                tolerance   = 3;
            else if (v < 10.0f)
                tolerance   = 2;
            else if (v < 100.0f)
                tolerance   = 1;
            else
                tolerance   = 0;

            // Now determine normal tolerance
            if (meta->flags & F_STEP)
            {
                size_t max_tol = 0;
                float step      = (meta->step < 0.0f) ? - meta->step : meta->step;
                while ((max_tol < 4) && (truncf(step) <= 0))
                {
                    step   *= 10;
                    max_tol++;
                }

                if (tolerance > max_tol)
                    tolerance = max_tol;
            }
        }
        else
            tolerance   = (precision > 4) ? 4 : precision;

        const char *fmt = "%.0f";
        switch (tolerance)
        {
            case 4:     fmt = "%.4f"; break;
            case 3:     fmt = "%.3f"; break;
            case 2:     fmt = "%.2f"; break;
            case 1:     fmt = "%.1f"; break;
            default:    fmt = "%.0f"; break;
        };

        snprintf(buf, len, fmt, value);
        buf[len - 1] = '\0';
    }

    void format_int(char *buf, size_t len, const port_t *meta, float value)
    {
        snprintf(buf, len, "%ld", long(value));
        buf[len - 1] = '\0';
    }

    void format_enum(char *buf, size_t len, const port_t *meta, float value)
    {
        float min   = (meta->flags & F_LOWER) ? meta->min: 0;
        float step  = (meta->flags & F_STEP) ? meta->step : 1.0;

        for (const port_item_t *p = meta->items; (p != NULL) && (p->text != NULL); ++p)
        {
            if (min >= value)
            {
                ::strncpy(buf, p->text, len);
                buf[len - 1] = '\0';
                return;
            }
            min    += step;
        }
        buf[0] = '\0';
    }

    void format_decibels(char *buf, size_t len, const port_t *meta, float value, ssize_t precision)
    {
        double mul      = (meta->unit == U_GAIN_AMP) ? 20.0 : 10.0;
        if (value < 0.0f)
            value           = - value;

        value = mul * log(value) / M_LN10;
        float thresh    = (meta->flags & F_EXT) ? -140.0f : -80.0f;
        if (value <= thresh)
        {
            strcpy(buf, "-inf");
            return;
        }

        const char *fmt;
        if (precision < 0)
            fmt = "%.2f";
        else if (precision == 1)
            fmt = "%.1f";
        else if (precision == 2)
            fmt = "%.2f";
        else if (precision == 3)
            fmt = "%.3f";
        else
            fmt = "%.4f";

        snprintf(buf, len, fmt, value);
        buf[len - 1] = '\0';
    }

    void format_bool(char *buf, size_t len, const port_t *meta, float value)
    {
        const port_item_t *list = (meta->items != NULL) ? meta->items : default_bool;
        if (value >= 0.5f)
            ++list;

        if (list->text != NULL)
        {
            ::strncpy(buf, list->text, len);
            buf[len-1] = '\0';
        }
        else
            buf[0] = '\0';
    }

    void format_value(char *buf, size_t len, const port_t *meta, float value, ssize_t precision)
    {
        if (meta->unit == U_BOOL)
            format_bool(buf, len, meta, value);
        else if (meta->unit == U_ENUM)
            format_enum(buf, len, meta, value);
        else if ((meta->unit == U_GAIN_AMP) || (meta->unit == U_GAIN_POW))
            format_decibels(buf, len, meta, value, precision);
        else if (meta->flags & F_INT)
            format_int(buf, len, meta, value);
        else
            format_float(buf, len, meta, value, precision);
    }

    status_t parse_bool(float *dst, const char *text)
    {
        if ((!::strcasecmp(text, "true")) ||
            (!::strcasecmp(text, "on")) ||
            (!::strcasecmp(text, "1")))
        {
            if (dst != NULL)
                *dst    = 1.0f;
            return STATUS_OK;
        }

        if ((!::strcasecmp(text, "false")) ||
            (!::strcasecmp(text, "off")) ||
            (!::strcasecmp(text, "0")))
        {
            if (dst != NULL)
                *dst    = 0.0f;
            return STATUS_OK;
        }

        return STATUS_INVALID_VALUE;
    }

    status_t parse_enum(float *dst, const char *text, const port_t *meta)
    {
        float min   = (meta->flags & F_LOWER) ? meta->min: 0;
        float step  = (meta->flags & F_STEP) ? meta->step : 1.0;

        for (const port_item_t *p = meta->items; (p != NULL) && (p->text != NULL); ++p)
        {
            if (!::strcasecmp(text, p->text))
            {
                if (dst != NULL)
                    *dst    = min;
                return STATUS_OK;
            }
            min    += step;
        }

        return STATUS_INVALID_VALUE;
    }

    status_t parse_decibels(float *dst, const char *text, const port_t *meta)
    {
        if (!::strcasecmp(text, "-inf"))
        {
            if (dst != NULL)
                *dst = 0.0f;
            return STATUS_OK;
        }

        float mul   = (meta->unit == U_GAIN_AMP) ? 0.05f : 0.1f;

        // Parse float value
        UPDATE_LOCALE(saved_locale, LC_NUMERIC, "C");
        errno       = 0;
        char *end   = NULL;
        float value = ::strtof(text, &end);
        status_t res= STATUS_INVALID_VALUE;

        if ((*end == '\0') && (errno == 0))
        {
            if (dst != NULL)
                *dst    = ::expf(value * M_LN10 * mul);
            res     = STATUS_OK;
        }

        if (saved_locale != NULL)
            ::setlocale(LC_NUMERIC, saved_locale);

        return res;
    }

    status_t parse_int(float *dst, const char *text, const port_t *meta)
    {
        errno       = 0;
        char *end   = NULL;
        long value  = ::strtol(text, &end, 10);
        if ((*end == '\0') && (errno == 0))
        {
            if (dst != NULL)
                *dst        = value;
            return STATUS_OK;
        }

        return STATUS_INVALID_VALUE;
    }

    status_t parse_float(float *dst, const char *text, const port_t *meta)
    {
        // Parse float value
        UPDATE_LOCALE(saved_locale, LC_NUMERIC, "C");
        errno       = 0;
        char *end   = NULL;
        float value = ::strtof(text, &end);
        status_t res= STATUS_INVALID_VALUE;

        if ((*end == '\0') && (errno == 0))
        {
            if (dst != NULL)
                *dst    = value;
            res     = STATUS_OK;
        }

        if (saved_locale != NULL)
            ::setlocale(LC_NUMERIC, saved_locale);

        return res;
    }

    status_t parse_value(float *dst, const char *text, const port_t *meta)
    {
        if ((text == NULL) || (meta == NULL) || (*text == '\0'))
            return STATUS_BAD_ARGUMENTS;

        if (meta->unit == U_BOOL)
            return parse_bool(dst, text);
        else if (meta->unit == U_ENUM)
            return parse_enum(dst, text, meta);
        else if ((meta->unit == U_GAIN_AMP) || (meta->unit == U_GAIN_POW))
            return parse_decibels(dst, text, meta);
        else if (meta->flags & F_INT)
            return parse_int(dst, text, meta);
        else
            return parse_float(dst, text, meta);

        return STATUS_BAD_ARGUMENTS;
    }

    void get_port_parameters(const port_t *p, float *min, float *max, float *step)
    {
        float f_min = 0.0f, f_max = 1.0f, f_step = 0.001f;

        if (p->unit == U_BOOL)
        {
            f_min       = 0.0f;
            f_max       = 1.0f;
            f_step      = 1.0f;
        }
        else if (p->unit == U_ENUM)
        {
            f_min       = (p->flags & F_LOWER) ? p->min : 0.0f;
            f_max       = f_min + list_size(p->items) - 1;
            f_step      = 1.0f;
        }
        else if (p->unit == U_SAMPLES)
        {
            f_min       = p->min;
            f_max       = p->max;
            f_step      = 1.0f;
        }
        else
        {
            f_min       = (p->flags & F_LOWER) ? p->min : 0.0f;
            f_max       = (p->flags & F_UPPER) ? p->max : 1.0f;

            if (p->flags & F_INT)
                f_step      = (p->flags & F_STEP) ? p->step : 1.0f;
            else
                f_step      = (p->flags & F_STEP) ? p->step : (f_max - f_min) * 0.001;
        }

        if (min != NULL)
            *min        = f_min;
        if (max != NULL)
            *max        = f_max;
        if (step != NULL)
            *step       = f_step;
    }

    port_t *clone_port_metadata(const port_t *metadata, const char *postfix)
    {
        if (metadata == NULL)
            return NULL;

        size_t  postfix_len     = (postfix != NULL) ? strlen(postfix) : 0;
        size_t  string_bytes    = 0;
        size_t  elements        = 1; // At least PORTS_END should be present

        for (const port_t *p=metadata; p->id != NULL; ++p)
        {
            elements        ++;
            if (postfix_len > 0)
                string_bytes    += strlen(p->id) + postfix_len + 1;
        }

        // Calculate the overall allocation size
        size_t to_copy          = sizeof(port_t) * elements;
        string_bytes            = ALIGN_SIZE(string_bytes, DEFAULT_ALIGN);
        elements                = ALIGN_SIZE(to_copy, DEFAULT_ALIGN);
        size_t allocate         = string_bytes + elements;
        uint8_t *ptr            = lsp_tmalloc(uint8_t, allocate);
        port_t *meta            = reinterpret_cast<port_t *>(ptr);

        // Copy port metadata
        ::memcpy(meta, metadata, to_copy);

        // Update identifiers if needed
        if (postfix_len > 0)
        {
            port_t *m               = meta;
            char *dst               = reinterpret_cast<char *>(ptr + elements);

            for (const port_t *p=metadata; p->id != NULL; ++p, ++m)
            {
                m->id                   = dst;
                size_t slen             = strlen(p->id);
                memcpy(dst, p->id, slen);
                dst                    += slen;
                memcpy(dst, postfix, postfix_len);
                dst                    += postfix_len;
                *(dst++)                = '\0';
            }

            lsp_paranoia
            (
                ptr        += allocate;
                lsp_assert(ptr >= reinterpret_cast<uint8_t *>(dst));
            );
        }

        return meta;
    }

    void drop_port_metadata(port_t *metadata)
    {
        if (metadata == NULL)
            return;
        lsp_free(metadata);
    }

    size_t port_list_size(const port_t *metadata)
    {
        size_t count = 0;
        while (metadata->id != NULL)
        {
            count       ++;
            metadata    ++;
        }
        return count;
    }
}


