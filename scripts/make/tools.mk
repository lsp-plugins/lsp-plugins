export CC              ?= gcc
export CXX             ?= g++
export PHP             ?= php
export LD              ?= ld

FLAG_RELRO              = -Wl,-z,relro,-z,now

ifeq ($(BUILD_PLATFORM),Solaris)
  FLAG_RELRO              =
endif

export MAKE_OPTS        = -s
export CFLAGS          += $(CC_ARCH) -std=c++98 -fdata-sections -pthread -ffunction-sections -fno-exceptions -fno-asynchronous-unwind-tables -Wall -pipe -fno-rtti $(CC_FLAGS) -DLSP_MAIN_VERSION=\"$(VERSION)\" -DLSP_INSTALL_PREFIX=\"$(PREFIX)\"
export CXXFLAGS        += $(CC_ARCH) -std=c++98 -fdata-sections -pthread -ffunction-sections -fno-exceptions -fno-asynchronous-unwind-tables -Wall -pipe -fno-rtti $(CC_FLAGS) -DLSP_MAIN_VERSION=\"$(VERSION)\" -DLSP_INSTALL_PREFIX=\"$(PREFIX)\"
export SO_FLAGS         = $(CC_ARCH) -Wl,-rpath,$(LD_PATH) $(FLAG_RELRO) -Wl,--gc-sections -shared -Llibrary -lc -fPIC
export MERGE_FLAGS      = $(LD_ARCH) -r
export EXE_TEST_FLAGS   = $(LDFLAGS) $(CC_ARCH) -Wl,-rpath,$(LD_PATH)
export EXE_FLAGS        = $(LDFLAGS) $(CC_ARCH) -Wl,-rpath,$(LD_PATH) $(FLAG_RELRO) -Wl,--gc-sections
