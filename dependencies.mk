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
  LSP_R3D_BASE_LIB

TEST_DEPENDENCIES = \
  LSP_TEST_FW

DEFAULT_FEATURES = clap doc ladspa lv2 ui vst2 vst3

#------------------------------------------------------------------------------
# Linux dependencies
LINUX_DEPENDENCIES = \
  LIBPTHREAD \
  LIBDL \
  LIBRT \
  LIBSNDFILE \
  LIBCAIRO \
  LIBFREETYPE \
  LIBX11 \
  LIBXRANDR \
  LIBGL \
  LIBGSTREAMER_AUDIO \
  LIBJACK \
  LSP_R3D_GLX_LIB

LINUX_TEST_DEPENDENCIES =

ifeq ($(PLATFORM),Linux)
  DEPENDENCIES             += $(LINUX_DEPENDENCIES)
  TEST_DEPENDENCIES        += $(LINUX_TEST_DEPENDENCIES)
  DEFAULT_FEATURES         += jack gst xdg
endif

#------------------------------------------------------------------------------
# BSD dependencies
BSD_DEPENDENCIES = \
  LIBPTHREAD \
  LIBDL \
  LIBRT \
  LIBSNDFILE \
  LIBICONV \
  LIBCAIRO \
  LIBFREETYPE \
  LIBX11 \
  LIBXRANDR \
  LIBGL \
  LIBGSTREAMER_AUDIO \
  LIBJACK \
  LSP_R3D_GLX_LIB

BSD_TEST_DEPENDENCIES = 

ifeq ($(PLATFORM),BSD)
  DEPENDENCIES             += $(BSD_DEPENDENCIES)
  TEST_DEPENDENCIES        += $(BSD_TEST_DEPENDENCIES)
  DEFAULT_FEATURES         += jack gst xdg
endif


ifeq ($(PLATFORM),Windows)
  DEPENDENCIES += \
    LIBSHLWAPI \
    LIBWINMM \
    LIBMSACM \
    LIBD2D1 \
    LIBOLE \
    LIBWINCODEC
endif

#------------------------------------------------------------------------------
# Windows dependencies
WINDOWS_DEPENDENCIES = \
  LIBADVAPI32 \
  LIBSHLWAPI \
  LIBWINMM \
  LIBMSACM \
  LIBMPR \
  LIBGDI32 \
  LIBD2D1 \
  LIBOLE \
  LIBWINCODEC \
  LIBDWRITE \
  LIBUUID \
  LIBOPENGL32 \
  LSP_R3D_WGL_LIB

WINDOWS_TEST_DEPENDENCIES = 

ifeq ($(PLATFORM),Windows)
  DEPENDENCIES             += $(WINDOWS_DEPENDENCIES)
  TEST_DEPENDENCIES        += $(WINDOWS_TEST_DEPENDENCIES)
endif

#------------------------------------------------------------------------------
# All possible dependencies
ALL_DEPENDENCIES = \
  $(DEPENDENCIES) \
  $(LINUX_DEPENDENCIES) \
  $(BSD_DEPENDENCIES) \
  $(WINDOWS_DEPENDENCIES) \
  $(TEST_DEPENDENCIES) \
  $(LINUX_TEST_DEPENDENCIES) \
  $(BSD_TEST_DEPENDENCIES) \
  $(WINDOWS_TEST_DEPENDENCIES)

