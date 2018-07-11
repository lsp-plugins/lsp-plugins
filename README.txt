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

The source code takes a lot of time to develop (instead of drinking beer
or doing something else) so it's fairly true that the spent time has
to be paid.

That's why the project uses donation model: the binary distributions
of the software are completely free but the source code for each plugin
will be published under the free license only after the donation goal
is reached.

For more information, please read LICENSE.txt.

==== SYSTEM REQUIREMENTS ====

LSP plugins are developed to support both LADSPA and LV2 formats and
at least i586 and x86_64 architectures. The LinuxVST format has only
partial support at this moment (no user interface).

The LADSPA distribution requirements:
  * glibc >= 2.19.
  * Host compatible with LADSPA

The LV2 distribution requirements:
  * glibc >= 2.19
  * libexpat >= 2.1
  * GTK+ >= 2.24
  * libcairo >= 1.14
  * Host compatible with LV2
  
The LinuxVST distribution requirements:
  * glibc >= 2.19
  * libexpat >= 2.1
  * GTK+ >= 2.24
  * libcairo >= 1.14
  * Host compatible with LinuxVST v2.4

==== INSTALLATION ====

The binary distributions are ready to launch, just copy them to
the appropriate directory.

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

==== BUILDING ====

For source code distributions you may build plugins from scratch.
You need the following packages to be installed:
  * gcc >= 4.7
  * gcc-c++ >= 4.7
  * libgcc_s1 >= 5.2
  * libstdc++ >= 4.7
  * libexpat-devel >= 2.1
  * GTK+-devel >= 2.24
  * libcairo-devel >= 1.14

Currently there is no automake supported, so to build plugins you
have to type:
  make
  make install

You may also specify the installation root by specifying DESTDIR attribute:
  make install DESTDIR=<installation-root>

==== CONTACTS ====

You may contact us on the special feedback page:
  http://lsp-plug.in/?page=feedback

Hope you have enjoyed our work!

