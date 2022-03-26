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

# Determine tools
X_PHP_TOOL         := php
ifeq ($(PLATFORM),OpenBSD)
  X_CC_TOOL          := egcc
  X_CXX_TOOL         := eg++
  X_AS_TOOL          := gas
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
CC                 := $(X_CC_TOOL)
CXX                := $(X_CXX_TOOL)
AS                 := $(X_AS_TOOL)
AR                 := $(X_AR_TOOL)
LD                 := $(X_LD_TOOL)
PHP                := $(X_PHP_TOOL)
PKG_CONFIG         := $(X_PKG_CONFIG)

# Define tool variables for host build
HOST_CC            := $(CC)
HOST_CXX           := $(CXX)
HOST_AS            := $(AS)
HOST_AR            := $(AR)
HOST_LD            := $(LD)
HOST_PHP           := $(PHP)
HOST_PKG_CONFIG    := $(PKG_CONFIG)

# Miscellaneous tools
GIT                := $(X_GIT_TOOL)
INSTALL            := $(X_INSTALL_TOOL)

# Patch flags and tools for (cross) build
FLAG_RELRO          = -Wl,-z,relro,-z,now
FLAG_STDLIB         = -lc
CFLAGS_EXT          = $(ARCHITECTURE_CFLAGS)
CXXFLAGS_EXT        = $(ARCHITECTURE_CFLAGS)
EXE_FLAGS_EXT       = $(ARCHITECTURE_CFLAGS)
SO_FLAGS_EXT        = $(ARCHITECTURE_CFLAGS)
LDFLAGS_EXT         = $(ARCHITECTURE_LDFLAGS)

ifeq ($(PLATFORM),Solaris)
  FLAG_RELRO          =
  LD                  = gld
else ifeq ($(PLATFORM),Windows)
  FLAG_RELRO          =
  FLAG_STDLIB         =
else ifeq ($(PLATFORM),BSD)
  EXE_FLAGS_EXT      += -L/usr/local/lib
  SO_FLAGS_EXT       += -L/usr/local/lib
endif

ifeq ($(DEBUG),1)
  CFLAGS_EXT         += -Og -g3 -DLSP_DEBUG
  CXXFLAGS_EXT       += -Og -g3 -DLSP_DEBUG
else
  CFLAGS_EXT         += -O2
  CXXFLAGS_EXT       += -O2
endif

ifeq ($(PROFILE),1)
  CFLAGS_EXT         += -pg -DLSP_PROFILE
  CXXFLAGS_EXT       += -pg -DLSP_PROFILE
endif

ifeq ($(TRACE),1)
  CFLAGS_EXT         += -DLSP_TRACE
  CXXFLAGS_EXT       += -DLSP_TRACE
endif

ifeq ($(TEST),1)
  CFLAGS_EXT         += -DLSP_TESTING
  CXXFLAGS_EXT       += -DLSP_TESTING
else
  ifneq ($(ARTIFACT_EXPORT_ALL),1)
    CFLAGS_EXT         += -fvisibility=hidden
    CXXFLAGS_EXT       += -fvisibility=hidden
  endif
endif

# Define flags for (cross) build
CDEFS              += -DLSP_INSTALL_PREFIX=\\\"$(PREFIX)\\\"
CXXDEFS            += -DLSP_INSTALL_PREFIX=\\\"$(PREFIX)\\\"

CFLAGS             := \
  $(CFLAGS_EXT) \
  -fdata-sections \
  -ffunction-sections \
  -fno-asynchronous-unwind-tables \
  -pipe \
  -Wall

CXXFLAGS           := \
  $(CXXFLAGS_EXT) \
  -std=c++98 \
  -fno-exceptions \
  -fno-rtti \
  -fdata-sections \
  -ffunction-sections \
  -fno-asynchronous-unwind-tables \
  -pipe \
  -Wall

CFLAGS             += $(CDEFS)
CXXFLAGS           += $(CXXDEFS)

INCLUDE            :=
LDFLAGS            := $(LDFLAGS_EXT) -r
EXE_FLAGS          := $(EXE_FLAGS_EXT) $(FLAG_RELRO) -Wl,--gc-sections
SO_FLAGS           := $(SO_FLAGS_EXT) $(FLAG_RELRO) -Wl,--gc-sections -shared -Llibrary $(FLAG_STDLIB) -fPIC 

# Define flags for host build
HOST_CFLAGS        := $(CFLAGS)
HOST_CDEFS         := $(CDEFS)
HOST_CXXFLAGS      := $(CXXFLAGS)
HOST_CXXDEFS       := $(CXXDEFS)
HOST_LDFLAGS       := $(LDFLAGS)
HOST_EXE_FLAGS     := $(EXE_FLAGS)
HOST_SO_FLAGS      := $(SO_FLAGS)

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

