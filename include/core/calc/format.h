/*
 * format.h
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: sadko
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
