# Common definitions
RELEASE_TEXT            = LICENSE.txt README.txt CHANGELOG.txt
RELEASE_SRC             = $(RELEASE_TEXT) src build-*.sh include res Makefile release.sh
RELEASE_SCRIPTS         = scripts/bash scripts/make
INSTALL                 = install
export RELEASE_TEXT

# Estimate different pre-requisites before launching build
include scripts/make/set_vars.mk
include scripts/make/tools.mk
include scripts/make/version.mk
include scripts/make/configure.mk

# Build directories
export ROOTDIR          = ${CURDIR}
export SRCDIR           = ${CURDIR}/src
export RESDIR           = ${CURDIR}/res
export RELEASE          = ${CURDIR}/.release
export RELEASE_BIN      = $(RELEASE)/$(BUILD_SYSTEM)-$(BUILD_PROFILE)
export BUILDDIR         = ${CURDIR}/.build
export TESTDIR          = ${CURDIR}/.test
OBJDIR                  = $(BUILDDIR)

# Installation locations
BIN_PATH               ?= $(PREFIX)/bin
LIB_PATH               ?= $(PREFIX)/lib
SHARE_PATH             ?= $(PREFIX)/share
ETC_PATH               ?= /etc
DOC_PATH               ?= $(SHARE_PATH)/doc
LADSPA_PATH             = $(LIB_PATH)/ladspa
LV2_PATH                = $(LIB_PATH)/lv2
VST_PATH                = $(LIB_PATH)/vst

# Location
export BASEDIR          = ${CURDIR}

# Objects
export OBJ_CORE         = $(OBJDIR)/core.o
export OBJ_DSP          = $(OBJDIR)/dsp.o
export OBJ_CTL_CORE     = $(OBJDIR)/ctl_core.o
export OBJ_TK_CORE      = $(OBJDIR)/tk_core.o
export OBJ_WS_CORE      = $(OBJDIR)/ws_core.o
export OBJ_WS_X11_CORE  = $(OBJDIR)/ws_x11_core.o
export OBJ_UI_CORE      = $(OBJDIR)/ui_core.o
export OBJ_RES_CORE     = $(OBJDIR)/res_core.o
export OBJ_TEST_CORE    = $(OBJDIR)/test_core.o
export OBJ_TESTING_CORE = $(OBJDIR)/testing_core.o
export OBJ_PLUGINS      = $(OBJDIR)/plugins.o
export OBJ_PLUGIN_UIS	= $(OBJDIR)/plugin_uis.o
export OBJ_METADATA     = $(OBJDIR)/metadata.o
export OBJ_FILES        = $(OBJ_CORE) $(OBJ_UI_CORE) $(OBJ_RES_CORE) $(OBJ_PLUGINS) $(OBJ_METADATA)

# Libraries
export LIB_LADSPA       = $(OBJDIR)/$(ARTIFACT_ID)-ladspa.so
export LIB_LV2          = $(OBJDIR)/$(ARTIFACT_ID)-lv2.so
export LIB_VST          = $(OBJDIR)/$(ARTIFACT_ID)-vst-core-$(VERSION).so
export LIB_JACK         = $(OBJDIR)/$(ARTIFACT_ID)-jack-core-$(VERSION).so
export LIB_R3D_GLX      = $(OBJDIR)/$(R3D_ARTIFACT_ID)-glx.so

# Binaries
export BIN_PROFILE      = $(OBJDIR)/$(ARTIFACT_ID)-profile
export BIN_TEST         = $(OBJDIR)/$(ARTIFACT_ID)-test

# Utils
export UTL_GENTTL       = $(OBJDIR)/lv2_genttl.exe
export UTL_VSTMAKE      = $(OBJDIR)/vst_genmake.exe
export UTL_JACKMAKE     = $(OBJDIR)/jack_genmake.exe
export UTL_GENPHP       = $(OBJDIR)/gen_php.exe
export UTL_RESGEN       = $(OBJDIR)/gen_resources.exe
export UTL_FILES        = $(UTL_GENTTL) $(UTL_VSTMAKE) $(UTL_GENPHP) $(UTL_RESGEN)

# Files
export PHP_PLUGINS      = $(OBJDIR)/plugins.php

FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
FILES                   =

LADSPA_ID              := $(ARTIFACT_ID)-ladspa-$(VERSION)
LV2_ID                 := $(ARTIFACT_ID)-lv2-$(VERSION)
VST_ID                 := $(ARTIFACT_ID)-lxvst-$(VERSION)
JACK_ID                := $(ARTIFACT_ID)-jack-$(VERSION)
PROFILE_ID             := $(ARTIFACT_ID)-profile-$(VERSION)
SRC_ID                 := $(ARTIFACT_ID)-src-$(VERSION)
DOC_ID                 := $(ARTIFACT_ID)-doc-$(VERSION)

.DEFAULT_GOAL          := all
.PHONY: all experimental trace debug tracefile debugfile profile gdb test testdebug testprofile compile test_compile
.PHONY: compile_info
.PHONY: install install_ladspa install_lv2 install_vst install_jack install_doc
.PHONY: uninstall uninstall_ladspa uninstall_lv2 uninstall_vst uninstall_jack uninstall_doc
.PHONY: release release_ladspa release_lv2 release_vst release_jack release_doc release_src
.PHONY: build_ladspa build_lv2 build_vst build_jack build_doc

default: all

all: export CFLAGS          += -O2 -DLSP_NO_EXPERIMENTAL
all: export CXXFLAGS        += -O2 -DLSP_NO_EXPERIMENTAL
all: export EXE_FLAGS       += -pie -fPIE
all: compile

experimental: export CFLAGS += -O2
experimental: export CXXFLAGS += -O2
experimental: compile

trace: export CFLAGS        += -O2 -DLSP_TRACE -g3 -fstack-protector
trace: export CXXFLAGS      += -O2 -DLSP_TRACE -g3 -fstack-protector
trace: export EXE_FLAGS     += -g3
trace: compile

test: OBJDIR                 = $(TESTDIR)
test: export CFLAGS         += -O2 -DLSP_TESTING -DLSP_TRACE -g3 -fstack-protector
test: export CXXFLAGS       += -O2 -DLSP_TESTING -DLSP_TRACE -g3 -fstack-protector
test: export EXE_TEST_FLAGS += -g3
test: export MAKE_OPTS      += LSP_TESTING=1
test: export BUILD_MODULES   = jack
test: test_compile

testdebug: OBJDIR                 = $(TESTDIR)
testdebug: export CFLAGS         += -O0 -DLSP_TESTING -DLSP_TRACE -g3 -fstack-protector
testdebug: export CXXFLAGS       += -O0 -DLSP_TESTING -DLSP_TRACE -g3 -fstack-protector
testdebug: export EXE_TEST_FLAGS += -g3
testdebug: export MAKE_OPTS      += LSP_TESTING=1
testdebug: export BUILD_MODULES   = jack
testdebug: test_compile

testprofile: OBJDIR                 = $(TESTDIR)
testprofile: export CFLAGS         += -g -pg -O2 -DLSP_PROFILING -DLSP_TESTING -DLSP_TRACE -g3 -no-pie -fno-pie -fPIC -fstack-protector
testprofile: export CXXFLAGS       += -g -pg -O2 -DLSP_PROFILING -DLSP_TESTING -DLSP_TRACE -g3 -no-pie -fno-pie -fPIC -fstack-protector
testprofile: export EXE_TEST_FLAGS += -g -pg -O2 -g3 -no-pie -fno-pie -fPIC
testprofile: export MAKE_OPTS      += LSP_TESTING=1
testprofile: compile

tracefile: export CFLAGS    += -DLSP_TRACEFILE
tracefile: export CXXFLAGS  += -DLSP_TRACEFILE
tracefile: trace

debug: export CFLAGS        += -O2 -DLSP_DEBUG
debug: export CXXFLAGS      += -O2 -DLSP_DEBUG
debug: compile

debugfile: export CFLAGS    += -DLSP_TRACEFILE
debugfile: export CXXFLAGS  += -DLSP_TRACEFILE
debugfile: debug

gdb: export CFLAGS          += -O0 -g3 -DLSP_TRACE
gdb: export CXXFLAGS        += -O0 -g3 -DLSP_TRACE
gdb: compile

