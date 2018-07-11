FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
SUBDIRS                 =

.PHONY: all $(SUBDIRS)

all: $(LIB_LADSPA) $(LIB_LV2) $(LIB_LV2_GTK2UI)

$(LIB_LADSPA): $(OBJ_PLUGINS) $(SUBDIRS)
	@echo "  $(CC) ladspa.cpp"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(LSP_UI) $(INCLUDE) $(HEADERS) $(PACKAGES) ladspa.cpp -o $(OBJDIR)/ladspa.o
	@echo "  $(CC) $(notdir $(LIB_LADSPA))"
	@$(CC) $(SO_FLAGS) -o $(LIB_LADSPA) $(OBJDIR)/ladspa.o $(OBJ_CORE) $(OBJ_PLUGINS)

$(LIB_LV2): $(OBJ_PLUGINS) $(SUBDIRS)
	@echo "  $(CC) lv2.cpp"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(LSP_UI) $(INCLUDE) $(HEADERS) $(PACKAGES) lv2.cpp -o $(OBJDIR)/lv2.o
	@echo "  $(CC) $(notdir $(LIB_LV2))"
	@$(CC) $(SO_FLAGS) -o $(LIB_LV2) $(OBJDIR)/lv2.o $(OBJ_CORE) $(OBJ_PLUGINS)

$(LIB_LV2_GTK2UI): HEADERS  := $(GTK2_HEADERS)
$(LIB_LV2_GTK2UI): PACKAGES := $(GTK2_LIBS) $(EXPAT_LIBS)
$(LIB_LV2_GTK2UI): GUI    	:= gtk2
$(LIB_LV2_GTK2UI): OBJ_UI   := $(OBJ_GTK2UI) $(OBJ_UI_CORE)
$(LIB_LV2_GTK2UI): LSP_UI  	:= -DLSP_UI_GTK2
	
$(LIB_LV2_GTK3UI): HEADERS  := $(GTK3_HEADERS)
$(LIB_LV2_GTK3UI): PACKAGES := $(GTK3_LIBS) $(EXPAT_LIBS)
$(LIB_LV2_GTK3UI): GUI    	:= gtk3
$(LIB_LV2_GTK3UI): OBJ_UI   := $(OBJ_GTK3UI) $(OBJ_UI_CORE)
$(LIB_LV2_GTK3UI): LSP_UI  	:= -DLSP_UI_GTK3

$(LIB_LV2_GTK2UI) $(LIB_LV2_GTK3UI): $(SUBDIRS) $(OBJ_PLUGINS) $(OBJ_UI)
	@echo "Building $(GUI)"
	@echo "  $(CC) lv2ui.cpp"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(LSP_UI) $(INCLUDE) $(HEADERS) $(PACKAGES) lv2ui.cpp -o $(OBJDIR)/lv2_$(GUI).o
	@echo "  $(CC) $(notdir $(@))"
	@$(CC) $(SO_FLAGS) $(PACKAGES) -o $(@) $(OBJDIR)/lv2_$(GUI).o $(OBJ_CORE) $(OBJ_UI) $(call rwildcard,$(OBJDIR)/$(GUI),*.o)

$(SUBDIRS):
	@echo "Building $@"
	@mkdir -p $(OBJDIR)/$(@)
	@$(MAKE) $(MAKE_OPTS) -C $@ $(MAKECMDGOALS) OBJDIR=$(OBJDIR)/$(@)
