rwildcard				= $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(call rwildcard, , *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
INCLUDE                += $(SNDFILE_HEADERS)
LIBS                    = $(PTHREAD_LIBS)

.PHONY: all

all: $(OBJ_CORE)

$(FILES):
	@echo "  $(CXX) $(FILE)"
	@mkdir -p $(dir $@)
	@$(CXX) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CXXFLAGS) $(LIBS) $(INCLUDE) 

$(OBJ_CORE): $(FILES)
	@echo "  $(LD) $(notdir $(OBJ_CORE))"
	@$(LD) -o $(OBJ_CORE) -r $(MERGE_FLAGS) $(FILES)