profile: export CFLAGS      += -g -pg -DLSP_PROFILING -no-pie -fno-pie -fPIC
profile: export CXXFLAGS    += -g -pg -DLSP_PROFILING -no-pie -fno-pie -fPIC
profile: export EXE_FLAGS   += -g -pg -no-pie -fno-pie -fPIC
profile: compile

# Compilation and cleaning targets
compile_info:
	@echo "-------------------------------------------------------------------------------"
	@echo "Building binaries"
	@echo "  target architecture : $(BUILD_PROFILE)"
	@echo "  target platform     : $(BUILD_PLATFORM)"
	@echo "  target system       : $(BUILD_SYSTEM)"
	@echo "  compiler            : $(BUILD_COMPILER)"
	@echo "  modules             : $(BUILD_MODULES)"
	@echo "  UI                  : LV2=$(LV2_UI), VST=$(VST_UI)"
	@echo "  3D rendering        : $(BUILD_R3D_BACKENDS)"
	@echo "  build directory     : $(OBJDIR)"
	@echo "-------------------------------------------------------------------------------"

compile: | compile_info
	@mkdir -p $(OBJDIR)/src
	@mkdir -p $(CFGDIR)
	@test -f "$(CFGDIR)/$(PREFIX_FILE)" || echo -n "$(PREFIX)" > "$(CFGDIR)/$(PREFIX_FILE)"
	@test -f "$(CFGDIR)/$(MODULES_FILE)" || echo -n "$(BUILD_MODULES)" > "$(CFGDIR)/$(MODULES_FILE)"
	@test -f "$(CFGDIR)/$(BUILD_PROFILE)" || echo -n "$(BUILD_PROFILE)" > "$(CFGDIR)/$(BUILD_PROFILE_FILE)"
	@test -f "$(CFGDIR)/$(R3D_BACKENDS_FILE)" || echo -n "$(BUILD_R3D_BACKENDS)" > "$(CFGDIR)/$(R3D_BACKENDS_FILE)"
	@$(MAKE) $(MAKE_OPTS) -C src all OBJDIR=$(OBJDIR)/src
	@echo "Build OK"
	
test_compile: | compile_info
	@mkdir -p $(OBJDIR)/src
	@$(MAKE) $(MAKE_OPTS) -C src all OBJDIR=$(OBJDIR)/src
	@echo "Test Build OK"

clean:
	@-rm -rf $(BUILDDIR)
	@-rm -rf $(TESTDIR)
	@-rm -rf $(CFGDIR)
	@echo "Clean OK"

# Build targets
build_ladspa: export BUILD_MODULES = ladspa
build_ladspa: compile

build_lv2: export BUILD_MODULES = lv2
build_lv2: compile

build_vst: export BUILD_MODULES = vst
build_vst: compile

build_jack: export BUILD_MODULES = jack
build_jack: compile

build_doc: export BUILD_MODULES = doc
build_doc: compile


# Installation targets
install: $(INSTALLATIONS)
	@echo "Install OK"

install_ladspa: all
	@echo "Installing LADSPA plugins to $(DESTDIR)$(LADSPA_PATH)/"
	@mkdir -p $(DESTDIR)$(LADSPA_PATH)
	@$(INSTALL) $(LIB_LADSPA) $(DESTDIR)$(LADSPA_PATH)/
	
install_lv2: all
	@echo "Installing LV2 plugins to $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2"
	@mkdir -p "$(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2"
	@$(INSTALL) $(LIB_LV2) "$(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2/"
	@test ! "$(BUILD_R3D_BACKENDS)" || $(INSTALL) $(OBJDIR)/$(R3D_ARTIFACT_ID)*.so $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2/
	@$(UTL_GENTTL) "$(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2"
	
