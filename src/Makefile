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

# Command-line flag to silence nested $(MAKE).
ifneq ($(VERBOSE),1)
.SILENT:
endif

MODDIR                  = $(CURDIR)/..
ROOTDIR                := $(MODDIR)
CONFIG                 := $(MODDIR)/.config.mk

# Project configuration
include $(CONFIG)
include $(MODDIR)/project.mk
include $(MODDIR)/make/functions.mk

# Injected plugin dependencies
include $(ROOTDIR)/dependencies.mk
include $(ROOTDIR)/plugins.mk

ifeq ($(TEST),1)
  UNIQ_DEPENDENCIES      := $(call uniq, $(DEPENDENCIES) $(ARTIFACT_ID) $(TEST_DEPENDENCIES))
else
  UNIQ_DEPENDENCIES      := $(call uniq, $(DEPENDENCIES) $(ARTIFACT_ID))
endif
DEPENDENCIES            = $(UNIQ_DEPENDENCIES)

ARTIFACT_BIN            = $($(ARTIFACT_ID)_BIN)
ARTIFACT_NAME           = $($(ARTIFACT_ID)_NAME)
ARTIFACT_OBJ_META       = $(ARTIFACT_BIN)/$($(ARTIFACT_ID)_NAME)-meta.o
ARTIFACT_OBJ_DSP        = $(ARTIFACT_BIN)/$($(ARTIFACT_ID)_NAME)-dsp.o
ARTIFACT_OBJ_UI         = $(ARTIFACT_BIN)/$($(ARTIFACT_ID)_NAME)-ui.o
ARTIFACT_OBJ_TEST       = $(ARTIFACT_BIN)/$($(ARTIFACT_ID)_NAME)-test.o
ARTIFACT_OBJ            = $(ARTIFACT_OBJ_META) $(ARTIFACT_OBJ_DSP) $(ARTIFACT_OBJ_UI)
ARTIFACT_CFLAGS         = $(foreach dep, $(DEPENDENCIES), $(if $($(dep)_CFLAGS), $($(dep)_CFLAGS)))

CXX_SRC_STUB            = $(ARTIFACT_BIN)/stub.cpp
CXX_SRC_MAIN_META       = $(call rwildcard, main/meta, *.cpp)
CXX_SRC_MAIN_DSP        = $(call rwildcard, main/plug, *.cpp)
CXX_SRC_MAIN_UI         = $(call rwildcard, main/ui, *.cpp)
CXX_SRC_TEST            = $(call rwildcard, test, *.cpp)
CXX_SRC                 = $(CXX_SRC_MAIN_META) $(CXX_SRC_MAIN_DSP) $(CXX_SRC_MAIN_UI)

OBJ_STUB                = $(patsubst %.cpp, %.o, $(CXX_SRC_STUB))
OBJ_MAIN_META           = $(patsubst %.cpp, $(ARTIFACT_BIN)/%.o, $(CXX_SRC_MAIN_META))
OBJ_MAIN_DSP            = $(patsubst %.cpp, $(ARTIFACT_BIN)/%.o, $(CXX_SRC_MAIN_DSP))
OBJ_MAIN_UI             = $(patsubst %.cpp, $(ARTIFACT_BIN)/%.o, $(CXX_SRC_MAIN_UI))
OBJ_TEST                = $(patsubst %.cpp, $(ARTIFACT_BIN)/%.o, $(CXX_SRC_TEST))
OBJ                     = $(OBJ_MAIN_META) $(OBJ_MAIN_DSP) $(OBJ_MAIN_UI)

XOBJ_MAIN_META          = $(if $(OBJ_MAIN_META),$(OBJ_MAIN_META),$(OBJ_STUB))
XOBJ_MAIN_DSP           = $(if $(OBJ_MAIN_DSP),$(OBJ_MAIN_DSP),$(OBJ_STUB))
XOBJ_MAIN_UI            = $(if $(OBJ_MAIN_UI),$(OBJ_MAIN_UI),$(OBJ_STUB))
XOBJ_TEST               = $(if $(OBJ_TEST),$(OBJ_TEST),$(OBJ_STUB))

CXX_FILE                = $(patsubst $(ARTIFACT_BIN)/%.o,%.cpp, $(@))

CFLAGS_DEPS             = $(foreach dep, $(call uniq, $(DEPENDENCIES)), $(if $($(dep)_CFLAGS), $($(dep)_CFLAGS)))
BUILD_ALL               = $(ARTIFACT_LIB) $(ARTIFACT_SLIB) $(ARTIFACT_PC)

