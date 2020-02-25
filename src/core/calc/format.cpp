/*
 * format.cpp
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: sadko
 */

#include <core/calc/format.h>
#include <core/io/InStringSequence.h>
#include <core/io/OutStringSequence.h>

namespace lsp
{
    namespace calc
    {
        status_t format(io::IOutSequence *out, const char *fmt, Resolver *r)
        {
            if ((out == NULL) || (fmt == NULL))
                return STATUS_BAD_ARGUMENTS;

            io::InStringSequence xfmt;
            status_t res = xfmt.wrap(fmt);
            if (res != STATUS_OK)
            {
                xfmt.close();
                return res;
            }

            res = format(out, &xfmt, r);
            if (res != STATUS_OK)
            {
                xfmt.close();
                return res;
            }

            return xfmt.close();
        }

        status_t format(io::IOutSequence *out, const LSPString *fmt, Resolver *r)
        {
            if ((out == NULL) || (fmt == NULL))
                return STATUS_BAD_ARGUMENTS;

            io::InStringSequence xfmt;
            status_t res = xfmt.wrap(fmt);
            if (res != STATUS_OK)
            {
                xfmt.close();
                return res;
            }

            res = format(out, &xfmt, r);
            if (res != STATUS_OK)
            {
                xfmt.close();
                return res;
            }

            return xfmt.close();
        }

        status_t format(LSPString *out, io::IInSequence *fmt, Resolver *r)
        {
            if ((out == NULL) || (fmt == NULL))
                return STATUS_BAD_ARGUMENTS;

            io::OutStringSequence xout;
            status_t res = xout.wrap(out, false);
            if (res != STATUS_OK)
            {
                xout.close();
                return res;
            }

            res = format(&xout, fmt, r);
            if (res != STATUS_OK)
            {
                xout.close();
                return res;
            }

            return xout.close();
        }

        status_t format(LSPString *out, const char *fmt, Resolver *r)
        {
            if ((out == NULL) || (fmt == NULL))
                return STATUS_BAD_ARGUMENTS;

            io::OutStringSequence xout;
            status_t res = xout.wrap(out, false);
            if (res != STATUS_OK)
            {
                xout.close();
                return res;
            }

            res = format(&xout, fmt, r);
            if (res != STATUS_OK)
            {
                xout.close();
                return res;
            }

            return xout.close();
        }

        status_t format(LSPString *out, const LSPString *fmt, Resolver *r)
        {
            if ((out == NULL) || (fmt == NULL))
                return STATUS_BAD_ARGUMENTS;

            io::OutStringSequence xout;
            status_t res = xout.wrap(out, false);
            if (res != STATUS_OK)
            {
                xout.close();
                return res;
            }

            res = format(&xout, fmt, r);
            if (res != STATUS_OK)
            {
                xout.close();
                return res;
            }

            return xout.close();
        }

        status_t format(io::IOutSequence *out, io::IInSequence *fmt, Resolver *r)
        {
            // TODO
            return STATUS_OK;
        }
    }
}

