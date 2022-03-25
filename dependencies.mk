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

#------------------------------------------------------------------------------
# List of all dependencies
DEPENDENCIES = \
  LIBPTHREAD \
  LIBDL \
  LIBGL \
  LSP_COMMON_LIB \
  LSP_DSP_LIB \
  LSP_DSP_UNITS \
  LSP_LLTL_LIB \
  LSP_RUNTIME_LIB \
  LSP_PLUGINS_SHARED \
  LSP_3RD_PARTY \
  LSP_PLUGIN_FW \
  LSP_R3D_IFACE \
  LSP_WS_LIB \
  LSP_TK_LIB \
  LSP_R3D_BASE_LIB \
  LSP_R3D_GLX_LIB

TEST_DEPENDENCIES = \
  LSP_TEST_FW

DEFAULT_FEATURES = doc ladspa lv2 vst2

#------------------------------------------------------------------------------
# Platform-specific dependencies
ifeq ($(PLATFORM),Linux)
  DEPENDENCIES += \
    LIBJACK \
    LIBSNDFILE \
    LIBX11 \
    LIBCAIRO \
    LIBFREETYPE
  
  DEFAULT_FEATURES += jack
endif

ifeq ($(PLATFORM),BSD)
  DEPENDENCIES += \
    LIBJACK \
    LIBSNDFILE \
    LIBX11 \
    LIBCAIRO \
    LIBICONV \
    LIBFREETYPE
  
  DEFAULT_FEATURES += jack
endif

ifeq ($(PLATFORM),Windows)
  DEPENDENCIES += \
    LIBSHLWAPI \
    LIBWINMM \
    LIBMSACM
endif

#------------------------------------------------------------------------------
# All possible dependencies
ALL_DEPENDENCIES = \
  $(DEPENDENCIES) \
  $(TEST_DEPENDENCIES) \
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