ifeq ($(TEST), 1)
  CXX_SRC                += $(CXX_SRC_TEST)
  ARTIFACT_OBJ           += $(ARTIFACT_OBJ_TEST)
  DEPENDENCIES           += $(TEST_DEPENDENCIES)
endif

CXX_DEPS                = $(foreach src,$(CXX_SRC),$(patsubst %.cpp,$(ARTIFACT_BIN)/%.d,$(src)))
CXX_DEPFILE             = $(patsubst $(ARTIFACT_BIN)/%.d,%.cpp,$(@))
CXX_DEPTARGET           = $(patsubst $(ARTIFACT_BIN)/%.d,%.o,$(@))

.DEFAULT_GOAL = all
.PHONY: compile depend dep_clean all install uninstall gen_stub

# Dependencies targets
dep_clean:

$(CXX_DEPS): dep_clean
	echo "  dep  [$(ARTIFACT_NAME)] $(CXX_DEPFILE)"
	mkdir -p $(dir $(@))
	$(CXX) -MM -MT "\$$($(ARTIFACT_ID)_BIN)/$(CXX_DEPTARGET)" -MF $(@) $(CXX_DEPFILE) $(CXXFLAGS) $(INCLUDE) $(CFLAGS_DEPS)

depend: $(CXX_DEPS)
	$(foreach dep,$(DEPENDENCIES) $(ARTIFACT_ID),\
	  $(if $($(dep)_INC), \
	    $(eval SED_RULES += s/$(shell echo "$($(dep)_INC)" | sed "s/\\//\\\\\//g")/\$$$$\\($(dep)_INC\\)/g;)\
	  )\
	  $(if $($(dep)_BIN), \
	    $(eval SED_RULES += s/$(shell echo "$($(dep)_BIN)" | sed "s/\\//\\\\\//g")/\\$$$$\\($(dep)_BIN\\)/g;)\
	  )\
	)
	$(if $(CXX_DEPS),cat $(CXX_DEPS),echo "") | sed -E "$(SED_RULES)" >Makefile.d

# Compilation targets
compile: $(ARTIFACT_OBJ)

$(CXX_SRC_STUB):
	echo "  gen  [$(ARTIFACT_NAME)] $(notdir $(CXX_FILE))"
	echo "#include <stdio.h>" > $(CXX_SRC_STUB)

$(OBJ_STUB): $(CXX_SRC_STUB)
	echo "  $(CXX)  [$(ARTIFACT_NAME)] $(CXX_FILE)"
	mkdir -p $(dir $@)
	$(CXX) -o $(@) -c $(CXX_SRC_STUB) -fPIC $(CXXFLAGS) $(ARTIFACT_MFLAGS) $(EXT_FLAGS) $(INCLUDE) $(CFLAGS_DEPS)

$(OBJ):
	echo "  $(CXX)  [$(ARTIFACT_NAME)] $(CXX_FILE)"
	mkdir -p $(dir $@)
	$(CXX) -o $(@) -c $(CXX_FILE) -fPIC $(CXXFLAGS) $(ARTIFACT_MFLAGS) $(EXT_FLAGS) $(INCLUDE) $(CFLAGS_DEPS)

# Linking targets
$(ARTIFACT_OBJ_META): $(XOBJ_MAIN_META)
	echo "  $(LD)   [$(ARTIFACT_NAME)] $(notdir $(ARTIFACT_OBJ_META))"
	$(LD) -o $(ARTIFACT_OBJ_META) -r $(XOBJ_MAIN_META)
	
$(ARTIFACT_OBJ_DSP): $(XOBJ_MAIN_DSP)
	echo "  $(LD)   [$(ARTIFACT_NAME)] $(notdir $(ARTIFACT_OBJ_DSP))"
	$(LD) -o $(ARTIFACT_OBJ_DSP) -r $(XOBJ_MAIN_DSP)
	
$(ARTIFACT_OBJ_UI): $(XOBJ_MAIN_UI)
	echo "  $(LD)   [$(ARTIFACT_NAME)] $(notdir $(ARTIFACT_OBJ_UI))"
	$(LD) -o $(ARTIFACT_OBJ_UI) -r $(XOBJ_MAIN_UI)
	
$(ARTIFACT_OBJ_TEST): $(XOBJ_TEST)
	echo "  $(LD)   [$(ARTIFACT_NAME)] $(notdir $(ARTIFACT_OBJ_TEST))"
	$(LD) -o $(ARTIFACT_OBJ_TEST) -r $(XOBJ_TEST)

# Deletaged targets
all install uninstall:
	$(MAKE) -C "$(LSP_PLUGIN_FW_PATH)" $(@) VERBOSE="$(VERBOSE)" CONFIG="$(CONFIG)"

# Dependencies
-include $(CURDIR)/Makefile.d

