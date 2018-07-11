FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
SUBDIRS					= 

.PHONY: all

$(FILES):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(FILE) -o $(@)

$(OBJ_CTL_CORE): $(FILES)
	@echo "  $(LD) $(notdir $(@))"
	@$(LD) $(MERGE_FLAGS) $(PACKAGES) -o $(OBJ_CTL_CORE) $(FILES)

all: $(OBJ_CTL_CORE) $(SUBDIRS)
