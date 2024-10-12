#
# Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
#           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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
  else ifeq ($(findstring Darwin,$(BUILD_SYSTEM)),Darwin)
    PLATFORM       := MacOS
  else ifeq ($(findstring Haiku,$(BUILD_SYSTEM)),Haiku)
    PLATFORM       := Haiku
  endif
endif

# Detect system processor architecture
ifeq ($(PLATFORM),Windows)
  HOST_BUILD_ARCH        := $(PROCESSOR_ARCHITECTURE)
else
  HOST_BUILD_ARCH        := $(shell uname -m)
endif
BUILD_ARCH          := $(if $(ARCHITECTURE),$(ARCHITECTURE),$(HOST_BUILD_ARCH))

ifeq ($(PLATFORM),Linux)
  OBJ_LDFLAGS_X86       =  -m elf_i386
  OBJ_LDFLAGS_X86_64    =  -m elf_x86_64
else ifeq ($(PLATFORM),BSD)
  OBJ_LDFLAGS_X86       =  -m elf_i386
  OBJ_LDFLAGS_X86_64    =  -m elf_x86_64
else ifeq ($(PLATFORM),Windows)
  OBJ_LDFLAGS_X86       =  -m i386pe
  OBJ_LDFLAGS_X86_64    =  -m i386pep
endif

# Set actual architecture for HOST and TARGET builds
# The current architecture can be obtained by: gcc -Q --help=target
define detect_architecture =
  ifeq ($(1),armel)
    $(2)_NAME        = $(1)
    $(2)_FAMILY      = generic
    $(2)_CFLAGS     :=
  else ifeq ($(1),armhf)
    $(2)_NAME        = arm32
    $(2)_FAMILY      = arm32
    $(2)_CFLAGS     := -march=armv7-a+fp -marm
  else ifeq ($(patsubst armv6%,armv6,$(1)),armv6)
    $(2)_NAME        = arm32
    $(2)_FAMILY      = arm32
    $(2)_CFLAGS     := -march=armv6 -marm
  else ifeq ($(patsubst armv7ve%,armv7ve,$(1)),armv7ve)
    $(2)_NAME        = arm32
    $(2)_FAMILY      = arm32
    $(2)_CFLAGS     := -march=armv7ve -marm
  else ifeq ($(patsubst armv7%,armv7,$(1)),armv7)
    $(2)_NAME        = arm32
    $(2)_FAMILY      = arm32
    $(2)_CFLAGS     := -march=armv7-a -marm
  else ifeq ($(patsubst armv8%,armv8,$(1)),armv8)
    $(2)_NAME        = arm32
    $(2)_FAMILY      = arm32
    $(2)_CFLAGS     := -march=armv7-a -marm
  else ifeq ($(patsubst aarch64%,aarch64,$(1)),aarch64)
    $(2)_NAME        = aarch64
    $(2)_FAMILY      = aarch64
    $(2)_CFLAGS     := -march=armv8-a
  else ifeq ($(1),arm64)
    $(2)_NAME        = aarch64
    $(2)_FAMILY      = aarch64
    $(2)_CFLAGS     := -march=armv8-a
  else ifeq ($(1),arm32)
    $(2)_NAME        = arm32
    $(2)_FAMILY      = arm32
    $(2)_CFLAGS     := -march=armv6 -marm
  else ifeq ($(1),arm)
    $(2)_NAME        = arm32
    $(2)_FAMILY      = arm32
    $(2)_CFLAGS     := -march=armv6 -marm
  else ifeq ($(patsubst %x86_64%,x86_64,$(1)),x86_64)
    $(2)_NAME        = x86_64
    $(2)_FAMILY      = x86_64
    $(2)_CFLAGS     := -march=x86-64 -m64
    $(2)_LDFLAGS    := $(OBJ_LDFLAGS_X86_64)
  else ifeq ($(patsubst %amd64%,amd64,$(1)),amd64)
    $(2)_NAME        = x86_64
    $(2)_FAMILY      = x86_64
    $(2)_CFLAGS     := -march=x86-64 -m64
    $(2)_LDFLAGS    := $(OBJ_LDFLAGS_X86_64)
  else ifeq ($(patsubst %AMD64%,AMD64,$(1)),AMD64)
    $(2)_NAME        = x86_64
    $(2)_FAMILY      = x86_64
    $(2)_CFLAGS     := -march=x86-64 -m64
    $(2)_LDFLAGS    := $(OBJ_LDFLAGS_X86_64)
  else ifeq ($(1),i86pc)
    $(2)_NAME        = x86_64
    $(2)_FAMILY      = x86_64
    $(2)_CFLAGS     := -march=x86-64 -m64
    $(2)_LDFLAGS    := $(OBJ_LDFLAGS_X86_64)
  else ifeq ($(patsubst %i686%,i686,$(1)),i686)
    $(2)_NAME        = i686
    $(2)_FAMILY      = ia32
    $(2)_CFLAGS     := -march=i686 -m32
    $(2)_LDFLAGS    := $(OBJ_LDFLAGS_X86)
  else ifeq ($(patsubst i%86,i586,$(1)),i586)
    $(2)_NAME        = i586
    $(2)_FAMILY      = ia32
    $(2)_CFLAGS     := -march=i586 -m32
    $(2)_LDFLAGS    := $(OBJ_LDFLAGS_X86)
  else ifeq ($(1),x86)
    $(2)_NAME        = i686
    $(2)_FAMILY      = ia32
    $(2)_CFLAGS     := -march=i686 -m32
    $(2)_LDFLAGS    := $(OBJ_LDFLAGS_X86)
  else ifeq ($(1),riscv32)
    $(2)_NAME        = riscv32
    $(2)_FAMILY      = riscv32
    $(2)_CFLAGS     := -march=rv32imafdc -mabi=lp32d
  else ifeq ($(1),riscv64)
    $(2)_NAME        = riscv64
    $(2)_FAMILY      = riscv64
    $(2)_CFLAGS     := -march=rv64imafdc -mabi=lp64d
  else
    $(2)_NAME        = $(1)
    $(2)_FAMILY      = generic
    $(2)_CFLAGS     :=
  endif
