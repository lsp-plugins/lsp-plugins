==== ABOUT ====

LSP (Linux Studio Plugins) is a collection of open-source plugins
currently compatible with LADSPA, LV2 and LinuxVST formats.

The basic idea is to fill the lack of good and useful plugins under
the GNU/Linux platform.

After some contributions to other open source projects the decision
was made to implement separate and independent plugin distribution.

All supplementary information you will find on official web site:
  http://lsp-plug.in/

==== LICENSING ====

The LSP project is an open-source project and cares about quality of
developed software.

Still there is no absolute warranty about stability of the software on
different platforms so you're using this software on your own risk.

The project also does encourage donations that can be submitted here:
https://salt.bountysource.com/teams/lsp-plugins

For more information about licensing, please read LICENSE.txt.

==== SYSTEM REQUIREMENTS ====

Currently supported platforms:
  * GNU/Linux;
  * FreeBSD (experimental).

Currently supported architectures are:
  * i586 (Intel IA-32 architecture, legacy support);
  * x86_64 (Intel EM64T/AMD64 architecture, full support);
  * ARMv7-AR (experimental, tested on Raspberry Pi 3 Model B).

Supported plugin formats:
  * LADSPA (partial support: not supported by plugins that use MIDI or file loading due to LADSPA plugin format restrictions);
  * LV2 (full support);
  * LinuxVST 2.4 (full support);
  * Standalone JACK (full support).

The LADSPA distribution requirements:
  * glibc >= 2.19
  * libsndfile >= 1.0.25
  * Host compatible with LADSPA

The LV2 distribution requirements:
  * glibc >= 2.19
  * libsndfile >= 1.0.25
  * libcairo >= 1.14
  * Host compatible with LV2
  
The LinuxVST distribution requirements:
  * glibc >= 2.19
  * libsndfile >= 1.0.25
  * libcairo >= 1.14
  * Host compatible with LinuxVST 2.4

The JACK distribution requirements:
  * glibc >= 2.19
  * libsndfile >= 1.0.25
  * libcairo >= 1.14
  * jack >= 1.9.5
  
The profiling distribution requirements:
  * glibc >= 2.19
  * libsndfile >= 1.0.25
  * libcairo >= 1.14
  * jack >= 1.9.5

Known list of supported plugin hosts:
  * Ardour
  * Bitwig Studio
  * Carla
  * Mixbus
  * Qtractor
  * Reaper (native Linux version)
  * Renoise
  * Tracktion

==== VERSIONING ====

Binary releases are the mostly actual releases and contain all
possible plugins. These are packaged into archive named according to
the following format:

  lsp-plugins-<format>-<major>.<minor>.<micro>-<arch>.tar.gz

