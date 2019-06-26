/*
 * pattern.cpp
 *
 *  Created on: 29 мая 2019 г.
 *      Author: sadko
 */

#include <core/protocol/osc.h>

namespace lsp
{
    namespace osc
    {
        static size_t pattern_calc_segments(const char *address, size_t len)
        {
            // Compute number of parts
            if ((len <= 0) || (*(address++) != '/'))
                return 0;

            // Estimate number of parts in the address
            const char *s;
            size_t parts    = 1;

            while (true)
            {
                if ((s = reinterpret_cast<const char *>(::memchr(address, '/', len))) == NULL)
                    break;
                ++parts;
                ++s;

                len    -= (s - address);
                address = s;
            }

            return parts;
        }

        static void pattern_split_segments(char **dst, const char *format, size_t len)
        {
            const char *s;
            size_t idx = 0;

            while (true)
            {
                dst[idx++]  = const_cast<char *>(format);
                if ((s = reinterpret_cast<const char *>(::memchr(format, '/', len))) == NULL)
                    break;
                ++s;

                len    -= (s - format);
                format  = s;
            }
        }

        // Non-valid characters for OSC: ' ', '#, '*', ',', '/', '?', '[', ']', '{', '}'
        static inline bool pattern_is_invalid(int ch)
        {
            switch (ch)
            {
                // End of part?
                case ' ': case '#': case '*': case ',':
                case '/': case '?': case '[': case ']':
                case '{': case '}': case '\0':
                    return true;

                default:
                    return ((ch < 0x20) || (ch >= 0x7f));
            }
            return true;
        }

        static inline bool pattern_is_printable(int ch)
        {
            return ((ch >= 0x20) && (ch < 0x7f));
        }

        static const char *pattern_validate_list(const char *part)
        {
            while (true)
            {
                uint8_t ch = uint8_t(*(part++));

                // Check validity of character
                if (pattern_is_invalid(ch))
                {
                    if (ch == '}') // End of list?
                        return part;
                    else if (ch != ',') // Separator?
                        return NULL;
                }
            }
        }

        static const char *pattern_validate_range(const char *part)
        {
            // Negotation mark?
            if (*part == '!')
                ++part;

            size_t count = 0;
            while (true)
            {
                uint8_t ch = uint8_t(*(part++));

                // Check validity of character
                if (pattern_is_invalid(ch))
                    return ((ch == ']') && (count > 0)) ? part : NULL;

                // Range character occurred?
                ++count;
                if (*part == '-')
                {
                    ++part;
                    if (pattern_is_invalid(*part))
                        return NULL;
                    ++part;
                }
            }
        }

        static bool pattern_validate_part(const char *part)
        {
            // Do not allow empty elements
            uint8_t ch = uint8_t(*(part++));
            if ((ch == '\0') || (ch == '/'))
                return false;

            while (true)
            {
                switch (ch)
                {
                    // Special cases?
                    case '{':
                        if (!(part = pattern_validate_list(part)))
                            return false;
                        break;
                    case '[':
                        if (!(part = pattern_validate_range(part)))
                            return false;
                        break;

                    case '*':
                    case '?':
                        break;

                    // End of part?
                    case '/':
                    case '\0':
                        return true;

                    // Not allowed characters?
                    case ' ':
                    case '#':
                    case ']':
                    case '}':
                    case ',':
                        return false;

                    // Each OSC Method and each OSC Container other than the root of the tree has a symbolic name,
                    // an ASCII string consisting of printable characters other than...
                    default:
                        if (!pattern_is_printable(ch))
                            return false;
                        break;
                }

                ch = uint8_t(*(part++));
            }
        }

        status_t pattern_create(pattern_t *pattern, const char *format)
        {
            // Check arguments
            if ((pattern == NULL) || (format == NULL))
                return STATUS_BAD_ARGUMENTS;

            // Estimate number of parts
            size_t len      = strlen(format);
            size_t parts    = pattern_calc_segments(format, len);
            if (parts <= 0)
                return STATUS_BAD_FORMAT;

            // Now allocate the memory
            size_t fmt_size = ALIGN_SIZE(len + 1, DEFAULT_ALIGN);
            size_t arg_size = ALIGN_SIZE(sizeof(char *) * parts, DEFAULT_ALIGN);

            // Try to allocate the memory
            char *xfmt      = reinterpret_cast<char *>(::malloc(fmt_size + arg_size));
            if (xfmt == NULL)
                return STATUS_NO_MEM;

            ::memcpy(xfmt, format, len);    // Copy string contents
            xfmt[len]       = '\0';
            char **xargs    = reinterpret_cast<char **>(&xfmt[fmt_size]);

            // Split segments of pattern
            pattern_split_segments(xargs, &xfmt[1], len);

            // Validate pattern segments
            for (size_t i=0; i<parts; ++i)
                if (!pattern_validate_part(xargs[i]))
                {
                    ::free(xfmt);
                    return STATUS_BAD_FORMAT;
                }

            // Store pattern data
            pattern->nparts     = parts;
            pattern->format     = xfmt;
            pattern->parts      = xargs;

            return STATUS_OK;
        }

