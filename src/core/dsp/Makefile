NATIVE_IMPL             = $(OBJDIR)/native.o
SSE_IMPL                = $(OBJDIR)/sse.o
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
INSTR_SET               = -mmmx -m3dnow -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2

.PHONY: all target

all: $(DSP_CORE)

target: all

$(DSP_CORE): $(NATIVE_IMPL) $(SSE_IMPL)
	@echo "  $(LD) $(notdir $(OBJ_CORE))"
	@$(LD) $(LDFLAGS) -r -o $(OBJ_CORE) $(NATIVE_IMPL) $(SSE_IMPL)

$(NATIVE_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(FILE) -o $(@)
	
$(SSE_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INSTR_SET) $(INCLUDE) $(FILE) -o $(@)
