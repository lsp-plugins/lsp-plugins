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

The source code takes a lot of time to develop, so the project uses
donation model to compensate time spent for development.

That's why the project uses donation model: the binary distributions
of the software are completely free but the source code for each plugin
will be published under the free license only after the donation goal
is reached.

For more information, please read LICENSE.txt.

==== SYSTEM REQUIREMENTS ====

LSP plugins are developed to support LADSPA, LV2 and LinuxVST formats and
at least i586 and x86_64 architectures. Plugins that require MIDI channels
are not implemented in LASDPA format due to format's restrictions. Also
there are available standalone versions for JACK server.

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
  * Host compatible with LinuxVST v2.4

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
  * Reaper native linux version
  * Renoise
  * Tracktion

==== VERSIONING ====

The distribution of plugins consists of two branches:
  * Binary releases with closed source.
  * Published source code (Source Code Release, SCR).

Binary releases are the mostly actual releases and contain all
possible plugins. These are packaged into archive named according to
the following format:

  lsp-plugins-<format>-<major>.<minor>.<micro>-<arch>.tar.gz

The property <format> is the format of plugins, currently available:
  * jack - standalone version of plugins that require JACK for execution
  * ladspa - plugins in LADSPA format (not all plugins due to format's restriction)
  * lv2 - plugins in LV2 format
  * lxvst - plugins in LinuxVST (VST v2.4) format

Supporetd architectures (<arch>) are currently only i586 and x86_64.
Properties <major>, <minor> and <micro> form the version of release.
If the <micro> version is odd, it contains mostly bug-fixes and patches for
the plugin modules. The even <micro> version of plugins contains new features
and, in most cases, new plugin series. The history of changes may be seen in
attached CHANGELOG.txt.

Published source code contains source code available to build
and launch plugins that have been donated by the community.
It is commited into the code repository and currently not packaged into
the official release to avoid confusion between binary and source code
package versioning. It's highly recommended to follow the versioning
of the SCR (source code release) according to the attached CHANGELOG.txt
file and use the 'scr' abbreviation for the packages that are built from
the source code. For example, Linux distribution packages could be named:

  lsp-plugins-<format>-scr-<major>.<minor>.<micro>-<arch>.tar.gz
  lsp-plugins-<format>-scr-<major>.<minor>.<micro>-<arch>.rpm
  lsp-plugins-<format>-scr-<major>.<minor>.<micro>-<arch>.deb

More information about donation and policy of source code publishing may
be obtained here:
  http://lsp-plug.in/?page=download

==== INSTALLATION ====

Binary distributions are ready to launch, just copy them to
the appropriate directory.

Releases containing odd number in minor version provide only
critical fixes for the previous release.

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

For source code distributions you may build plugins from scratch.
You need the following packages to be installed:
  * gcc >= 4.7
  * gcc-c++ >= 4.7
  * libgcc_s1 >= 5.2
  * libstdc++ >= 4.7
  * libexpat-devel >= 2.1
  * libsndfile-devel >= 1.0.25
  * libcairo-devel >= 1.14
  * jack-devel >= 1.9.5

Currently there is no automake supported, so to build plugins you
have to type:
  make
  make install

To build binaries for debuggin/profiling, use the following commands:
  make profile

You may also specify the installation root by specifying DESTDIR attribute:
  make install DESTDIR=<installation-root>
  
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

You may contact us on the special feedback page:
  http://lsp-plug.in/?page=feedback

Hope you have enjoyed our work!