        status_t pattern_destroy(pattern_t *pattern)
        {
            if (pattern == NULL)
                return STATUS_BAD_ARGUMENTS;
            if ((pattern->format == NULL) || (pattern->parts == NULL))
                return STATUS_OK;

            ::free(pattern->format);
            pattern->nparts     = 0;
            pattern->parts      = NULL;
            pattern->format     = NULL;

            return STATUS_OK;
        }

        static const char *pattern_match_range(const char **xpart, const char *address)
        {
            const char *part = *xpart;
            bool invert = false;

            // Negotation mark?
            if (*part == '!')
            {
                ++part;
                invert = true;
            }

            // Immediate end of range?
            if (*part == ']')
                return (invert) ? NULL : address;

            // Get character for check and validate
            uint8_t ch = uint8_t(*(address++));
            if (pattern_is_invalid(ch))
                return NULL;

            bool matched = false;
            while (true)
            {
                // Get control character
                uint8_t sch = uint8_t(*(part++));
                if (sch == ']')
                {
                    *xpart  = part;
                    return (matched ^ invert) ? address : NULL;
                }
                else if (matched)
                    continue;

                // Range check ?
                if (*part == '-')
                {
                    ++part;
                    uint8_t dch = uint8_t(*(part++));
                    if (sch <= dch)
                        matched = ((ch >= sch) && (ch <= dch));
                    else
                        matched = ((ch >= dch) && (ch <= sch));
                }
                else
                    matched = (ch == sch);
            }
        }

        static const char *pattern_match_list(const char **xpart, const char *address)
        {
            size_t len = 0;
            bool matched = false;
            const char *part = *xpart, *item = *xpart;

            while (true)
            {
                uint8_t ch      = uint8_t(*(part++));

                switch (ch)
                {
                    // End of list?
                    case '}':
                        if (!matched)
                        {
                            if ((matched = (::memcmp(item, address, len) == 0)))
                                address    = &address[len];
                        }
                        *xpart  = part;
                        return (matched) ? address : NULL;

                    // Next token?
                    case ',':
                        if (matched)
                            break;
                        else if ((matched = (::memcmp(item, address, len) == 0)))
                            address    = &address[len];

                        // Update list item parameters
                        item = part;
                        len = 0;
                        break;

                    // Another chatacter?
                    default:
                        ++len;
                        break;
                }
            }
        }

        static const char *pattern_match_part(const char *part, const char *address)
        {
            while (true)
            {
                uint8_t pch     = uint8_t(*(part++));
                switch (pch)
                {
                    // End of segment?
                    case '\0':
                    case '/':
                        pch     = uint8_t(*address);
                        return ((pch == '\0') || (pch == '/')) ? address : NULL;

                    // Any character?
                    case '?':
                        if (pattern_is_invalid(*(address++)))
                            return NULL;
                        break;

                    // Wildcard?
                    case '*':
                    {
                        do
                        {
                            // Do forward lookup
                            const char *new_address = pattern_match_part(part, address);
                            if (new_address)
                                return new_address;
                            pch     = uint8_t(*(address++));
                        } while ((pch != '\0') && (pch != '/'));

                        return NULL;
                    }

                    // Start of character range?
                    case '[':
                        if ((address = pattern_match_range(&part, address)))
                            break;
                        return address;

                    // Start of string list?
                    case '{':
                        if ((address = pattern_match_list(&part, address)))
                            break;
                        return address;

                    // Simple character match?
                    default:
                        if (*(address++) != pch)
                            return NULL;
                        break;
                }
            }
        }

        bool pattern_match(const pattern_t *pattern, const char *address)
        {
            if ((pattern == NULL) || (address == NULL))
                return false;

            uint8_t ch = uint8_t(*(address++));
            if (ch != '/')
                return false;

            for (size_t parts=0; parts < pattern->nparts; )
            {
                const char *part = pattern->parts[parts++];

                // The substring in address should be not empty
                ch = *address;
                if ((ch == '\0') || (ch == '/'))
                    return false;

                // Match address string with pattern
                if (!(address = pattern_match_part(part, address)))
                    return false;

                switch (*address)
                {
                    case '\0':
                        return parts == pattern->nparts;
                    case '/':
                        ++address;
                        break;
                    default:
                        return false;
                }
            }

            return false;
        }

        status_t pattern_get_format(pattern_t *pattern, const char **format)
        {
            if ((pattern == NULL) || (format == NULL))
                return STATUS_BAD_ARGUMENTS;
            *format = pattern->format;
            return STATUS_OK;
        }
    }
}


