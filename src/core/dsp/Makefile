NATIVE_IMPL             = $(OBJDIR)/native.o
X86_IMPL                = $(OBJDIR)/x86.o
SSE_IMPL                = $(OBJDIR)/sse.o
SSE3_IMPL               = $(OBJDIR)/sse3.o
AVX_IMPL                = $(OBJDIR)/avx.o
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
SSE_INSTR_SET           = -mmmx -m3dnow -msse -msse2
SSE3_INSTR_SET          = -msse3 -mssse3
AVX_INSTR_SET           = $(SSE_INSTR_SET) -mavx

LINK_OBJECTS            = $(NATIVE_IMPL) $(X86_IMPL) $(SSE_IMPL) $(AVX_IMPL) $(SSE3_IMPL)

.PHONY: all target

all: $(DSP_CORE)

target: all

$(DSP_CORE): $(LINK_OBJECTS)
	@echo "  $(LD) $(notdir $(DSP_CORE))"
	@$(LD) $(LDFLAGS) -r -o $(DSP_CORE) $(LINK_OBJECTS)

$(NATIVE_IMPL) $(X86_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(FILE) -o $(@)
	
$(SSE_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(SSE_INSTR_SET) $(INCLUDE) $(FILE) -o $(@)
	
$(SSE3_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(SSE_INSTR_SET) $(SSE3_INSTR_SET) $(INCLUDE) $(FILE) -o $(@)

$(AVX_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(AVX_INSTR_SET) $(INCLUDE) $(FILE) -o $(@)
	