#
# Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
#           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
#
# This file is part of lsp-plugins
#
# lsp-plugins is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# lsp-plugins is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with lsp-plugins.  If not, see <https://www.gnu.org/licenses/>.
#

# Detect operating system
ifndef PLATFORM
  ifeq ($(findstring Windows,$(OS)),Windows)
    BUILD_SYSTEM   := Windows
  else
    BUILD_SYSTEM   := $(shell uname -s 2>/dev/null || echo "Unknown")
  endif
  
  PLATFORM       := Unknown

  ifeq ($(BUILD_SYSTEM),Windows)
    PLATFORM       := Windows
  else ifeq ($(findstring OpenBSD,$(BUILD_SYSTEM)),OpenBSD)
    PLATFORM       := OpenBSD
  else ifeq ($(findstring BSD,$(BUILD_SYSTEM)),BSD)
    PLATFORM       := BSD
  else ifeq ($(findstring Linux,$(BUILD_SYSTEM)),Linux)
    PLATFORM       := Linux
  else ifeq ($(findstring SunOS,$(BUILD_SYSTEM)),SunOS)
    PLATFORM       := Solaris
  endif
endif

# Detect system processor architecture
ifndef ARCHITECTURE
  ifeq ($(PLATFORM),Windows)
    BUILD_ARCH             := $(PROCESSOR_ARCHITECTURE)
  else
    BUILD_ARCH             := $(shell uname -m)
  endif
else
  BUILD_ARCH             := $(ARCHITECTURE)
endif

# Set actual architecture
# The current architecture can be obtained by: gcc -Q --help=target
ifeq ($(BUILD_ARCH),armel)
  override ARCHITECTURE   = $(BUILD_ARCH)
  ARCHITECTURE_FAMILY     = generic
  ARCHITECTURE_CFLAGS    :=
else ifeq ($(patsubst armv6%,armv6,$(BUILD_ARCH)),armv6)
  override ARCHITECTURE   = arm32
  ARCHITECTURE_FAMILY     = arm32
  ARCHITECTURE_CFLAGS    := -march=armv6 -marm
else ifeq ($(patsubst armv7ve%,armv7ve,$(BUILD_ARCH)),armv7ve)
  override ARCHITECTURE   = arm32
  ARCHITECTURE_FAMILY     = arm32
  ARCHITECTURE_CFLAGS    := -march=armv7ve -marm
else ifeq ($(patsubst armv7%,armv7,$(BUILD_ARCH)),armv7)
  override ARCHITECTURE   = arm32
  ARCHITECTURE_FAMILY     = arm32
  ARCHITECTURE_CFLAGS    := -march=armv7-a+fp -marm
else ifeq ($(patsubst armv8%,armv8,$(BUILD_ARCH)),armv8)
  override ARCHITECTURE   = arm32
  ARCHITECTURE_FAMILY     = arm32
  ARCHITECTURE_CFLAGS    := -march=armv7-a+fp -marm
else ifeq ($(patsubst aarch64%,aarch64,$(BUILD_ARCH)),aarch64)
  override ARCHITECTURE   = aarch64
  ARCHITECTURE_FAMILY     = aarch64
  ARCHITECTURE_CFLAGS    := -march=armv8-a
else ifeq ($(BUILD_ARCH),arm64)
  override ARCHITECTURE   = aarch64
  ARCHITECTURE_FAMILY     = aarch64
  ARCHITECTURE_CFLAGS    := -march=armv8-a
else ifeq ($(BUILD_ARCH),arm32)
  override ARCHITECTURE   = arm32
  ARCHITECTURE_FAMILY     = arm32
  ARCHITECTURE_CFLAGS    := -march=armv6 -marm
else ifeq ($(BUILD_ARCH),arm)
  override ARCHITECTURE   = arm32
  ARCHITECTURE_FAMILY     = arm32
  ARCHITECTURE_CFLAGS    := -march=armv6 -marm
else ifeq ($(patsubst %x86_64%,x86_64,$(BUILD_ARCH)),x86_64)
  override ARCHITECTURE   = x86_64
  ARCHITECTURE_FAMILY     = x86_64
  ARCHITECTURE_CFLAGS    := -march=x86-64 -m64
else ifeq ($(patsubst %amd64%,amd64,$(BUILD_ARCH)),amd64)
  override ARCHITECTURE   = x86_64
  ARCHITECTURE_FAMILY     = x86_64
  ARCHITECTURE_CFLAGS    := -march=x86-64 -m64
else ifeq ($(patsubst %AMD64%,AMD64,$(BUILD_ARCH)),AMD64)
  override ARCHITECTURE   = x86_64
  ARCHITECTURE_FAMILY     = x86_64
  ARCHITECTURE_CFLAGS    := -march=x86-64 -m64
else ifeq ($(BUILD_ARCH),i86pc)
  override ARCHITECTURE   = x86_64
  ARCHITECTURE_FAMILY     = x86_64
  ARCHITECTURE_CFLAGS    := -march=x86-64 -m64
else ifeq ($(patsubst %i686%,i686,$(BUILD_ARCH)),i686)
  override ARCHITECTURE   = i686
  ARCHITECTURE_FAMILY     = ia32
  ARCHITECTURE_CFLAGS    := -march=i686 -m32
