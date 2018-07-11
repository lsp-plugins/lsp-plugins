/*
 * parse.cpp
 *
 *  Created on: 23 дек. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        bool parse_float(const char *variable, float *res)
        {
            UPDATE_LOCALE(saved_locale, LC_NUMERIC, "C");
            errno = 0;
            char *end   = NULL;
            float value = strtof(variable, &end);

            bool success = (errno == 0);
            if ((end != NULL) && (success))
            {
                // Skip spaces
                while ((*end) == ' ')
                    ++ end;
                if (((end[0] == 'd') || (end[0] == 'D')) &&
                    ((end[1] == 'b') || (end[1] == 'B')))
                    value   = expf(value * M_LN10 * 0.05);
            }

            if (saved_locale != NULL)
                setlocale(LC_NUMERIC, saved_locale);

            if (res != NULL)
                *res        = value;
            return success;
        }

        bool parse_double(const char *variable, double *res)
        {
            UPDATE_LOCALE(saved_locale, LC_NUMERIC, "C");
            errno = 0;
            char *end       = NULL;
            double value    = strtod(variable, &end);

            bool success    = (errno == 0);
            if ((end != NULL) && (success))
            {
                // Skip spaces
                while ((*end) == ' ')
                    ++ end;
                if (((end[0] == 'd') || (end[0] == 'D')) &&
                    ((end[1] == 'b') || (end[1] == 'B')))
                    value   = expf(value * M_LN10 * 0.05);
            }

            if (saved_locale != NULL)
                setlocale(LC_NUMERIC, saved_locale);

            if (res != NULL)
                *res        = value;
            return success;
        }
    }
}

