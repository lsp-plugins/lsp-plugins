FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
OBJ_HELPERS             = $(OBJDIR)/CairoCanvas.o

# Detemine what modules to build
OBJFILES                = $(OBJ_CORE) $(OBJ_DSP) $(OBJ_METADATA) $(OBJ_PLUGINS)
LIBS                    = $(SNDFILE_LIBS) $(PTHREAD_LIBS) $(MATH_LIBS)

ifeq ($(BUILD_PLATFORM),BSD)
  LIBS                   += $(ICONV_LIBS)
endif

# Configure set of modules to build
ifeq ($(LSP_TESTING),1)
  OBJFILES               += $(OBJ_TEST_CORE)
  MODULES                 = $(BIN_TEST)
endif
ifneq ($(LSP_TESTING),1)
  MODULES                 =
  ifeq ($(findstring ladspa,$(BUILD_MODULES)),ladspa)
    MODULES                += $(LIB_LADSPA)
  endif
  ifeq ($(findstring lv2,$(BUILD_MODULES)),lv2)
    MODULES                += $(LIB_LV2)
  endif
  ifeq ($(findstring vst,$(BUILD_MODULES)),vst)
    MODULES                += $(LIB_VST)
  endif
  ifeq ($(findstring jack,$(BUILD_MODULES)),jack)
    MODULES                += $(LIB_JACK)
  endif
  ifeq ($(findstring profile,$(BUILD_MODULES)),profile)
    MODULES                += $(BIN_PROFILE)
  endif
endif

UI_OBJFILES             = $(OBJFILES) $(OBJ_UI_CORE) $(OBJ_RES_CORE) 
UI_LIBS                 = $(LIBS) $(XLIB_LIBS) $(CAIRO_LIBS)

.PHONY: all

all: $(MODULES)

$(OBJ_HELPERS):
	@echo "  $(CC) $(FILE)"
	@$(CC) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(CAIRO_HEADERS)

$(LIB_LADSPA):
	@echo "  $(CC) ladspa.cpp"
	@$(CC) -o $(OBJDIR)/ladspa.o -c ladspa.cpp -fPIC $(CPPFLAGS) $(CFLAGS) $(INCLUDE) 
	@echo "  $(CC) $(notdir $(LIB_LADSPA))"
	@$(CC) -o $(LIB_LADSPA) $(OBJDIR)/ladspa.o $(OBJFILES) $(SO_FLAGS) $(LIBS)

$(LIB_LV2): $(OBJ_HELPERS)
	@echo "  $(CC) lv2.cpp"
	@$(CC) -o $(OBJDIR)/lv2.o -c lv2.cpp -fPIC $(CPPFLAGS) $(CFLAGS) $(INCLUDE) 
	@echo "  $(CC) $(notdir $(LIB_LV2))"
	@$(CC) -o $(LIB_LV2) $(OBJDIR)/lv2.o $(OBJ_HELPERS) $(UI_OBJFILES) $(SO_FLAGS) $(UI_LIBS)

$(LIB_VST):
	@echo "  $(CC) vst.cpp"
	@$(CC) -o $(OBJDIR)/vst.o -c vst.cpp -fPIC $(CPPFLAGS) $(CFLAGS) $(INCLUDE) 
	@echo "  $(CC) $(notdir $(LIB_VST))"
	@$(CC) -o $(LIB_VST) $(OBJDIR)/vst.o $(UI_OBJFILES) $(SO_FLAGS) $(UI_LIBS) 

$(LIB_JACK): $(OBJ_HELPERS)
	@echo "  $(CC) jack.cpp"
	@$(CC) -o $(OBJDIR)/jack.o -c jack.cpp -fPIC $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(JACK_HEADERS) 
	@echo "  $(CC) $(notdir $(LIB_JACK))"
	@$(CC) -o $(LIB_JACK) $(OBJDIR)/jack.o $(OBJ_HELPERS) $(UI_OBJFILES) $(SO_FLAGS) $(UI_LIBS) $(JACK_LIBS)

$(BIN_PROFILE): $(LIB_JACK)
	@echo "  $(CC) profile.cpp"
	@$(CC) -o $(OBJDIR)/profile.o -c profile.cpp -fPIC $(CPPFLAGS) -DLSP_PROFILING_MAIN $(CFLAGS) $(INCLUDE) $(JACK_HEADERS) 
	@echo "  $(CC) $(notdir $(BIN_PROFILE))"
	@$(CC) -o $(BIN_PROFILE) $(OBJDIR)/jack.o $(OBJDIR)/profile.o $(OBJ_HELPERS) $(UI_OBJFILES) $(EXE_FLAGS) $(UI_LIBS) $(JACK_LIBS) 

$(BIN_TEST): $(LIB_JACK)
	@echo "  $(CC) test.cpp"
	@$(CC) -o $(OBJDIR)/test.o -c test.cpp -fPIC $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(JACK_HEADERS) 
	@echo "  $(CC) $(notdir $(BIN_TEST))"
	@$(CC) -o $(BIN_TEST) $(OBJDIR)/jack.o $(OBJDIR)/test.o $(OBJ_HELPERS) $(UI_OBJFILES) $(EXE_TEST_FLAGS) $(UI_LIBS) $(JACK_LIBS) $(OPENGL_LIBS)   
