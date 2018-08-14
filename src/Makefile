SUBDIRS                 = core dsp metadata plugins ui utils container doc
MODULES                 = $(SUBDIRS) jack_stub vst_stub

.PHONY: all $(MODULES)

all: $(MODULES)

target: all

# Rules
$(SUBDIRS):
	@echo "Building $(@)"
	@mkdir -p $(OBJDIR)/$(@)
	@$(MAKE) $(MAKE_OPTS) -C $@ $(MAKECMDGOALS) OBJDIR=$(OBJDIR)/$(@)

# Object dependencies
$(OBJ_CORE): core

$(OBJ_DSP): dsp

$(OBJ_PLUGINS): plugins

$(OBJ_METADATA): metadata

$(OBJ_UI_CORE): ui

$(UTL_FILES): utils

$(UTL_JACKMAKE) : utils

$(OBJ_RES_CORE): $(OBJ_CORE) $(UTL_RESGEN)
	@echo "Generating XML resources"
	@mkdir -p $(OBJDIR)/res
	@$(UTL_RESGEN) $(BASEDIR)/res/ui $(OBJDIR)/res/xml_resource.cpp
	@-rm gmon.out &>/dev/null 2>&1
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(OBJDIR)/res/xml_resource.cpp -o $(OBJ_RES_CORE)

# Target dependencies
container: $(OBJ_CORE) $(OBJ_DSP) $(OBJ_METADATA) $(OBJ_UI_CORE) $(OBJ_RES_CORE) $(UTL_FILES) $(OBJ_PLUGINS)

utils: $(OBJ_CORE) $(OBJ_DSP) $(OBJ_METADATA)

doc: utils $(PHP_PLUGINS)

# Additional processing
$(PHP_PLUGINS): $(UTL_GENPHP)
	@echo "Generating PHP file $(notdir $(PHP_PLUGINS))"
	@$(UTL_GENPHP) $(PHP_PLUGINS)

vst_stub: $(UTL_VSTMAKE)
	@echo "Building VST stub"
	@mkdir -p $(OBJDIR)/vst
	@$(UTL_VSTMAKE) $(OBJDIR)/vst
	@-rm gmon.out &>/dev/null 2>&1
	@$(MAKE) $(MAKE_OPTS) -C $(OBJDIR)/vst all OBJDIR=$(OBJDIR)/vst

jack_stub: $(UTL_JACKMAKE)
	@echo "Building JACK stub"
	@mkdir -p $(OBJDIR)/jack
	@$(UTL_JACKMAKE) $(OBJDIR)/jack
	@-rm gmon.out &>/dev/null 2>&1
	@$(MAKE) $(MAKE_OPTS) -C $(OBJDIR)/jack all OBJDIR=$(OBJDIR)/jack
