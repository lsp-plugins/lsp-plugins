SUBDIRS                 = core metadata utils 
MODULES                 = $(SUBDIRS)
CONTAINER_DEPS          = $(OBJ_METADATA) $(OBJ_CORE) $(UTL_FILES)
UTL_DEPS                = $(OBJ_CORE) $(OBJ_DSP) $(OBJ_METADATA)

NEED_UI                 = 0
NEED_PLUGINS            = 0

RESOURCE_PATHS = \
  $(BASEDIR)/res/ui \
  $(BASEDIR)/res/presets \
  $(BASEDIR)/res/models \
  $(BASEDIR)/res/i18n

ifeq ($(LSP_TESTING),1)
  SUBDIRS                += test testing
  CONTAINER_DEPS         += $(OBJ_TEST_CORE) $(OBJ_TESTING_CORE)
  UTL_DEPS               += $(OBJ_TEST_CORE)
  NEED_UI                 = 1
  NEED_PLUGINS            = 1
endif

ifneq ($(LSP_TESTING),1)
  ifeq ($(findstring ladspa,$(BUILD_MODULES)),ladspa)
    NEED_PLUGINS            = 1
  endif
  ifeq ($(findstring lv2,$(BUILD_MODULES)),lv2)
    ifeq ($(LV2_UI),1)
      NEED_UI                 = 1
    endif
    NEED_PLUGINS            = 1
  endif
  ifeq ($(findstring vst,$(BUILD_MODULES)),vst)
    MODULES                += vst_stub
    ifeq ($(VST_UI),1)
      NEED_UI                 = 1
    endif
    NEED_PLUGINS            = 1
  endif
  ifeq ($(findstring jack,$(BUILD_MODULES)),jack)
    MODULES                += jack_stub
    NEED_UI                 = 1
    NEED_PLUGINS            = 1
  endif
  ifeq ($(findstring doc,$(BUILD_MODULES)),doc)
    SUBDIRS                += doc
    MODULES                += doc
  endif
endif

ifeq ($(NEED_PLUGINS), 1)
  SUBDIRS                += dsp plugins container
  CONTAINER_DEPS         += $(OBJ_DSP) $(OBJ_PLUGINS)
endif
ifeq ($(NEED_UI),1)
  SUBDIRS                += ui rendering
  MODULES                += ui
  CONTAINER_DEPS         += $(OBJ_UI_CORE) $(OBJ_RES_CORE) rendering
endif

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

$(UTL_RESGEN): utils

$(UTL_JACKMAKE): utils

$(OBJ_TEST_CORE): test

$(OBJ_TESTING_CORE): test testing rendering

$(OBJ_RES_CORE): $(OBJ_CORE) $(UTL_RESGEN)
	@echo "Generating builtin resources"
	@mkdir -p $(OBJDIR)/res
	@$(UTL_RESGEN) $(OBJDIR)/res/builtin_resource.cpp $(RESOURCE_PATHS)
	@-rm gmon.out >/dev/null 2>&1
	@$(CXX) -o $(OBJ_RES_CORE) -c $(OBJDIR)/res/builtin_resource.cpp -fPIC $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE) 

# Target dependencies
container: $(CONTAINER_DEPS)

utils: $(UTL_DEPS)

doc: utils $(PHP_PLUGINS)

# Additional processing
$(PHP_PLUGINS): $(UTL_GENPHP)
	@echo "Generating PHP file $(notdir $(PHP_PLUGINS))"
	@$(UTL_GENPHP) $(PHP_PLUGINS)

vst_stub: $(UTL_VSTMAKE)
	@echo "Building VST stub"
	@mkdir -p $(OBJDIR)/vst
	@$(UTL_VSTMAKE) $(OBJDIR)/vst
	@-rm gmon.out >/dev/null 2>&1
	@$(MAKE) $(MAKE_OPTS) -C $(OBJDIR)/vst all OBJDIR=$(OBJDIR)/vst

jack_stub: $(UTL_JACKMAKE)
	@echo "Building JACK stub"
	@mkdir -p $(OBJDIR)/jack
	@$(UTL_JACKMAKE) $(OBJDIR)/jack
	@-rm gmon.out >/dev/null 2>&1
	@$(MAKE) $(MAKE_OPTS) -C $(OBJDIR)/jack all OBJDIR=$(OBJDIR)/jack
