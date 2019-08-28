rwildcard				= $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(call rwildcard, , *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)

.PHONY: all

all: $(OBJ_TESTING_CORE)

$(FILES):
	@echo "  $(CXX) $(FILE)"
	@mkdir -p $(dir $@)
	@$(CXX) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE) $(OPENGL_HEADERS) 

$(OBJ_TESTING_CORE): $(FILES)
	@echo "  $(LD) $(notdir $(OBJ_TESTING_CORE))"
	@$(LD) -o $(OBJ_TESTING_CORE) -r $(MERGE_FLAGS) $(FILES)  

