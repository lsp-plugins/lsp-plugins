FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
INCLUDE                += $(XLIB_HEADERS) $(CAIRO_HEADERS)

.PHONY: all

all: $(OBJ_WS_X11_CORE)

$(OBJ_WS_X11_CORE): $(FILES)
	@echo "  $(LD) $(notdir $(@))"
	@$(LD) -o $(OBJ_WS_X11_CORE) $(MERGE_FLAGS) -r $(FILES)

$(FILES):
	@echo "  $(CXX) $(FILE)"
	@$(CXX) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE) 

