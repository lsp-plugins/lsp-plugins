# ABOUT

LSP (Linux Studio Plugins) is a collection of open-source plugins
currently compatible with CLAP, LADSPA, LV2 and LinuxVST formats.

The basic idea is to fill the lack of good and useful plugins under
the GNU/Linux platform.

After some contributions to other open source projects the decision
was made to implement separate and independent plugin distribution.

All supplementary information you will find on official web site:
  https://lsp-plug.in/

Note that after 1.2.0 release the lsp-plugins repository was decomposed
into multiple subprojects. As a result, it is the repository without any code
and for build purposes it gathers source code as dependencies from another
repositories which are located here:
  https://github.com/lsp-plugins/

# LICENSING

The LSP project is an open-source project and cares about quality of
developed software.

Still there is no absolute warranty about stability of the software on
different platforms so you're using this software on your own risk.

The project also does encourage donations that can be submitted here:
  * https://liberapay.com/sadko4u/donate
  * https://www.patreon.com/sadko4u
  * https://paypal.me/sadko4u
  * https://etherscan.io/address/0x079b24da78d78302cd3cfbb80c728cd554606cc6
  * https://salt.bountysource.com/teams/lsp-plugins

Licensed under the terms of GNU Lesser Public License v3 (LGPLv3):
  * https://www.gnu.org/licenses/lgpl-3.0.en.html

For more information about licensing, please read COPYING and COPYING.LESSER.

# SYSTEM REQUIREMENTS

Current matrix of hardware architecture and platform (OS) support is:

```
  ┌───────────┬───────────┬─────────┐
  │Arch / OS  │ GNU/Linux │ FreeBSD │
  ╞═══════════╪═══════════╪═════════╡
  │aarch64    │     E     │    E    │
  ├───────────┼───────────┼─────────┤
  │armv6-a    │     E     │    E    │
  ├───────────┼───────────┼─────────┤
  │armv7-ar   │     E     │    E    │
  ├───────────┼───────────┼─────────┤
  │i586       │     F     │    E    │
  ├───────────┼───────────┼─────────┤
  │loongarch32│     C     │    U    │
  ├───────────┼───────────┼─────────┤
  │loongarch64│     C     │    U    │
  ├───────────┼───────────┼─────────┤
  │ppc64      │     C     │    U    │
  ├───────────┼───────────┼─────────┤
  │riscv-64   │     C     │    U    │
  ├───────────┼───────────┼─────────┤
  │s390x      │     C     │    U    │
  ├───────────┼───────────┼─────────┤
  │x86_64     │     F     │    E    │
  └───────────┴───────────┴─────────┘ 
```

The table legend is the following:
 * F - Full support.
 * C - The code does compile, not enough knowledge about it's correct work.
 * E - Experimental support, not enough feedback from users.
 * U - Unknown, the code may be built but the correctness of it's work has not been tested.
 * N - No support, the code may compile but the work has not been tested.

Supported plugin formats:
  * LADSPA (partial support: not supported by plugins that use MIDI or file loading due to LADSPA plugin format restrictions);
  * LV2 (full support);
  * LinuxVST 2.4 (full support);
  * Standalone JACK (full support).

The Linux distribution requirements:
  * glibc >= 2.19
  * libsndfile >= 1.0.25
  * libcairo >= 1.14
  * libfreetype >= 2.10
  * libGL
  * Host compatible with LV2
  
Known list of supported plugin hosts:
  * Ardour
  * Bitwig Studio
  * Carla
  * Mixbus
  * Qtractor
  * Reaper (native Linux version)
  * Renoise
  * Tracktion

# VERSIONING

Binary releases contain all possible plugins in one bundle. The binaries are packaged
into archive named according to the following format:

```
  lsp-plugins-<format>-<major>.<minor>.<micro>-<platform>-<arch>.tar.gz
```

