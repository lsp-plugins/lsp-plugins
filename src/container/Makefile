FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
OBJ_HELPERS             = $(OBJDIR)/CairoCanvas.o

# Detemine what modules to build
OBJFILES                = $(OBJ_CORE) $(OBJ_DSP) $(OBJ_METADATA) $(OBJ_PLUGINS)
LIBS                    = $(SNDFILE_LIBS) $(PTHREAD_LIBS) $(MATH_LIBS) $(DL_LIBS)

# Configure set of modules to build
ifeq ($(LSP_TESTING),1)
  OBJFILES               += $(OBJ_TEST_CORE) $(OBJ_TESTING_CORE)
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

UI_OBJFILES             = $(OBJ_UI_CORE) $(OBJ_RES_CORE)
UI_LIBS                 = $(XLIB_LIBS) $(CAIRO_LIBS)

ifeq ($(BUILD_PLATFORM),BSD)
  LIBS                   += $(ICONV_LIBS)
endif

LV2_OBJFILES            = $(OBJFILES)
LV2_LIBS                = $(LIBS)
LV2_DEPS                =
ifeq ($(LV2_UI),1)
  LV2_DEPS               += $(OBJ_HELPERS)
  LV2_OBJFILES           += $(OBJ_HELPERS) $(UI_OBJFILES)
  LV2_LIBS               += $(UI_LIBS)
endif

VST_OBJFILES            = $(OBJFILES)
VST_LIBS                = $(LIBS)
ifeq ($(VST_UI),1)
  VST_OBJFILES           += $(UI_OBJFILES)
  VST_LIBS               += $(UI_LIBS)
endif

.PHONY: all

all: $(MODULES)

$(OBJ_HELPERS):
	@echo "  $(CXX) $(FILE)"
	@$(CXX) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE) $(CAIRO_HEADERS)

$(LIB_LADSPA):
	@echo "  $(CXX) ladspa.cpp"
	@$(CXX) -o $(OBJDIR)/ladspa.o -c ladspa.cpp -fPIC $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE) 
	@echo "  $(CXX) $(notdir $(LIB_LADSPA))"
	@$(CXX) -o $(LIB_LADSPA) $(OBJDIR)/ladspa.o $(OBJFILES) $(SO_FLAGS) $(LIBS)

$(LIB_LV2): $(LV2_DEPS)
	@echo "  $(CXX) lv2.cpp"
	@$(CXX) -o $(OBJDIR)/lv2.o -c lv2.cpp -fPIC $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE) $(CAIRO_HEADERS)
	@echo "  $(CXX) $(notdir $(LIB_LV2))"
	@$(CXX) -o $(LIB_LV2) $(OBJDIR)/lv2.o $(LV2_OBJFILES) $(SO_FLAGS) $(LV2_LIBS)

$(LIB_VST):
	@echo "  $(CXX) vst.cpp"
	@$(CXX) -o $(OBJDIR)/vst.o -c vst.cpp -fPIC $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE) 
	@echo "  $(CXX) $(notdir $(LIB_VST))"
	@$(CXX) -o $(LIB_VST) $(OBJDIR)/vst.o $(VST_OBJFILES) $(SO_FLAGS) $(VST_LIBS) 

$(LIB_JACK): $(OBJ_HELPERS)
	@echo "  $(CXX) jack.cpp"
	@$(CXX) -o $(OBJDIR)/jack.o -c jack.cpp -fPIC $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE) $(JACK_HEADERS) $(CAIRO_HEADERS)
	@echo "  $(CXX) $(notdir $(LIB_JACK))"
	@$(CXX) -o $(LIB_JACK) $(OBJDIR)/jack.o $(OBJ_HELPERS) $(OBJFILES) $(UI_OBJFILES) $(SO_FLAGS) $(LIBS) $(UI_LIBS) $(JACK_LIBS)

$(BIN_PROFILE): $(LIB_JACK)
	@echo "  $(CXX) profile.cpp"
	@$(CXX) -o $(OBJDIR)/profile.o -c profile.cpp -fPIC $(CPPFLAGS) -DLSP_PROFILING_MAIN $(CXXFLAGS) $(INCLUDE) $(JACK_HEADERS) 
	@echo "  $(CXX) $(notdir $(BIN_PROFILE))"
	@$(CXX) -o $(BIN_PROFILE) $(OBJDIR)/jack.o $(OBJDIR)/profile.o $(OBJ_HELPERS) $(OBJFILES) $(UI_OBJFILES) $(EXE_FLAGS) $(LIBS) $(UI_LIBS) $(JACK_LIBS) 

$(BIN_TEST): $(LIB_JACK)
	@echo "  $(CXX) test.cpp"
	@$(CXX) -o $(OBJDIR)/test.o -c test.cpp -fPIC $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE) $(JACK_HEADERS) 
	@echo "  $(CXX) $(notdir $(BIN_TEST))"
	@$(CXX) -o $(BIN_TEST) $(OBJDIR)/jack.o $(OBJDIR)/test.o $(OBJ_HELPERS) $(OBJFILES) $(UI_OBJFILES) $(EXE_TEST_FLAGS) $(LIBS) $(UI_LIBS) $(JACK_LIBS) $(OPENGL_LIBS) $(DL_LIBS)   