endef

$(eval $(call detect_architecture,$(BUILD_ARCH),ARCHITECTURE))
$(eval $(call detect_architecture,$(HOST_BUILD_ARCH),HOST_ARCHITECTURE))

override ARCHITECTURE          = $(ARCHITECTURE_NAME)
override HOST_ARCHITECTURE     = $(HOST_ARCHITECTURE_NAME)

# Extension of libraries
ifndef LIBRARY_EXT
  ifeq ($(PLATFORM),Windows)
    LIBRARY_EXT              := .dll
  else ifeq ($(PLATFORM),MacOS) 
    LIBRARY_EXT              := .dylib
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

TEST                       := 0

# Set-up list of common variables
COMMON_VARS = \
	ARCHITECTURE \
	ARCHITECTURE_FAMILY \
	ARCHITECTURE_CFLAGS \
	BUILD_FEATURES \
	CROSS_COMPILE \
	DEBUG \
	EXECUTABLE_EXT \
	EXPORT_SYMBOLS \
	HOST_ARCHITECTURE \
	HOST_ARCHITECTURE_FAMILY \
	HOST_ARCHITECTURE_CFLAGS \
	INSTALL_HEADERS \
	LIBRARY_EXT \
	LIBRARY_PREFIX \
	PKGCONFIG_EXT \
	PLATFORM \
	ROOT_ARTIFACT_ID \
	PROFILE \
	STATICLIB_EXT \
	STRICT \
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
	echo "  CROSS_COMPILE             enable/disable cross-compilation"
	echo "  DEBUG                     build with debug options"
	echo "  DEVEL                     build with modules checked out for read/write URL"
	echo "  EXECUTABLE_EXT            file extension for executable files"
	echo "  EXPORT_SYMBOLS            make export symbols visible" 
	echo "  FEATURES                  list of features enabled in the build"
	echo "  INSTALL_HEADERS           install headers (enabled by default)"
	echo "  LIBRARY_EXT               file extension for library files"
	echo "  LIBRARY_PREFIX            prefix used for library file"
	echo "  PKGCONFIG_EXT             file extension for pkgconfig files"
	echo "  PLATFORM                  target software platform to perform build"
	echo "  PROFILE                   build with profile options"
	echo "  STATICLIB_EXT             file extension for static library files"
	echo "  STRICT                    strict compilation: treat compilation warnings as errors"
	echo "  SUB_FEATURES              list of features disabled in the build as a subtraction of default"
	echo "  TEST                      use test build"
	echo "  TRACE                     compile with additional trace information output"

