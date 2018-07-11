FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
OBJ_HELPERS             = $(OBJDIR)/CairoCanvas.o

# Detemine what modules to build
MODULES                 = $(LIB_LADSPA) $(LIB_LV2) $(LIB_JACK) $(BIN_PROFILE) $(LIB_VST)
PACKAGES                = $(OBJ_CORE) $(OBJ_METADATA) $(OBJ_PLUGINS) $(SNDFILE_LIBS)
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
	@$(CC) -o $(LIB_LADSPA) $(SO_FLAGS) $(PACKAGES) $(OBJDIR)/ladspa.o  

$(LIB_LV2): $(OBJ_HELPERS)
	@echo "  $(CC) lv2.cpp"
	@$(CC) -o $(OBJDIR)/lv2.o -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) lv2.cpp 
	@echo "  $(CC) $(notdir $(LIB_LV2))"
	@$(CC) -o $(LIB_LV2) $(SO_FLAGS) $(UI_PACKAGES) $(OBJ_HELPERS) $(OBJDIR)/lv2.o

$(LIB_VST):
	@echo "  $(CC) vst.cpp"
	@$(CC) -o $(OBJDIR)/vst.o -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) vst.cpp 
	@echo "  $(CC) $(notdir $(LIB_VST))"
	@$(CC) -o $(LIB_VST) $(SO_FLAGS) $(UI_PACKAGES) $(OBJDIR)/vst.o 

$(LIB_JACK): $(OBJ_HELPERS)
	@echo "  $(CC) jack.cpp"
	@$(CC) -o $(OBJDIR)/jack.o -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(JACK_HEADERS) jack.cpp 
	@echo "  $(CC) $(notdir $(LIB_JACK))"
	@$(CC) -o $(LIB_JACK) $(SO_FLAGS) $(JACK_LIBS) $(OBJ_HELPERS) $(UI_PACKAGES) $(OBJDIR)/jack.o

$(BIN_PROFILE): $(LIB_JACK)
	@echo "  $(CC) profile.cpp"
	@$(CC) -o $(OBJDIR)/profile.o -c $(CPPFLAGS) -DLSP_PROFILING_MAIN $(CFLAGS) $(INCLUDE) $(JACK_HEADERS) profile.cpp
	@echo "  $(CC) $(notdir $(BIN_PROFILE))"
	@$(CC) -o $(BIN_PROFILE) $(EXE_FLAGS) $(JACK_LIBS) $(OBJ_HELPERS) $(UI_PACKAGES) $(OBJDIR)/jack.o $(OBJDIR)/profile.o 

