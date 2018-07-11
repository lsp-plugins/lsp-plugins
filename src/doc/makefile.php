<?php
	// Pass path to root as the first argument
	$PHPDIR     = isset($argv[1]) ? $argv[1] : '.';

	// Include common modules
	require_once("${PHPDIR}/inc/menu.php");
	require_once("${PHPDIR}/inc/plugins.php");

	// Include configuration
	require_once("${PHPDIR}/config/plugins.php");
	sort_plugins();
	
	require_once("${PHPDIR}/config/menu.php");
	
	// Generate list of files
	$ITEMS      = array();
	$HTML_FILES = array('$(HTMLDIR)/index.html');
	$HTML_DIRS  = array();
	raw_menu_list($ITEMS);
	foreach ($ITEMS as &$item)
	{
		$item['dir']    = 'html' . ((isset($item['parent'])) ? "/" . $item['parent'] : "");
		$item['html']   = $item['dir'] . '/' . $item['id'] . ".html";
		
		$HTML_DIRS[$item['dir']] = "$(HTMLDIR)/" . $item['dir'];
		array_push($HTML_FILES, "$(HTMLDIR)/" . $item['html']);
	}
?>
# Auto generated makefile, do not edit

HTMLDIR     = $(BUILDDIR)/html
FILE        = $(@:$(HTMLDIR)/%.html=%.html)
PAGE        = $(patsubst %.html,%,$(notdir $(@)))
DIR         = $(@:$(HTMLDIR)/%=%)
DIRS        = <?php print(implode(" \\\n\t\t", $HTML_DIRS) . "\n") ?>
FILES       = <?php print(implode(" \\\n\t\t", $HTML_FILES) . "\n") ?>

.PHONY: all target

all: $(DIRS) $(FILES)

target: all

# Common rules
$(DIRS):
	@echo "  mkdir $(DIR)"
	@mkdir -p $(@)  

$(FILES): $(DIRS)
	@echo "  $(PHP) $(HTMLDIR) $(FILE)"
	@$(PHP) -f index.php $(PAGE) >$(HTMLDIR)/$(FILE)
