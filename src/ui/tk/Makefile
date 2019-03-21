rwildcard				= $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(call rwildcard, , *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)

.PHONY: all

$(FILES):
	@echo "  $(CXX) $(FILE)"
	@mkdir -p $(dir $@)
	@$(CXX) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE)

$(OBJ_TK_CORE): $(FILES)
	@echo "  $(LD) $(notdir $(@))"
	@$(LD) -o $(OBJ_TK_CORE) $(MERGE_FLAGS) $(PACKAGES) $(FILES)

all: $(OBJ_TK_CORE)
