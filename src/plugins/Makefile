FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
SUBDIRS                 =
rwildcard               = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

.PHONY: all files subdirs $(SUBDIRS)

all:  $(OBJ_CORE) $(OBJ_PLUGINS)

$(OBJ_PLUGINS): $(FILES) $(SUBDIRS)
	@echo "  $(LD) $(notdir $(OBJ_PLUGINS))"
	@$(LD) $(LDFLAGS) -r -o $(OBJ_PLUGINS) $(call rwildcard,$(OBJDIR),*.o)

$(SUBDIRS):
	@echo "Building $@"
	@mkdir -p $(OBJDIR)/$(@)
	@$(MAKE) $(MAKE_OPTS) -C $@ $(MAKECMDGOALS) OBJDIR=$(OBJDIR)/$(@)

$(FILES):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(FILE) -o $(@)
