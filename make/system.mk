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

# Detect/set processor architecture
ifndef ARCHITECTURE
  ifeq ($(PLATFORM),Windows)
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
      ARCHITECTURE             := i586
      ARCHITECTURE_CFLAGS      := -march=i586 -m32
    else ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
      ARCHITECTURE             := x86_64
      ARCHITECTURE_CFLAGS      := -march=x86-64 -m64
    else
      ARCHITECTURE             := i586
      ARCHITECTURE_CFLAGS      := -march=i586 -m32
    endif
  else # BUILD_PLATFORM != Windows
    BUILD_ARCH             := $(shell uname -m)
    ifeq ($(patsubst armv6%,armv6,$(BUILD_ARCH)),armv6)
      ARCHITECTURE           := arm32
      ARCHITECTURE_CFLAGS    := -march=armv6 -marm
    else ifeq ($(patsubst armv7ve%,armv7ve,$(BUILD_ARCH)),armv7ve)
      ARCHITECTURE           := arm32
      ARCHITECTURE_CFLAGS    := -march=armv7ve -marm
    else ifeq ($(patsubst armv7%,armv7,$(BUILD_ARCH)),armv7)
      ARCHITECTURE           := arm32
      ARCHITECTURE_CFLAGS    := -march=armv7-a -marm
    else ifeq ($(patsubst armv8%,armv8,$(BUILD_ARCH)),armv8)
      ARCHITECTURE           := aarch64
      ARCHITECTURE_CFLAGS    := -march=armv8-a
    else ifeq ($(patsubst aarch64%,aarch64,$(BUILD_ARCH)),aarch64)
      ARCHITECTURE           := aarch64
      ARCHITECTURE_CFLAGS    := -march=armv8-a
    else ifeq ($(BUILD_ARCH),arm)
      ARCHITECTURE           := arm32
      ARCHITECTURE_CFLAGS    := -march=armv6 -marm
    else ifeq ($(BUILD_ARCH),x86_64)
      ARCHITECTURE           := x86_64
      ARCHITECTURE_CFLAGS    := -march=x86-64 -m64
    else ifeq ($(BUILD_ARCH),amd64)
      ARCHITECTURE           := x86_64
      ARCHITECTURE_CFLAGS    := -march=x86-64 -m64
    else ifeq ($(BUILD_ARCH),i86pc)
      ARCHITECTURE           := x86_64
      ARCHITECTURE_CFLAGS    := -march=x86-64 -m64
    else ifeq ($(patsubst i%86,i586,$(BUILD_ARCH)),i586)
      ARCHITECTURE           := i586
      ARCHITECTURE_CFLAGS    := -march=i586 -m32
    else ifeq ($(BUILD_ARCH),x86)
      ARCHITECTURE           := i586
      ARCHITECTURE_CFLAGS    := -march=i586 -m32
    else
      override ARCHITECTURE   =
      ARCHITECTURE_CFLAGS    :=
    endif
  endif # PLATFORM != Windows
else
  ifeq ($(ARCHITECTURE),x86_64)
    ARCHITECTURE_CFLAGS    := -march=x86-64 -m64
  else ifeq ($(ARCHITECTURE),amd64)
    override ARCHITECTURE   = x86_64
    ARCHITECTURE_CFLAGS    := -march=x86-64 -m64
  else ifeq ($(ARCHITECTURE),i586)
    ARCHITECTURE_CFLAGS    := -march=i586 -m32
  else ifeq ($(ARCHITECTURE),ia32)
    override ARCHITECTURE   = i586
    ARCHITECTURE_CFLAGS    := -march=i586 -m32
  else ifeq ($(ARCHITECTURE),x86)
    override ARCHITECTURE   = i586
    ARCHITECTURE_CFLAGS    := -march=i586 -m32
  else ifeq ($(ARCHITECTURE),arm32)
    override ARCHITECTURE   = arm32
    ARCHITECTURE_CFLAGS    := -march=armv6 -marm
  else ifeq ($(ARCHITECTURE),arm32-v6)
    override ARCHITECTURE   = arm32
    ARCHITECTURE_CFLAGS    := -march=armv6 -marm
  else ifeq ($(ARCHITECTURE),armv6)
    override ARCHITECTURE   = arm32
    ARCHITECTURE_CFLAGS    := -march=armv6 -marm
  else ifeq ($(ARCHITECTURE),arm32-v7)
    override ARCHITECTURE   = arm32
    ARCHITECTURE_CFLAGS    := -march=armv7-a -marm
  else ifeq ($(ARCHITECTURE),armv7)
    override ARCHITECTURE   = arm32
    ARCHITECTURE_CFLAGS    := -march=armv7-a -marm
  else ifeq ($(ARCHITECTURE),aarch64)
    ARCHITECTURE_CFLAGS    := -march=armv8-a
  else ifeq ($(ARCHITECTURE),armv8)
    override ARCHITECTURE   = aarch64
    ARCHITECTURE_CFLAGS    := -march=armv8-a
  else
    ARCHITECTURE_CFLAGS    :=
  endif
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
	ROOTDIR \
	ROOT_ARTIFACT_ID \
	PLATFORM \
	ARCHITECTURE \
	ARCHITECTURE_CFLAGS \
	FEATURES \
	LIBRARY_EXT \
	LIBRARY_PREFIX \
	STATICLIB_EXT \
	EXECUTABLE_EXT \
	PKGCONFIG_EXT \
	PREFIX \
	LIBDIR \
	SHAREDDIR \
	BINDIR \
	INCDIR \
	ETCDIR \
	TEMPDIR \
	TEST \
	DEBUG \
	PROFILE \
	TRACE

.PHONY: sysvars

sysvars:
	echo "List of available system variables:"
	echo "  ADD_FEATURES              list of features enabled in the build as an addition to default"
	echo "  ARCHITECTURE              target architecture to perform build"
	echo "  ARCHITECTURE_CFLAGS       compiler flags to specify architecture"
	echo "  BINDIR                    location of the binaries"
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

