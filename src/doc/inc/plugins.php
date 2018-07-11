<?php
	
	// Sort plugins by name
	function plugin_cmp($a, $b)
	{
		$a = $a['name'];
		$b = $b['name'];
		return ($a == $b) ? 0 : ($a > $b) ? 1 : -1;
	}
	
	function sort_plugins()
	{
		global $PLUGINS;
		usort($PLUGINS, 'plugin_cmp');
	}
	
	function plugin_header()
	{
		global $PLUGINS, $PAGE, $DOCROOT;

		// Output plugin
		foreach ($PLUGINS as $plugin)
		{
			if ($plugin['id'] != $PAGE)
				continue;
			
			$fmt = array();
			if (isset($plugin['fmt_ladspa']) && ($plugin['fmt_ladspa'] > 0))
				array_push($fmt, 'LADSPA');
			if ((isset($plugin['fmt_lv2'])) && (strlen($plugin['fmt_lv2']) > 0))
				array_push($fmt, 'LV2');
			if ((isset($plugin['fmt_vst'])) && (strlen($plugin['fmt_vst']) > 0))
				array_push($fmt, 'LinuxVST');
			if (isset($plugin['fmt_jack']) && ($plugin['fmt_jack']))
				array_push($fmt, 'JACK');
		
			echo "<img class=\"plugin\" src=\"${DOCROOT}img/plugins/{$plugin['id']}.png\" alt=\"{$plugin['name']}\">\n";
			echo "<p><b>Detailed:&nbsp;</b>LSP {$plugin['name']} - {$plugin['description']} ({$plugin['acronym']})</p>\n";
			echo "<p><b>Formats:&nbsp;</b>" . implode(',&nbsp;', $fmt) . "</p>\n";
			echo "<p><b>Categories:&nbsp;</b>" . implode(',&nbsp;', $plugin['groups']) . "</p>\n";
			echo "<p><b>Developer:&nbsp;</b>{$plugin['author']}</p>\n";
			echo "<p><b>Description:&nbsp;</b></p>\n";
			break;
		}
	}
	
	function plugin_ref($id)
	{
		global $DOCROOT;
		$page       = find_menu_item($id);
		if (!isset($page))
			return;
		
		print("<b><a href=\"${DOCROOT}html/plugins/${page['id']}.html\">LSP " . htmlspecialchars($page['text']) . "</a></b>");
	}

?>
