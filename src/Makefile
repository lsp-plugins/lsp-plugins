SUBDIRS                 = core plugins ui utils container
POST_PROCESSING         = vst_stub
ALL_UTILS               = $(UTL_GENTTL) $(UTL_VSTMAKE) $(UTL_GENPHP) $(UTL_RESGEN)

.PHONY: all $(SUBDIRS)

all: $(SUBDIRS) $(POST_PROCESSING)

target: all

# Rules
$(SUBDIRS):
	@echo "Building $(@)"
	@mkdir -p $(OBJDIR)/$(@)
	@$(MAKE) $(MAKE_OPTS) -C $@ $(MAKECMDGOALS) OBJDIR=$(OBJDIR)/$(@)

# Object dependencies
$(OBJ_CORE): core

$(OBJ_UI_CORE): ui

$(OBJ_RES_CORE): $(OBJ_CORE) $(UTL_RESGEN)
	@echo "Generating XML resources"
	@mkdir -p $(OBJDIR)/res
	@$(UTL_RESGEN) $(BASEDIR)/res/ui $(OBJDIR)/res/xml_resource.cpp
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(OBJDIR)/res/xml_resource.cpp -o $(OBJ_RES_CORE)

$(ALL_UTILS): utils

# Target dependencies
plugins: $(OBJ_CORE)

container: $(OBJ_CORE) $(OBJ_UI_CORE) $(OBJ_RES_CORE) $(ALL_UTILS)

utils: $(OBJ_CORE)

# Additional post-processing
vst_stub: $(UTL_VSTMAKE)
	@echo "Building VST stub"
	@mkdir -p $(OBJDIR)/vst
	@$(UTL_VSTMAKE) $(OBJDIR)/vst
	@$(MAKE) $(MAKE_OPTS) -C $(OBJDIR)/vst all OBJDIR=$(OBJDIR)/vst