install_vst: all
	@echo "Installing VST plugins to $(DESTDIR)$(VST_PATH)/$(VST_ID)"
	@mkdir -p "$(DESTDIR)$(VST_PATH)/$(VST_ID)"
	@$(INSTALL) $(LIB_VST) "$(DESTDIR)$(VST_PATH)/$(VST_ID)/"
	@test ! "$(BUILD_R3D_BACKENDS)" || $(INSTALL) $(OBJDIR)/$(R3D_ARTIFACT_ID)*.so $(DESTDIR)$(VST_PATH)/$(VST_ID)/
	@$(INSTALL) $(OBJDIR)/src/vst/*.so $(DESTDIR)$(VST_PATH)/$(VST_ID)/

install_jack: all
	@echo "Installing JACK core to $(DESTDIR)$(LIB_PATH)/$(ARTIFACT_ID)"
	@mkdir -p "$(DESTDIR)$(LIB_PATH)/$(ARTIFACT_ID)"
	@$(INSTALL) $(LIB_JACK) "$(DESTDIR)$(LIB_PATH)/$(ARTIFACT_ID)/"
	@test ! "$(BUILD_R3D_BACKENDS)" || $(INSTALL) $(OBJDIR)/$(R3D_ARTIFACT_ID)*.so "$(DESTDIR)$(LIB_PATH)/$(ARTIFACT_ID)/"
	@echo "Installing JACK standalone plugins to $(DESTDIR)$(BIN_PATH)"
	@mkdir -p "$(DESTDIR)$(BIN_PATH)"
	@$(MAKE) $(MAKE_OPTS) -C $(OBJDIR)/src/jack install TARGET_PATH="$(DESTDIR)$(BIN_PATH)" INSTALL="$(INSTALL)"
	@echo "Installing desktop icons to $(DESTDIR)$(SHARE_PATH)/applications"
	@mkdir -p "$(DESTDIR)$(SHARE_PATH)/applications"
	@mkdir -p "$(DESTDIR)$(SHARE_PATH)/desktop-directories"
	@mkdir -p "$(DESTDIR)$(ETC_PATH)/xdg/menus/applications-merged"
	@mkdir -p "$(DESTDIR)$(SHARE_PATH)/icons/hicolor/scalable/apps"
	@mkdir -p "$(DESTDIR)$(SHARE_PATH)/icons/hicolor/16x16/apps"
	@mkdir -p "$(DESTDIR)$(SHARE_PATH)/icons/hicolor/22x22/apps"
	@mkdir -p "$(DESTDIR)$(SHARE_PATH)/icons/hicolor/24x24/apps"
	@mkdir -p "$(DESTDIR)$(SHARE_PATH)/icons/hicolor/32x32/apps"
	@mkdir -p "$(DESTDIR)$(SHARE_PATH)/icons/hicolor/48x48/apps"
	@mkdir -p "$(DESTDIR)$(SHARE_PATH)/icons/hicolor/64x64/apps"
	@mkdir -p "$(DESTDIR)$(SHARE_PATH)/icons/hicolor/128x128/apps"
	@mkdir -p "$(DESTDIR)$(SHARE_PATH)/icons/hicolor/256x256/apps"
	@cp res/xdg/*.desktop "$(DESTDIR)$(SHARE_PATH)/applications/"
	@cp res/xdg/lsp-plugins.directory "$(DESTDIR)$(SHARE_PATH)/desktop-directories/"
	@cp res/xdg/lsp-plugins.menu "$(DESTDIR)$(ETC_PATH)/xdg/menus/applications-merged/"
	@cp -f res/icons/$(ARTIFACT_ID)-16.png "$(DESTDIR)$(SHARE_PATH)/icons/hicolor/16x16/apps/$(ARTIFACT_ID).png"
	@cp -f res/icons/$(ARTIFACT_ID)-22.png "$(DESTDIR)$(SHARE_PATH)/icons/hicolor/22x22/apps/$(ARTIFACT_ID).png"
	@cp -f res/icons/$(ARTIFACT_ID)-24.png "$(DESTDIR)$(SHARE_PATH)/icons/hicolor/24x24/apps/$(ARTIFACT_ID).png"
	@cp -f res/icons/$(ARTIFACT_ID)-32.png "$(DESTDIR)$(SHARE_PATH)/icons/hicolor/32x32/apps/$(ARTIFACT_ID).png"
	@cp -f res/icons/$(ARTIFACT_ID)-48.png "$(DESTDIR)$(SHARE_PATH)/icons/hicolor/48x48/apps/$(ARTIFACT_ID).png"
	@cp -f res/icons/$(ARTIFACT_ID)-64.png "$(DESTDIR)$(SHARE_PATH)/icons/hicolor/64x64/apps/$(ARTIFACT_ID).png"
	@cp -f res/icons/$(ARTIFACT_ID)-128.png "$(DESTDIR)$(SHARE_PATH)/icons/hicolor/128x128/apps/$(ARTIFACT_ID).png"
	@cp -f res/icons/$(ARTIFACT_ID)-256.png "$(DESTDIR)$(SHARE_PATH)/icons/hicolor/256x256/apps/$(ARTIFACT_ID).png"
	@cp -f res/icons/$(ARTIFACT_ID)-exp.svg "$(DESTDIR)$(SHARE_PATH)/icons/hicolor/scalable/apps/$(ARTIFACT_ID).svg"

install_doc: all
	@echo "Installing documentation to $(DESTDIR)$(DOC_PATH)"
	@mkdir -p $(DESTDIR)$(DOC_PATH)/$(ARTIFACT_ID)
	@cp -r $(OBJDIR)/html/* $(DESTDIR)$(DOC_PATH)/$(ARTIFACT_ID)

# Release targets
dbg_release: export CFLAGS        += -DLSP_TRACE -O2
dbg_release: export CXXFLAGS      += -DLSP_TRACE -O2
dbg_release: $(RELEASES)
	@echo "Debug Release OK"

release: INSTALL        += -s
release: $(RELEASES)
	@echo "Release OK"

release_prepare: all
	@echo "Releasing plugins for architecture $(BUILD_PROFILE)"
	@mkdir -p $(RELEASE)
	@mkdir -p $(DESTDIR)
	
release_ladspa: DESTDIR=$(RELEASE_BIN)/$(LADSPA_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
release_ladspa: | release_prepare install_ladspa
	@echo "Releasing LADSPA binaries"
	@cp $(RELEASE_TEXT) $(DESTDIR)/
	@tar -C $(RELEASE_BIN) -czf $(RELEASE_BIN)/$(LADSPA_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE).tar.gz $(LADSPA_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
	@rm -rf $(DESTDIR)
	
release_lv2: DESTDIR=$(RELEASE_BIN)/$(LV2_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
release_lv2: | release_prepare install_lv2
	@echo "Releasing LV2 binaries"
	@cp $(RELEASE_TEXT) $(DESTDIR)/
	@tar -C $(RELEASE_BIN) -czf $(RELEASE_BIN)/$(LV2_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE).tar.gz $(LV2_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
	@rm -rf $(DESTDIR)
	
release_vst: DESTDIR=$(RELEASE_BIN)/$(VST_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
release_vst: | release_prepare install_vst
	@echo "Releasing VST binaries"
	@cp $(RELEASE_TEXT) $(DESTDIR)/
	@tar -C $(RELEASE_BIN) -czf $(RELEASE_BIN)/$(VST_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE).tar.gz $(VST_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
	@rm -rf $(DESTDIR)
	
release_jack: DESTDIR=$(RELEASE_BIN)/$(JACK_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
release_jack: | release_prepare install_jack
	@echo "Releasing JACK binaries"
	@cp $(RELEASE_TEXT) $(DESTDIR)/
	@tar -C $(RELEASE_BIN) -czf $(RELEASE_BIN)/$(JACK_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE).tar.gz $(JACK_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
	@rm -rf $(DESTDIR)

release_profile: DESTDIR=$(RELEASE_BIN)/$(PROFILE_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
release_profile: | release_prepare
	@echo "Releasing PROFILE binaries"
	@$(INSTALL) $(BIN_PROFILE) $(DESTDIR)/
	@cp $(RELEASE_TEXT) $(DESTDIR)/
	@tar -C $(RELEASE_BIN) -czf $(RELEASE_BIN)/$(PROFILE_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE).tar.gz $(PROFILE_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
	@rm -rf $(RELEASE_BIN)/$(PROFILE_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)

release_src: DESTDIR=$(RELEASE)/$(SRC_ID)
release_src: | release_prepare
	@echo "Releasing source code binaries"
	@mkdir -p $(DESTDIR)
	@mkdir -p $(DESTDIR)/scripts
	@cp -R $(RELEASE_SRC) $(DESTDIR)/
	@cp -R $(RELEASE_SCRIPTS) $(DESTDIR)/scripts/
	@tar -C $(RELEASE) -czf $(RELEASE)/$(SRC_ID).tar.gz $(SRC_ID)
	@rm -rf $(DESTDIR)

release_doc: DESTDIR=$(RELEASE)/$(DOC_ID)
release_doc: | release_prepare
	@echo "Releasing documentation"
	@mkdir -p $(DESTDIR)
	@cp -r $(OBJDIR)/html/* $(DESTDIR)/
	@cp $(RELEASE_TEXT) $(DESTDIR)/
	@tar -C $(RELEASE) -czf $(RELEASE)/$(DOC_ID).tar.gz $(DOC_ID)
	@rm -rf $(DESTDIR)

# Unrelease target
unrelease: clean
	@-rm -rf $(RELEASE)
	@echo "Unrelease OK"

# Uninstall target
uninstall: $(UNINSTALLATIONS)
	@echo "Uninstall OK"
	
uninstall_ladspa:
	@echo "Uninstalling LADSPA"
	@-rm -f $(DESTDIR)$(LADSPA_PATH)/$(ARTIFACT_ID)-ladspa.so
	
uninstall_lv2:
	@echo "Uninstalling LV2"
	@-rm -rf $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2
	
uninstall_vst:
	@echo "Uninstalling VST"
	@-rm -f $(DESTDIR)$(VST_PATH)/$(ARTIFACT_ID)-vst-*.so
	@-rm -rf $(DESTDIR)$(VST_PATH)/$(ARTIFACT_ID)-lxvst-*
	@-rm -rf $(DESTDIR)$(VST_PATH)/$(VST_ID)
	
uninstall_jack:
	@echo "Uninstalling JACK"
	@-rm -f $(DESTDIR)$(BIN_PATH)/$(ARTIFACT_ID)-*
	@-rm -f $(DESTDIR)$(LIB_PATH)/$(ARTIFACT_ID)-jack-core-*.so
	@-rm -f $(DESTDIR)$(LIB_PATH)/$(R3D_ARTIFACT_ID)
	@-rm -f $(DESTDIR)$(SHARE_PATH)/applications/in.lsp_plug.*.desktop
	@-rm -f $(DESTDIR)$(SHARE_PATH)/desktop-directories/lsp-plugins.directory
	@-rm -f $(DESTDIR)$(ETC_PATH)/xdg/menus/applications-merged/lsp-plugins.menu
	@-rm -f $(DESTDIR)$(SHARE_PATH)/icons/hicolor/16x16/apps/$(ARTIFACT_ID).*
	@-rm -f $(DESTDIR)$(SHARE_PATH)/icons/hicolor/22x22/apps/$(ARTIFACT_ID).*
	@-rm -f $(DESTDIR)$(SHARE_PATH)/icons/hicolor/24x24/apps/$(ARTIFACT_ID).*
	@-rm -f $(DESTDIR)$(SHARE_PATH)/icons/hicolor/32x32/apps/$(ARTIFACT_ID).*
	@-rm -f $(DESTDIR)$(SHARE_PATH)/icons/hicolor/48x48/apps/$(ARTIFACT_ID).*
	@-rm -f $(DESTDIR)$(SHARE_PATH)/icons/hicolor/64x64/apps/$(ARTIFACT_ID).*
	@-rm -f $(DESTDIR)$(SHARE_PATH)/icons/hicolor/128x128/apps/$(ARTIFACT_ID).*
	@-rm -f $(DESTDIR)$(SHARE_PATH)/icons/hicolor/256x256/apps/$(ARTIFACT_ID).*
	@-rm -f $(DESTDIR)$(SHARE_PATH)/icons/hicolor/scalable/apps/$(ARTIFACT_ID).*
	@-rm -rf $(DESTDIR)$(LIB_PATH)/$(ARTIFACT_ID)

uninstall_doc:
	@echo "Uninstalling DOC"
	@-rm -rf $(DESTDIR)$(DOC_PATH)/$(ARTIFACT_ID)
