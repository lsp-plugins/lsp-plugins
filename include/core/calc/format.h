/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 25 февр. 2020 г.
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

#ifndef CORE_CALC_FORMAT_H_
#define CORE_CALC_FORMAT_H_

#include <core/io/IOutSequence.h>
#include <core/io/IInSequence.h>
#include <core/calc/Parameters.h>

namespace lsp
{
    namespace calc
    {
        /**
         * Format the set of parameters provided by Resolver using input character
         * sequence as format specifier and output the result to output character
         * sequence
         *
         * @param out output character sequence
         * @param fmt format specifier
         * @param r parameter resolver
         * @return status of operation
         */
        status_t format(io::IOutSequence *out, io::IInSequence *fmt, const Parameters *r);

        /**
         * Format the set of parameters provided by Resolver using format string
         * and output the result to output character sequence
         *
         * @param out output character sequence
         * @param fmt format specifier (UTF-8 character sequence)
         * @param r parameter resolver
         * @return status of operation
         */
        status_t format(io::IOutSequence *out, const char *fmt, const Parameters *r);

        /**
         * Format the set of parameters provided by Resolver using format string
         * and output the result to output character sequence
         *
         * @param out output character sequence
         * @param fmt format specifier
         * @param r parameter resolver
         * @return status of operation
         */
        status_t format(io::IOutSequence *out, const LSPString *fmt, const Parameters *r);

        /**
         * Format the set of parameters provided by Resolver using input character
         * sequence as format specifier and output the result to string
         *
         * @param out output string
         * @param fmt format specifier
         * @param r parameter resolver
         * @return status of operation
         */
        status_t format(LSPString *out, io::IInSequence *fmt, const Parameters *r);

        /**
         * Format the set of parameters provided by Resolver using format string
         * and output the result to string
         *
         * @param out output string
         * @param fmt format specifier (UTF-8 character sequence)
         * @param r parameter resolver
         * @return status of operation
         */
        status_t format(LSPString *out, const char *fmt, const Parameters *r);

        /**
         * Format the set of parameters provided by Resolver using format string
         * and output the result to string
         *
         * @param out output string
         * @param fmt format specifier
         * @param r parameter resolver
         * @return status of operation
         */
        status_t format(LSPString *out, const LSPString *fmt, const Parameters *r);
    }
}


#endif /* CORE_CALC_FORMAT_H_ */
