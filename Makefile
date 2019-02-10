# Estimate different pre-requisites before launching build
include scripts/make/set_vars.mk
include scripts/make/version.mk
include scripts/make/configure.mk

# Common definitions
OBJDIR                  = ${CURDIR}/.build
RELEASE_TEXT            = LICENSE.txt README.txt CHANGELOG.txt
RELEASE_SRC             = $(RELEASE_TEXT) src build-*.sh include res Makefile release.sh
RELEASE_SCRIPTS         = scripts/bash scripts/make
INSTALL                 = install

# Installation locations
BIN_PATH                = $(PREFIX)/bin
LIB_PATH                = $(PREFIX)/lib
DOC_PATH                = $(PREFIX)/share/doc
LADSPA_PATH             = $(LIB_PATH)/ladspa
LV2_PATH                = $(LIB_PATH)/lv2
VST_PATH                = $(LIB_PATH)/vst

# Directories
export RELEASE_TEXT
export ROOTDIR          = ${CURDIR}
export SRCDIR           = ${CURDIR}/src
export RESDIR           = ${CURDIR}/res
export RELEASE          = ${CURDIR}/.release
export RELEASE_BIN      = $(RELEASE)/$(BUILD_SYSTEM)-$(BUILD_PROFILE)
export BUILDDIR         = $(OBJDIR)

# Includes
INC_FLAGS               = -I"${CURDIR}/include"

# Location
export BASEDIR          = ${CURDIR}
export INCLUDE          = ${INC_FLAGS}
export MAKE_OPTS        = -s
export CFLAGS           = $(CC_ARCH) -std=c++98 -fdata-sections -pthread -ffunction-sections -fno-exceptions -fno-asynchronous-unwind-tables -Wall -pipe -fno-rtti $(CC_FLAGS) -DLSP_MAIN_VERSION=\"$(VERSION)\" -DLSP_INSTALL_PREFIX=\"$(PREFIX)\"
export CC               = g++
export PHP              = php
export LD               = ld
export LDFLAGS          = $(LD_ARCH) -L$(LD_PATH)
export SO_FLAGS         = $(CC_ARCH) -Wl,-rpath,$(LD_PATH) -Wl,-z,relro,-z,now -Wl,--gc-sections -shared -Llibrary -lc -fPIC
export MERGE_FLAGS      = $(LD_ARCH) -r
export EXE_TEST_FLAGS   = $(CC_ARCH) -Wl,-rpath,$(LD_PATH)
export EXE_FLAGS        = $(CC_ARCH) -Wl,-rpath,$(LD_PATH) -Wl,-z,relro,-z,now -Wl,--gc-sections

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
export OBJ_PLUGINS      = $(OBJDIR)/plugins.o
export OBJ_METADATA     = $(OBJDIR)/metadata.o
export OBJ_FILES        = $(OBJ_CORE) $(OBJ_UI_CORE) $(OBJ_RES_CORE) $(OBJ_PLUGINS) $(OBJ_METADATA)

# Libraries
export LIB_LADSPA       = $(OBJDIR)/$(ARTIFACT_ID)-ladspa.so
export LIB_LV2          = $(OBJDIR)/$(ARTIFACT_ID)-lv2.so
export LIB_VST          = $(OBJDIR)/$(ARTIFACT_ID)-vst-core-$(VERSION).so
export LIB_JACK         = $(OBJDIR)/$(ARTIFACT_ID)-jack-core-$(VERSION).so

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

.PHONY: all experimental trace debug tracefile debugfile profile gdb test compile 
.PHONY: install install_ladspa install_lv2 install_vst install_jack install_doc
.PHONY: uninstall uninstall_ladspa uninstall_lv2 uninstall_vst uninstall_jack uninstall_doc
.PHONY: release release_ladspa release_lv2 release_vst release_jack release_doc release_src
.PHONY: build_ladspa build_lv2 build_vst build_jack build_doc

default: all

all: export CFLAGS          += -O2 -DLSP_NO_EXPERIMENTAL
all: export EXE_FLAGS       += -pie -fPIE
all: compile

experimental: export CFLAGS += -O2
experimental: compile

trace: export CFLAGS        += -O2 -DLSP_TRACE -g3
trace: export EXE_FLAGS     += -g3
trace: compile

test: export CFLAGS         += -O2 -DLSP_TESTING -DLSP_TRACE -g3
test: export EXE_TEST_FLAGS += -g3
test: export MAKE_OPTS      += LSP_TESTING=1
test: compile

