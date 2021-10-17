export CFGDIR			= ${CURDIR}/.buildconfig

# Determine installation prefix
ifndef PREFIX
  PREFIX                 := $(shell if (test -f "$(CFGDIR)/$(PREFIX_FILE)" )  then cat "$(CFGDIR)/$(PREFIX_FILE)" 2>/dev/null; else echo "/usr/local"; fi;)
endif

# Determine list of modules to build
ifndef BUILD_MODULES
  BUILD_MODULES          := $(shell if (test -f "$(CFGDIR)/$(MODULES_FILE)" )  then cat "$(CFGDIR)/$(MODULES_FILE)" 2>/dev/null; else echo "ladspa lv2 vst jack profile src doc"; fi;)
endif

ifndef BUILD_R3D_BACKENDS
  BUILD_R3D_BACKENDS     := $(shell if (test -f "$(CFGDIR)/$(R3D_BACKENDS_FILE)" )  then cat "$(CFGDIR)/$(R3D_BACKENDS_FILE)" 2>/dev/null; else echo "glx"; fi;)
endif

BUILD_COMPILER         := $(shell $(CXX) --version | head -n 1 || echo "unknown")
HOST_BUILD_COMPILER    := $(shell $(HOST_CXX) --version | head -n 1 || echo "unknown")

export BUILD_MODULES
export BUILD_R3D_BACKENDS

# Configure list of targets to execute
INSTALLATIONS           =
UNINSTALLATIONS         = uninstall_xdg
RELEASES                =
INCLUDE                := -I"${CURDIR}/include"

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

# Build profile
# SKIP_CC_LD_ARCH is set by distro package build systems
# which manage cross-compilation flags on their own
ifeq ($(SKIP_CC_LD_ARCH),)
  ifeq ($(BUILD_PROFILE),i586)
    CC_ARCH          = -m32
    ifeq ($(BUILD_PLATFORM), Linux)
      LD_ARCH          = -m elf_i386
    endif
    ifeq ($(BUILD_PLATFORM), BSD)
      LD_ARCH          = -m elf_i386_fbsd
    endif
  endif
  
  ifeq ($(BUILD_PROFILE),x86_64)
    CC_ARCH          = -m64
    ifeq ($(BUILD_PLATFORM), Linux)
      LD_ARCH          = -m elf_x86_64
    endif
    ifeq ($(BUILD_PLATFORM), BSD)
      LD_ARCH          = -m elf_x86_64_fbsd
    endif
  endif
  
  ifeq ($(BUILD_PLATFORM), BSD)
    INCLUDE          += -I/usr/local/include
    ifeq ($(BUILD_PROFILE),arm)
      CC_ARCH          = -marm
      ifneq ($(LD_PATH),)
        CC_ARCH          += -Wl,-rpath=$(LD_PATH)
      endif
    endif
  endif
  
  ifeq ($(BUILD_PROFILE),armv6a)
    CC_ARCH          = -march=armv6-a -marm
  endif
  
  ifeq ($(BUILD_PROFILE),armv7a)
    CC_ARCH          = -march=armv7-a -marm
  endif
  
  ifeq ($(BUILD_PROFILE),armv7ve)
    CC_ARCH          = -march=armv7ve -marm
  endif
  
  ifeq ($(BUILD_PROFILE),arm32)
    CC_ARCH          = -marm
  endif
  
  ifeq ($(BUILD_PROFILE),armv8a)
    CC_ARCH          = -march=armv7-a -marm
  endif
  
  ifeq ($(BUILD_PROFILE),aarch64)
    CC_ARCH          = -march=armv8-a
  endif
endif

export CC_ARCH
export LD_ARCH
export LD_PATH
export INCLUDE

# Dependencies: compile headers and linkage libraries
ifeq ($(BUILD_SYSTEM),Windows)
  export BIN_SUFFIX          := .exe
else
  export BIN_SUFFIX          :=
  export PTHREAD_LIBS         = -lpthread
  export ICONV_LIBS           = -liconv
  export MATH_LIBS            = -lm
  export DL_LIBS              = -ldl
  export CAIRO_HEADERS        = $(shell $(PKG_CONFIG) --cflags cairo)
  export CAIRO_LIBS           = $(shell $(PKG_CONFIG) --libs cairo)
  export XLIB_HEADERS         = $(shell $(PKG_CONFIG) --cflags x11)
  export XLIB_LIBS            = $(shell $(PKG_CONFIG) --libs x11)
  export LV2_HEADERS          = $(shell $(PKG_CONFIG) --cflags lv2)
  export LV2_HEADERS          = $(shell $(PKG_CONFIG) --libs lv2)
  export SNDFILE_HEADERS      = $(shell $(PKG_CONFIG) --cflags sndfile)
  export SNDFILE_LIBS         = $(shell $(PKG_CONFIG) --libs sndfile)
  export JACK_HEADERS         = $(shell $(PKG_CONFIG) --cflags jack)
  export JACK_LIBS            = $(shell $(PKG_CONFIG) --libs jack)
  export OPENGL_HEADERS       = $(shell $(PKG_CONFIG) --cflags gl 2>/dev/null || echo "")
  export OPENGL_LIBS          = $(shell $(PKG_CONFIG) --libs gl 2>/dev/null || echo "")
  export HOST_SNDFILE_HEADERS = $(shell $(HOST_PKG_CONFIG) --cflags sndfile)
  export HOST_SNDFILE_LIBS    = $(shell $(HOST_PKG_CONFIG) --libs sndfile)
  export HOST_LV2_HEADERS     = $(shell $(HOST_PKG_CONFIG) --cflags lv2)
  export HOST_LV2_LIBS        = $(shell $(HOST_PKG_CONFIG) --libs lv2)
endif

