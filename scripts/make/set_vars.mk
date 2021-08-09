PREFIX_FILE            := install-prefix.txt
MODULES_FILE           := install-modules.txt
R3D_BACKENDS_FILE      := install-r3d-backends.txt
BUILD_PROFILE_FILE     := install-build-profile.txt

LV2_UI                 ?= 1
VST_UI                 ?= 1

# Detect operating system
ifndef BUILD_SYSTEM
  ifeq ($(findstring Windows,$(OS)),Windows)
    BUILD_SYSTEM = Windows
  else
    BUILD_SYSTEM = $(shell uname -s 2>/dev/null || echo "Unknown")
  endif
endif

ifndef BUILD_PLATFORM
  BUILD_PLATFORM  = Unknown

  ifeq ($(BUILD_SYSTEM),Windows)
    BUILD_PLATFORM          = Windows
  endif
  ifeq ($(findstring BSD,$(BUILD_SYSTEM)),BSD)
    BUILD_PLATFORM          = BSD
  endif
  ifeq ($(findstring Linux,$(BUILD_SYSTEM)),Linux)
    BUILD_PLATFORM          = Linux
  endif
  ifeq ($(findstring SunOS,$(BUILD_SYSTEM)),SunOS)
    BUILD_PLATFORM          = Solaris
  endif
endif

export BUILD_SYSTEM
export BUILD_PLATFORM
export LV2_UI
export VST_UI

# Detect host processor architecture
ifeq ($(BUILD_PLATFORM),Windows)
  ifndef HOST_BUILD_PROFILE
    HOST_BUILD_ARCH           = i586
    HOST_BUILD_PROFILE             = i586
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
      HOST_BUILD_ARCH         = i586
      HOST_BUILD_PROFILE      = i586
    endif
    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
      HOST_BUILD_ARCH              = x86_64
      HOST_BUILD_PROFILE           = x86_64
    endif
  endif
else # BUILD_PLATFORM != Windows
  ifndef HOST_BUILD_PROFILE
    HOST_BUILD_ARCH    ?= $(shell cat "$(OBJDIR)/$(BUILD_PROFILE_FILE)" 2>/dev/null || uname -m)
    HOST_BUILD_PROFILE  = $(HOST_BUILD_ARCH)
    ifeq ($(patsubst armv6%,armv6,$(HOST_BUILD_ARCH)), armv6)
      HOST_BUILD_PROFILE      = armv6a
    endif
    ifeq ($(patsubst armv7%,armv7,$(HOST_BUILD_ARCH)), armv7)
      HOST_BUILD_PROFILE      = armv7a
    endif
    ifeq ($(patsubst armv8%,armv8,$(HOST_BUILD_ARCH)), armv8)
      HOST_BUILD_PROFILE      = armv8a
    endif
    ifeq ($(patsubst aarch64%,aarch64,$(HOST_BUILD_ARCH)), aarch64)
      HOST_BUILD_PROFILE      = aarch64
    endif
    ifeq ($(HOST_BUILD_ARCH),x86_64)
      HOST_BUILD_PROFILE      = x86_64
    endif
    ifeq ($(HOST_BUILD_ARCH),amd64)
      HOST_BUILD_PROFILE      = x86_64
    endif
    ifeq ($(HOST_BUILD_ARCH),i86pc)
      HOST_BUILD_PROFILE      = x86_64
    endif
    ifeq ($(patsubst i%86,i586,$(HOST_BUILD_ARCH)), i586)
      HOST_BUILD_PROFILE      = i586
    endif
  endif
endif # BUILD_PLATFORM != Windows

# Detect target processor architecture
ifeq ($(BUILD_PLATFORM),Windows)
  ifndef BUILD_PROFILE
    BUILD_ARCH                = i586
    BUILD_PROFILE             = i586
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
      BUILD_ARCH                = i586
      BUILD_PROFILE             = i586
    endif
    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
      BUILD_ARCH                = x86_64
      BUILD_PROFILE             = x86_64
    endif
  endif
else # BUILD_PLATFORM != Windows
  ifndef BUILD_PROFILE
    BUILD_ARCH              ?= $(shell cat "$(OBJDIR)/$(BUILD_PROFILE_FILE)" 2>/dev/null || uname -m)
    BUILD_PROFILE           = $(BUILD_ARCH)
    ifeq ($(patsubst armv6%,armv6,$(BUILD_ARCH)), armv6)
      BUILD_PROFILE           = armv6a
    endif
    ifeq ($(patsubst armv7%,armv7,$(BUILD_ARCH)), armv7)
      BUILD_PROFILE           = armv7a
    endif
    ifeq ($(patsubst armv8%,armv8,$(BUILD_ARCH)), armv8)
      BUILD_PROFILE           = armv8a
    endif
    ifeq ($(patsubst aarch64%,aarch64,$(BUILD_ARCH)), aarch64)
      BUILD_PROFILE           = aarch64
    endif
    ifeq ($(BUILD_ARCH),x86_64)
      BUILD_PROFILE           = x86_64
    endif
    ifeq ($(BUILD_ARCH),amd64)
      BUILD_PROFILE           = x86_64
    endif
    ifeq ($(BUILD_ARCH),i86pc)
      BUILD_PROFILE           = x86_64
    endif
    ifeq ($(patsubst i%86,i586,$(BUILD_ARCH)), i586)
      BUILD_PROFILE           = i586
    endif
  endif
endif # BUILD_PLATFORM != Windows

HOST_BUILD_PROFILE ?= $(shell uname -m)

export BUILD_PROFILE
export HOST_BUILD_PROFILE
