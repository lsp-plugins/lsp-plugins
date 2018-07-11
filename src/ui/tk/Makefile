rwildcard				= $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(call rwildcard, , *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)

.PHONY: all

$(FILES):
	@echo "  $(CC) $(FILE)"
	@mkdir -p $(dir $@)
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(FILE) -o $(@)

$(OBJ_TK_CORE): $(FILES)
	@echo "  $(LD) $(notdir $(@))"
	@$(LD) $(MERGE_FLAGS) $(PACKAGES) -o $(OBJ_TK_CORE) $(FILES)

all: $(OBJ_TK_CORE)
