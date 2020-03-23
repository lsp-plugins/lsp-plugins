<!DOCTYPE html>
<?php
	// Main configuration
	require_once('config/config.php');

	// Include common modules
	require_once('inc/menu.php');
	require_once('inc/plugins.php');
	require_once('inc/files.php');

	// Include configuration
	require_once('config/plugins.php');
	sort_plugins();
	
	require_once('config/menu.php');
	require_once('config/youtube.php');
	
	// Determine current page
	$PAGE       = isset($argv[1]) ? $argv[1] : 'index';
	$MENUITEM   = find_menu_item($PAGE);
	if (!isset($MENUITEM))
	    $MENUITEM = array(
	        'id' => 'index',
	        'parent' => null,
	        'text' => 'LSP Plugins Documentation',
	        'root' => '',
	        'path' => '',
	        'file' => $PAGE
	    );
	
	$DOCROOT    = ((strlen($MENUITEM['root']) > 0) ? ($MENUITEM['root'] . '/') : '');
	$HEADER		= $MENUITEM['text'];
	$FILENAME   = (isset($MENUITEM['file'])) ? $MENUITEM['file'] : $PAGE;
	$RES_ROOT   = $DOCROOT;
	$DOC_BASE   = '.';
?>

<html>
	<head>
		<title>Linux Studio Plugins Project</title>

		<meta content="text/html; charset=utf-8" http-equiv="Content-Type">

		<link rel="stylesheet" href="<?= $DOCROOT ?>css/style.css" type="text/css">
	</head>
	<body>
		<div id="body">
			<!-- Header -->
			<div id="header">
				<img src="<?= $DOCROOT ?>img/header.png" alt="Linux Studio Plugins Project" >
			</div>
			
			<!-- Navigation top -->
			<div id="menu">
				<?php require("./manuals/menu.php"); ?>
			</div>
		
			<!-- Main content -->
			<div id="main">
				<?php 
				    if ($MENUITEM['parent'] != 'plugins')
				        echo '<h1>' . htmlspecialchars($HEADER) . '</h1>';
			        require("./manuals/${MENUITEM['path']}/${FILENAME}.php");
				?>
			</div>
			
			<!-- Navigation bottom -->
			<div id="menu">
				<?php require("./manuals/menu.php"); ?>
			</div>
		
			<!-- Footer -->
			<div id="footer">
				<p>(C) Linux Studio Plugins, 2015-2016</p>
				<p>All rights reserved</p>
			</div>
		</div>
	</body>
</html>