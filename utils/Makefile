FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
SUBDIRS                 =

CFLAGS                 += -DLV2_GENTTL -DPLUGIN_GENPHP

.PHONY: all target files subdirs $(SUBDIRS)

all: $(UTL_GENTTL) $(UTL_GENPHP)

target: all

$(UTL_GENTTL): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_GENTTL))"
	@$(CC) $(EXE_FLAGS) -o $(UTL_GENTTL) $(OBJDIR)/lv2_genttl.o $(OBJ_CORE)
	
$(UTL_GENPHP): $(FILES) $(SUBDIRS)
	@echo "  $(CC) $(notdir $(UTL_GENPHP))"
	@$(CC) $(EXE_FLAGS) -o $(UTL_GENPHP) $(OBJDIR)/gen_php.o $(OBJ_CORE)

$(SUBDIRS):
	@echo "Building $@"
	@mkdir -p $(OBJDIR)/$(@)
	@$(MAKE) $(MAKE_OPTS) -C $@ $(MAKECMDGOALS) OBJDIR=$(OBJDIR)/$(@)

$(FILES):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(FILE) -o $(@)
