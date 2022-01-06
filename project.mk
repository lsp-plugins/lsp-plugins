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

# Package version
ARTIFACT_ID                 = LSP_PLUGINS
ARTIFACT_NAME               = lsp-plugins
ARTIFACT_DESC               = Linux Studio Plugins Collection
ARTIFACT_HEADERS            = lsp-plug.in
ARTIFACT_EXPORT_ALL         = 1
ARTIFACT_VERSION            = 1.2.0-devel

#------------------------------------------------------------------------------
# Plugin dependencies
DEPENDENCIES_COMMON = \
  LIBPTHREAD \
  LIBDL \
  LSP_COMMON_LIB \
  LSP_DSP_LIB \
  LSP_DSP_UNITS \
  LSP_LLTL_LIB \
  LSP_RUNTIME_LIB \
  LSP_PLUGIN_FW \
  LSP_PLUGINS_SHARED \
  LSP_3RD_PARTY

DEPENDENCIES_COMMON_UI = \
  LSP_R3D_IFACE \
  LSP_WS_LIB \
  LSP_TK_LIB \
  LSP_R3D_BASE_LIB

ifeq ($(PLATFORM),Windows)
endif

#------------------------------------------------------------------------------
# Jack build dependencies
DEPENDENCIES_JACK = \
  $(DEPENDENCIES_COMMON)

DEPENDENCIES_JACK_UI = \
  $(DEPENDENCIES_COMMON_UI)

DEPENDENCIES_JACK_WRAP = \
  LIBPTHREAD \
  LIBDL \
  LSP_COMMON_LIB

ifeq ($(PLATFORM),Linux)
  DEPENDENCIES_JACK += \
    LIBJACK \
    LIBSNDFILE

  DEPENDENCIES_JACK_UI += \
    LIBJACK \
    LIBSNDFILE \
    LIBX11 \
    LIBCAIRO \
    LIBFREETYPE
endif

ifeq ($(PLATFORM),BSD)
  DEPENDENCIES_JACK += \
    LIBJACK \
    LIBSNDFILE
    
  DEPENDENCIES_JACK_UI += \
    LIBJACK \
    LIBSNDFILE \
    LIBX11 \
    LIBCAIRO \
    LIBFREETYPE
endif

ifeq ($(PLATFORM),Windows)
  DEPENDENCIES_JACK += \
    LIBSHLWAPI \
    LIBWINMM \
    LIBMSACM
endif

#------------------------------------------------------------------------------
# LADSPA build dependencies
DEPENDENCIES_LADSPA = \
  $(DEPENDENCIES_COMMON)

ifeq ($(PLATFORM),Linux)
  DEPENDENCIES_LADSPA += \
    LIBLADSPA \
    LIBSNDFILE
endif

ifeq ($(PLATFORM),BSD)
  DEPENDENCIES_LADSPA += \
    LIBLADSPA \
    LIBSNDFILE
endif

ifeq ($(PLATFORM),Windows)
  DEPENDENCIES_LADSPA += \
    LIBSHLWAPI \
    LIBWINMM \
    LIBMSACM
endif

#------------------------------------------------------------------------------
# LV2 build dependencies
DEPENDENCIES_LV2 = \
  $(DEPENDENCIES_COMMON)
  
DEPENDENCIES_LV2_UI = \
  $(DEPENDENCIES_COMMON) \
  $(DEPENDENCIES_COMMON_UI)
  
DEPENDENCIES_LV2TTL_GEN = \
  LIBPTHREAD \
  LIBDL \
  LSP_COMMON_LIB

ifeq ($(PLATFORM),Linux)
  DEPENDENCIES_LV2 += \
    LIBLV2 \
    LIBSNDFILE \
    LIBCAIRO
    
  DEPENDENCIES_LV2_UI += \
    LIBLV2 \
    LIBSNDFILE \
    LIBX11 \
    LIBCAIRO \
    LIBFREETYPE
endif

