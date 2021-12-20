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

$(OBJ_CTL_CORE): $(FILES)
	@echo "  $(LD) $(notdir $(@))"
	$(LD) -o $(OBJ_CTL_CORE) $(MERGE_FLAGS) $(PACKAGES) $(FILES)

all: $(OBJ_CTL_CORE) $(SUBDIRS)
