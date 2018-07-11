FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
SUBDIRS                 =
rwildcard               = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

.PHONY: all $(SUBDIRS)

all: $(OBJ_GTK2UI)

$(OBJ_GTK2UI): $(FILES)
	@echo "  $(LD) $(notdir $(@))"
	@$(LD) $(LDFLAGS) -r $(PACKAGES) -o $(OBJ_GTK2UI) $(FILES)

$(FILES):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(GTK2_HEADERS) $(FILE) -o $(@)

