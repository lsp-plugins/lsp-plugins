LADSPA_PATH             = /usr/local/lib/ladspa
LV2_PATH                = /usr/local/lib/lv2
VST_PATH                = /usr/local/lib/vst
PRODUCT                 = lsp
ARTIFACT_ID             = $(PRODUCT)-plugins
OBJDIR                  = ${CURDIR}/.build
RELEASE                 = ${CURDIR}/.release
RELEASE_TEXT            = LICENSE.txt README.txt CHANGELOG.txt
INSTALL                 = install

# Dependencies
export VST_SDK          = /home/sadko/eclipse/lsp-plugins-vst3sdk

# Includes
INC_FLAGS               = -I"${CURDIR}/include"
INSTALLATIONS           = install_php install_ladspa install_lv2
RELEASES                = release_ladspa release_lv2
ifdef VST_SDK
INC_FLAGS              += -I"$(VST_SDK)"
INSTALLATIONS          += install_vst
RELEASES               += release_vst
endif

# Flags
ifndef CPU_ARCH
export CPU_ARCH         = x86_64
export CC_ARCH          = -m64
export LD_ARCH          = -m elf_x86_64
export LD_PATH          = /usr/lib64:/lib64

#export CPU_ARCH         = i586
#export CC_ARCH          = -m32 -msse -msse2
#export LD_ARCH          = -m elf_i386
#export LD_PATH          = /usr/lib:/lib
endif

export VERSION          = 1.0.6
export BASEDIR          = ${CURDIR}
export INCLUDE          = ${INC_FLAGS}
export MAKE_OPTS        = -s
export CFLAGS           = $(CC_ARCH) -fPIC -O2 -fno-exceptions -Wall -pthread -pipe -fno-rtti $(CC_FLAGS)
export CC               = g++
export LD               = ld
export LDFLAGS          = $(LD_ARCH) 
export SO_FLAGS         = $(CC_ARCH) -Wl,-rpath,$(LD_PATH) -shared -Llibrary -lc -lm -fPIC -lpthread
export MERGE_FLAGS      = $(LD_ARCH) -r
export EXE_FLAGS        = $(CC_ARCH) -Wl,-rpath,$(LD_PATH) -lm -fPIC -pthread

# Objects
export OBJ_CORE         = $(OBJDIR)/core.o
export OBJ_UI_CORE      = $(OBJDIR)/ui_core.o
export OBJ_RES_CORE     = $(OBJDIR)/res_core.o
export OBJ_PLUGINS      = $(OBJDIR)/plugins.o
export OBJ_METADATA     = $(OBJDIR)/metadata.o
export OBJ_GTK2UI       = $(OBJDIR)/ui_gtk2.o
export OBJ_GTK3UI       = $(OBJDIR)/ui_gtk3.o
export OBJ_FILES		= $(OBJ_CORE) $(OBJ_UI_CORE) $(OBJ_RES_CORE) $(OBJ_PLUGINS) $(OBJ_METADATA) $(OBJ_GTK2UI)


# Libraries
export LIB_LADSPA       = $(OBJDIR)/$(ARTIFACT_ID)-ladspa.so
export LIB_LV2          = $(OBJDIR)/$(ARTIFACT_ID)-lv2.so
export LIB_LV2_GTK2UI   = $(OBJDIR)/$(ARTIFACT_ID)-lv2-gtk2.so
export LIB_LV2_GTK3UI   = $(OBJDIR)/$(ARTIFACT_ID)-lv2-gtk3.so
export LIB_VST          = $(OBJDIR)/$(ARTIFACT_ID)-vst-core.so

# Utils
export UTL_GENTTL       = $(OBJDIR)/lv2_genttl.exe
export UTL_VSTMAKE      = $(OBJDIR)/vst_genmake.exe
export UTL_GENPHP       = $(OBJDIR)/gen_php.exe
export UTL_RESGEN       = $(OBJDIR)/gen_resources.exe
export UTL_FILES        = $(UTL_GENTTL) $(UTL_VSTMAKE) $(UTL_GENPHP) $(UTL_RESGEN)

# Compile headers and linkage libraries
export GTK2_HEADERS     = $(shell pkg-config --cflags gtk+-2.0)
export GTK2_LIBS        = $(shell pkg-config --libs gtk+-2.0)
export GTK3_HEADERS     = $(shell pkg-config --cflags gtk+-3.0)
export GTK3_LIBS        = $(shell pkg-config --libs gtk+-3.0)
export EXPAT_HEADERS    = $(shell pkg-config --cflags expat)
export EXPAT_LIBS       = $(shell pkg-config --libs expat)
export SNDFILE_HEADERS  = $(shell pkg-config --cflags sndfile)
export SNDFILE_LIBS     = $(shell pkg-config --libs sndfile)

FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
FILES                   =

.PHONY: all trace debug install uninstall release
.PHONY: install_ladspa install_lv2 install_vst
.PHONY: release_ladspa release_lv2 release_vst

