FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
rwildcard               = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

.PHONY: all target

all: $(OBJ_METADATA)

target: all

$(OBJ_METADATA): $(FILES)
	@echo "  $(LD) $(notdir $(OBJ_METADATA))"
	@$(LD) $(LDFLAGS) -r -o $(OBJ_METADATA) $(call rwildcard,$(OBJDIR),*.o)

$(FILES):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(FILE) -o $(@)
