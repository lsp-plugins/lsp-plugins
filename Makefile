#!/usr/bin/make -f
#
# Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
#           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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
DISTSRC_PATH                = $(BUILDDIR)/.distsrc
DISTSRC                     = $(DISTSRC_PATH)/$(ARTIFACT_NAME)

.DEFAULT_GOAL              := all
.PHONY: all compile install uninstall depend clean

compile all install uninstall depend:
	$(CHK_CONFIG)
	$(MAKE) -C "$(BASEDIR)/src" $(@) VERBOSE="$(VERBOSE)" CONFIG="$(CONFIG)" PLUGINS="$(PLUGINS)" DESTDIR="$(DESTDIR)" ROOTDIR="$(BASEDIR)" ARTIFACT_VARS="$(ARTIFACT_VARS)"

clean:
	echo "Cleaning build directory $(BUILDDIR)"
	-rm -rf $(BUILDDIR)
	echo "Clean OK"
	
# Module-related tasks
.PHONY: fetch tree prune
fetch:
	$(CHK_CONFIG)
	echo "Fetching desired source code dependencies"
	$(MAKE) -f "$(BASEDIR)/make/modules.mk" $(@) VERBOSE="$(VERBOSE)" BASEDIR="$(BASEDIR)" CONFIG="$(CONFIG)" MODULES="$(MODULES)"
	echo "Fetch OK"
	
tree:
	echo "Fetching all possible source code dependencies"
	$(MAKE) -f "$(BASEDIR)/make/modules.mk" $(@) VERBOSE="$(VERBOSE)" BASEDIR="$(BASEDIR)" TREE="1"
	echo "Fetch OK"

prune: clean
	echo "Pruning the whole project tree"
	$(MAKE) -f "$(BASEDIR)/make/modules.mk" prune VERBOSE="$(VERBOSE)" BASEDIR="$(BASEDIR)" CONFIG="$(CONFIG)"
	$(MAKE) -f "$(BASEDIR)/make/modules.mk" prune VERBOSE="$(VERBOSE)" BASEDIR="$(BASEDIR)" TREE="1"
	-rm -rf "$(CONFIG)"
	echo "Prune OK"

# Configuration-related targets
.PHONY: config help chkconfig

testconfig:
	$(MAKE) -f "$(BASEDIR)/make/configure.mk" $(@) VERBOSE="$(VERBOSE)" -$(MAKEFLAGS) CONFIG="$(CONFIG)" PLUGINS="$(PLUGINS)" TEST="1" 

config:
	$(MAKE) -f "$(BASEDIR)/make/configure.mk" $(@) VERBOSE="$(VERBOSE)" -$(MAKEFLAGS) CONFIG="$(CONFIG)" PLUGINS="$(PLUGINS)" 

# Release-related targets
.PHONY: distsrc
distsrc:
	echo "Building source code archive"
	mkdir -p "$(DISTSRC)/modules"
	$(MAKE) -f "$(BASEDIR)/make/modules.mk" tree VERBOSE="$(VERBOSE)" BASEDIR="$(BASEDIR)" MODULES="$(DISTSRC)/modules" TREE="1"
	cp -R $(BASEDIR)/include $(BASEDIR)/make $(BASEDIR)/src "$(DISTSRC)/"
	cp $(BASEDIR)/CHANGELOG $(BASEDIR)/COPYING* $(BASEDIR)/Makefile $(BASEDIR)/*.mk "$(DISTSRC)/"
	find "$(DISTSRC)" -iname '.git' | xargs -exec rm -rf {}
	find "$(DISTSRC)" -iname '.gitignore' | xargs -exec rm -rf {}
	tar -C $(DISTSRC_PATH) -czf "$(BUILDDIR)/$(ARTIFACT_NAME)-$(ARTIFACT_VERSION)-src.tar.gz" "$(ARTIFACT_NAME)"
	echo "Created archive: $(BUILDDIR)/$(ARTIFACT_NAME)-$(ARTIFACT_VERSION)-src.tar.gz"
	ln -sf "$(ARTIFACT_NAME)-$(ARTIFACT_VERSION)-src.tar.gz" "$(BUILDDIR)/$(ARTIFACT_NAME)-src.tar.gz"
	echo "Created symlink: $(BUILDDIR)/$(ARTIFACT_NAME)-src.tar.gz"
	rm -rf $(DISTSRC_PATH)
	echo "Build OK"

# Help
help:
	echo "Available targets:"
	echo "  all                       Build all binaries"
	echo "  clean                     Clean all build files and configuration file"
	echo "  config                    Configure build"
	echo "  depend                    Update build dependencies for current project"
	echo "  distsrc                   Make tarball with source code for packagers"
	echo "  fetch                     Fetch all desired source code dependencies from git"
	echo "  help                      Print this help message"
	echo "  info                      Output build configuration"
	echo "  install                   Install all binaries into the system"
	echo "  prune                     Cleanup build and all fetched dependencies from git"
	echo "  tree                      Fetch all possible source code dependencies from git"
	echo "                            to make source code portable between machines"
	echo "  uninstall                 Uninstall binaries"
	echo ""
	$(MAKE) -f "$(BASEDIR)/make/configure.mk" $(@) VERBOSE="$(VERBOSE)"
	echo ""
