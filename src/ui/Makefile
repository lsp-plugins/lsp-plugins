FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
rwildcard               = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
SUBDIRS					= gtk2

.PHONY: all $(SUBDIRS)

$(SUBDIRS):
	@echo "Building $@"
	@mkdir -p $(OBJDIR)/$(@)
	@$(MAKE) $(MAKE_OPTS) -C $@ $(MAKECMDGOALS) OBJDIR=$(OBJDIR)/$(@)

$(FILES):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(EXPAT_HEADERS) $(FILE) -o $(@)

$(OBJ_UI_CORE): $(FILES)
	@echo "  $(LD) $(notdir $(@))"
	@$(LD) $(MERGE_FLAGS) $(PACKAGES) -o $(OBJ_UI_CORE) $(FILES)

all: $(OBJ_UI_CORE) $(SUBDIRS)