The property <format> is the format of plugins, currently available:
  * clap - plugins in [CLAP](https://github.com/free-audio/clap) format;
  * doc - documentation;
  * jack - standalone version of plugins that require [JACK](https://jackaudio.org/) server for execution;
  * ladspa - plugins in [LADSPA](https://en.wikipedia.org/wiki/LADSPA) format (not all plugins due to format's restriction);
  * lv2 - plugins in [LV2](https://lv2plug.in/) format;
  * src - source code;
  * vst2 - plugins in [VST 2.4](https://www.steinberg.net/) format.

Property 'arch' contains short name of architecture the binaries are build for.
Properties 'major', 'minor' and 'micro' form the version of release.
If the <micro> version is odd, it contains mostly bug-fixes and patches for
the plugin modules. The even 'micro' version of plugins contains new features
and, in most cases, new plugin series. The history of changes may be seen in
attached CHANGELOG.txt.

Source code is also ready for building, see 'BUILDING' section for details.

# INSTALLATION

Binary distributions are ready to launch, just copy them to the appropriate directory.

Releases containing odd number in minor version provide only enhancements
and critical fixes for the previous release.

IMPORTANT FOR VST INSTALLATIONS: If you deploy plugins as a subdirectory
of your VST directory, the subdirectory should contain substring
'lsp-plugins'. Otherwise plugins won't find the VST core library.
Please notice that '~' means user's home directory.

The usual directories for LADSPA are:
  * /usr/lib/ladspa
  * /usr/local/lib/ladspa
  * /usr/lib64/ladspa
  * /usr/local/lib64/ladspa
  * ~/.ladspa
  
The usual directories for LV2 are:
  * /usr/lib/lv2
  * /usr/local/lib/lv2
  * /usr/lib64/lv2
  * /usr/local/lib64/lv2
  * ~/.lv2

The usual directories for LinuxVST are:
  * /usr/lib/vst
  * /usr/local/lib/vst
  * /usr/lib64/vst
  * /usr/local/lib64/vst
  * ~/.lxvst
  * ~/.vst

The usual directories for JACK core library are:
  * /usr/lib
  * /usr/local/lib
  * /lib
  * /usr/lib64
  * /usr/local/lib64
  * /lib64

The usual directories for JACK binaries are:
  * /usr/bin
  * /usr/local/bin
  * /bin
  * /usr/sbin
  * /usr/local/sbin
  * /sbin
  
The usual directories for CLAP are:
  * /usr/lib/clap
  * /usr/local/lib/clap
  * /usr/lib64/clap
  * /usr/local/lib64/clap
  * ~/.clap

# BUILDING

You may build plugins from scratch.

For successful build you need the following packages to be installed:
  * gcc >= 4.7 OR clang >= 10.0.1
  * gcc-c++ >= 4.7 OR clang-c++ >= 10.0.1
  * libgcc_s1 >= 5.2
  * libstdc++-devel >= 4.7
  * jack-devel >= 1.9.5
  * lv2-devel >= 1.10
  * ladspa-devel >= 1.13
  * libsndfile-devel >= 1.0.25
  * libcairo-devel >= 1.14
  * php >= 5.5.14
  * libGL-devel >= 11.2.2

To build the project from archive with source code, the following sequence of commands 
should be performed:

```
  make clean
  make config
  make
  make install
```

To build the project from GIT repository, the additional 'make fetch' command should be issued
to obtain all source code dependencies:

```
  make clean
  make config
  make fetch
  make
  make install
```

By default, all supported formats of plugins are built except XDG. 
Several DEs like GNOME don't support XDG format well, so desktop icon installations
are disabled by default.
The list of modules for build can be adjusted by specifying FEATURES variable 
at the configuration stage:

```
  make config FEATURES='lv2 vst2 doc'
```

Available modules are:
  * clap - CLAP plugin binaries;
  * doc - HTML documentation;
  * jack - JACK plugin binaries;
  * ladspa - LADSPA plugin binaries;
  * lv2 - LV2 plugin binaries;
  * vst2 - VST2 plugin binaries;
  * xdg - the X11 desktop integration icons/ 


By default plugins use '/usr/local' path as a target directory for installation.
To override this path, the PREFIX variable can be overridden:

```
  make config PREFIX=/usr
```

To build binaries for debugging, use the following commands:

```
  make config DEBUG=1
```

To build binaries for testing (developers only), use the following commands:

```
  make config TEST=1
```

To install plugins at the desired root directory, the DESTDIR variable can be specified:

```
  make install DESTDIR=<installation-root>
```

To build standalone source code package, the following commands can be issued:

```
  make config
  make distsrc
```

After that, a stanalone archive with source code will be created in the `.build` directory.

For more build options, issue:

```
  make help
```

# DEBUGGING

For debugging and getting crash stack trace with Ardour, please follow these steps:
  * Open console
  * Run ardour from console with --gdb option
  * After gdb starts, issue 'run' command
  * Do usual stuff to reproduce the problem
  * After Ardour crashes, type 'thread apply all bt' in console and attach the output
    to the bug report.

# TESTING

Since release 1.1.4 there is implemented testing subsystem that allows:
  * perform manual tests (mtest module);
  * perform automated unit testing (utest module);
  * perform automated single-core performance testing (ptest module).

Manual tests are semi-automated or not automated at all. You may launch these tests
to perform manual interaction with system or for retrieving some text data for further
processing.

Unit tests are completely automated and check that the produced result is correct for the
specific input data. By default unit tests are launched in separate processes and utilize
all CPU cores for maximum performance. The status of each unit test is collected during
execution and list of all failed tests are printed after completion.

Performance tests measure performance of some function/module under different conditions, 
gather final statistics and output them in a table format. These tests are very useful for
measuring single-core performance of different modules and functions and performing code
optimizations.

To build testing subsystem, issue the following commands:
  make clean
  make config TEST=1
  make

After build, we can launch the test binary by issuing command:
  .build/host/lsp-plugin-fw/lsp-plugins-test

This binary provides simple command-line interface, so here's the full usage:  
  USAGE: {utest|ptest|mtest} [args...] [test name...]
    First argument:
      utest                 Unit testing subsystem
      ptest                 Performance testing subsystem
      mtest                 Manual testing subsystem
    Additional arguments:
      -a, --args [args...]  Pass arguments to test
      -d, --debug           Disable time restrictions for unit tests
                            for debugging purposes
      -e, --execute         Launch tests specified after this switch
      -f, --fork            Fork child processes (opposite to --nofork)
      -h, --help            Display help
      -i, --ignore          Ignore tests specified after this switch
      -j, --jobs            Set number of job workers for unit tests
      -l, --list            List all available tests
      -mt, --mtrace         Enable mtrace log
      -nf, --nofork         Do not fork child processes (for better 
                            debugging capabilities)
      -nt, --nomtrace       Disable mtrace log
      -o, --outfile file    Output performance test statistics to specified file
      -s, --silent          Do not output additional information from tests
      -t, --tracepath path  Override default trace path with specified value
      -v, --verbose         Output additional information from tests

Each test has fully-qualified name separated by dot symbols, tests from different
test spaces (utest, ptest, mtest) may have similar fully-qualified names.

To obtain a list of all unit tests we can issue:
  .build/host/lsp-plugin-fw/lsp-plugins-test utest --list

And then we can launch all complex number processing unit tests and additionally
'dsp.mix' unit test:
  .build/host/lsp-plugin-fw/lsp-plugins-test utest dsp.complex.* dsp.pcomplex.* dsp.mix

If we don's specify any unit test name in argument, then all available unit tests
will be launched.

To start debugging of some unit test, you need to pass additional arguments:
  .build/host/lsp-plugin-fw/lsp-plugins-test/lsp-plugins-test utest --nofork --debug --verbose
  
Because unit tests are short-time fully-automated tests, they are parallelized and
executed by default by number_of_cores*2 processes. To disable this, we specify option
--nofork. Also, unit test execution time is limited by 5 seconds by default, so when
debugging, our tests will be killed. That's why we specify --debug option to disable time
control. Option --verbose allows to output additional information by the unit test that is
turned off by default.

We also can use performance tests to obtain full performance profile of target machine.
Because performance tests in most cases take much time for gathering statistics,
the final statistics for each test can be saved in a separate file by specifying --outfile
option:
  .build/host/lsp-plugin-fw/lsp-plugins-test ptest -o performance-test.log

Manual tests are mostly designed for developers' purposes.

# TROUBLESHOOTING

Because there are many GNU/Linux distributions and software configurations,
and, possible, internal bugs, plugins may have bad performance or even crash.
The good way to find the solutiong is to submit a bug or crash report. This
section describes the way to give detailed information to developers.

To submit a good bug or crash report, you should answer these questions:
  * Did you use the previous version of plugins? If yes, did they properly work and
    have you properly uninstalled the previous version?
  * What version of plugins caused a crash? Was it LV2, LinuxVST, LADSPA or
    standalone package?
  * Did you try standalone version of plugins? Did they crash, too?
  * What CPU and memory configuration do you use? This can be done by
    issuing 'cat /proc/cpuinfo' and 'free' commands.
  * What Linux distribution do you use? This may be done by issuing
    'uname -a' and 'cat /etc/issue' commands.
  * What version of required (see 'INSTALLATION') packages are present in your
    system?
  * Provide the full list (besides LSP Plugins) of plugins that you used in your host.
  * If possible, provide the stack trace of the crash, given by GDB.
    See 'PROFILING / DEBUGGING' section for details.
  * If plugins have serious performance regression, please provide profiling file.
    See 'PROFILING / DEBUGGING' section for details.
  * If the plugin behaves incorrectly, there is also good reason to provide the dump
    of internal state of the plugin which can be performed by selectecting the
    'Dump for debug' menu item from the main menu. The internal state of the plugin
    will be saved into the 'test-dumps' subdirectory of the system temporary directory.

# CONTACTS

There is possible to submit feature requests and bugs on the
GitHub.com page of the project:
  https://github.com/sadko4u/lsp-plugins/issues

Sourceforge.net page of the project:
  https://sourceforge.net/p/lsp-plugins/tickets/

You may contact us on the special feedback page:
  https://lsp-plug.in/?page=feedback

Hope you have enjoyed our work!

