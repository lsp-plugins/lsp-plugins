NATIVE_IMPL             = $(OBJDIR)/native.o
SSE_IMPL                = $(OBJDIR)/sse.o
AVX256_IMPL             = $(OBJDIR)/avx256.o
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
SSE_INSTR_SET           = -mmmx -m3dnow -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2
AVX256_INSTR_SET        = $(SSE_INSTR_SET) -mavx

.PHONY: all target

all: $(DSP_CORE)

target: all

$(DSP_CORE): $(NATIVE_IMPL) $(SSE_IMPL)
	@echo "  $(LD) $(notdir $(DSP_CORE))"
	@$(LD) $(LDFLAGS) -r -o $(DSP_CORE) $(NATIVE_IMPL) $(SSE_IMPL)

$(NATIVE_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(FILE) -o $(@)
	
$(SSE_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(SSE_INSTR_SET) $(INCLUDE) $(FILE) -o $(@)

$(AVX256_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(AVX256_INSTR_SET) $(INCLUDE) $(FILE) -o $(@)
	