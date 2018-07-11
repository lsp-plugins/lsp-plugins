FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
SUBDIRS                 =
OBJFILE                 = $(OBJDIR)/$(addsuffix .o, $(notdir $(OBJDIR)))
rwildcard               = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

.PHONY: all files subdirs $(SUBDIRS)

all:  $(OBJFILE)

$(OBJFILE): $(FILES) $(SUBDIRS)
	@echo "  $(LD) $(notdir $(OBJFILE))"
	@$(LD) $(LDFLAGS) -r -o $(OBJFILE) $(call rwildcard,$(OBJDIR),*.o)

$(SUBDIRS):
	@echo "Building $@"
	@mkdir -p $(OBJDIR)/$(@)
	@$(MAKE) $(MAKE_OPTS) -C $@ $(MAKECMDGOALS) OBJDIR=$(OBJDIR)/$(@)

$(FILES):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(FILE) -o $(@)