default: all

trace: export CFLAGS        += -DLSP_TRACE
trace: all

debug: export CFLAGS        += -DLSP_DEBUG
debug: all

all:
	@echo "Building binaries"
	@mkdir -p $(OBJDIR)/src
	@$(MAKE) $(MAKE_OPTS) -C src all OBJDIR=$(OBJDIR)/src
	@echo "Build OK"

clean:
	@-rm -rf $(OBJDIR)
	@echo "Clean OK"

unrelease:
	@-rm -rf $(RELEASE)
	@echo "Unrelease OK"

install: $(INSTALLATIONS)

install_php: all
	@echo "Generating PHP file"
	@$(UTL_GENPHP) $(OBJDIR)/plugins.php

install_ladspa: all
	@echo "Installing LADSPA plugins to $(DESTDIR)$(LADSPA_PATH)/"
	@mkdir -p $(DESTDIR)$(LADSPA_PATH)
	@$(INSTALL) $(LIB_LADSPA) $(DESTDIR)$(LADSPA_PATH)/
	
install_lv2: all
	@echo "Installing LV2 plugins to $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2"
	@mkdir -p $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2
	@$(INSTALL) $(LIB_LV2) $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2/
	@$(INSTALL) $(LIB_LV2_GTK2UI) $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2/
	@$(UTL_GENTTL) $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2
	
install_vst: all
	@echo "Installing VST plugins to $(DESTDIR)$(VST_PATH)/"
	@mkdir -p $(DESTDIR)$(VST_PATH)
	@$(INSTALL) $(LIB_VST) $(DESTDIR)$(VST_PATH)/
	@$(INSTALL) $(OBJDIR)/src/vst/*.so $(DESTDIR)$(VST_PATH)/
	@echo "Install OK"

release: INSTALL        += -s
release: LADSPA_ID      := $(ARTIFACT_ID)-ladspa-$(VERSION)-$(CPU_ARCH)
release: LV2_ID         := $(ARTIFACT_ID)-lv2-$(VERSION)-$(CPU_ARCH)
release: VST_ID         := $(ARTIFACT_ID)-lxvst-$(VERSION)-$(CPU_ARCH)
release: $(RELEASES)

release_prepare: all
	@echo "Releasing plugins for architecture $(CPU_ARCH)"
	@mkdir -p $(RELEASE)
	
release_ladspa: release_prepare
	@echo "Releasing LADSPA binaries"
	@mkdir -p $(RELEASE)/$(LADSPA_ID)
	@$(INSTALL) $(LIB_LADSPA) $(RELEASE)/$(LADSPA_ID)/
	@cp $(RELEASE_TEXT) $(RELEASE)/$(LADSPA_ID)/
	@tar -C $(RELEASE) -czf $(RELEASE)/$(LADSPA_ID).tar.gz $(LADSPA_ID)
	@rm -rf $(RELEASE)/$(LADSPA_ID)
	
release_lv2: release_prepare
	@echo "Releasing LV2 binaries"
	@mkdir -p $(RELEASE)/$(LV2_ID)
	@mkdir -p $(RELEASE)/$(LV2_ID)/$(ARTIFACT_ID).lv2
	@$(INSTALL) $(LIB_LV2) $(RELEASE)/$(LV2_ID)/$(ARTIFACT_ID).lv2/
	@$(INSTALL) $(LIB_LV2_GTK2UI) $(RELEASE)/$(LV2_ID)/$(ARTIFACT_ID).lv2/
	@cp $(RELEASE_TEXT) $(RELEASE)/$(LV2_ID)/
	@$(UTL_GENTTL) $(RELEASE)/$(LV2_ID)/$(ARTIFACT_ID).lv2
	@tar -C $(RELEASE) -czf $(RELEASE)/$(LV2_ID).tar.gz $(LV2_ID)
	@rm -rf $(RELEASE)/$(LV2_ID)
	
release_vst: release_prepare
	@echo "Releasing VST binaries"
	@mkdir -p $(RELEASE)/$(VST_ID)
	@$(INSTALL) $(LIB_VST) $(RELEASE)/$(VST_ID)/
	@$(INSTALL) $(OBJDIR)/src/vst/*.so $(RELEASE)/$(VST_ID)/
	@cp $(RELEASE_TEXT) $(RELEASE)/$(VST_ID)/
	@tar -C $(RELEASE) -czf $(RELEASE)/$(VST_ID).tar.gz $(VST_ID)
	@rm -rf $(RELEASE)/$(VST_ID)
	@echo "Release OK"

uninstall:
	@-rm $(DESTDIR)$(LADSPA_PATH)/$(ARTIFACT_ID)-ladspa.so
	@-rm -rf $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2
	@-rm -f $(DESTDIR)$(VST_PATH)/$(ARTIFACT_ID)-vst-*.so
	@echo "Uninstall OK"
