<?php
	plugin_header();
	
	$sc     =   (strpos($PAGE, 'sc_') === 0);
	$m      =   (strpos($PAGE, '_mono') > 0) ? 'm' : (
				(strpos($PAGE, '_stereo') > 0) ? 's' : (
				(strpos($PAGE, '_lr') > 0) ? 'lr' : (
				(strpos($PAGE, '_ms') > 0) ? 'ms' : '?'
				)));
	$cc     =   ($m == 'm') ? 'mono' : 'stereo';
	$sm     =   ($m == 'ms') ? ' M, S' : (($m != 'm') ? ' L, R' : '');
?>

<!-- TODO -->
