FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
SUBDIRS                 =

# Detemine what modules to build
MODULES                 = $(LIB_LADSPA) $(LIB_LV2) $(LIB_LV2_GTK2UI) $(LIB_JACK) $(BIN_PROFILE)
ifdef VST_SDK
MODULES                += $(LIB_VST)
endif

.PHONY: all $(SUBDIRS)

all: $(MODULES)

$(LIB_LADSPA): $(SUBDIRS)
	@echo "  $(CC) ladspa.cpp"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(HEADERS) $(PACKAGES) ladspa.cpp -o $(OBJDIR)/ladspa.o
	@echo "  $(CC) $(notdir $(LIB_LADSPA))"
	@$(CC) $(SO_FLAGS) $(SNDFILE_LIBS) $(PACKAGES) -o $(LIB_LADSPA) $(OBJDIR)/ladspa.o $(OBJ_CORE) $(OBJ_METADATA) $(OBJ_PLUGINS)

$(LIB_LV2): $(SUBDIRS)
	@echo "  $(CC) lv2.cpp"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(LSP_UI) $(INCLUDE) $(HEADERS) $(PACKAGES) lv2.cpp -o $(OBJDIR)/lv2.o
	@echo "  $(CC) $(notdir $(LIB_LV2))"
	@$(CC) $(SO_FLAGS) $(SNDFILE_LIBS) $(PACKAGES) -o $(LIB_LV2) $(OBJDIR)/lv2.o $(OBJ_CORE) $(OBJ_METADATA) $(OBJ_PLUGINS)

$(LIB_VST): HEADERS  := $(GTK2_HEADERS)
$(LIB_VST): PACKAGES := $(GTK2_LIBS) $(SNDFILE_LIBS)
$(LIB_VST): GUI      := gtk2
$(LIB_VST): OBJ_UI   := $(OBJ_GTK2UI) $(OBJ_UI_CORE) $(OBJ_RES_CORE)
$(LIB_VST): LSP_UI   := -DLSP_UI_GTK2

$(LIB_VST): $(SUBDIRS)
	@echo "  $(CC) vst.cpp"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(LSP_UI) $(INCLUDE) $(HEADERS) $(PACKAGES) vst.cpp -o $(OBJDIR)/vst.o
	@echo "  $(CC) $(notdir $(LIB_VST))"
	@$(CC) $(SO_FLAGS) -o $(LIB_VST) $(OBJDIR)/vst.o $(PACKAGES) $(OBJ_CORE) $(OBJ_METADATA) $(OBJ_PLUGINS) $(OBJ_UI) $(call rwildcard,$(OBJDIR)/$(GUI),*.o)

$(LIB_LV2_GTK2UI): HEADERS  := $(GTK2_HEADERS)
$(LIB_LV2_GTK2UI): PACKAGES := $(GTK2_LIBS) $(SNDFILE_LIBS)
$(LIB_LV2_GTK2UI): GUI    	:= gtk2
$(LIB_LV2_GTK2UI): OBJ_UI   := $(OBJ_GTK2UI) $(OBJ_UI_CORE) $(OBJ_RES_CORE)
$(LIB_LV2_GTK2UI): LSP_UI  	:= -DLSP_UI_GTK2
	
$(LIB_LV2_GTK3UI): HEADERS  := $(GTK3_HEADERS)
$(LIB_LV2_GTK3UI): PACKAGES := $(GTK3_LIBS) $(SNDFILE_LIBS)
$(LIB_LV2_GTK3UI): GUI    	:= gtk3
$(LIB_LV2_GTK3UI): OBJ_UI   := $(OBJ_GTK3UI) $(OBJ_UI_CORE) $(OBJ_RES_CORE)
$(LIB_LV2_GTK3UI): LSP_UI  	:= -DLSP_UI_GTK3

$(LIB_LV2_GTK2UI) $(LIB_LV2_GTK3UI): $(SUBDIRS)
	@echo "Building $(GUI)"
	@echo "  $(CC) lv2ui.cpp"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(LSP_UI) $(INCLUDE) $(HEADERS) $(PACKAGES) lv2ui.cpp -o $(OBJDIR)/lv2_$(GUI).o
	@echo "  $(CC) $(notdir $(@))"
	@$(CC) $(SO_FLAGS) $(PACKAGES) -o $(@) $(OBJDIR)/lv2_$(GUI).o $(OBJ_CORE) $(OBJ_METADATA) $(OBJ_UI) $(call rwildcard,$(OBJDIR)/$(GUI),*.o)

$(LIB_JACK): HEADERS  := $(GTK2_HEADERS) $(JACK_HEADERS)
$(LIB_JACK): PACKAGES := $(GTK2_LIBS) $(SNDFILE_LIBS) $(JACK_LIBS)
$(LIB_JACK): GUI      := gtk2
$(LIB_JACK): OBJ_UI   := $(OBJ_GTK2UI) $(OBJ_UI_CORE) $(OBJ_RES_CORE)
$(LIB_JACK): LSP_UI   := -DLSP_UI_GTK2
$(LIB_JACK): $(SUBDIRS)
	@echo "  $(CC) jack.cpp"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(LSP_UI) $(INCLUDE) $(HEADERS) $(PACKAGES) jack.cpp -o $(OBJDIR)/jack.o
	@echo "  $(CC) $(notdir $(LIB_JACK))"
	@$(CC) $(SO_FLAGS) -o $(LIB_JACK) $(OBJDIR)/jack.o $(PACKAGES) $(OBJ_CORE) $(OBJ_METADATA) $(OBJ_PLUGINS) $(OBJ_UI) $(call rwildcard,$(OBJDIR)/$(GUI),*.o)

$(BIN_PROFILE): HEADERS  := $(GTK2_HEADERS) $(JACK_HEADERS)
$(BIN_PROFILE): PACKAGES := $(GTK2_LIBS) $(SNDFILE_LIBS) $(JACK_LIBS)
$(BIN_PROFILE): GUI      := gtk2
$(BIN_PROFILE): OBJ_UI   := $(OBJ_GTK2UI) $(OBJ_UI_CORE) $(OBJ_RES_CORE)
$(BIN_PROFILE): LSP_UI   := -DLSP_UI_GTK2
$(BIN_PROFILE): $(SUBDIRS) $(LIB_JACK)
	@echo "  $(CC) profile.cpp"
	@$(CC) -c $(CPPFLAGS) -DLSP_PROFILING_MAIN $(CFLAGS) $(LSP_UI) $(INCLUDE) $(HEADERS) $(PACKAGES) profile.cpp -o $(OBJDIR)/profile.o
	@echo "  $(CC) $(notdir $(BIN_PROFILE))"
	@$(CC) $(EXE_FLAGS) -o $(BIN_PROFILE) $(OBJDIR)/jack.o $(OBJDIR)/profile.o $(PACKAGES) $(OBJ_CORE) $(OBJ_METADATA) $(OBJ_PLUGINS) $(OBJ_UI) $(call rwildcard,$(OBJDIR)/$(GUI),*.o)

$(SUBDIRS):
	@echo "Building $@"
	@mkdir -p $(OBJDIR)/$(@)
	@$(MAKE) $(MAKE_OPTS) -C $@ $(MAKECMDGOALS) OBJDIR=$(OBJDIR)/$(@)
