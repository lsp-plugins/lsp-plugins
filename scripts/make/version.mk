PRODUCT                 = lsp
ARTIFACT_ID             = $(PRODUCT)-plugins
R3D_ARTIFACT_ID         = $(ARTIFACT_ID)-r3d

# Package version
ifndef LSP_VERSION
  LSP_VERSION                 = 1.1.29
endif

export LSP_VERSION
