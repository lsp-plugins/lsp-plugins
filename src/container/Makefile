FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
OBJ_HELPERS             = $(OBJDIR)/CairoCanvas.o

# Detemine what modules to build
PACKAGES                = $(OBJ_CORE) $(OBJ_DSP) $(OBJ_METADATA) $(OBJ_PLUGINS) $(SNDFILE_LIBS)
ifeq ($(LSP_TESTING),1)
PACKAGES               += $(OBJ_TEST_CORE) $(OPENGL_LIBS)
MODULES                 = $(BIN_TEST)
endif
ifneq ($(LSP_TESTING),1)
MODULES                 = $(LIB_LADSPA) $(LIB_LV2) $(LIB_JACK) $(BIN_PROFILE) $(LIB_VST)
endif

UI_PACKAGES             = $(PACKAGES) $(OBJ_UI_CORE) $(OBJ_RES_CORE) $(XLIB_LIBS) $(CAIRO_LIBS)

.PHONY: all

all: $(MODULES)

$(OBJ_HELPERS):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(CAIRO_HEADERS) $(FILE) -o $(@)

$(LIB_LADSPA):
	@echo "  $(CC) ladspa.cpp"
	@$(CC) -o $(OBJDIR)/ladspa.o -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) ladspa.cpp  
	@echo "  $(CC) $(notdir $(LIB_LADSPA))"
	@$(CC) -o $(LIB_LADSPA) $(OBJDIR)/ladspa.o $(PACKAGES) $(SO_FLAGS)  

$(LIB_LV2): $(OBJ_HELPERS)
	@echo "  $(CC) lv2.cpp"
	@$(CC) -o $(OBJDIR)/lv2.o -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) lv2.cpp 
	@echo "  $(CC) $(notdir $(LIB_LV2))"
	@$(CC) -o $(LIB_LV2) $(OBJDIR)/lv2.o $(OBJ_HELPERS) $(UI_PACKAGES) $(SO_FLAGS)

$(LIB_VST):
	@echo "  $(CC) vst.cpp"
	@$(CC) -o $(OBJDIR)/vst.o -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) vst.cpp 
	@echo "  $(CC) $(notdir $(LIB_VST))"
	@$(CC) -o $(LIB_VST) $(OBJDIR)/vst.o $(UI_PACKAGES) $(SO_FLAGS)  

$(LIB_JACK): $(OBJ_HELPERS)
	@echo "  $(CC) jack.cpp"
	@$(CC) -o $(OBJDIR)/jack.o -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(JACK_HEADERS) jack.cpp 
	@echo "  $(CC) $(notdir $(LIB_JACK))"
	@$(CC) -o $(LIB_JACK) $(OBJDIR)/jack.o $(JACK_LIBS) $(OBJ_HELPERS) $(UI_PACKAGES) $(SO_FLAGS) 

$(BIN_PROFILE): $(LIB_JACK)
	@echo "  $(CC) profile.cpp"
	@$(CC) -o $(OBJDIR)/profile.o -c $(CPPFLAGS) -DLSP_PROFILING_MAIN $(CFLAGS) $(INCLUDE) $(JACK_HEADERS) profile.cpp
	@echo "  $(CC) $(notdir $(BIN_PROFILE))"
	@$(CC) -o $(BIN_PROFILE) $(OBJDIR)/jack.o $(OBJDIR)/profile.o $(JACK_LIBS) $(OBJ_HELPERS) $(UI_PACKAGES) $(EXE_FLAGS)  

$(BIN_TEST): $(LIB_JACK)
	@echo "  $(CC) test.cpp"
	@$(CC) -o $(OBJDIR)/test.o -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(JACK_HEADERS) test.cpp
	@echo "  $(CC) $(notdir $(BIN_TEST))"
	@$(CC) -o $(BIN_TEST) $(OBJDIR)/jack.o $(OBJDIR)/test.o $(JACK_LIBS) $(OBJ_HELPERS) $(UI_PACKAGES) $(EXE_FLAGS)  
