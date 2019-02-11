/*
 * config.h
 *
 *  Created on: 11 февр. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CONTAINER_TEST_CONFIG_H_
#define INCLUDE_CONTAINER_TEST_CONFIG_H_

#include <core/types.h>
#include <data/cvector.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

namespace lsp
{
    enum test_mode_t
    {
        UNKNOWN,
        PTEST,
        UTEST,
        MTEST
    };

    typedef struct config_t
    {
        public:
            mode_t                      mode;
            bool                        fork;
            bool                        verbose;
            bool                        debug;
            bool                        list_all;
            bool                        mtrace;
            bool                        ilist;
            bool                        sysinfo;
            bool                        is_child;
            size_t                      threads;
            const char                 *outfile;
            const char                 *tracepath;
            cvector<char>               list;
            cvector<char>               ignore;
            cvector<char>               args;

        public:
            explicit config_t() { clear(); }
            ~config_t()         { clear(); }

            status_t        parse(FILE *out, int argc, const char **argv);
            status_t        print_usage(FILE *out, bool detailed=false);
            void            clear();

    } config_t;

    status_t config_t::print_usage(FILE *out, bool detailed)
    {
        fputs("USAGE: {utest|ptest|mtest} [args...] [test name...]\n", out);
        if (!detailed)
            return STATUS_INSUFFICIENT;

        fputs("  First argument:\n", out);
        fputs("    utest                 Unit testing subsystem\n", out);
        fputs("    ptest                 Performance testing subsystem\n", out);
        fputs("    mtest                 Manual testing subsystem\n", out);
        fputs("  Additional arguments:\n", out);
        fputs("    -a, --args [args...]  Pass arguments to test\n", out);
        fputs("    -d, --debug           Disable time restrictions for unit tests\n", out);
        fputs("                          for debugging purposes\n", out);
        fputs("    -e, --execute         Launch tests specified after this switch\n", out);
        fputs("    -f, --fork            Fork child processes (opposite to --nofork)\n", out);
        fputs("    -h, --help            Display help\n", out);
        fputs("    -i, --ignore          Ignore tests specified after this switch\n", out);
        fputs("    -j, --jobs            Set number of job workers for unit tests\n", out);
        fputs("    -l, --list            List all available tests\n", out);
    #ifdef PLATFORM_LINUX
        fputs("    -mt, --mtrace         Enable mtrace log\n", out);
    #endif /* PLATFORM_LINUX */
        fputs("    -nf, --nofork         Do not fork child processes (for better \n", out);
        fputs("                          debugging capabilities)\n", out);
    #ifdef PLATFORM_LINUX
        fputs("    -nt, --nomtrace       Disable mtrace log\n", out);
    #endif /* PLATFORM_LINUX */
        fputs("    -nsi, --nosysinfo     Do not output system information\n", out);
        fputs("    -o, --outfile file    Output performance test statistics to specified file\n", out);
        fputs("    -s, --silent          Do not output additional information from tests\n", out);
        fputs("    -si, --sysinfo        Output system information\n", out);
        fputs("    -t, --tracepath path  Override default trace path with specified value\n", out);
        fputs("    -v, --verbose         Output additional information from tests\n", out);

        return STATUS_INSUFFICIENT;
    }

    status_t config_t::parse(FILE *out, int argc, const char **argv)
    {
        clear();
        if (argc < 2)
            return print_usage(out);

        if (!strcmp(argv[1], "ptest"))
            mode    = PTEST;
        else if (!strcmp(argv[1], "utest"))
            mode    = UTEST;
        else if (!strcmp(argv[1], "mtest"))
            mode    = MTEST;
        else if ((!strcmp(argv[1], "--help")) || ((!strcmp(argv[1], "-h"))))
            return print_usage(out, true);
        else
            return print_usage(out);

        for (int i=2; i<argc; ++i)
        {
            if ((!strcmp(argv[i], "--nofork")) || (!strcmp(argv[i], "-nf")))
                fork        = false;
            else if ((!strcmp(argv[i], "--fork")) || (!strcmp(argv[i], "-f")))
                fork        = true;
            else if ((!strcmp(argv[i], "--verbose")) || (!strcmp(argv[i], "-v")))
                verbose     = true;
            else if ((!strcmp(argv[i], "--silent")) || (!strcmp(argv[i], "-s")))
                verbose     = false;
            else if ((!strcmp(argv[i], "--sysinfo")) || (!strcmp(argv[i], "-si")))
                sysinfo     = true;
            else if ((!strcmp(argv[i], "--nosysinfo")) || (!strcmp(argv[i], "-nsi")))
                sysinfo     = false;
            else if ((!strcmp(argv[i], "--debug")) || (!strcmp(argv[i], "-d")))
                debug       = true;
            else if ((!strcmp(argv[i], "--list")) || (!strcmp(argv[i], "-l")))
                list_all    = true;
    #ifdef PLATFORM_LINUX
            else if ((!strcmp(argv[i], "--mtrace")) || (!strcmp(argv[i], "-mt")))
                mtrace      = true;
            else if ((!strcmp(argv[i], "--nomtrace")) || (!strcmp(argv[i], "-nt")))
                mtrace      = false;
    #endif /* PLATFORM_LINUX */
            else if ((!strcmp(argv[i], "--tracepath")) || (!strcmp(argv[i], "-t")))
            {
                if ((++i) >= argc)
                {
                    fprintf(stderr, "Not specified trace path\n");
                    return STATUS_INVALID_VALUE;
                }
                tracepath   = argv[i];
            }
            else if ((!strcmp(argv[i], "--outfile")) || (!strcmp(argv[i], "-o")))
            {
                if ((++i) >= argc)
                {
                    fprintf(stderr, "Not specified name of output file\n");
                    return STATUS_INVALID_VALUE;
                }
                outfile     = argv[i];
            }
            else if ((!strcmp(argv[i], "--args")) || (!strcmp(argv[i], "-a")))
            {
                while (++i < argc)
                    args.add(const_cast<char *>(argv[i]));
            }
            else if ((!strcmp(argv[i], "--jobs")) || (!strcmp(argv[i], "-j")))
            {
                if ((++i) >= argc)
                {
                    fprintf(stderr, "Not specified number of jobs for --jobs parameter\n");
                    return STATUS_INVALID_VALUE;
                }

                errno           = 0;
                char *end       = NULL;
                threads         = strtol(argv[i], &end, 10);
                if ((errno != 0) || ((*end) != '\0'))
                {
                    fprintf(stderr, "Invalid value for --threads parameter: %s\n", argv[i]);
                    return STATUS_INVALID_VALUE;
                }
            }
            else if ((!strcmp(argv[i], "--help")) || ((!strcmp(argv[i], "-h"))))
                return print_usage(out, true);
            else if ((!strcmp(argv[i], "--ignore")) || ((!strcmp(argv[i], "-i"))))
                ilist           = true;
            else if ((!strcmp(argv[i], "--execute")) || ((!strcmp(argv[i], "-e"))))
                ilist           = false;
#ifdef PLATFORM_WINDOWS
            else if (!strcmp(argv[i], "--run-as-nested-process"))
                is_child        = true;
#endif /* PLATFORM_WINDOWS */
            else
            {
                if (ilist)
                    ignore.add(const_cast<char *>(argv[i]));
                else
                    list.add(const_cast<char *>(argv[i]));
            }
        }

        return 0;
    }

    void config_t::clear()
    {
        mode        = UNKNOWN;
        fork        = true;
        verbose     = false;
        list_all    = false;
        mtrace      = false;
        ilist       = false;
        sysinfo     = true;
        is_child    = false;
        tracepath   = "/tmp/lsp-plugins-trace";
        threads     = sysconf(_SC_NPROCESSORS_ONLN) * 2;
        outfile     = NULL;

        list.flush();
        ignore.flush();
        args.flush();
    }
}

#endif /* INCLUDE_CONTAINER_TEST_CONFIG_H_ */
