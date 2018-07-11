FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
SUBDIRS					= x11

.PHONY: all $(SUBDIRS)

all: $(OBJ_WS_CORE)

$(SUBDIRS):
	@echo "Building $@"
	@mkdir -p $(OBJDIR)/$(@)
	@$(MAKE) $(MAKE_OPTS) -C $@ $(MAKECMDGOALS) OBJDIR=$(OBJDIR)/$(@)

$(FILES):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(FILE) -o $(@)

$(OBJ_WS_CORE): $(FILES) $(OBJ_WS_X11_CORE)
	@echo "  $(LD) $(notdir $(@))"
	@$(LD) $(MERGE_FLAGS) $(PACKAGES) -o $(OBJ_WS_CORE) $(FILES) $(OBJ_WS_X11_CORE)

$(OBJ_WS_X11_CORE): $(SUBDIRS)
