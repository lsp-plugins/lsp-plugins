/*
 * config.h
 *
 *  Created on: 11 февр. 2019 г.
 *      Author: sadko
 */

#ifndef TEST_MAIN_CONFIG_H_
#define TEST_MAIN_CONFIG_H_

#include <core/types.h>
#include <core/stdlib/stdio.h>
#include <core/io/charset.h>
#include <test/main/types.h>
#include <data/cvector.h>
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
            const char                 *executable;
            const char                 *outfile;
            const char                 *tracepath;
            cvector<char>               list;
            cvector<char>               ignore;
            cvector<char>               args;

#ifdef PLATFORM_WINDOWS
            size_t                      utf8_argc;
            char                      **utf8_argv;
#endif /* PLATFORM_WINDOWS */

        public:
            explicit config_t();
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

#if defined(PLATFORM_WINDOWS)
        // Get number of processors for system
        SYSTEM_INFO     os_sysinfo;
        GetSystemInfo(&os_sysinfo);
        threads         = os_sysinfo.dwNumberOfProcessors;

        // Get command line
        LPWSTR cmdline  = GetCommandLineW();
        int nargs = 0;
        LPWSTR *arglist = CommandLineToArgvW(cmdline, &nargs);
        if ((arglist == NULL) || (nargs < 1))
        {
            fprintf(stderr, "Error obtaining command-line arguments\n");
            fflush(stderr);
            return STATUS_UNKNOWN_ERR;
        }

        // Convert UTF-16-encoded command line arguments to UTF-8-encoded
        utf8_argc       = nargs;
        utf8_argv       = reinterpret_cast<char **>(malloc(nargs * sizeof(char *)));
        for (size_t i=0; i<utf8_argc; ++i)
            utf8_argv[i]        = NULL;
        for (size_t i=0; i<utf8_argc; ++i)
        {
            utf8_argv[i]        = lsp::utf16_to_utf8(arglist[i]);
            if (utf8_argv[i] == NULL)
                return STATUS_NO_MEM;
        }

        LocalFree(arglist);

        // Patch arguments
        argc        = utf8_argc;
        argv        = const_cast<const char **>(utf8_argv);
#else
        threads     = sysconf(_SC_NPROCESSORS_ONLN);
#endif /* PLATFORM_WINDOWS */

        if (argc < 2)
            return print_usage(out);

        executable  = argv[0];

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
                long jobs       = strtol(argv[i], &end, 10);
                if ((errno != 0) || ((*end) != '\0') || (jobs <= 0))
                {
                    fprintf(stderr, "Invalid value for --jobs parameter: %s\n", argv[i]);
                    return STATUS_INVALID_VALUE;
                }
                threads         = size_t(jobs);
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

    config_t::config_t()
    {
        mode        = UNKNOWN;
        fork        = true;
        verbose     = false;
        debug       = false;
        list_all    = false;
        mtrace      = false;
        ilist       = false;
        sysinfo     = true;
        is_child    = false;
        executable  = NULL;
        tracepath   = "/tmp/lsp-plugins-trace";
        outfile     = NULL;
        threads     = 1;

#if defined(PLATFORM_WINDOWS)
        utf8_argc   = 0;
        utf8_argv   = NULL;
#endif
    }

    void config_t::clear()
    {
#if defined(PLATFORM_WINDOWS)
        if (utf8_argv != NULL)
        {
            for (size_t i=0; i<utf8_argc; ++i)
            {
                if (utf8_argv[i] != NULL)
                    free(utf8_argv[i]);
            }
            free(utf8_argv);

            utf8_argv       = NULL;
            utf8_argc       = 0;
        }
#endif /* PLATFORM_WINDOWS */
        list.flush();
        ignore.flush();
        args.flush();
    }
}

#endif /* TEST_MAIN_CONFIG_H_ */