else ifeq ($(patsubst i%86,i586,$(BUILD_ARCH)),i586)
  override ARCHITECTURE   = i586
  ARCHITECTURE_FAMILY     = ia32
  ARCHITECTURE_CFLAGS    := -march=i586 -m32
else ifeq ($(BUILD_ARCH),x86)
  override ARCHITECTURE   = i586
  ARCHITECTURE_FAMILY     = ia32
  ARCHITECTURE_CFLAGS    := -march=i586 -m32
else ifeq ($(BUILD_ARCH),riscv32)
  override ARCHITECTURE   = riscv32
  ARCHITECTURE_FAMILY     = riscv32
  ARCHITECTURE_CFLAGS    := -march=rv32imafdc -mabi=lp32d
else ifeq ($(BUILD_ARCH),riscv64)
  override ARCHITECTURE   = riscv64
  ARCHITECTURE_FAMILY     = riscv64
  ARCHITECTURE_CFLAGS    := -march=rv64imafdc -mabi=lp64d
else
  override ARCHITECTURE   = $(BUILD_ARCH)
  ARCHITECTURE_FAMILY     = generic
  ARCHITECTURE_CFLAGS    :=
endif

# Extension of libraries
ifndef LIBRARY_EXT
  ifeq ($(PLATFORM),Windows)
    LIBRARY_EXT              := .dll
  else
    LIBRARY_EXT              := .so
  endif
endif

# Extension of libraries
ifndef LIBRARY_PREFIX
  ifeq ($(PLATFORM),Windows)
    LIBRARY_PREFIX           :=
  else
    LIBRARY_PREFIX           := lib
  endif
endif

# Extension of executables
ifndef EXECUTABLE_EXT
  ifeq ($(PLATFORM),Windows)
    EXECUTABLE_EXT           := .exe
  else
    EXECUTABLE_EXT           :=
  endif
endif

# Extension of executables
ifndef STATICLIB_EXT
  STATICLIB_EXT            := .a
endif

# Extension of pkgconfig files
ifndef PKGCONFIG_EXT
  PKGCONFIG_EXT            := .pc
endif

# Installation prefix
ifndef PREFIX
  ifeq ($(PLATFORM),Windows)
    PREFIX                   := $(ProgramFiles)
  else
    PREFIX                   := /usr/local
  endif
endif

# Library prefix
ifndef LIBDIR
  LIBDIR                   := $(PREFIX)/lib
endif

# Binaries prefix
ifndef BINDIR
  BINDIR                   := $(PREFIX)/bin
endif

# Binaries prefix
ifndef INCDIR
  INCDIR                   := $(PREFIX)/include
endif

# Temporary directory
ifndef TEMPDIR
  ifeq ($(PLATFORM),Windows)
    TEMPDIR                  := $(TEMP)
  else
    TEMPDIR                  := /tmp
  endif
endif

TEST                       := 0

# Set-up list of common variables
COMMON_VARS = \
	ARCHITECTURE \
	ARCHITECTURE_FAMILY \
	ARCHITECTURE_CFLAGS \
	BINDIR \
	BUILDDIR \
	DEBUG \
	ETCDIR \
	EXECUTABLE_EXT \
	EXPORT_SYMBOLS \
	FEATURES \
	INCDIR \
	INSTALL_HEADERS \
	LIBDIR \
	LIBRARY_EXT \
	LIBRARY_PREFIX \
	PKGCONFIG_EXT \
	PLATFORM \
	PREFIX \
	ROOTDIR \
	ROOT_ARTIFACT_ID \
	PROFILE \
	SHAREDDIR \
	STATICLIB_EXT \
	TEMPDIR \
	TEST \
	TRACE

.PHONY: sysvars

sysvars:
	echo "List of available system variables:"
	echo "  ADD_FEATURES              list of features enabled in the build as an addition to default"
	echo "  ARCHITECTURE              target architecture to perform build"
	echo "  ARCHITECTURE_CFLAGS       compiler flags to specify architecture"
	echo "  ARCHITECTURE_FAMILY       compiler flags to specify architecture family"
	echo "  ARCHITECTURE_LDFLAGS      linker flags to specify architecture"
	echo "  BINDIR                    location of the binaries"
	echo "  BUILDDIR                  location of the build directory"
	echo "  DEBUG                     build with debug options"
	echo "  DEVEL                     build with modules checked out for read/write URL"
	echo "  ETCDIR                    location of system configuration files"
	echo "  EXECUTABLE_EXT            file extension for executable files"
	echo "  FEATURES                  list of features enabled in the build"
	echo "  INCDIR                    location of the header files"
	echo "  LIBDIR                    location of the library"
	echo "  LIBRARY_EXT               file extension for library files"
	echo "  LIBRARY_PREFIX            prefix used for library file"
	echo "  PKGCONFIG_EXT             file extension for pkgconfig files"
	echo "  PLATFORM                  target software platform to perform build"
	echo "  PREFIX                    installation prefix for binary files"
	echo "  PROFILE                   build with profile options"
	echo "  SHAREDDIR                 location of the shared files"
	echo "  STATICLIB_EXT             file extension for static library files"
	echo "  SUB_FEATURES              list of features disabled in the build as a subtraction of default"
	echo "  TEMPDIR                   location of temporary directory"
	echo "  TEST                      use test build"
	echo "  TRACE                     compile with additional trace information output"

