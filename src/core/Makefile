FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
rwildcard               = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
export DSP_CORE         = $(OBJDIR)/dsp/dsp.o
SUBDIRS                 = dynamics filters util 3d files sampling io
SUBFILES				= $(addprefix $(OBJDIR)/, $(foreach subdir, $(SUBDIRS), $(subdir)/$(subdir).o)) 

.PHONY: all target dsp $(SUBDIRS)

all: $(OBJ_CORE)

target: all

$(OBJ_CORE): $(SUBDIRS) $(DSP_CORE) $(FILES)
	@echo "  $(LD) $(notdir $(OBJ_CORE))"
	@$(LD) -r $(LDFLAGS) -o $(OBJ_CORE) $(DSP_CORE) $(FILES) $(SUBFILES)

$(SUBDIRS):
	@echo "Building $(@)"
	@mkdir -p $(OBJDIR)/$(@)
	@$(MAKE) $(MAKE_OPTS) -C $(@) $(MAKECMDGOALS) OBJDIR=$(OBJDIR)/$(@)

$(DSP_CORE):
	@echo "Building dsp"
	@mkdir -p $(OBJDIR)/dsp
	@$(MAKE) $(MAKE_OPTS) -C dsp $(MAKECMDGOALS) OBJDIR=$(OBJDIR)/dsp

$(FILES):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(SNDFILE_HEADERS) $(SNDFILE_LIBS) $(FILE) -o $(@)
