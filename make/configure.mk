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
ifneq ($(VERBOSE),1)
.SILENT:
endif

BASEDIR                    := $(CURDIR)
ROOTDIR                    := $(CURDIR)
PLUGINS                    := $(BASEDIR)/plugins.mk
TEST                       := 0
UI                         := 1
DEBUG                      := 0
PROFILE                    := 0
TRACE                      := 0

# Configure system settings
include $(BASEDIR)/project.mk
include $(BASEDIR)/make/functions.mk
include $(BASEDIR)/make/system.mk
include $(BASEDIR)/make/paths.mk
include $(BASEDIR)/make/tools.mk
include $(BASEDIR)/modules.mk
include $(BASEDIR)/dependencies.mk
include $(PLUGINS)

ifeq ($(DEVEL),1)
  X_URL_SUFFIX                = _RW
else
  X_URL_SUFFIX                = _RO
endif

# Compute the full list of dependencies
MERGED_DEPENDENCIES        := \
  $(DEPENDENCIES) \
  $(TEST_DEPENDENCIES) \
  $(PLUGIN_DEPENDENCIES) \
  $(PLUGIN_SHARED)

UNIQ_MERGED_DEPENDENCIES   := $(call uniq, $(MERGED_DEPENDENCIES))
DEPENDENCIES                = $(UNIQ_MERGED_DEPENDENCIES)
FEATURES                   := $(sort $(call subtraction,$(SUB_FEATURES),$(DEFAULT_FEATURES) $(ADD_FEATURES)))

ifeq ($(UI),1)
  MERGED_DEPENDENCIES += $(UI_DEPENDENCIES)
else ifeq ($(TEST),1)
  $(error "Can TEST=1 can not be set when UI=0")
else ifeq ($(call fcheck,clap jack vst2 vst3,$(FEATURES),1),1)
  $(error "Can not set UI=0 when FEATURES contains clap, jack, vst2 or vst3")
endif

