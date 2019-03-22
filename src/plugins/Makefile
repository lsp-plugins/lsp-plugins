rwildcard               = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)

.PHONY: all files subdirs $(SUBDIRS)

all:  $(OBJ_PLUGINS)

$(OBJ_PLUGINS): $(FILES) $(SUBDIRS)
	@echo "  $(LD) $(notdir $(OBJ_PLUGINS))"
	@$(LD) -o $(OBJ_PLUGINS) -r $(MERGE_FLAGS) $(FILES) 

$(FILES):
	@echo "  $(CXX) $(FILE)"
	@$(CXX) -o $(@) -c -fPIC $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE) $(FILE)
