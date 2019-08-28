/*
 * debug.cpp
 *
 *  Created on: 23 апр. 2017 г.
 *      Author: sadko
 */
#include <core/debug.h>
#include <limits.h>
#include <stdint.h>

namespace lsp
{
#ifdef LSP_TRACEFILE
    FILE *log_fd = stderr;

    void init_debug(const char *subsystem)
    {
        if (log_fd != stderr)
            return;

        char path[PATH_MAX];
        snprintf(path, PATH_MAX, "/tmp/lsp-plugins-%s.log", subsystem);
        fprintf(stderr, "Log data will be written to file: %s\n", path);

        log_fd      = fopen(path, "a");
        if (log_fd != NULL)
            setvbuf(log_fd, NULL, _IONBF, BUFSIZ);
    }
#endif /* LSP_TRACEFILE */

#ifdef LSP_DEBUG
    void __lsp_dumpf(const char *s, const char *fmt, const float *f, size_t n)
    {
        fputs(s, LSP_LOG_FD);
        fputs(": ", LSP_LOG_FD);
        while (n--)
        {
            fprintf(LSP_LOG_FD, fmt, *(f++));
            if (n > 0)
                fputc(' ', LSP_LOG_FD);
        }
        fputs("\n", LSP_LOG_FD);
    }

    void __lsp_dumpb(const char *s, const void *b, size_t sz)
    {
        fputs(s, LSP_LOG_FD);
        fputs(":\n", LSP_LOG_FD);

        const uint8_t *ddump        = reinterpret_cast<const uint8_t *>(b);

        for (size_t offset=0; offset < sz; offset += 16)
        {
            // Print HEX dump
            lsp_nprintf("%08x: ", int(offset));
            for (size_t i=0; i<0x10; ++i)
            {
                if ((offset + i) < sz)
                    lsp_nprintf("%02x ", int(ddump[i]));
                else
                    lsp_nprintf("   ");
            }
            lsp_nprintf("   ");

            // Print character dump
            for (size_t i=0; i<0x10; ++i)
            {
                if ((offset + i) < sz)
                {
                    uint8_t c   = ddump[i];
                    if ((c < 0x20) || (c >= 0x80))
                        c           = '.';
                    lsp_nprintf("%c", c);
                }
                else
                    lsp_nprintf(" ");
            }
            lsp_printf("");

            // Move pointer
            ddump       += 0x10;
        }
    }
#endif /* LSP_DEBUG */
}