tracefile: export CFLAGS    += -DLSP_TRACEFILE
tracefile: trace

debug: export CFLAGS        += -O2 -DLSP_DEBUG
debug: compile

debugfile: export CFLAGS    += -DLSP_TRACEFILE
debugfile: debug

gdb: export CFLAGS          += -O0 -g3 -DLSP_TRACE
gdb: compile

profile: export CFLAGS      += -g -pg -DLSP_PROFILING -no-pie
profile: export EXE_FLAGS   += -g -pg -no-pie
profile: compile

# Compilation and cleaning targets
compile:
	@echo "-------------------------------------------------------------------------------"
	@echo "Building binaries"
	@echo "  target architecture : $(BUILD_PROFILE)"
	@echo "  target platform     : $(BUILD_PLATFORM)"
	@echo "  target system       : $(BUILD_SYSTEM)"
	@echo "  modules             : $(BUILD_MODULES)"
	@echo "-------------------------------------------------------------------------------"
	@mkdir -p $(OBJDIR)/src
	@test -f $(OBJDIR)/$(PREFIX_FILE) || echo -n "$(PREFIX)" > $(OBJDIR)/$(PREFIX_FILE)
	@test -f $(OBJDIR)/$(MODULES_FILE) || echo -n "$(BUILD_MODULES)" > $(OBJDIR)/$(MODULES_FILE)
	@$(MAKE) $(MAKE_OPTS) -C src all OBJDIR=$(OBJDIR)/src
	@echo "Build OK"

clean:
	@-rm -rf $(OBJDIR)
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
	@mkdir -p $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2
	@$(INSTALL) $(LIB_LV2) $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2/
	@$(UTL_GENTTL) $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2
	