The property <format> is the format of plugins, currently available:
  * jack - standalone version of plugins that require JACK for execution
  * ladspa - plugins in LADSPA format (not all plugins due to format's restriction)
  * lv2 - plugins in LV2 format
  * lxvst - plugins in LinuxVST (VST v2.4) format

Supporetd architectures (<arch>) are currently i586, x86_64 and armv7a.
Properties <major>, <minor> and <micro> form the version of release.
If the <micro> version is odd, it contains mostly bug-fixes and patches for
the plugin modules. The even <micro> version of plugins contains new features
and, in most cases, new plugin series. The history of changes may be seen in
attached CHANGELOG.txt.

Source code is also ready for building, see 'BUILDING' section for details.

==== INSTALLATION ====

Binary distributions are ready to launch, just copy them to
the appropriate directory.

Releases containing odd number in minor version provide only enhancements
and critical fixes for the previous release.

IMPORTANT FOR VST INSTALLATIONS: If you deploy plugins as a subdirectory
of your VST directory, the subdirectory should contain substring
'lsp-plugins'. Otherwise plugins won't find the VST core library.

The usual directories for LADSPA are:
  * /usr/lib/ladspa
  * /usr/local/lib/ladspa
  * /usr/lib64/ladspa
  * /usr/local/lib64/ladspa

The usual directories for LV2 are:
  * /usr/lib/lv2
  * /usr/local/lib/lv2
  * /usr/lib64/lv2
  * /usr/local/lib64/lv2

The usual directories for LinuxVST are:
  * /usr/lib/vst
  * /usr/local/lib/vst
  * /usr/lib64/vst
  * /usr/local/lib64/vst

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

The profiling release does not require special installations and can be executed
from any location. After execution, the gprof profiling file 'gmon.out' is created.

==== BUILDING ====

You may build plugins from scratch.

For successful build you need the following packages to be installed:
  * gcc >= 4.7
  * gcc-c++ >= 4.7
  * libgcc_s1 >= 5.2
  * libstdc++-devel >= 4.7
  * jack-devel >= 1.9.5
  * lv2-devel >= 1.10
  * ladspa-devel >= 1.13
  * libexpat-devel >= 2.1
  * libsndfile-devel >= 1.0.25
  * libcairo-devel >= 1.14
  * php >= 5.5.14

For development, additional packages are required to be installed:
  * glu-devel >= 9.0.0
  * libGL-devel >= 11.2.2

Currently there is no automake/CMake supported, so to build plugins you
have to type:
  make clean
  make
  make install

By default, all supported formats of plugins are built. You may control
list of built plugin formats by specifying BUILD_MODULES variable:
  make clean
  make BUILD_MODULES='lv2 vst doc'
  make install

Available modules are:
  * ladspa - LADSPA plugin binaries
  * lv2 - LV2 plugin binaries
  * vst - LinuxVST plugin binaries
  * jack - JACK plugin binaries
  * doc - HTML documentation

Also possible (but not recommended) to specify compile targets:
  make clean
  make build_ladspa
  make build_lv2
  make build_vst
  make build_jack
  make build_doc

By default plugins use '/usr/local' path as installation directory. To
override this path, you can run build with specifying PREFIX variable:
  make clean
  make PREFIX=/usr
  make install

By default, 'make install' will install all plugin formats (LADSPA, LV2,
LinuxVST, JACK) and documentation. To install them separately, you can issue
following commands:
  make clean
  make
  make install_ladspa
  make install_lv2
  make install_vst
  make install_jack
  make install_doc

To build binaries for debugging/profiling, use the following commands:
  make clean
  make profile

To build binaries for testing (developers only), use the following commands:
  make clean
  make test

You may also specify the installation root by specifying DESTDIR attribute:
  make install DESTDIR=<installation-root>

To perform cross-building between different architectures, you first should
have a corresponding toolchain. By default, LSP plugins use 'uname' tool for
detecting target architecture and set internal variable BUILD_PROFILE to the
detected value. Currently supported values are 'x86_64', 'i586', 'armv7a'.
To build plugins for another architecture, just issue following commands:
  make clean
  make BUILD_PROFILE=<target architecture>

To automatically build tarballs with binaries, you may use 'release' target:
  make clean
  make release

After issuing these commands, '.release' subdirectory will contain tarballs with
binaries, documentation and sources. To perform release for another architecture,
same way with BUILD_PROFILE is possible:
  make clean
  make BUILD_PROFILE=<target architecture> release

To remove all previsously built tarballs, just issue:
  make unrelease 

==== PROFILING / DEBUGGING ====

To profile code, untar special profiling release into directory on the file
system and simply launch it:
  tar xzf lsp-plugins-profile-<version>-<arch>.tar.gz
  cd lsp-plugins-profile-<version>-<arch>
  ./lsp-plugins-profile <plugin-id>

After the execution, the profiling file 'gmon.out' will be generated.
To generate profiling information, issue the gprof command:
  gprof ./lsp-plugins-profile gmon.out >lsp-plugins.profile

For debugging purposes the GNU Debugger (gdb) may be used:
  gdb --args ./lsp-plugins-profile <plugin-id>

For debugging and getting crash stack trace with Ardour, please follow these steps:
  * Open console
  * Run ardour from console with --gdb option
  * After gdb starts, issue 'run' command
  * Do usual stuff to reproduce the problem
  * After Ardour crashes, type 'thread apply all bt' in console and attach the output
    to the bug report.

==== TESTING ====

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
  make test

After build, we can launch the test binary by issuing command:
  .build/lsp-plugins-test

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
  .build/lsp-plugins-test utest --list

And then we can launch all complex number processing unit tests and additionally
'dsp.mix' unit test:
  .build/lsp-plugins-test utest dsp.complex.* dsp.pcomplex.* dsp.mix

If we don's specify any unit test name in argument, then all available unit tests
will be launched.

To start debugging of some unit test, you need to pass additional arguments:
  .build/lsp-plugins-test utest --nofork --debug --verbose
  
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
  .build/lsp-plugins-test ptest -o performance-test.log

Manual tests are mostly designed for developers' purposes.

==== TROUBLESHOOTING ====

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

==== CONTACTS ====

There is possible to submit feature requests and bugs on the
Sourceforge.net page of the project:
  https://sourceforge.net/p/lsp-plugins/tickets/

GitHub.com page of the project:
  https://github.com/sadko4u/lsp-plugins/issues

You may contact us on the special feedback page:
  http://lsp-plug.in/?page=feedback

Hope you have enjoyed our work!

