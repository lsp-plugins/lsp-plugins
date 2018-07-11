LADSPA_PATH             = /usr/local/lib/ladspa
LV2_PATH                = /usr/local/lib/lv2
ARTIFACT_ID             = lsp-plugins
OBJDIR                  = ${CURDIR}/.build
RELEASE                 = ${CURDIR}/.release

# Flags
ifndef CPU_ARCH
export CPU_ARCH         = x86_64
export CC_ARCH          = -m64
export LD_ARCH          = -m elf_x86_64
endif

export VERSION          = 1.0.0
export INCLUDE          = -I${CURDIR}/include
export MAKE_OPTS        = -s
export CFLAGS           = $(CC_ARCH) -fPIC -O2 -fno-exceptions -Wall
export CC               = g++
export LD               = ld
export LDFLAGS          = $(LD_ARCH)
export SO_FLAGS         = $(CC_ARCH) -shared -Llibrary -lc -lm -fPIC
export MERGE_FLAGS      = $(LD_ARCH) -r
export EXE_FLAGS        = $(CC_ARCH) -lc -lm -fPIC

# Objects
export OBJ_CORE         = $(OBJDIR)/core.o
export OBJ_PLUGINS      = $(OBJDIR)/plugins.o
export OBJ_UI_CORE      = $(OBJDIR)/ui_core.o
export OBJ_GTK2UI       = $(OBJDIR)/ui_gtk2.o
export OBJ_GTK3UI       = $(OBJDIR)/ui_gtk3.o

# Libraries
export LIB_LADSPA       = $(OBJDIR)/ladspa.so
export LIB_LV2          = $(OBJDIR)/lv2.so
export LIB_LV2_GTK2UI   = $(OBJDIR)/lv2_gtk2.so
export LIB_LV2_GTK3UI   = $(OBJDIR)/lv2_gtk3.so

# Utils
export UTL_GENTTL       = $(OBJDIR)/lv2_genttl.exe
export UTL_GENPHP       = $(OBJDIR)/gen_php.exe

# Compile headers and linkage libraries
export GTK2_HEADERS     = $(shell pkg-config --cflags gtk+-2.0)
export GTK2_LIBS        = $(shell pkg-config --libs gtk+-2.0)
export GTK3_HEADERS     = $(shell pkg-config --cflags gtk+-3.0)
export GTK3_LIBS        = $(shell pkg-config --libs gtk+-3.0)
export EXPAT_HEADERS    = $(shell pkg-config --cflags expat)
export EXPAT_LIBS       = $(shell pkg-config --libs expat)

FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
FILES                   =

.PHONY: all trace debug binaries install uninstall sources utils

default: all

$(OBJ_CORE) $(LIB_LADSPA) $(LIB_LV2) $(LIB_LV2_GTK2UI) $(LIB_LV2_GTK3UI): binaries

$(UTL_GENTTL) $(UTL_GENPHP): utils

all: binaries utils
	@echo "Build OK"

trace: export CFLAGS        += -DLSP_TRACE
trace: all

debug: export CFLAGS        += -DLSP_DEBUG
debug: all

binaries:
	@echo "Building src"
	@mkdir -p $(OBJDIR)/src
	@$(MAKE) $(MAKE_OPTS) -C src all OBJDIR=$(OBJDIR)/src

utils: $(OBJ_CORE)
	@echo "Building utils"
	@mkdir -p $(OBJDIR)/utils
	@$(MAKE) $(MAKE_OPTS) -C utils all OBJDIR=$(OBJDIR)/utils
	@$(UTL_GENPHP) $(OBJDIR)/plugins.php

clean:
	@-rm -rf $(OBJDIR)
	@echo "Clean OK"

unrelease:
	@-rm -rf $(RELEASE)
	@echo "Unrelease OK"

install: all
	@echo "Installing LADSPA plugins to $(DESTDIR)$(LADSPA_PATH)/$(ARTIFACT_ID).so"
	@mkdir -p $(DESTDIR)$(LADSPA_PATH)
	@install -s $(LIB_LADSPA) $(DESTDIR)$(LADSPA_PATH)/$(ARTIFACT_ID).so
	@echo "Installing LV2 plugins to $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2/$(ARTIFACT_ID).so"
	@mkdir -p $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2
	@install -s $(LIB_LV2) $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2/$(ARTIFACT_ID).so
	@install -s $(LIB_LV2_GTK2UI) $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2/$(ARTIFACT_ID)-gtk2.so
	@echo "Copying resources to $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2"
	@cp -r res/ui $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2
	@$(UTL_GENTTL) $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2
	@echo "Install OK"

release: LADSPA_ID      := $(ARTIFACT_ID)-ladspa-$(VERSION)-$(CPU_ARCH)
release: LV2_ID         := $(ARTIFACT_ID)-lv2-$(VERSION)-$(CPU_ARCH)
release: all
	@echo "Releasing plugins for architecture $(CPU_ARCH)"
	@mkdir -p $(RELEASE)
	@echo "Releasing LADSPA binaries"
	@mkdir -p $(RELEASE)/$(LADSPA_ID)
	@install -s $(LIB_LADSPA) $(RELEASE)/$(LADSPA_ID)/$(ARTIFACT_ID).so
	@cp res/LICENSE-LSP.txt $(RELEASE)/$(LADSPA_ID)/LICENSE.txt
	@cp res/CHANGELOG.txt $(RELEASE)/$(LADSPA_ID)/CHANGELOG.txt
	@tar -C $(RELEASE) -czf $(RELEASE)/$(LADSPA_ID).tar.gz $(LADSPA_ID)
	@md5sum $(RELEASE)/$(LADSPA_ID).tar.gz | sed -r "s/ .*\/(.+)/  \1/g" > $(RELEASE)/$(LADSPA_ID).md5
	@rm -rf $(RELEASE)/$(LADSPA_ID)
	@echo "Releasing LV2 binaries"
	@mkdir -p $(RELEASE)/$(LV2_ID)
	@mkdir -p $(RELEASE)/$(LV2_ID)/$(ARTIFACT_ID).lv2
	@install -s $(LIB_LV2) $(RELEASE)/$(LV2_ID)/$(ARTIFACT_ID).lv2/$(ARTIFACT_ID).so
	@install -s $(LIB_LV2_GTK2UI) $(RELEASE)/$(LV2_ID)/$(ARTIFACT_ID).lv2/$(ARTIFACT_ID)-gtk2.so
	@cp -r res/ui $(RELEASE)/$(LV2_ID)/$(ARTIFACT_ID).lv2
	@cp res/LICENSE-LSP.txt $(RELEASE)/$(LV2_ID)/LICENSE.txt
	@cp res/CHANGELOG.txt $(RELEASE)/$(LV2_ID)/CHANGELOG.txt
	@$(UTL_GENTTL) $(RELEASE)/$(LV2_ID)/$(ARTIFACT_ID).lv2
	@tar -C $(RELEASE) -czf $(RELEASE)/$(LV2_ID).tar.gz $(LV2_ID)
	@md5sum $(RELEASE)/$(LV2_ID).tar.gz | sed -r "s/ .*\/(.+)/  \1/g" > $(RELEASE)/$(LV2_ID).md5
	@rm -rf $(RELEASE)/$(LV2_ID)

uninstall:
	@-rm $(DESTDIR)$(LADSPA_PATH)/$(ARTIFACT_ID).so
	@-rm -rf $(DESTDIR)$(LV2_PATH)/$(ARTIFACT_ID).lv2
	@echo "Uninstall OK"
