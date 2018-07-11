NATIVE_IMPL             = $(OBJDIR)/native.o
X86_IMPL                = $(OBJDIR)/x86.o
SSE_IMPL                = $(OBJDIR)/sse.o
SSE3_IMPL               = $(OBJDIR)/sse3.o
SSE4_IMPL               = $(OBJDIR)/sse4.o
AVX_IMPL                = $(OBJDIR)/avx.o
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
SSE_INSTR_SET           = -mmmx -m3dnow -msse -msse2
SSE3_INSTR_SET          = $(SSE_INSTR_SET) -msse3 -mssse3
SSE4_INSTR_SET          = $(SSE_INSTR_SET) -msse4.1 -msse4.2
AVX_INSTR_SET           = -mavx

LINK_OBJECTS            = $(NATIVE_IMPL)

ifeq ($(CPU_ARCH), i586)
LINK_OBJECTS           += $(X86_IMPL) $(SSE_IMPL) $(SSE3_IMPL) $(SSE4_IMPL) $(AVX_IMPL)
endif
ifeq ($(CPU_ARCH), x86_64)
LINK_OBJECTS           += $(X86_IMPL) $(SSE_IMPL) $(SSE3_IMPL) $(SSE4_IMPL) $(AVX_IMPL)
endif

.PHONY: all target

target: all

all: $(DSP_CORE)

$(DSP_CORE): $(LINK_OBJECTS)
	@echo "  $(LD) $(notdir $(DSP_CORE))"
	@$(LD) -r $(LDFLAGS) -o $(DSP_CORE) $(LINK_OBJECTS)

$(NATIVE_IMPL) $(X86_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(FILE) -o $(@)
	
$(SSE_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(SSE_INSTR_SET) $(INCLUDE) $(FILE) -o $(@)
	
$(SSE3_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(SSE3_INSTR_SET) $(INCLUDE) $(FILE) -o $(@)
	
$(SSE4_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(SSE4_INSTR_SET) $(INCLUDE) $(FILE) -o $(@)

$(AVX_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(AVX_INSTR_SET) $(INCLUDE) $(FILE) -o $(@)
	