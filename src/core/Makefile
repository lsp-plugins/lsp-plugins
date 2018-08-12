FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
rwildcard               = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
SUBDIRS                 = dynamics filters util 3d files sampling io
SUBFILES				= $(addprefix $(OBJDIR)/, $(foreach subdir, $(SUBDIRS), $(subdir)/$(subdir).o)) 

.PHONY: all target dsp $(SUBDIRS)

all: $(OBJ_CORE)

target: all

$(OBJ_CORE): $(SUBDIRS) $(FILES)
	@echo "  $(LD) $(notdir $(OBJ_CORE))"
	@$(LD) -r $(LDFLAGS) -o $(OBJ_CORE) $(FILES) $(SUBFILES)

$(SUBDIRS):
	@echo "Building $(@)"
	@mkdir -p $(OBJDIR)/$(@)
	@$(MAKE) $(MAKE_OPTS) -C $(@) $(MAKECMDGOALS) OBJDIR=$(OBJDIR)/$(@)

$(FILES):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(SNDFILE_HEADERS) $(SNDFILE_LIBS) $(FILE) -o $(@)
