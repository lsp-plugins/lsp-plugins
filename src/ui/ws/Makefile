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
	@$(CC) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CFLAGS) $(INCLUDE) 

$(OBJ_WS_CORE): $(FILES) $(OBJ_WS_X11_CORE)
	@echo "  $(LD) $(notdir $(@))"
	@$(LD) -o $(OBJ_WS_CORE) $(MERGE_FLAGS) $(PACKAGES) $(OBJ_WS_X11_CORE) $(FILES)

$(OBJ_WS_X11_CORE): $(SUBDIRS)
