FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
SUBDIRS					= 

.PHONY: all

$(FILES):
	@echo "  $(CC) $(FILE)"
	@$(CC) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CFLAGS) $(INCLUDE)  

$(OBJ_CTL_CORE): $(FILES)
	@echo "  $(LD) $(notdir $(@))"
	@$(LD) -o $(OBJ_CTL_CORE) $(MERGE_FLAGS) $(PACKAGES) $(FILES)

all: $(OBJ_CTL_CORE) $(SUBDIRS)