# Determine versions
ifeq ($(findstring -devel,$(ARTIFACT_VERSION)),-devel)
  $(foreach dep, $(DEPENDENCIES), \
    $(eval $(dep)_BRANCH=devel) \
  )
  # Strip '-devel' from version
  tmp_version :=$(shell echo "$(ARTIFACT_VERSION)" | sed s/-devel//g)
  ARTIFACT_VERSION=$(tmp_version)
else
  $(foreach dep,$(DEPENDENCIES),\
    $(eval $(dep)_BRANCH="$($(dep)_VERSION)") \
  )
endif

define pkgconfig =
  $(eval name=$(1))
  
  $(if $($(name)_NAME), \
    $(if $($(name)_CFLAGS),,       $(eval $(name)_CFLAGS       := $(shell $(PKG_CONFIG)      --cflags "$($(name)_NAME)"))) \
    $(if $($(name)_LDLAGS),,       $(eval $(name)_LDFLAGS      := $(shell $(PKG_CONFIG)      --libs "$($(name)_NAME)"))) \
    \
    $(if $(HOST_$(name)_CFLAGS),,  $(eval HOST_$(name)_CFLAGS  := $(shell $(HOST_PKG_CONFIG) --cflags "$($(name)_NAME)"))) \
    $(if $(HOST_$(name)_LDLAGS),,  $(eval HOST_$(name)_LDFLAGS := $(shell $(HOST_PKG_CONFIG) --libs "$($(name)_NAME)"))) \
  )
  
  $(if $($(name)_OBJ),,          $(eval $(name)_OBJ      :=))
  $(if $(HOST_$(name)_OBJ),,     $(eval HOST_$(name)_OBJ :=))
endef

define libconfig =
  $(eval name=$(1))
  
  $(if $($(name)_NAME), \
    $(if $($(name)_LDLAGS),,       $(eval $(name)_LDFLAGS      := -l$($(name)_NAME))) \
    \
    $(if $(HOST_$(name)_LDLAGS),,  $(eval HOST_$(name)_LDFLAGS := -l$($(name)_NAME))) \
  )
  
  $(if $($(name)_OBJ),,          $(eval $(name)_OBJ      :=))
  $(if $(HOST_$(name)_OBJ),,     $(eval HOST_$(name)_OBJ :=))
endef

define optconfig =
  $(eval name=$(1))
  
  $(if $($(name)_NAME), \
    $(if $(HOST_$(name)_LDLAGS),,  $(eval HOST_$(name)_LDFLAGS := $($(name)_LDFLAGS))) \
  )
  
  $(if $($(name)_OBJ),,          $(eval $(name)_OBJ      :=))
  $(if $(HOST_$(name)_OBJ),,     $(eval HOST_$(name)_OBJ :=))
endef

define _modconfig =
  $(eval name=$(1))
  $(eval publisher=$(2))
  
  $(if $($(name)_DESC),,         $(eval $(name)_DESC         := $($(name)_DESC)))
  $(if $($(name)_URL),,          $(eval $(name)_URL          := $($(name)_URL$(X_URL_SUFFIX))))
  
  $(if $($(name)_PATH),,         $(eval $(name)_PATH         := $(MODULES)/$($(name)_NAME)))
  $(if $($(name)_INC),,          $(eval $(name)_INC          := $($(name)_PATH)/include))
  $(if $($(name)_SRC),,          $(eval $(name)_SRC          := $($(name)_PATH)/src))
  $(if $($(name)_TEST),,         $(eval $(name)_TEST         := $($(name)_PATH)/test))
  $(if $($(name)_TESTING),,      $(eval $(name)_TESTING      := 0))
  $(if $($(name)_BIN),,          $(eval $(name)_BIN          := $(TARGET_BUILDDIR)/$($(name)_NAME)))
  $(if $($(name)_CFLAGS),,       $(eval $(name)_CFLAGS       := "$(if $($(name)_INC_OPT),$($(name)_INC_OPT) ,-I )\"$($(name)_INC)\"" -D$(name)_BUILTIN$(if $(publisher), -D$(name)_PUBLISHER)))
  $(if $($(name)_LDLAGS),,       $(eval $(name)_LDFLAGS      :=))
  $(if $($(name)_OBJ),,          $(eval $(name)_OBJ          := "$($(name)_BIN)/$($(name)_NAME).o"))
  $(if $($(name)_OBJ_TEST),,     $(eval $(name)_OBJ_TEST     := "$($(name)_BIN)/$($(name)_NAME)-test.o"))
  $(if $($(name)_MFLAGS),,       $(eval $(name)_MFLAGS       := $(if $(publisher),,"-D$(name)_BUILTIN -fvisibility=hidden")))
  
  $(if $(HOST_$(name)_PATH),,    $(eval HOST_$(name)_PATH    := $(MODULES)/$($(name)_NAME)))
  $(if $(HOST_$(name)_INC),,     $(eval HOST_$(name)_INC     := $(HOST_$(name)_PATH)/include))
  $(if $(HOST_$(name)_SRC),,     $(eval HOST_$(name)_SRC     := $(HOST_$(name)_PATH)/src))
  $(if $(HOST_$(name)_TEST),,    $(eval HOST_$(name)_TEST    := $(HOST_$(name)_PATH)/test))
  $(if $(HOST_$(name)_TESTING),, $(eval HOST_$(name)_TESTING := 0))
  $(if $(HOST_$(name)_BIN),,     $(eval HOST_$(name)_BIN     := $(HOST_BUILDDIR)/$($(name)_NAME)))
  $(if $(HOST_$(name)_CFLAGS),,  $(eval HOST_$(name)_CFLAGS  := "$(if $($(name)_INC_OPT),$($(name)_INC_OPT) ,-I )\"$($(name)_INC)\"" -D$(name)_BUILTIN$(if $(publisher), -D$(name)_PUBLISHER)))
  $(if $(HOST_$(name)_LDLAGS),,  $(eval HOST_$(name)_LDFLAGS :=))
  $(if $(HOST_$(name)_OBJ),,     $(eval HOST_$(name)_OBJ     := "$(HOST_$(name)_BIN)/$($(name)_NAME).o"))
  $(if $(HOST_$(name)_OBJ_TEST),,$(eval HOST_$(name)_OBJ_TEST:= "$(HOST_$(name)_BIN)/$($(name)_NAME)-test.o"))
  $(if $(HOST_$(name)_MFLAGS),,  $(eval HOST_$(name)_MFLAGS  := $(if $(publisher),,"-D$(name)_BUILTIN -fvisibility=hidden")))
endef

define srcconfig =
  $(eval name=$(1))
  $(eval publisher=$(findstring $(ARTIFACT_NAME),$($(name)_NAME)))
  $(eval $(call _modconfig,$(name),$(publisher)))
endef 

define binconfig =
  $(eval name=$(1))
  $(eval publisher=1)
  $(eval $(call _modconfig,$(name),$(publisher)))
endef

define hdrconfig =
  $(eval name=$(1))
  $(eval publisher=$(findstring $(ARTIFACT_NAME),$($(name)_NAME)))
  
  $(if $($(name)_DESC),,         $(eval $(name)_DESC         := $($(name)_DESC)))
  $(if $($(name)_URL),,          $(eval $(name)_URL          := $($(name)_URL$(X_URL_SUFFIX))))
  
  $(if $($(name)_PATH),,         $(eval $(name)_PATH         := $(MODULES)/$($(name)_NAME)))
  $(if $($(name)_INC),,          $(eval $(name)_INC          := $($(name)_PATH)/include))
  $(if $($(name)_TESTING),,      $(eval $(name)_TESTING      := 0))
  $(if $($(name)_CFLAGS),,       $(eval $(name)_CFLAGS       := "$(if $($(name)_INC_OPT),$($(name)_INC_OPT) ,-I )\"$($(name)_INC)\""$(if $(publisher), "-D$(name)_PUBLISHER")))
  $(if $($(name)_MFLAGS),,       $(eval $(name)_MFLAGS       := "-D$(name)_BUILTIN -fvisibility=hidden"))
  
  $(if $(HOST_$(name)_PATH),,    $(eval HOST_$(name)_PATH    := $(MODULES)/$($(name)_NAME)))
  $(if $(HOST_$(name)_INC),,     $(eval HOST_$(name)_INC     := $(HOST_$(name)_PATH)/include))
  $(if $(HOST_$(name)_TESTING),, $(eval HOST_$(name)_TESTING := 0))
  $(if $(HOST_$(name)_CFLAGS),,  $(eval HOST_$(name)_CFLAGS  := "$(if $($(name)_INC_OPT),$($(name)_INC_OPT) ,-I )\"$(HOST_$(name)_INC)\""$(if $(publisher), "-D$(name)_PUBLISHER")))
  $(if $(HOST_$(name)_MFLAGS),,  $(eval HOST_$(name)_MFLAGS  := "-D$(name)_BUILTIN -fvisibility=hidden"))
endef

define plugconfig =
  $(eval name=$(1))
  $(eval publisher=$(findstring $(ARTIFACT_NAME),$($(name)_NAME)))
  
  $(if $($(name)_DESC),,            $(eval $(name)_DESC             := $($(name)_DESC)))
  $(if $($(name)_URL),,             $(eval $(name)_URL              := $($(name)_URL$(X_URL_SUFFIX))))
  $(if $($(name)_TESTING),,         $(eval $(name)_TESTING          := 0))

  $(if $($(name)_PATH),,            $(eval $(name)_PATH             := $(MODULES)/$($(name)_NAME)))    
  $(if $($(name)_INC),,             $(eval $(name)_INC              := $($(name)_PATH)/include))
  $(if $($(name)_SRC),,             $(eval $(name)_SRC              := $($(name)_PATH)/src))
  $(if $($(name)_TEST),,            $(eval $(name)_TEST             := $($(name)_PATH)/test))
  $(if $($(name)_BIN),,             $(eval $(name)_BIN              := $(TARGET_BUILDDIR)/$($(name)_NAME)))
  $(if $($(name)_CFLAGS),,          $(eval $(name)_CFLAGS           := "-I\"$($(name)_INC)\"" -D$(name)_BUILTIN$(if $(publisher), -D$(name)_PUBLISHER)))
  $(if $($(name)_LDLAGS),,          $(eval $(name)_LDFLAGS          :=))
  $(if $($(name)_OBJ_META),,        $(eval $(name)_OBJ_META         := "$($(name)_BIN)/$($(name)_NAME)-meta.o"))
  $(if $($(name)_OBJ_DSP),,         $(eval $(name)_OBJ_DSP          := "$($(name)_BIN)/$($(name)_NAME)-dsp.o"))
  $(if $($(name)_OBJ_UI),,          $(eval $(name)_OBJ_UI           := "$($(name)_BIN)/$($(name)_NAME)-ui.o"))
  $(if $($(name)_OBJ_TEST),,        $(eval $(name)_OBJ_TEST         := "$($(name)_BIN)/$($(name)_NAME)-test.o"))
  $(if $($(name)_MFLAGS),,          $(eval $(name)_MFLAGS           := $(if $(publisher),,"-D$(name)_BUILTIN -fvisibility=hidden")))
  
  $(if $(HOST_$(name)_PATH),,       $(eval HOST_$(name)_PATH        := $(MODULES)/$($(name)_NAME)))
  $(if $(HOST_$(name)_INC),,        $(eval HOST_$(name)_INC         := $(HOST_$(name)_PATH)/include))
  $(if $(HOST_$(name)_SRC),,        $(eval HOST_$(name)_SRC         := $(HOST_$(name)_PATH)/src))
  $(if $(HOST_$(name)_TEST),,       $(eval HOST_$(name)_TEST        := $(HOST_$(name)_PATH)/test))
  $(if $(HOST_$(name)_BIN),,        $(eval HOST_$(name)_BIN         := $(HOST_BUILDDIR)/$($(name)_NAME)))
  $(if $(HOST_$(name)_CFLAGS),,     $(eval HOST_$(name)_CFLAGS      := "-I\"$(HOST_$(name)_INC)\"" -D$(name)_BUILTIN$(if $(publisher), -D$(name)_PUBLISHER)))
  $(if $(HOST_$(name)_LDLAGS),,     $(eval HOST_$(name)_LDFLAGS     :=))
  $(if $(HOST_$(name)_OBJ_META),,   $(eval HOST_$(name)_OBJ_META    := "$(HOST_$(name)_BIN)/$($(name)_NAME)-meta.o"))
  $(if $(HOST_$(name)_OBJ_DSP),,    $(eval HOST_$(name)_OBJ_DSP     := "$(HOST_$(name)_BIN)/$($(name)_NAME)-dsp.o"))
  $(if $(HOST_$(name)_OBJ_UI),,     $(eval HOST_$(name)_OBJ_UI      := "$(HOST_$(name)_BIN)/$($(name)_NAME)-ui.o"))
  $(if $(HOST_$(name)_OBJ_SHARED),, $(eval HOST_$(name)_OBJ_SHARED  := "$(HOST_$(name)_BIN)/$($(name)_NAME)-shared.o"))
  $(if $(HOST_$(name)_OBJ_TEST),,   $(eval HOST_$(name)_OBJ_TEST    := "$(HOST_$(name)_BIN)/$($(name)_NAME)-test.o"))
  $(if $(HOST_$(name)_MFLAGS),,     $(eval HOST_$(name)_MFLAGS      := $(if $(publisher),,"-D$(name)_BUILTIN -fvisibility=hidden")))
endef

define vardef =
  $(eval name = $(1))
  # Override variables if they are not defined
  $(if $(findstring pkg, $($(name)_TYPE)), $(eval $(call pkgconfig,  $(name))))
  $(if $(findstring src, $($(name)_TYPE)), $(eval $(call srcconfig,  $(name))))
  $(if $(findstring hdr, $($(name)_TYPE)), $(eval $(call hdrconfig,  $(name))))
  $(if $(findstring lib, $($(name)_TYPE)), $(eval $(call libconfig,  $(name))))
  $(if $(findstring bin, $($(name)_TYPE)), $(eval $(call binconfig,  $(name))))
  $(if $(findstring opt, $($(name)_TYPE)), $(eval $(call optconfig,  $(name))))
  $(if $(findstring plug,$($(name)_TYPE)), $(eval $(call plugconfig, $(name))))
endef

# Define predefined variables
ifndef ARTIFACT_TYPE
  ARTIFACT_TYPE              := src
endif

ifndef $(ARTIFACT_ID)_NAME
  $(ARTIFACT_ID)_NAME        := $(ARTIFACT_NAME)
endif
ifndef $(ARTIFACT_ID)_TYPE
  $(ARTIFACT_ID)_TYPE        := $(ARTIFACT_TYPE)
endif
ifndef $(ARTIFACT_ID)_DESC
  $(ARTIFACT_ID)_DESC        := $(ARTIFACT_DESC)
endif
ifndef $(ARTIFACT_ID)_VERSION 
  $(ARTIFACT_ID)_VERSION     := $(ARTIFACT_VERSION)
endif
ifndef $(ARTIFACT_ID)_PATH
  $(ARTIFACT_ID)_PATH        := $(BASEDIR)
endif
ifndef HOST_$(ARTIFACT_ID)_PATH
  HOST_$(ARTIFACT_ID)_PATH   := $(BASEDIR)
endif

ROOT_ARTIFACT_ID           := $(ARTIFACT_ID)
$(ARTIFACT_ID)_TESTING      = $(TEST)

OVERALL_DEPS := $(call uniq,$(DEPENDENCIES) $(ARTIFACT_ID))
__tmp := $(foreach dep,$(OVERALL_DEPS),$(call vardef, $(dep)))

CONFIG_VARS = \
  $(PATH_VARS) \
  $(COMMON_VARS) \
  $(TOOL_VARS) \
  $(foreach name, $(OVERALL_DEPS), \
    $(name)_NAME \
    $(name)_DESC \
    $(name)_VERSION \
    $(name)_TYPE \
    $(name)_URL \
    $(name)_BRANCH \
    \
    $(name)_PATH \
    $(name)_INC \
    $(name)_SRC \
    $(name)_TEST \
    $(name)_TESTING \
    $(name)_BIN \
    $(name)_CFLAGS \
    $(name)_MFLAGS \
    $(name)_LDFLAGS \
    $(name)_OBJ \
    $(name)_OBJ_META \
    $(name)_OBJ_DSP \
    $(name)_OBJ_UI \
    $(name)_OBJ_TEST \
    \
    HOST_$(name)_PATH \
    HOST_$(name)_INC \
    HOST_$(name)_SRC \
    HOST_$(name)_TEST \
    HOST_$(name)_TESTING \
    HOST_$(name)_BIN \
    HOST_$(name)_CFLAGS \
    HOST_$(name)_MFLAGS \
    HOST_$(name)_LDFLAGS \
    HOST_$(name)_OBJ \
    HOST_$(name)_OBJ_META \
    HOST_$(name)_OBJ_DSP \
    HOST_$(name)_OBJ_UI \
    HOST_$(name)_OBJ_TEST \
  )

.DEFAULT_GOAL      := config
.PHONY: config prepare help
.PHONY: $(CONFIG_VARS)

prepare:
	echo "Configuring build..."
	echo "# Project settings" > "$(CONFIG)"

$(CONFIG_VARS): prepare
	echo "$(@)=$($(@))" >> "$(CONFIG)"

config: $(CONFIG_VARS)
	echo "Architecture: $(ARCHITECTURE_FAMILY)/$(ARCHITECTURE) ($(ARCHITECTURE_CFLAGS))"
	echo "Features:     $(FEATURES)"
	echo "Configured OK"

help: | pathvars toolvars sysvars
	echo ""
	echo "List of variables for each dependency:"
	echo "  <ARTIFACT>_BIN            location to put all binaries when building artifact"
	echo "  <ARTIFACT>_BRANCH         git branch used to checkout source code"
	echo "  <ARTIFACT>_CFLAGS         C/C++ flags to access headers of the artifact"
	echo "  <ARTIFACT>_DESC           Full description of the artifact"
	echo "  <ARTIFACT>_INC            path to include files of the artifact"
	echo "  <ARTIFACT>_LDFLAGS        linker flags to link with artifact"
	echo "  <ARTIFACT>_MFLAGS         artifact-specific compilation flags"
	echo "  <ARTIFACT>_NAME           the artifact name used in pathnames"
	echo "  <ARTIFACT>_OBJ            path to output object file for artifact"
	echo "  <ARTIFACT>_PATH           location of the source code of the artifact"
	echo "  <ARTIFACT>_SRC            path to source code files of the artifact"
	echo "  <ARTIFACT>_TEST           location of test files of the artifact"
	echo "  <ARTIFACT>_TYPE           artifact usage type"
	echo "                            - bin  - binaries build from source code"
	echo "                            - hdr  - use headers only from git"
	echo "                            - lib  - use system headers and -l<libname> flags"
	echo "                            - opt  - use optional configuration"
	echo "                            - pkg  - use pkgconfig for configuration"
	echo "                            - plug - use source as plugin for LSP plugin framework"
	echo "                            - src  - use sources and headers from git"
	echo "  <ARTIFACT>_URL            location of the artifact git repoisitory"
	echo "  <ARTIFACT>_VERSION        version of the artifact used for building"
	echo ""
	echo "Artifacts used for build:"
	echo "  $(DEPENDENCIES)"
	echo ""
	echo "Plugins used for build:"
	echo "  $(PLUGINS_DEPENDENCIES)"

