/*
 * Gtk2Indicator.cpp
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>
#include <core/debug.h>

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

namespace lsp
{
    typedef struct rect_t
    {
        int x, y, w, h;
    } rect_t;

    const rect_t segments[] =
    {
        {   4,  6,  1,  5   },  // 0
        {   0, 10,  1,  5   },  // 1
        {   0,  2,  1,  5   },  // 2
        {   2,  0,  5,  1   },  // 3
        {   8,  2,  1,  5   },  // 4
        {   8, 10,  1,  5   },  // 5
        {   2, 16,  5,  1   },  // 6
        {   2,  8,  5,  1   },  // 7
        {  12, 16,  1,  1   },  // 8
        {  12,  5,  1,  1   },  // 9
        {  12, 11,  1,  1   },  // 10
//        {   4,  6,  2,  6   },  // 0
//        {   0, 10,  2,  6   },  // 1
//        {   0,  2,  2,  6   },  // 2
//        {   2,  0,  6,  2   },  // 3
//        {   8,  2,  2,  6   },  // 4
//        {   8, 10,  2,  6   },  // 5
//        {   2, 16,  6,  2   },  // 6
//        {   2,  8,  6,  2   },  // 7
//        {  11, 16,  2,  2   }   // 8
    };


    Gtk2Indicator::Gtk2Indicator(plugin_ui *ui): Gtk2CustomWidget(ui)
    {
        sBgColor.set(pUI->theme(), C_BACKGROUND);
        sColor.set(pUI->theme(), C_GLASS);
        sTextColor.set(pUI->theme(), C_LABEL_TEXT);

        pPort       = NULL;
        fValue      = 0.0f;
        nFormat     = F_UNKNOWN;
        sDigits     = DIGITS_DFL;
        nItems      = 0;
        nFlags      = 0;
    }

    Gtk2Indicator::~Gtk2Indicator()
    {
    }

    bool Gtk2Indicator::parseLong(char *p, char **ret, long *value)
    {
        *ret        = p;
        if (!isdigit(*p))
            return false;

        errno       = 0;
        long digits = strtol(p, ret, 10);
        if (errno != 0)
            return false;

        *value      = digits;
        return true;
    }

    bool Gtk2Indicator::parseFormat(const char *format)
    {
        nFormat     = F_UNKNOWN;
        sDigits     = 0;
        nItems      = 0;
        nFlags      = 0;

        // Get predicates
        char *p         = const_cast<char *>(format);
        while (true)
        {
            if (*p == '+')
                nFlags      |= F_PLUS;
            else if (*p == '-')
            {
                nFlags      |= F_SIGN;
                sDigits     ++;
            }
            else if (*p == '0')
                nFlags      |= F_PAD_ZERO;
            else
                break;
            p++;
        }

        char c      = *(p++);
        if (c == '\0')
            return false;

        // Single value ?
        if ((c == 'f') || (c == 'i'))
        {
            if ((*p) == 'x')
            {
                nFlags     |= F_NO_ZERO;
                p++;
            }

            long digits = DIGITS_DFL;
            parseLong(p, &p, &digits);

            // Add item
            item_t *item    = &vItems[nItems++];
            item->type      = c;
            item->digits    = digits;
            item->precision = 0;

            sDigits        += digits;

            if (c == 'i')
            {
                nFormat     = F_INT;
                return (*p == '\0');
            }

            nFormat     = F_FLOAT;

            if (*p == '.')
                nFlags     |= F_DOT;
            else if (*p != ',')
                return (*p == '\0');

            p++;
            if (parseLong(p, &p, &digits))
                item->precision = (digits < 0) ? 0 : digits;

            if (*p == '!')
            {
                nFlags     |= F_FIXED_PREC;
                p++;
            }
            else if (*p == '+')
            {
                nFlags     |= F_TOLERANCE;
                p++;
            }

            return (*p == '\0');
        }

        // Time
        nFormat         = F_TIME;
        if (nFlags & F_PLUS)
            sDigits         ++;
        while (true)
        {
            switch (c)
            {
                case ':': // Separators, they do not take digits
                case '.':
                {
                    item_t *item    = &vItems[nItems++];
                    item->type      = c;
                    item->digits    = 0;
                    item->precision = 0;
                    break;
                }
                case 'u': // Microseconds 000000-999999
                {
                    long digits     = 6;
                    parseLong(p, &p, &digits);

                    item_t *item    = &vItems[nItems++];
                    item->type      = c;
                    item->digits    = digits;
                    item->precision = 0;
                    sDigits        += digits;
                    break;
                }
                case 'S': // Second 00-59
                case 'M': // Minute 00-59
                case 'H': // Hour 00-23
                case 'h': // Hour 01-12
                {
                    item_t *item    = &vItems[nItems++];
                    item->type      = c;
                    item->digits    = 2;
                    item->precision = 0;
                    sDigits        += 2;
                    break;
                }
                case 'D': // Day
                {
                    long digits         = 1;
                    parseLong(p, &p, &digits);
                    if (digits <= 0)
                        digits      = 1;

                    item_t *item    = &vItems[nItems++];
                    item->type      = c;
                    item->digits    = digits;
                    item->precision = 0;
                    sDigits        += digits;
                    break;
                }
                default:
                    return false;
            }

            // Next character
            c   = *(p++);
            if (c == '\0')
                return true;
        }

        return false;
    }

    void Gtk2Indicator::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_ID:
                pPort       = pUI->port(value);
                if (pPort != NULL)
                    pPort->bind(this);
                break;
            case A_FORMAT:
                if (!parseFormat(value))
                {
                    nFormat     = F_UNKNOWN;
                    sDigits     = DIGITS_DFL;
                    nItems      = 0;
                    nFlags      = 0;
                }
                break;
            case A_COLOR:
                sColor.set(pUI->theme(), value);
                break;
            case A_BG_COLOR:
                sBgColor.set(pUI->theme(), value);
                break;
            case A_TEXT_COLOR:
                sTextColor.set(pUI->theme(), value);
                break;
            default:
                Gtk2CustomWidget::set(att, value);
                break;
        }
    }

    void Gtk2Indicator::end()
    {
    }

    void Gtk2Indicator::drawDigit(cairo_t *cr, int x, int y, char ch, char mod)
    {
        double l_r = sTextColor.red(), l_g = sTextColor.green(), l_b = sTextColor.blue(); // Light color
        double b_r = sColor.red(), b_g = sColor.green(), b_b = sColor.blue(); // Background color
        double a_r = b_r + (l_r - b_r) * 0.05, a_g = b_g + (l_g - b_g) * 0.05, a_b = b_b + (l_b - b_b) * 0.05; // Ambience color

        size_t  sm = 0;
        switch (ch)
        {
            case ' ':   sm      = 0x0000;   break;
            case '+':   sm      = 0x0081;   break;
            case '-':   sm      = 0x0080;   break;
            case '0':   sm      = 0x007e;   break;
            case '1':   sm      = 0x0030;   break;
            case '2':   sm      = 0x00da;   break;
            case '3':   sm      = 0x00f8;   break;
            case '4':   sm      = 0x00b4;   break;
            case '5':   sm      = 0x00ec;   break;
            case '6':   sm      = 0x00ee;   break;
            case '7':   sm      = 0x0038;   break;
            case '8':   sm      = 0x00fe;   break;
            case '9':   sm      = 0x00fc;   break;
            case '.':   sm      = 0x0100;   break;
            case '*':   sm      = 0x07ff;   break;
            default:    sm      = 0x07ff;   break;
        }
        switch (mod)
        {
            case '.':   sm     |= 0x0100;   break;
            case ':':   sm     |= 0x0600;   break;
            default:    break;
        }

        const rect_t *r = segments;
        for (size_t i=0, m=1; i<11; ++i, m <<= 1, ++r)
        {
            if (sm & m)
                cairo_set_source_rgb(cr, l_r, l_g, l_b);
            else
                cairo_set_source_rgb(cr, a_r, a_g, a_b);

            cairo_rectangle(cr, x + r->x, y + r->y, r->w, r->h);
            cairo_stroke(cr);
        }
    }

    bool Gtk2Indicator::formatInt(buffer_t &buf, item_t *descr, ssize_t value)
    {
        char tmp[64];

        size_t digits   = sDigits;

        if (value < 0)
        {
            value = -value;

            if (nFlags & (F_SIGN | F_PAD_ZERO))
            {
                if (!append_buf(buf, '-'))
                    return false;
                if ((--digits) <= 0)
                    return append_buf(buf, '\0');
            }

            // Calculate maximum value
            ssize_t max_value = 1;
            for (size_t i=0; i<digits; ++i)
                max_value  *= 10;

            // Fill with error if overflow
            if (value >= max_value)
            {
                for (size_t i=0; i<digits; ++i)
                {
                    if (!append_buf(buf, '-'))
                        return false;
                }
                return append_buf(buf, '\0');
            }

            char pad = (nFlags & F_PAD_ZERO) ? '0' : ' ';
            const char *format = "-%ld";
            if (nFlags & (F_SIGN | F_PAD_ZERO))
                format          = "%ld";

            // Pad value
            int count = snprintf(tmp, sizeof(tmp), format, long(value));
            int padding     = digits - count;
            while ((padding--) > 0)
            {
                if (!append_buf(buf, pad))
                    return false;
            }

            if (!append_buf(buf, tmp, count))
                return false;
        }
        else if (value > 0)
        {
            // Calculate maximum value
            ssize_t max_value = (nFlags & (F_SIGN | F_PLUS)) ? 1 : 10;
            for (size_t i=1; i<digits; ++i)
                max_value  *= 10;

            // Fill with error if overflow
            if (value >= max_value)
            {
                for (size_t i=0; i<digits; ++i)
                {
                    if (!append_buf(buf, '+'))
                        return false;
                }
                return append_buf(buf, '\0');
            }

            char pad = (nFlags & F_PAD_ZERO) ? '0' : ' ';
            const char *format = "%ld";

            // Sign has to be at the left
            if (nFlags & F_SIGN)
            {
                if (!append_buf(buf, (nFlags & F_PLUS) ? '+' : pad))
                    return false;
                if ((--digits) <= 0)
                    return append_buf(buf, '\0');
            }
            else if (nFlags & F_PLUS)
                format = "+%ld";

            // Pad value
            int count = snprintf(tmp, sizeof(tmp), format, long(value));
            int padding     = digits - count;
            while ((padding--) > 0)
            {
                if (!append_buf(buf, pad))
                    return false;
            }

            if (!append_buf(buf, tmp, count))
                return false;
        }
        else
        {
            if ((digits > 1) && (nFlags & F_SIGN))
            {
                if (!append_buf(buf, ' '))
                    return false;
                digits      --;
            }
            while (digits > 1)
            {
                if (!append_buf(buf, (nFlags & F_PAD_ZERO) ? '0' : ' '))
                    return false;
                digits--;
            }

            if (!append_buf(buf, '0'))
                return false;
        }

        return append_buf(buf, tmp, '\0');
    }

    bool Gtk2Indicator::formatFloat(buffer_t &buf, item_t *descr, double value)
    {
        char tmp[64];

        // Do not format NAN
        if (value == NAN)
            return false;

        ssize_t digits   = sDigits;

        // FLOAT FORMAT: {s1}{pad}{s2}{zero}{int_p}{dot}{frac_p}
        ssize_t s1 = 0, pad = 0, s2 = 0, z_p = 0, int_p = 0, frac_p = 0;

        // Remember sign and make value positive
        char c_sign = (value < 0.0) ? '-' : (value > 0.0) ? '+' : ' ';
        char c_pad  = (nFlags & F_PAD_ZERO) ? '0' : ' ';
        if (value < 0.0)
            value   = -value;

        // Calculate s1 and s2
        if (nFlags & F_SIGN)
            s1  ++;
        else if (c_sign == '-')
        {
            if (nFlags & F_PAD_ZERO)
                s1  ++;
            else
                s2  ++;
        }
        else if (c_sign == '+')
        {
            if (nFlags & F_PLUS)
                s2  ++;
        }

        // Now calculate maximum available value
        if (s1 || s2)
            digits  --;

        // Calculate integer part
        double tmp_v         = value;
        while (truncf(tmp_v) > 0.0)
        {
            tmp_v       *= 0.1f;
            int_p       ++;
        }

        // Calculate zero digit
        if ((int_p <= 0) && (!(nFlags & F_NO_ZERO)))
            z_p++;

        double max_value     =  1.0;
        for (ssize_t i=z_p; i<digits; ++i)
            max_value  *= 10.0;

        // Calculate fraction part
        if (!(nFlags & F_FIXED_PREC))
        {
            // Maximize fraction part
            if (value == 0.0)
            {
                int_p       = 0; // No integer part for 0
                frac_p      = (nFlags & F_TOLERANCE) ? (digits - z_p - int_p) : descr->precision;
            }
            else
            {
                tmp_v       = value;

                while (true)
                {
                    tmp_v       *= 10.0;
                    if (truncf(tmp_v) >= max_value)
                        break;
                    if ((frac_p >= descr->precision) && (!(nFlags & F_TOLERANCE)))
                        break;
                    frac_p++;
                }
            }
        }
        else
            frac_p  = descr->precision;

        // Calculate padding
        pad = digits - z_p - int_p - frac_p;
        if (pad >= 0) // Okay
        {
            // Normalize and format
            for (ssize_t i=0; i < frac_p; ++i)
                value  *= 10.0;
            sprintf(tmp, "%ld", (unsigned long)(value));
            char *p = tmp;

            // Output value
            // FLOAT FORMAT: {s1}{pad}{s2}{int_p}{dot}{frac_p}

            // Extra sign
            if (s1 > 0)
            {
                if (!append_buf(buf, c_sign))
                    return false;
            }

            // Padding
            while ((pad--) > 0)
            {
                if (!append_buf(buf, c_pad))
                    return false;
            }

            // Sign
            if (s2 > 0)
            {
                if (!append_buf(buf, c_sign))
                    return false;
            }

            // Integer part (zero)
            if (z_p > 0)
            {
                if (!append_buf(buf, '0'))
                    return false;
            }

            // Integer part (non-zero)
            while ((int_p--) > 0)
            {
                char c  = (*p == '\0') ? '0' : *(p++);
                if (!append_buf(buf, c))
                    return false;
            }

            // Fraction part: check if need to place dot
            if ((frac_p > 0) || (nFlags & F_DOT))
            {
                if (!append_buf(buf, '.'))
                    return false;
            }

            while ((frac_p--) > 0)
            {
                char c  = (*p == '\0') ? '0' : *(p++);
                if (!append_buf(buf, c))
                    return false;
            }
        }
        else // Overflow
        {
            // Extra sign
            if (s1 || s2)
            {
                if (!append_buf(buf, c_sign))
                    return false;
            }

            // Re-calculate attributes
            if (c_sign == ' ')
                c_sign = '*';

            frac_p  = (descr->precision >= digits) ? digits - 1 : descr->precision;
            int_p   = digits - frac_p;

            // Integer part
            while ((int_p--) > 0)
            {
                if (!append_buf(buf, c_sign))
                    return false;
            }

            // Fraction part: check if need to place dot
            if ((frac_p > 0) || (nFlags & F_DOT))
            {
                if (!append_buf(buf, '.'))
                    return false;
            }

            while ((frac_p--) > 0)
            {
                if (!append_buf(buf, c_sign))
                    return false;
            }
        }

        return append_buf(buf, tmp, '\0');
    }

    bool Gtk2Indicator::formatTimeInterval(buffer_t &buf, item_t *descr, double value)
    {
        char tmp[64];

        // Do not format NAN
        if (value == NAN)
            return false;

        char c_sign = (value < 0.0) ? '-' : (value > 0.0) ? '+' : ' ';
        char c_pad  = (nFlags & F_PAD_ZERO) ? '0' : ' ';
        if (value < 0.0)
            value       = -value;

        bool overflow = false;

        if (nFlags & (F_SIGN | F_PLUS))
        {
            if ((!(nFlags & F_PLUS)) && (c_sign == '+'))
                c_sign = ' ';
            if (!append_buf(buf, c_sign))
                return false;
        }
        else if (c_sign == '-')
            overflow = true;

        // Find day marker and validate overflow
        for (size_t i=0; i < nItems; ++i)
        {
            item_t *item    = &vItems[i];
            if (item->type == 'D')
            {
                size_t field = size_t(value) / (60 * 60 * 24);
                size_t digits = 1;
                while (field >= 10)
                {
                    field /= 10;
                    digits++;
                }
                if (digits > item->digits)
                {
                    overflow = true;
                    break;
                }
            }
        }

        for (size_t i=0; i < nItems; ++i)
        {
            item_t *item    = &vItems[i];
            size_t field    = 0;
            size_t digits   = item->digits;

            if (!overflow)
            {
                switch (item->type)
                {
                    case 'u': // Microseconds 000000-999999
                    {
                        double intf;
                        double fract = modf(value, &intf);
                        for (size_t j=0; j<digits; ++j)
                            fract       *= 10.0;
                        field           = fract;
                        break;
                    }
                    case 'S': // Second 00-59
                        field           = size_t(value) % 60;
                        break;
                    case 'M': // Minute 00-59
                        field           = (size_t(value) / 60) % 60;
                        break;
                    case 'H': // Hour 00-23
                        field           = (size_t(value) / (60 * 60)) % 24;
                        break;
                    case 'h': // Hour 01-12
                        field           = (size_t(value) / (60 * 60)) % 12;
                        if (!field)
                            field           = 12;
                        break;
                    case 'D': // Day
                        field           = size_t(value) / (60 * 60 * 24);
                        break;
                    default:
                        if (!append_buf(buf, item->type))
                            return false;
                        continue;
                }

                digits = snprintf(tmp, sizeof(tmp), "%ld", (long)(field));
                ssize_t pad = item->digits - digits;
                while ((pad--) > 0)
                {
                    if (!append_buf(buf, c_pad))
                        return false;
                }
                for (size_t i=0; i<digits; ++i)
                {
                    if (!append_buf(buf, tmp[i]))
                        return false;
                }
            }
            else
            {
                switch (item->type)
                {
                    case 'u': case 'S': case 'M': case 'H':
                    case 'h': case 'D':
                        break;
                    default:
                        if (!append_buf(buf, item->type))
                            return false;
                        continue;
                }

                for (size_t i=0; i<digits; ++i)
                {
                    if (!append_buf(buf, c_sign))
                        return false;
                }
            }
        }

        return append_buf(buf, tmp, '\0');
    }

    bool Gtk2Indicator::append_buf(buffer_t &buf, char c)
    {
        if (buf.offset >= buf.len)
            return false;
        buf.data[buf.offset++] = c;
        return true;
    }

    bool Gtk2Indicator::append_buf(buffer_t &buf, char *s)
    {
        size_t len = strlen(s);
        if ((buf.offset + len) > buf.len)
            return false;

        memcpy(&buf.data[buf.offset], s, len);
        buf.offset  += len;
        return true;
    }

    bool Gtk2Indicator::append_buf(buffer_t &buf, char *s, size_t count)
    {
        if ((buf.offset + count) > buf.len)
            return false;

        memcpy(&buf.data[buf.offset], s, count);
        buf.offset  += count;
        return true;
    }

    bool Gtk2Indicator::format(double value, char *buf, size_t len)
    {
        buffer_t tbuf;
        tbuf.data   = buf;
        tbuf.offset = 0;
        tbuf.len    = len;
        memset(buf, 0, len);

        bool result = false;

        switch (nFormat)
        {
            case F_INT:
                result = formatInt(tbuf, &vItems[0], ssize_t(value));
                break;
            case F_TIME:
                result = formatTimeInterval(tbuf, &vItems[0], value);
                break;
            case F_FLOAT:
                result = formatFloat(tbuf, &vItems[0], value);
                break;
            default:
                break;
        }

        if (!result)
        {
            tbuf.offset     = 0;
            for (size_t i=0; i<sDigits; ++i)
            {
                if (!append_buf(tbuf, '*'))
                    return false;
            }
            if (!append_buf(tbuf, '\0'))
                return false;
        }

        return true;
    }

    void Gtk2Indicator::render()
    {
        char buf[128];

//        lsp_trace("render(%d, %d, %d, %d)", int(nLeft), int(nTop), int(nWidth), int(nHeight));
        size_t width = ((15 + 1) * sDigits) + 2, height = 18 + 4;

        cairo_t *cr = gdk_cairo_create(pWidget->window);
        cairo_save(cr);

        // Draw background
        cairo_set_source_rgb(cr, sBgColor.red(), sBgColor.green(), sBgColor.blue());
        cairo_rectangle(cr, 0, 0, nWidth, nHeight);
        cairo_fill(cr);

        cairo_translate(cr, ssize_t(nWidth - width) >> 1, ssize_t(nHeight - height) >> 1);
        cairo_new_path(cr);

        // Draw glass
        cairo_set_source_rgb(cr, sColor.red(), sColor.green(), sColor.blue());
        cairo_set_line_width(cr, 1);
        cairo_rectangle(cr, 0, 0, width, height);
        cairo_stroke_preserve(cr);
        cairo_fill(cr);

        cairo_set_line_width(cr, 1);

        format(fValue, buf, sizeof(buf) - 1);

        size_t x = 3;
        const char *s = buf;
        for (size_t i=0; i < sDigits; ++i, x += (15 + 1))
        {
            char c = *s, m = '\0';
            if (c == '\0')
                c   = ' ';
            else if ((c == '.') || (c == ':'))
            {
                m   = c;
                c   = ' ';
            }
            else
            {
                m = *(++s);
                if ((m == '.') || (m == ':'))
                    ++s;
                else
                    m = '\0';
            }

            // Draw digit with character and modifier
            drawDigit(cr, x, 3, c, m);
        }

        cairo_restore(cr);
        cairo_destroy(cr);
    }

    void Gtk2Indicator::resize(size_t &w, size_t &h)
    {
        w   = ((15 + 1) * sDigits) + 2;
        h   = 18 + 4;
    }

    void Gtk2Indicator::notify(IUIPort *port)
    {
        if (port == pPort)
        {
            const port_t *mdata = pPort->metadata();
            fValue = 0;
            if (mdata != NULL)
                fValue      = pPort->getValue();

            // Request for redraw
            gtk_widget_queue_draw(pWidget);
        }
    }
} /* namespace lsp */
