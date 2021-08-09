# Command-line flag to silence nested $(MAKE).
MAKE_OPTS              += VERBOSE=$(VERBOSE)
ifneq ($(VERBOSE),1)
.SILENT:
endif

CXX                     = $(HOST_CXX)
CXXFLAGS                = $(HOST_CXXFLAGS)
MAKE_OPTS              += CXX=$(HOST_CXX) CXXFLAGS=$(HOST_CXXFLAGS)

MODULES                 = $(UTL_RESGEN) 
FILELIST                = resource_gen.o 
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
SUBDIRS                 =
OBJ_FILES               = $(HOST_OBJ_CORE) $(HOST_OBJ_METADATA) $(HOST_OBJ_DSP)
OBJ_LIBS                = $(PTHREAD_LIBS) $(MATH_LIBS)
OBJ_EXTRA               =
INCLUDE_EXTRA           =

export PKG_CONFIG_SYSROOT_DIR = /

ifeq ($(BUILD_PLATFORM),BSD)
  OBJ_LIBS               += $(ICONV_LIBS)
endif

ifeq ($(LSP_TESTING),1)
  OBJ_EXTRA              += $(OBJ_TEST_CORE)
endif

ifeq ($(findstring lv2,$(BUILD_MODULES)),lv2)
  MODULES                += $(UTL_GENTTL)
  FILELIST               += lv2_genttl.o
  INCLUDE_EXTRA          += $(HOST_LV2_HEADERS)
endif
ifeq ($(findstring vst,$(BUILD_MODULES)),vst)
  MODULES                += $(UTL_VSTMAKE)
  FILELIST               += vst_genmake.o
endif
ifeq ($(findstring jack,$(BUILD_MODULES)),jack)
  MODULES                += $(UTL_JACKMAKE)
  FILELIST               += jack_genmake.o
endif
ifeq ($(findstring doc,$(BUILD_MODULES)),doc)
  MODULES                += $(UTL_GENPHP)
  FILELIST               += gen_php.o
endif

FILES                   = $(addprefix $(OBJDIR)/, $(FILELIST))

.PHONY: all target files subdirs $(SUBDIRS)

all: $(MODULES)

target: all

# Common rules
$(SUBDIRS):
	@echo "Building $@"
	mkdir -p $(OBJDIR)/$(@)
	$(MAKE) $(MAKE_OPTS) -C $@ $(MAKECMDGOALS) OBJDIR=$(OBJDIR)/$(@)

$(FILES):
	@echo "  $(HOST_CXX) $(FILE)"
	$(HOST_CXX) -o $(@) -c $(FILE) $(CPPFLAGS) -fPIC $(HOST_CXXFLAGS) $(INCLUDE) $(INCLUDE_EXTRA)

# Rules for each utility
$(UTL_GENTTL): $(OBJ_FILES) $(FILES) $(SUBDIRS)
	@echo "  $(HOST_CXX) $(notdir $(UTL_GENTTL))"
	$(HOST_CXX) -o $(UTL_GENTTL) $(OBJDIR)/lv2_genttl.o $(OBJ_FILES) $(HOST_EXE_FLAGS) $(HOST_SNDFILE_LIBS) $(DL_LIBS) $(OBJ_LIBS) $(LV2_LIBS)
	
$(UTL_JACKMAKE): $(OBJ_FILES) $(FILES) $(SUBDIRS)
	@echo "  $(HOST_CXX) $(notdir $(UTL_JACKMAKE))"
	$(HOST_CXX) -o $(UTL_JACKMAKE) $(OBJDIR)/jack_genmake.o $(OBJ_FILES) $(HOST_EXE_FLAGS) $(HOST_SNDFILE_LIBS) $(DL_LIBS) $(OBJ_LIBS)
	
$(UTL_VSTMAKE): $(OBJ_FILES) $(FILES) $(SUBDIRS)
	@echo "  $(HOST_CXX) $(notdir $(UTL_VSTMAKE))"
	$(HOST_CXX) -o $(UTL_VSTMAKE) $(OBJDIR)/vst_genmake.o $(OBJ_FILES) $(HOST_EXE_FLAGS) $(HOST_SNDFILE_LIBS) $(DL_LIBS) $(OBJ_LIBS)
	
$(UTL_GENPHP): $(OBJ_FILES) $(FILES) $(SUBDIRS)
	@echo "  $(HOST_CXX) $(notdir $(UTL_GENPHP))"
	$(HOST_CXX) -o $(UTL_GENPHP) $(OBJDIR)/gen_php.o $(OBJ_FILES) $(HOST_EXE_FLAGS) $(HOST_SNDFILE_LIBS) $(DL_LIBS) $(OBJ_LIBS)
	
$(UTL_RESGEN): $(OBJ_FILES) $(FILES) $(SUBDIRS)
	@echo "  $(HOST_CXX) $(notdir $(UTL_RESGEN))"
	$(HOST_CXX) -o $(UTL_RESGEN) $(OBJDIR)/resource_gen.o $(OBJ_FILES) $(OBJ_EXTRA) $(HOST_EXE_FLAGS) $(HOST_SNDFILE_LIBS) $(DL_LIBS) $(OBJ_LIBS)
