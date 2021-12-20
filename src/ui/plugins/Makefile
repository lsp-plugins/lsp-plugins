# Command-line flag to silence nested $(MAKE).
MAKE_OPTS              += VERBOSE=$(VERBOSE)
ifneq ($(VERBOSE),1)
.SILENT:
endif

FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
SUBDIRS					= 

.PHONY: all

$(FILES):
	@echo "  $(CXX) $(FILE)"
	$(CXX) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE)  

$(OBJ_PLUGIN_UIS): $(FILES)
	@echo "  $(LD) $(notdir $(@))"
	$(LD) -o $(OBJ_PLUGIN_UIS) $(MERGE_FLAGS) $(PACKAGES) $(FILES)

all: $(OBJ_PLUGIN_UIS) $(SUBDIRS)