ifeq ($(PLATFORM),BSD)
  DEPENDENCIES_LV2 += \
    LIBLV2 \
    LIBSNDFILE \
    LIBCAIRO
    
  DEPENDENCIES_LV2_UI += \
    LIBLV2 \
    LIBSNDFILE \
    LIBX11 \
    LIBCAIRO \
    LIBFREETYPE
endif

ifeq ($(PLATFORM),Windows)
  DEPENDENCIES_LV2 += \
    LIBSHLWAPI \
    LIBWINMM \
    LIBMSACM
    
  DEPENDENCIES_LV2_UI += \
    LIBSHLWAPI \
    LIBWINMM \
    LIBMSACM
endif

#------------------------------------------------------------------------------
# VST build dependencies
DEPENDENCIES_VST2 = \
  $(DEPENDENCIES_COMMON) \
  $(DEPENDENCIES_COMMON_UI)

DEPENDENCIES_VST2_WRAP = \
  LIBPTHREAD \
  LIBDL \
  LSP_COMMON_LIB \
  LSP_3RD_PARTY

ifeq ($(PLATFORM),Linux)
  DEPENDENCIES_VST2 += \
    LIBSNDFILE \
    LIBX11 \
    LIBCAIRO \
    LIBFREETYPE
endif

ifeq ($(PLATFORM),BSD)
  DEPENDENCIES_VST2 += \
    LIBSNDFILE \
    LIBX11 \
    LIBCAIRO \
    LIBFREETYPE
endif

ifeq ($(PLATFORM),Windows)
  DEPENDENCIES_VST2 += \
    LIBSHLWAPI \
    LIBWINMM \
    LIBMSACM
endif

#------------------------------------------------------------------------------
# List of dependencies
DEPENDENCIES = \
  $(DEPENDENCIES_PLUGINS) \
  $(DEPENDENCIES_JACK) \
  $(DEPENDENCIES_JACK_UI) \
  $(DEPENDENCIES_JACK_WRAP) \
  $(DEPENDENCIES_LADSPA) \
  $(DEPENDENCIES_LV2) \
  $(DEPENDENCIES_LV2_UI) \
  $(DEPENDENCIES_LV2TTL_GEN) \
  $(DEPENDENCIES_VST2)

TEST_DEPENDENCIES = \
  LSP_TEST_FW
  
TEST_DEPENDENCIES_UI = \
  LSP_TEST_FW
  
DEPENDENCIES_BIN =

#------------------------------------------------------------------------------
# Platform-specific dependencies
ifeq ($(PLATFORM),Linux)
  DEPENDENCIES_BIN += \
    LSP_R3D_GLX_LIB

  TEST_DEPENDENCIES_UI += \
    LSP_R3D_GLX_LIB \
    LSP_R3D_COMMON_LIB \
    LIBGL
endif

ifeq ($(PLATFORM),BSD)
  DEPENDENCIES_BIN += \
    LSP_R3D_GLX_LIB

  TEST_DEPENDENCIES_UI += \
    LSP_R3D_GLX_LIB \
    LSP_R3D_COMMON_LIB \
    LIBGL
    
  DEPENDENCIES_COMMON += \
    LIBICONV
endif

ifeq ($(PLATFORM),Windows)
  DEPENDENCIES_COMMON += \
    LIBSHLWAPI \
    LIBWINMM \
    LIBMSACM
endif

#------------------------------------------------------------------------------
# All possible dependencies
ALL_DEPENDENCIES = \
  $(DEPENDENCIES_COMMON) \
  $(DEPENDENCIES_COMMON_UI) \
  $(TEST_DEPENDENCIES) \
  $(TEST_DEPENDENCIES_UI) \
  LIBJACK \
  LIBGL \
  LIBSNDFILE \
  LIBX11 \
  LIBCAIRO \
  LIBDL \
  LIBICONV \
  LIBFREETYPE \
  LIBSHLWAPI \
  LIBWINMM \
  LIBMSACM


