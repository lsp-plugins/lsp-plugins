/*
 * debug.cpp
 *
 *  Created on: 23 апр. 2017 г.
 *      Author: sadko
 */
#include <core/debug.h>

#ifdef LSP_TRACEFILE
    namespace lsp
    {
        FILE *log_fd = stderr;

        void init_debug(const char *subsystem)
        {
            if (log_fd != stderr)
                return;

            char path[PATH_MAX];
            snprintf(path, PATH_MAX, "/tmp/lsp-plugins-%s.log", subsystem);

            log_fd      = fopen(path, "a");
            if (log_fd != NULL)
                setvbuf(log_fd, NULL, _IONBF, BUFSIZ);
        }
    }
#endif /* LSP_DEBUG */
