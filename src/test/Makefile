rwildcard				= $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(call rwildcard, , *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)

.PHONY: all

all: $(OBJ_TEST_CORE)

$(FILES):
	@echo "  $(CC) $(FILE)"
	@mkdir -p $(dir $@)
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(OPENGL_HEADERS) $(FILE) -o $(@)

$(OBJ_TEST_CORE): $(FILES)
	@echo "  $(LD) $(notdir $(OBJ_TEST_CORE))"
	@$(LD) $(LDFLAGS) -r -o $(OBJ_TEST_CORE) $(FILES) 

