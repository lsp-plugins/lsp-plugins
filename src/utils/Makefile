MODULES                 = $(UTL_GENTTL) $(UTL_GENPHP) $(UTL_RESGEN) $(UTL_JACKMAKE) $(UTL_VSTMAKE)
FILELIST                = gen_php.o lv2_genttl.o resource_gen.o jack_genmake.o vst_genmake.o
FILES                   = $(addprefix $(OBJDIR)/, $(FILELIST))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
SUBDIRS                 =

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
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(FILE) -o $(@)

# Rules for each utility
$(UTL_GENTTL): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_GENTTL))"
	@$(CC) $(EXE_FLAGS) $(SNDFILE_LIBS) -o $(UTL_GENTTL) $(OBJDIR)/lv2_genttl.o $(OBJ_CORE) $(OBJ_METADATA)
	
$(UTL_JACKMAKE): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_JACKMAKE))"
	@$(CC) $(EXE_FLAGS) $(SNDFILE_LIBS) -o $(UTL_JACKMAKE) $(OBJDIR)/jack_genmake.o $(OBJ_CORE) $(OBJ_METADATA)
	
$(UTL_VSTMAKE): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_VSTMAKE))"
	@$(CC) $(EXE_FLAGS) $(SNDFILE_LIBS) -o $(UTL_VSTMAKE) $(OBJDIR)/vst_genmake.o $(OBJ_CORE) $(OBJ_METADATA)
	
$(UTL_GENPHP): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_GENPHP))"
	@$(CC) $(EXE_FLAGS) $(SNDFILE_LIBS) -o $(UTL_GENPHP) $(OBJDIR)/gen_php.o $(OBJ_CORE) $(OBJ_METADATA)
	
$(UTL_RESGEN): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_RESGEN))"
	@$(CC) $(EXE_FLAGS) $(SNDFILE_LIBS) $(EXPAT_HEADERS) $(EXPAT_LIBS) -o $(UTL_RESGEN) $(OBJDIR)/resource_gen.o $(OBJ_CORE) $(OBJ_METADATA)
