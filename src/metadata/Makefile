rwildcard               = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)

.PHONY: all target

all: $(OBJ_METADATA)

target: all

$(OBJ_METADATA): $(FILES)
	@echo "  $(LD) $(notdir $(OBJ_METADATA))"
	@$(LD) -o $(OBJ_METADATA) -r $(LDFLAGS) $(FILES) 

$(FILES):
	@echo "  $(CC) $(FILE)"
	@$(CC) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CFLAGS) $(INCLUDE) 
