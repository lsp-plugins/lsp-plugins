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

# Determine tools
X_PHP_TOOL         := php
ifeq ($(PLATFORM),OpenBSD)
  X_CC_TOOL          := egcc
  X_CXX_TOOL         := eg++
  X_AS_TOOL          := gas
  X_AR_TOOL          := ar
  X_LD_TOOL          := ld
  X_PKG_CONFIG       := pkg-config
else ifeq ($(PLATFORM),BSD)
  X_CC_TOOL          := clang
  X_CXX_TOOL         := clang++
  X_AS_TOOL          := as
  X_AR_TOOL          := ar
  X_LD_TOOL          := ld
  X_PKG_CONFIG       := pkg-config
else ifeq ($(PLATFORM),MacOS)
  X_CC_TOOL          := clang
  X_CXX_TOOL         := clang++
  X_AS_TOOL          := as
  X_AR_TOOL          := ar
  X_LD_TOOL          := ld
  X_PKG_CONFIG       := pkg-config
else
  X_CC_TOOL          := gcc
  X_CXX_TOOL         := g++
  X_AS_TOOL          := as
  X_AR_TOOL          := ar
  X_LD_TOOL          := ld
  X_PKG_CONFIG       := pkg-config
endif

X_GIT_TOOL         := git
X_INSTALL_TOOL     := install

# Define tool variables for (cross) build
CC                 ?= $(X_CC_TOOL)
CXX                ?= $(X_CXX_TOOL)
AS                 ?= $(X_AS_TOOL)
AR                 ?= $(X_AR_TOOL)
LD                 ?= $(X_LD_TOOL)
PHP                ?= $(X_PHP_TOOL)
PKG_CONFIG         ?= $(X_PKG_CONFIG)

# Define tool variables for host build
ifeq ($(CROSS_COMPILE),1)
  HOST_CC            ?= $(X_CC_TOOL)
  HOST_CXX           ?= $(X_CXX_TOOL)
  HOST_AS            ?= $(X_AS_TOOL)
  HOST_AR            ?= $(X_AR_TOOL)
  HOST_LD            ?= $(X_LD_TOOL)
  HOST_PHP           ?= $(X_PHP_TOOL)
  HOST_PKG_CONFIG    ?= $(X_PKG_CONFIG)
else
  HOST_CC            ?= $(CC)
  HOST_CXX           ?= $(CXX)
  HOST_AS            ?= $(AS)
  HOST_AR            ?= $(AR)
  HOST_LD            ?= $(LD)
  HOST_PHP           ?= $(PHP)
  HOST_PKG_CONFIG    ?= $(PKG_CONFIG)
endif

# Miscellaneous tools
GIT                ?= $(X_GIT_TOOL)
INSTALL            ?= $(X_INSTALL_TOOL)

# Patch flags and tools for (cross) build
FLAG_RELRO         := -Wl,-z,relro,-z,now
FLAG_STDLIB        := 
FLAG_GC_SECTIONS   := -Wl,--gc-sections
NOARCH_CFLAGS      := 
NOARCH_CXXFLAGS    := 
NOARCH_EXE_FLAGS   := 
NOARCH_SO_FLAGS    := 
NOARCH_LDFLAGS     := 

ifeq ($(PLATFORM),Solaris)
  FLAG_RELRO          =
  LD                  = gld
else ifeq ($(PLATFORM),Windows)
  FLAG_RELRO          =
  FLAG_STDLIB         =
  NOARCH_CFLAGS      += -DWINVER=0x600 -D_WIN32_WINNT=0x600
  NOARCH_CXXFLAGS    += -DWINVER=0x600 -D_WIN32_WINNT=0x600
  NOARCH_EXE_FLAGS   += -static-libgcc -static-libstdc++
  NOARCH_SO_FLAGS    += -static-libgcc -static-libstdc++
  NOARCH_LDFLAGS     += -T $(CURDIR)/make/ld-windows.script
else ifeq ($(PLATFORM),MacOS)
  FLAG_RELRO          =
  FLAG_GC_SECTIONS    = 
  NOARCH_CXXFLAGS    += -std=c++0x
  NOARCH_LDFLAGS     += -keep_private_externs
else ifeq ($(PLATFORM),BSD)
  NOARCH_EXE_FLAGS   += -L/usr/local/lib
  NOARCH_SO_FLAGS    += -L/usr/local/lib
endif

ifeq ($(DEBUG),1)
  NOARCH_CFLAGS      += -Og -g3 -DLSP_DEBUG -falign-functions=16
  NOARCH_CXXFLAGS    += -Og -g3 -DLSP_DEBUG -falign-functions=16
else
  NOARCH_CFLAGS      += -O2
  NOARCH_CXXFLAGS    += -O2
endif

ifeq ($(ASAN),1)
  NOARCH_CFLAGS      += -fsanitize=address
  NOARCH_CXXFLAGS    += -fsanitize=address
  NOARCH_EXE_FLAGS   += -fsanitize=address
  NOARCH_SO_FLAGS    += -fsanitize=address
endif

ifeq ($(PROFILE),1)
  NOARCH_CFLAGS      += -pg -DLSP_PROFILE
  NOARCH_CXXFLAGS    += -pg -DLSP_PROFILE
endif

ifeq ($(TRACE),1)
  NOARCH_CFLAGS      += -DLSP_TRACE
  NOARCH_CXXFLAGS    += -DLSP_TRACE
endif

ifeq ($(STRICT),1)
  NOARCH_CFLAGS      += -Werror
  NOARCH_CXXFLAGS    += -Werror
endif

ifeq ($(TEST),1)
  NOARCH_CFLAGS      += -DLSP_TESTING
  NOARCH_CXXFLAGS    += -DLSP_TESTING
  EXPORT_SYMBOLS     ?= 1