install_vst: all
	@echo "Installing VST plugins to $(DESTDIR)$(VST_PATH)/$(VST_ID)"
	@mkdir -p $(DESTDIR)$(VST_PATH)/$(VST_ID)
	@$(INSTALL) $(LIB_VST) $(DESTDIR)$(VST_PATH)/$(VST_ID)/
	@$(INSTALL) $(OBJDIR)/src/vst/*.so $(DESTDIR)$(VST_PATH)/$(VST_ID)/

install_jack: all
	@echo "Installing JACK core to $(DESTDIR)$(LIB_PATH)"
	@mkdir -p $(DESTDIR)$(LIB_PATH)
	@$(INSTALL) $(LIB_JACK) $(DESTDIR)$(LIB_PATH)/
	@echo "Installing JACK standalone plugins to $(DESTDIR)$(BIN_PATH)"
	@mkdir -p $(DESTDIR)$(BIN_PATH)
	@$(MAKE) $(MAKE_OPTS) -C $(OBJDIR)/src/jack install TARGET_PATH=$(DESTDIR)$(BIN_PATH) INSTALL="$(INSTALL)"

install_doc: all
	@echo "Installing documentation to $(DESTDIR)$(DOC_PATH)"
	@mkdir -p $(DESTDIR)$(DOC_PATH)/$(ARTIFACT_ID)
	@cp -r $(OBJDIR)/html/* $(DESTDIR)$(DOC_PATH)/$(ARTIFACT_ID)

# Release targets
dbg_release: export CFLAGS        += -DLSP_TRACE -O2
dbg_release: $(RELEASES)
	@echo "Debug Release OK"

release: INSTALL        += -s
release: $(RELEASES)
	@echo "Release OK"

release_prepare: all
	@echo "Releasing plugins for architecture $(BUILD_PROFILE)"
	@mkdir -p $(RELEASE)
	@mkdir -p $(RELEASE_BIN)
	
release_ladspa: release_prepare
	@echo "Releasing LADSPA binaries"
	@mkdir -p $(RELEASE_BIN)/$(LADSPA_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
	@$(INSTALL) $(LIB_LADSPA) $(RELEASE_BIN)/$(LADSPA_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)/
	@cp $(RELEASE_TEXT) $(RELEASE_BIN)/$(LADSPA_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)/
	@tar -C $(RELEASE_BIN) -czf $(RELEASE_BIN)/$(LADSPA_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE).tar.gz $(LADSPA_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
	@rm -rf $(RELEASE_BIN)/$(LADSPA_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
	
release_lv2: release_prepare
	@echo "Releasing LV2 binaries"
	@mkdir -p $(RELEASE_BIN)/$(LV2_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
	@mkdir -p $(RELEASE_BIN)/$(LV2_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)/$(ARTIFACT_ID).lv2
	@$(INSTALL) $(LIB_LV2) $(RELEASE_BIN)/$(LV2_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)/$(ARTIFACT_ID).lv2/
	@cp $(RELEASE_TEXT) $(RELEASE_BIN)/$(LV2_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)/
	@$(UTL_GENTTL) $(RELEASE_BIN)/$(LV2_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)/$(ARTIFACT_ID).lv2
	@tar -C $(RELEASE_BIN) -czf $(RELEASE_BIN)/$(LV2_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE).tar.gz $(LV2_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
	@rm -rf $(RELEASE_BIN)/$(LV2_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
	
release_vst: release_prepare
	@echo "Releasing VST binaries"
	@mkdir -p $(RELEASE_BIN)/$(VST_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
	@$(INSTALL) $(LIB_VST) $(RELEASE_BIN)/$(VST_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)/
	@$(INSTALL) $(OBJDIR)/src/vst/*.so $(RELEASE_BIN)/$(VST_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)/
	@cp $(RELEASE_TEXT) $(RELEASE_BIN)/$(VST_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)/
	@tar -C $(RELEASE_BIN) -czf $(RELEASE_BIN)/$(VST_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE).tar.gz $(VST_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
	@rm -rf $(RELEASE_BIN)/$(VST_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
	
release_jack: release_prepare
	@echo "Releasing JACK binaries"
	@mkdir -p $(RELEASE_BIN)/$(JACK_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
	@mkdir -p $(RELEASE_BIN)/$(JACK_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)/lib
	@mkdir -p $(RELEASE_BIN)/$(JACK_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)/bin
	@$(INSTALL) $(LIB_JACK) $(RELEASE_BIN)/$(JACK_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)/lib
	@$(MAKE) $(MAKE_OPTS) -C $(OBJDIR)/src/jack install TARGET_PATH=$(RELEASE_BIN)/$(JACK_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)/bin INSTALL="$(INSTALL)"
	@cp $(RELEASE_TEXT) $(RELEASE_BIN)/$(JACK_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)/
	@tar -C $(RELEASE_BIN) -czf $(RELEASE_BIN)/$(JACK_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE).tar.gz $(JACK_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
	@rm -rf $(RELEASE_BIN)/$(JACK_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)

release_profile: release_prepare
	@echo "Releasing PROFILE binaries"
	@mkdir -p $(RELEASE_BIN)/$(PROFILE_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
	@$(INSTALL) $(BIN_PROFILE) $(RELEASE_BIN)/$(PROFILE_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
	@cp $(RELEASE_TEXT) $(RELEASE_BIN)/$(PROFILE_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)/
	@tar -C $(RELEASE_BIN) -czf $(RELEASE_BIN)/$(PROFILE_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE).tar.gz $(PROFILE_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)
	@rm -rf $(RELEASE_BIN)/$(PROFILE_ID)-$(BUILD_SYSTEM)-$(BUILD_PROFILE)

release_src:
	@echo "Releasing source code binaries"
	@mkdir -p $(RELEASE)/$(SRC_ID)
	@mkdir -p $(RELEASE)/$(SRC_ID)/scripts
	@cp -R $(RELEASE_SRC) $(RELEASE)/$(SRC_ID)/
	@cp -R $(RELEASE_SCRIPTS) $(RELEASE)/$(SRC_ID)/scripts/
	@tar -C $(RELEASE) -czf $(RELEASE)/$(SRC_ID).tar.gz $(SRC_ID)
	@rm -rf $(RELEASE)/$(SRC_ID)

release_doc: release_prepare
	@echo "Releasing documentation"
	@mkdir -p $(RELEASE)/$(DOC_ID)
	@cp -r $(OBJDIR)/html/* $(RELEASE)/$(DOC_ID)/
	@cp $(RELEASE_TEXT) $(RELEASE)/$(DOC_ID)/
	@tar -C $(RELEASE) -czf $(RELEASE)/$(DOC_ID).tar.gz $(DOC_ID)
	@rm -rf $(RELEASE)/$(DOC_ID)

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
	
uninstall_doc:
	@echo "Uninstalling DOC"
	@-rm -rf $(DESTDIR)$(DOC_PATH)/$(ARTIFACT_ID)
