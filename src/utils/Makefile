MODULES                 = $(UTL_RESGEN) 
FILELIST                = resource_gen.o 
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
SUBDIRS                 =
OBJ_FILES               = $(OBJ_CORE) $(OBJ_METADATA)
OBJ_LIBS                = $(PTHREAD_LIBS) $(MATH_LIBS)

ifeq ($(findstring lv2,$(BUILD_MODULES)),lv2)
  MODULES                += $(UTL_GENTTL)
  FILELIST               += lv2_genttl.o
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
	@mkdir -p $(OBJDIR)/$(@)
	@$(MAKE) $(MAKE_OPTS) -C $@ $(MAKECMDGOALS) OBJDIR=$(OBJDIR)/$(@)

$(FILES):
	@echo "  $(CC) $(FILE)"
	@$(CC) -o $(@) -c $(FILE) $(CPPFLAGS) -fPIC $(CFLAGS) $(INCLUDE)

# Rules for each utility
$(UTL_GENTTL): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_GENTTL))"
	@$(CC) -o $(UTL_GENTTL) $(OBJDIR)/lv2_genttl.o $(OBJ_FILES) $(EXE_FLAGS) $(SNDFILE_LIBS) $(OBJ_LIBS)
	
$(UTL_JACKMAKE): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_JACKMAKE))"
	@$(CC) -o $(UTL_JACKMAKE) $(OBJDIR)/jack_genmake.o $(OBJ_FILES) $(EXE_FLAGS) $(SNDFILE_LIBS) $(OBJ_LIBS)
	
$(UTL_VSTMAKE): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_VSTMAKE))"
	@$(CC) -o $(UTL_VSTMAKE) $(OBJDIR)/vst_genmake.o $(OBJ_FILES) $(EXE_FLAGS) $(OBJ_LIBS)
	
$(UTL_GENPHP): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_GENPHP))"
	@$(CC) -o $(UTL_GENPHP) $(OBJDIR)/gen_php.o $(OBJ_FILES) $(EXE_FLAGS) $(SNDFILE_LIBS) $(OBJ_LIBS)
	
$(UTL_RESGEN): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_RESGEN))"
	@$(CC) -o $(UTL_RESGEN) $(OBJDIR)/resource_gen.o $(OBJ_FILES) $(EXPAT_HEADERS) $(EXE_FLAGS) $(EXPAT_LIBS) $(SNDFILE_LIBS) $(OBJ_LIBS)
