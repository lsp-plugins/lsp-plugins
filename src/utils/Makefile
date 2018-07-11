FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
SUBDIRS                 =

.PHONY: all target files subdirs $(SUBDIRS)

all: $(UTL_GENTTL) $(UTL_VSTMAKE) $(UTL_GENPHP) $(UTL_RESGEN)

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
	@$(CC) $(EXE_FLAGS) -o $(UTL_GENTTL) $(OBJDIR)/lv2_genttl.o $(OBJ_CORE)
	
$(UTL_VSTMAKE): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_VSTMAKE))"
	@$(CC) $(EXE_FLAGS) -o $(UTL_VSTMAKE) $(OBJDIR)/vst_genmake.o $(OBJ_CORE)
	
$(UTL_GENPHP): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_GENPHP))"
	@$(CC) $(EXE_FLAGS) -o $(UTL_GENPHP) $(OBJDIR)/gen_php.o $(OBJ_CORE)
	
$(UTL_RESGEN): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_RESGEN))"
	@$(CC) $(EXE_FLAGS) $(EXPAT_HEADERS) $(EXPAT_LIBS) -o $(UTL_RESGEN) $(OBJDIR)/resource_gen.o $(OBJ_CORE)



	