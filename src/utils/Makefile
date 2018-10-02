MODULES                 = $(UTL_GENTTL) $(UTL_GENPHP) $(UTL_RESGEN) $(UTL_JACKMAKE) $(UTL_VSTMAKE)
FILELIST                = gen_php.o lv2_genttl.o resource_gen.o jack_genmake.o vst_genmake.o
FILES                   = $(addprefix $(OBJDIR)/, $(FILELIST))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
SUBDIRS                 =
OBJ_FILES               = $(OBJ_CORE) $(OBJ_METADATA)
OBJ_LIBS                = $(PTHREAD_LIBS) $(MATH_LIBS)

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
