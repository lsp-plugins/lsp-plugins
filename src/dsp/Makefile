NATIVE_IMPL             = $(OBJDIR)/native.o

X86_IMPL                = $(OBJDIR)/x86.o
SSE_IMPL                = $(OBJDIR)/sse.o
SSE2_IMPL               = $(OBJDIR)/sse2.o
SSE3_IMPL               = $(OBJDIR)/sse3.o
SSE4_IMPL               = $(OBJDIR)/sse4.o
AVX_IMPL                = $(OBJDIR)/avx.o
AVX2_IMPL               = $(OBJDIR)/avx2.o
NEON_D32_IMPL           = $(OBJDIR)/neon-d32.o
DSP_IMPL                = $(OBJDIR)/dsp.o
BITS_IMPL               = $(OBJDIR)/bits.o
ARM_IMPL                = $(OBJDIR)/arm.o

SSE_INSTR_SET           = -mmmx -m3dnow -msse -msse2
SSE2_INSTR_SET          = -mmmx -m3dnow -msse -msse2
SSE3_INSTR_SET          = $(SSE_INSTR_SET) -msse3 -mssse3
SSE4_INSTR_SET          = $(SSE_INSTR_SET) -msse4.1 -msse4.2
AVX_INSTR_SET           = -mavx
AVX2_INSTR_SET          = -mavx -mavx2
NEON_D32_INSTR_SET      = -mfpu=neon-vfpv4

FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
LINK_OBJECTS            = $(DSP_IMPL) $(BITS_IMPL) $(NATIVE_IMPL)

# Build different DSP modules dependent on architecture
ifeq ($(BUILD_PROFILE), i586)
LINK_OBJECTS           += $(X86_IMPL) $(SSE_IMPL) $(SSE2_IMPL) $(SSE3_IMPL) $(SSE4_IMPL) $(AVX_IMPL) $(AVX2_IMPL)
endif
ifeq ($(BUILD_PROFILE), x86_64)
LINK_OBJECTS           += $(X86_IMPL) $(SSE_IMPL) $(SSE2_IMPL) $(SSE3_IMPL) $(SSE4_IMPL) $(AVX_IMPL) $(AVX2_IMPL)
endif
ifeq ($(BUILD_PROFILE), armv7a)
LINK_OBJECTS           += $(ARM_IMPL) $(NEON_D32_IMPL)
endif

.PHONY: all target

target: all

all: $(OBJ_DSP)

$(OBJ_DSP): $(LINK_OBJECTS)
	@echo "  $(LD) $(notdir $(OBJ_DSP))"
	@$(LD) -o $(OBJ_DSP) -r $(LDFLAGS) $(LINK_OBJECTS)

$(DSP_IMPL) $(BITS_IMPL) $(NATIVE_IMPL) $(X86_IMPL) $(ARM_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CFLAGS) $(INCLUDE) 
	
$(SSE_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CFLAGS) $(SSE_INSTR_SET) $(INCLUDE)
	
$(SSE2_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CFLAGS) $(SSE2_INSTR_SET) $(INCLUDE)
	
$(SSE3_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CFLAGS) $(SSE3_INSTR_SET) $(INCLUDE)
	
$(SSE4_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CFLAGS) $(SSE4_INSTR_SET) $(INCLUDE)

$(AVX_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CFLAGS) $(AVX_INSTR_SET) $(INCLUDE)

$(AVX2_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CFLAGS) $(AVX2_INSTR_SET) $(INCLUDE)
	
$(NEON_D32_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CFLAGS) $(NEON_D32_INSTR_SET) $(INCLUDE)
