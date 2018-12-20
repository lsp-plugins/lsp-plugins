PREFIX_FILE            := .install-prefix.txt
MODULES_FILE           := .install-modules.txt

# Determine installation prefix
ifndef PREFIX
  PREFIX                 := $(shell cat "$(OBJDIR)/$(PREFIX_FILE)" 2>/dev/null || echo "/usr/local")
endif

# Determine list of modules to build
ifndef BUILD_MODULES
  BUILD_MODULES          := $(shell cat "$(OBJDIR)/$(MODULES_FILE)" 2>/dev/null || echo "ladspa lv2 vst jack profile src doc")
endif

export BUILD_MODULES

# Configure list of targets to execute
INSTALLATIONS           =
UNINSTALLATIONS         =
RELEASES                =

ifeq ($(findstring ladspa,$(BUILD_MODULES)),ladspa)
  INSTALLATIONS          += install_ladspa
  UNINSTALLATIONS        += uninstall_ladspa
  RELEASES               += release_ladspa
endif
ifeq ($(findstring lv2,$(BUILD_MODULES)),lv2)
  INSTALLATIONS          += install_lv2
  UNINSTALLATIONS        += uninstall_lv2
  RELEASES               += release_lv2
endif
ifeq ($(findstring vst,$(BUILD_MODULES)),vst)
  INSTALLATIONS          += install_vst
  UNINSTALLATIONS        += uninstall_vst
  RELEASES               += release_vst
endif
ifeq ($(findstring jack,$(BUILD_MODULES)),jack)
  INSTALLATIONS          += install_jack
  UNINSTALLATIONS        += uninstall_jack
  RELEASES               += release_jack
endif
ifeq ($(findstring doc,$(BUILD_MODULES)),doc)
  INSTALLATIONS          += install_doc
  UNINSTALLATIONS        += uninstall_doc
  RELEASES               += release_doc
endif
ifeq ($(findstring src,$(BUILD_MODULES)),src)
  RELEASES               += release_src
endif

export INSTALLATIONS
export UNINSTALLATIONS
export RELEASES

# Configure compiler and linker flags
LD_ARCH         =
CC_ARCH         =
LD_PATH         =

# Build profile
ifeq ($(BUILD_PROFILE),i586)
  CC_ARCH          = -m32
  ifeq ($(BUILD_PLATFORM), Linux)
    LD_ARCH          = -m elf_i386
  endif
  ifeq ($(BUILD_PLATFORM), BSD)
    LD_ARCH          = -m elf_i386_fbsd
  endif
  LD_PATH          = /usr/lib:/lib:/usr/local/lib
endif

ifeq ($(BUILD_PROFILE),x86_64)
  CC_ARCH          = -m64
  ifeq ($(BUILD_PLATFORM), Linux)
    LD_ARCH          = -m elf_x86_64
  endif
  ifeq ($(BUILD_PLATFORM), BSD)
  	LD_ARCH          = -m elf_x86_64_fbsd
  endif
  LD_PATH          = /usr/lib:/lib:/usr/local/lib
endif

ifeq ($(BUILD_PROFILE),armv6a)
  CC_ARCH          = -march=armv6-a
  LD_PATH          = /usr/lib64:/lib64:/usr/local/lib64
endif

ifeq ($(BUILD_PROFILE),armv7a)
  CC_ARCH          = -march=armv7-a
  LD_PATH          = /usr/lib64:/lib64:/usr/local/lib64
endif

ifeq ($(BUILD_PROFILE),armv8a)
  CC_ARCH          = -march=armv8-a
  LD_PATH          = /usr/lib:/lib:/usr/local/lib
endif

export CC_ARCH
export LD_ARCH
export LD_PATH


# Dependencies: compile headers and linkage libraries
ifeq ($(BUILD_SYSTEM),Windows)
# TODO
else
	export PTHREAD_LIBS     = -lpthread
	export ICONV_LIBS       = -liconv
	export MATH_LIBS        = -lm
	export DL_LIBS          = -ldl
	export CAIRO_HEADERS    = $(shell pkg-config --cflags cairo)
	export CAIRO_LIBS       = $(shell pkg-config --libs cairo)
	export XLIB_HEADERS     = $(shell pkg-config --cflags x11)
	export XLIB_LIBS        = $(shell pkg-config --libs x11)
	export EXPAT_HEADERS    = $(shell pkg-config --cflags expat)
	export EXPAT_LIBS       = $(shell pkg-config --libs expat)
	export SNDFILE_HEADERS  = $(shell pkg-config --cflags sndfile)
	export SNDFILE_LIBS     = $(shell pkg-config --libs sndfile)
	export JACK_HEADERS     = $(shell pkg-config --cflags jack)
	export JACK_LIBS        = $(shell pkg-config --libs jack)
	export OPENGL_HEADERS   = $(shell pkg-config --cflags gl glu 2>/dev/null || echo "")
	export OPENGL_LIBS      = $(shell pkg-config --libs gl glu 2>/dev/null || echo "")
endif

