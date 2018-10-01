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
	@$(CC) -o $(@) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(FILE)

# Rules for each utility
$(UTL_GENTTL): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_GENTTL))"
	@$(CC) -o $(UTL_GENTTL) $(EXE_FLAGS) $(OBJDIR)/lv2_genttl.o $(OBJ_CORE) $(OBJ_METADATA) $(SNDFILE_LIBS) 
	
$(UTL_JACKMAKE): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_JACKMAKE))"
	@$(CC) -o $(UTL_JACKMAKE) $(EXE_FLAGS) $(OBJDIR)/jack_genmake.o $(OBJ_CORE) $(OBJ_METADATA) $(SNDFILE_LIBS) 
	
$(UTL_VSTMAKE): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_VSTMAKE))"
	@$(CC) -o $(UTL_VSTMAKE) $(EXE_FLAGS) $(OBJDIR)/vst_genmake.o $(OBJ_CORE) $(OBJ_METADATA) $(SNDFILE_LIBS) 
	
$(UTL_GENPHP): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_GENPHP))"
	@$(CC) -o $(UTL_GENPHP) $(EXE_FLAGS) $(OBJDIR)/gen_php.o $(OBJ_CORE) $(OBJ_METADATA) $(SNDFILE_LIBS)
	
$(UTL_RESGEN): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_RESGEN))"
	@$(CC) -o $(UTL_RESGEN) $(EXE_FLAGS) $(OBJDIR)/resource_gen.o $(OBJ_CORE) $(OBJ_METADATA) $(EXPAT_HEADERS) $(EXPAT_LIBS) $(SNDFILE_LIBS) 