else
  ifeq ($(ARTIFACT_EXPORT_SYMBOLS),1)
    EXPORT_SYMBOLS     ?= 1
  else
    EXPORT_SYMBOLS     ?= 0
  endif
endif

ifneq ($(EXPORT_SYMBOLS),1)
  NOARCH_CFLAGS      += -fvisibility=hidden
  NOARCH_CXXFLAGS    += -fvisibility=hidden
endif

ifneq ($(ARTIFACT_EXPORT_HEADERS),0)
  INSTALL_HEADERS    ?= 1
else
  INSTALL_HEADERS    ?= 0
endif

# Define flags for (cross) build
NOARCH_CFLAGS      += \
  -fdata-sections \
  -ffunction-sections \
  -fno-asynchronous-unwind-tables \
  -pipe \
  -Wall
CFLAGS             += $(ARCHITECTURE_CFLAGS) $(NOARCH_CFLAGS)
HOST_CFLAGS        += $(HOST_ARCHITECTURE_CFLAGS) $(NOARCH_CFLAGS)

CDEFS              += -DLSP_INSTALL_PREFIX=\\\"$(PREFIX)\\\"
  
NOARCH_CXXFLAGS    += \
  -fno-exceptions \
  -fno-rtti \
  -fdata-sections \
  -ffunction-sections \
  -fno-asynchronous-unwind-tables \
  -pipe \
  -Wall
CXXFLAGS           += $(ARCHITECTURE_CFLAGS) $(NOARCH_CXXFLAGS)
HOST_CXXFLAGS      += $(HOST_ARCHITECTURE_CFLAGS) $(NOARCH_CXXFLAGS)

CXXDEFS            += -DLSP_INSTALL_PREFIX=\\\"$(PREFIX)\\\"

INCLUDE            :=

NOARCH_LDFLAGS     += -r
LDFLAGS            := $(ARCHITECTURE_LDFLAGS) $(NOARCH_LDFLAGS)
HOST_LDFLAGS       := $(HOST_ARCHITECTURE_LDFLAGS) $(NOARCH_LDFLAGS)

NOARCH_EXE_FLAGS   += $(FLAG_RELRO) $(FLAG_GC_SECTIONS)
EXE_FLAGS          := $(ARCHITECTURE_CFLAGS) $(NOARCH_EXE_FLAGS)
HOST_EXE_FLAGS     := $(HOST_ARCHITECTURE_CFLAGS) $(NOARCH_EXE_FLAGS)

NOARCH_SO_FLAGS    += $(FLAG_RELRO) $(FLAG_GC_SECTIONS) -shared $(FLAG_STDLIB) -fPIC 
SO_FLAGS           := $(ARCHITECTURE_CFLAGS) $(NOARCH_SO_FLAGS)
HOST_SO_FLAGS      := $(HOST_ARCHITECTURE_CFLAGS) $(NOARCH_SO_FLAGS)

# The overall list of exported variables
TOOL_VARS := \
  GIT INSTALL \
  PKG_CONFIG AS AR CC CXX LD PHP \
  CFLAGS CDEFS CXXFLAGS CXXDEFS LDFLAGS EXE_FLAGS SO_FLAGS \
  INCLUDE \
  HOST_PKG_CONFIG HOST_AS HOST_AR HOST_CC HOST_CXX HOST_LD HOST_PHP \
  HOST_CFLAGS HOST_CDEFS HOST_CXXFLAGS HOST_CXXDEFS HOST_LDFLAGS HOST_EXE_FLAGS HOST_SO_FLAGS \
  

.PHONY: toolvars
toolvars:
	echo "List of tool variables:"
	echo "  AR                        Archiver tool for target build"
	echo "  AS                        Assembler tool for target build"
	echo "  CC                        C compiler execution command line for target build"
	echo "  CDEFS                     C compiler build macro definitions for target build"
	echo "  CFLAGS                    C compiler build flags for target build"
	echo "  CXX                       C++ compiler execution command line for target build"
	echo "  CXXDEFS                   C++ compiler build defiintions for target build"
	echo "  CXXFLAGS                  C++ compiler build flags for target build"
	echo "  EXE_FLAGS                 Flags to link executable files for target build"
	echo "  GIT                       The name of the Git version control tool"
	echo "  HOST_AR                   Archiver tool for host build"
	echo "  HOST_AS                   Assembler tool for host build"
	echo "  HOST_CC                   C compiler execution command line for host build"
	echo "  HOST_CDEFS                C compiler macro definitions for host build"
	echo "  HOST_CFLAGS               C compiler build flags for host build"
	echo "  HOST_CXX                  C++ compiler execution command line for target build"
	echo "  HOST_CXXDEFS              C++ compiler macro definitions for host build"
	echo "  HOST_CXXFLAGS             C++ compiler build flags for host build"
	echo "  HOST_EXE_FLAGS            Flags to link executable files for target build"
	echo "  HOST_CXX                  C++ compiler execution command line for host build"
	echo "  HOST_LD                   Linker execution command line for host build "
	echo "  HOST_LDFLAGS              Linker flags for merging object files for host build"
	echo "  HOST_PKG_CONFIG           Installed package management tool for host build"
	echo "  HOST_SO_FLAGS             Flags to link shared object/library files for host build"
	echo "  INCLUDE                   Additional paths for include files"
	echo "  LD                        Linker execution command line for target build"
	echo "  LDFLAGS                   Linker flags for merging object files for target build"
	echo "  PHP                       Installed PHP interpreter tool for building documentation"
	echo "  PKG_CONFIG                Installed package management tool for target build"
	echo "  SO_FLAGS                  Flags to link shared object/library files for target build"
	echo ""

