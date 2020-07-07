<?php
	plugin_header();
	
	$m      =   ($PAGE == 'loud_comp_mono') ? 'm' : (
	            ($PAGE == 'loud_comp_stereo') ? 's' : null
	            );
	$c      =   ($m == 'm') ? 'mono' : (
	            ($m == 's') ? 'stereo' : null
	            );
?>
