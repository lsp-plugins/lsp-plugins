#!/usr/bin/make -f
#
# Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
#           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
#
# This file is part of lsp-lltl-lib
#
# lsp-lltl-lib is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# lsp-lltl-lib is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with lsp-lltl-lib.  If not, see <https://www.gnu.org/licenses/>.
#

# Command-line flag to silence nested $(MAKE).
ifneq ($(VERBOSE),1)
.SILENT:
endif

# Location
BASEDIR                    := $(CURDIR)
MODULES                    := $(BASEDIR)/modules
BUILDDIR                   := $(BASEDIR)/.build
CONFIG                     := $(BASEDIR)/.config.mk
PLUGINS                    := $(BASEDIR)/plugins.mk
PROJECT                    := $(BASEDIR)/project.mk

# Basic initialization
# Checks
ifeq ("$(wildcard $(CONFIG))", "")
  CONFIGURED          = 0
else
  CONFIGURED          = 1
endif

include $(BASEDIR)/project.mk

# Setup paths
CHK_CONFIG                  = test -f "$(CONFIG)" || (echo "System not properly configured. Please launch 'make config' first" && exit 1)
DISTSRC_PATH                = $(BUILDDIR)/distsrc
DISTSRC                     = $(DISTSRC_PATH)/$(ARTIFACT_NAME)
DISTSRC_DIRS                = \
  $(if $(wildcard $(BASEDIR)/include/*), $(BASEDIR)/include) \
  $(if $(wildcard $(BASEDIR)/src/*), $(BASEDIR)/src) \
  $(if $(wildcard $(BASEDIR)/make/*), $(BASEDIR)/make) \
  $(if $(wildcard $(BASEDIR)/res/*), $(BASEDIR)/res)
DISTSRC_FILES               = \
  $(wildcard $(BASEDIR)/CHANGELOG) \
  $(wildcard $(BASEDIR)/COPYING*) \
  $(wildcard $(BASEDIR)/*LICENSE*) \
  $(wildcard $(BASEDIR)/Makefile) \
  $(wildcard $(BASEDIR)/*.mk) \
  $(wildcard $(BASEDIR)/*.md) \
  $(wildcard $(BASEDIR)/*.txt)

.DEFAULT_GOAL              := all
.PHONY: all compile install uninstall clean package

compile all install uninstall package:
	$(CHK_CONFIG)
	$(MAKE) -C "$(BASEDIR)/src" $(@) VERBOSE="$(VERBOSE)" CONFIG="$(CONFIG)" DESTDIR="$(DESTDIR)"

clean:
	echo "Cleaning build directory $(BUILDDIR)"
	-rm -rf $(BUILDDIR)
	echo "Clean OK"
	
# Module-related tasks
.PHONY: fetch tree prune
fetch:
	$(CHK_CONFIG)
	echo "Fetching desired source code dependencies"
	$(MAKE) -f "make/modules.mk" $(@) VERBOSE="$(VERBOSE)" BASEDIR="$(BASEDIR)" CONFIG="$(CONFIG)"
	echo "Fetch OK"
	
tree:
	echo "Fetching all possible source code dependencies"
	$(MAKE) -f "make/modules.mk" $(@) VERBOSE="$(VERBOSE)" BASEDIR="$(BASEDIR)" TREE="1"
	echo "Fetch OK"

prune: clean
	echo "Pruning the whole project tree"
	$(MAKE) -f "make/modules.mk" prune VERBOSE="$(VERBOSE)" BASEDIR="$(BASEDIR)" CONFIG="$(CONFIG)"
	$(MAKE) -f "make/modules.mk" prune VERBOSE="$(VERBOSE)" BASEDIR="$(BASEDIR)" TREE="1"
	-rm -rf "$(CONFIG)"
	echo "Prune OK"

# Configuration-related targets
.PHONY: config testconfig devel help chkconfig

config: CONFIG_FLAGS=
testconfig: CONFIG_FLAGS=TEST=1
devel: CONFIG_FLAGS=TEST=1 DEVEL=1

config testconfig devel:
	$(MAKE) -f "make/configure.mk" config VERBOSE="$(VERBOSE)" CONFIG="$(CONFIG)" -$(MAKEFLAGS)

# Release-related targets
.PHONY: distsrc
distsrc:
	echo "Building source code archive"
	mkdir -p "$(DISTSRC)/modules"
	$(MAKE) -f "make/modules.mk" tree DEVEL=$(DEVEL) VERBOSE="$(VERBOSE)" BASEDIR="$(BASEDIR)" MODULES="$(DISTSRC)/modules" TREE="1"
	$(if $(DISTSRC_DIRS), cp -R $(DISTSRC_DIRS) "$(DISTSRC)/")
	$(if $(DISTSRC_FILES), cp $(DISTSRC_FILES) "$(DISTSRC)/")
	find "$(DISTSRC)" -iname '.git' | xargs rm -rf {}
	find "$(DISTSRC)" -iname '.gitignore' | xargs rm -rf {}
	tar -C $(DISTSRC_PATH) -czf "$(BUILDDIR)/$(ARTIFACT_NAME)-src-$(ARTIFACT_VERSION).tar.gz" "$(ARTIFACT_NAME)"
	echo "Created archive: $(BUILDDIR)/$(ARTIFACT_NAME)-src-$(ARTIFACT_VERSION).tar.gz"
	rm -rf $(DISTSRC_PATH)
	echo "Build OK"

# Help
help:
	echo "Available targets:"
	echo "  all                       Build all binaries"
	echo "  clean                     Clean all build files and configuration file"
	echo "  config                    Configure build"
	echo "  devel                     Configure build as development build"
	echo "  distsrc                   Make tarball with source code for packagers"
	echo "  fetch                     Fetch all desired source code dependencies from git"
	echo "  help                      Print this help message"
	echo "  info                      Output build configuration"
	echo "  install                   Install all binaries into the system"
	echo "  package                   Create archive files with binaries"
	echo "  prune                     Cleanup build and all fetched dependencies from git"
	echo "  testconfig                Configure test build"
	echo "  tree                      Fetch all possible source code dependencies from git"
	echo "                            to make source code portable between machines"
	echo "  uninstall                 Uninstall binaries"
	echo ""
	$(MAKE) -f "$(BASEDIR)/make/configure.mk" $(@) VERBOSE="$(VERBOSE)"
	echo ""
	echo "Available FEATURES:"
	echo "  clap                      CLAP plugins"
	echo "  doc                       Generate standalone HTML documentation"
	echo "  gst                       GStreamer plugins"
	echo "  jack                      Standalone JACK plugins"
	echo "  ladspa                    LADSPA plugins"
	echo "  lv2                       LV2 plugins"
	echo "  vst2                      VST 2.x plugin binaries"
	echo "  vst3                      VST 3.x plugin binaries"
	echo "  xdg                       Desktop integration icons"

	