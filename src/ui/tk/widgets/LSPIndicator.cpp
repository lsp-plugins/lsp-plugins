/*
 * LSPIndicator.cpp
 *
 *  Created on: 7 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>

namespace lsp
{
    namespace tk
    {
        typedef struct rect_t
        {
            int x, y, w, h;
        } rect_t;

        static const rect_t segments[] =
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
            {  12, 11,  1,  1   }   // 10
        };
        
        const w_class_t LSPIndicator::metadata = { "LSPIndicator", &LSPWidget::metadata };

        LSPIndicator::LSPIndicator(LSPDisplay *dpy): LSPWidget(dpy),
            sColor(this),
            sTextColor(this)
        {
            fValue      = 0.0f;
            sFormat     = NULL;
            nFormat     = F_UNKNOWN;
            sDigits     = DIGITS_DFL;
            nFlags      = 0;
            pClass      = &metadata;
        }
        
        LSPIndicator::~LSPIndicator()
        {
            drop_data();
        }

        status_t LSPIndicator::init()
        {
            status_t result = LSPWidget::init();
            if (result != STATUS_OK)
                return result;

            init_color(C_GLASS, &sColor);
            init_color(C_LABEL_TEXT, &sTextColor);

            return STATUS_OK;
        }

        bool LSPIndicator::parse_long(char *p, char **ret, long *value)
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

        void LSPIndicator::destroy()
        {
            drop_data();
        }

        bool LSPIndicator::parse_format(const char *format)
        {
            nFormat     = F_UNKNOWN;
            sDigits     = 0;
            nFlags      = 0;
            vItems.clear();

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
                parse_long(p, &p, &digits);

                // Add item
                item_t *item    = vItems.append();
                if (item == NULL)
                    return false;
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
                if (parse_long(p, &p, &digits))
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
                        item_t *item    = vItems.append();
                        if (item == NULL)
                            return false;
                        item->type      = c;
                        item->digits    = 0;
                        item->precision = 0;
                        break;
                    }
                    case 'u': // Microseconds 000000-999999
                    {
                        long digits     = 6;
                        parse_long(p, &p, &digits);

                        item_t *item    = vItems.append();
                        if (item == NULL)
                            return false;
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
                        item_t *item    = vItems.append();
                        if (item == NULL)
                            return false;
                        item->type      = c;
                        item->digits    = 2;
                        item->precision = 0;
                        sDigits        += 2;
                        break;
                    }
                    case 'D': // Day
                    {
                        long digits         = 1;
                        parse_long(p, &p, &digits);
                        if (digits <= 0)
                            digits      = 1;

                        item_t *item    = vItems.append();
                        if (item == NULL)
                            return false;
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

        void LSPIndicator::draw_digit(ISurface *s, int x, int y, char ch, char mod, const Color &front, const Color &back)
        {
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
                    s->wire_rect(x + r->x, y + r->y, r->w, r->h, 1.0f, front);
                else
                    s->wire_rect(x + r->x, y + r->y, r->w, r->h, 1.0f, back);
            }
        }

        bool LSPIndicator::fmt_time(buffer_t *buf, double value)
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
            size_t n_items = vItems.size();
            for (size_t i=0; i < n_items; ++i)
            {
                item_t *item    = vItems.at(i);
                if (item->type != 'D')
                    continue;

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

            for (size_t i=0; i < n_items; ++i)
            {
                item_t *item    = vItems.at(i);
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

            return true;
        }

        bool LSPIndicator::fmt_float(buffer_t *buf, double value)
        {
            char tmp[64];
            item_t *descr = vItems.at(0);

            // Do not format NAN
            if (isnan(value))
                return false;
            if (isinf(value))
            {
                char sign = (signbit(value)) ? '-' : '+';
                for (size_t i=0; i<sDigits; ++i)
                {
                    if (!append_buf(buf, sign))
                        return false;
                }
                if (!append_buf(buf, '\0'))
                    return false;
                return true;
            }

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
                ssize_t frac_len = snprintf(tmp, sizeof(tmp), "%ld", (unsigned long)(value));
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

                while (frac_p > 0)
                {
                    char c  = (frac_p > frac_len) ? '0' :
                            (*p == '\0') ? '0' : *(p++);
                    if (!append_buf(buf, c))
                        return false;
                    frac_p--;
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

            return true;
        }

        bool LSPIndicator::fmt_int(buffer_t *buf, ssize_t value)
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

            return true;
        }

        bool LSPIndicator::format(buffer_t *buf, double value)
        {
            bool result = false;

            switch (nFormat)
            {
                case F_INT:
                    result = fmt_int(buf, ssize_t(value));
                    break;
                case F_TIME:
                    result = fmt_time(buf, value);
                    break;
                case F_FLOAT:
                    result = fmt_float(buf, value);
                    break;
                default:
                    break;
            }

            if (!result)
            {
                clear_buf(buf);
                for (size_t i=0; i<sDigits; ++i)
                {
                    if (!append_buf(buf, '*'))
                        return false;
                }
            }

            return true;
        }

        void LSPIndicator::drop_data()
        {
            vItems.flush();
            if (sFormat != NULL)
            {
                free(sFormat);
                sFormat     = NULL;
            }
        }

        status_t LSPIndicator::set_format(const char *fmt)
        {
            char *cfmt = strdup(fmt);
            if (cfmt == NULL)
                return STATUS_NO_MEM;

            drop_data();
            sFormat = cfmt;

            // Query for resize
            query_resize();

            // Try to parse format
            if (!parse_format(sFormat))
            {
                nFormat     = F_UNKNOWN;
                sDigits     = DIGITS_DFL;
                nFlags      = 0;

                return STATUS_BAD_FORMAT;
            }

            return STATUS_OK;
        }

        void LSPIndicator::set_value(float value)
        {
            if (fValue == value)
                return;

            fValue      = value;
            query_draw();
        }

        void LSPIndicator::size_request(size_request_t *r)
        {
            r->nMinWidth    = ((15 + 1) * sDigits) + 2;
            r->nMinHeight   = 18 + 4;
            r->nMaxWidth    = r->nMinWidth;
            r->nMaxHeight   = r->nMinHeight;
        }

        void LSPIndicator::draw(ISurface *s)
        {
            // Prepare palette
            Color bg_color(sBgColor);
            Color color(sColor);
            Color front(sTextColor);
            Color back(sTextColor);

            back.blend(color, 0.05f);
            back.scale_lightness(brightness());
            color.scale_lightness(brightness());

            // Draw background
            s->fill_rect(0, 0, sSize.nWidth, sSize.nHeight, bg_color);

            // Draw glass
            size_t width = ((15 + 1) * sDigits) + 2, height = 18 + 4;
            size_t left  = (sSize.nWidth - width) >> 1, top = (sSize.nHeight - height) >> 1;

            s->fill_rect(left, top, width, height, color);

            buffer_t buf;
            init_buf(&buf, 128);
            format(&buf, fValue);

            bool aa = s->set_antialiasing(true);

            size_t x    = left + 3;
            top        += 3;
            const char *str = buf.pString;
            for (size_t i=0; i < sDigits; ++i, x += (15 + 1))
            {
                char c = *str, m = '\0';
                if (c == '\0')
                    c   = ' ';
                else if ((c == '.') || (c == ':'))
                {
                    m   = c;
                    c   = ' ';
                }
                else
                {
                    m = *(++str);
                    if ((m == '.') || (m == ':'))
                        ++str;
                    else
                        m = '\0';
                }

                // Draw digit with character and modifier
                draw_digit(s, x, top, c, m, front, back);
            }

            s->set_antialiasing(aa);

            destroy_buf(&buf);
        }

    } /* namespace tk */
} /* namespace lsp */
