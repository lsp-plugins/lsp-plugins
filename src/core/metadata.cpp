/*
 * metadata.cpp
 *
 *  Created on: 08 окт. 2015 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/metadata.h>

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

namespace lsp
{
    const port_t lv2_atom_ports[] =
    {
        // Input audio ports
        { LSP_LV2_ATOM_PORT_IN,     "Communication IN",     U_NONE,         R_UI_SYNC, F_IN, 0, 0, 0, 0, NULL       },
        { LSP_LV2_ATOM_PORT_OUT,    "Communication OUT",    U_NONE,         R_UI_SYNC, F_OUT, 0, 0, 0, 0, NULL      },

        { NULL, NULL }
    };

    const port_t lv2_latency_port =
    {
        LSP_LV2_LATENCY_PORT,   "Latency OUT",          U_NONE,         R_CONTROL, F_OUT | F_INT | F_LOWER | F_UPPER, 0, MAX_SAMPLE_RATE, 0, 0, NULL
    };

    const char *unit_names[] =
    {
        NULL,
        NULL,
        "%",

        "mm",
        "cm",
        "m",
        "\"",
        "km",

        "samp",

        "Hz",
        "kHz",
        "MHz",
        "bpm",

        "cent",

        "bar",
        "beat",
        "s",
        "ms",

        "dB",
        "G",
        "G",

        "°C",
        "°F",
        "°K",

        NULL
    };

    static const char *default_bool[] =
    {
        "off", "on", NULL
    };

    const char *encode_unit(size_t unit)
    {
        if ((unit >= 0) && (unit <= U_ENUM))
            return unit_names[unit];

        return NULL;
    }

    unit_t decode_unit(const char *name)
    {
        for (ssize_t i=0; i<= U_ENUM; ++i)
        {
            const char *uname = unit_names[i];
            if ((uname != NULL) && (!strcmp(name, uname)))
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

    size_t list_size(const char **list)
    {
        size_t size = 0;
        while ((list != NULL) && (*list != NULL))
        {
            size    ++;
            list    ++;
        }
        return size;
    }

    void format_float(char *buf, size_t len, const port_t *meta, float value)
    {
        if (value < 0.0f)
            value           = - value;
        size_t tolerance    = 0;

        // Determine tolerance
        if (value < 0.1)
            tolerance   = 4;
        else if (value < 1.0)
            tolerance   = 3;
        else if (value < 10.0)
            tolerance   = 2;
        else if (value < 100.0)
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
//        float max   = meta->min + list_size(meta->items) - 1.0f;
        float step  = (meta->flags & F_STEP) ? meta->step : 1.0;

        for (const char **p = meta->items; (p != NULL) && (*p != NULL); ++p)
        {
            if (min >= value)
            {
                strncpy(buf, *p, len);
                buf[len - 1] = '\0';
                return;
            }
            min    += step;
        }
        buf[0] = '\0';
    }

    void format_decibels(char *buf, size_t len, const port_t *meta, float value)
    {
        double mul       = (meta->unit == U_GAIN_AMP) ? 20.0 : 10.0;
        if (value < 0.0f)
            value           = - value;

        value = mul * log(value) / M_LN10;
        if (value <= -75.0)
        {
            strcpy(buf, "-inf");
            return;
        }
        snprintf(buf, len, "%.2f", value);
        buf[len - 1] = '\0';
    }

    void format_bool(char *buf, size_t len, const port_t *meta, float value)
    {
        const char **list = (meta->items != NULL) ? meta->items : default_bool;
        if (value >= 0.5f)
            list++;

        if (*list != NULL)
        {
            strncpy(buf, *list, len);
            buf[len-1] = '\0';
        }
        else
            buf[0] = '\0';
    }

    void format_value(char *buf, size_t len, const port_t *meta, float value)
    {
        if (meta->unit == U_BOOL)
            format_bool(buf, len, meta, value);
        else if (meta->unit == U_ENUM)
            format_enum(buf, len, meta, value);
        else
        {
            if ((meta->unit == U_GAIN_AMP) || (meta->unit == U_GAIN_POW))
                format_decibels(buf, len, meta, value);
            else if (meta->flags & F_INT)
                format_int(buf, len, meta, value);
            else
                format_float(buf, len, meta, value);
        }
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
}


