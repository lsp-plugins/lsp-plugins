# Command-line flag to silence nested $(MAKE).
MAKE_OPTS              += VERBOSE=$(VERBOSE)
ifneq ($(VERBOSE),1)
.SILENT:
endif

PHPDIR      = $(BUILDDIR)/src/doc
HTMLDIR     = $(BUILDDIR)/html

.DEFAULT_GOAL          := all
.PHONY: all target

target: all

# Common rules
$(PHPDIR)/Makefile:
	@echo "Preparing PHP documentation generator"
	mkdir -p $(PHPDIR)
	mkdir -p $(HTMLDIR)
	cp -rf * $(PHPDIR)/
	cp -r $(addprefix $(ROOTDIR)/, $(RELEASE_TEXT)) $(PHPDIR)/
	cp -rf $(RESDIR)/doc/* $(HTMLDIR)/
	cp -f $(PHP_PLUGINS) $(PHPDIR)/config/plugins.php
	$(PHP) -f makefile.php $(PHPDIR) >$(PHPDIR)/Makefile

all: $(PHPDIR)/Makefile
	@echo "Generating HTML documentation"
	$(MAKE) $(MAKE_OPTS) -C $(PHPDIR)
