FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
SUBDIRS					= ws tk ctl
INCLUDE                += $(CAIRO_HEADERS) $(XLIB_HEADERS)
PACKAGES                = $(OBJ_WS_CORE) $(OBJ_TK_CORE) $(OBJ_CTL_CORE)

.PHONY: all $(SUBDIRS)

$(SUBDIRS):
	@echo "Building $@"
	@mkdir -p $(OBJDIR)/$(@)
	@$(MAKE) $(MAKE_OPTS) -C $@ $(MAKECMDGOALS) OBJDIR=$(OBJDIR)/$(@)

$(FILES):
	@echo "  $(CC) $(FILE)"
	@$(CC) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CFLAGS) $(INCLUDE)

$(OBJ_UI_CORE): $(FILES) $(SUBDIRS)
	@echo "  $(LD) $(notdir $(@))"
	@$(LD) -o $(OBJ_UI_CORE) $(MERGE_FLAGS) $(PACKAGES) $(FILES)

all: $(OBJ_UI_CORE)
