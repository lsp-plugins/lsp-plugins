/*
 * debug.cpp
 *
 *  Created on: 23 апр. 2017 г.
 *      Author: sadko
 */
#include <core/debug.h>
#include <limits.h>

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
#endif /* LSP_DEBUG */
}
