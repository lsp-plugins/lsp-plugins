rwildcard				= $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(call rwildcard, , *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)

.PHONY: all

all: $(OBJ_CORE)

$(FILES):
	@echo "  $(CC) $(FILE)"
	@mkdir -p $(dir $@)
	@$(CC) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CFLAGS) $(INCLUDE) 

$(OBJ_CORE): $(FILES)
	@echo "  $(LD) $(notdir $(OBJ_CORE))"
	@$(LD) -o $(OBJ_CORE) -r $(LDFLAGS) $(FILES)  
