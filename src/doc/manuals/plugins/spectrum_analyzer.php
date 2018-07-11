<?php
	plugin_header();
	
	$nc     =   (strpos($PAGE, '_x16') > 0) ? 16 : (
				(strpos($PAGE, '_x12') > 0) ? 12 : (
				(strpos($PAGE, '_x8') > 0) ? 8 : (
				(strpos($PAGE, '_x4') > 0) ? 4 : (
				(strpos($PAGE, '_x2') > 0) ? 2 : (
				(strpos($PAGE, '_x1') > 0) ? 1 : '?'
				)))));
	$cd     =   ($nc == 1) ? 'a single audio channel.' : (
				($nc == 2) ? 'a pair of audio channels. Stereo channel may be analyzed by this version of plugin, too.' :
					"${nc} independent channels simultaneously."
				);
?>

<p>This plugin performs spectral analysis of <?= $cd ?>
	It does not affect any changes on the input signal, so can be placed anywhere where the metering is needed.
</p>

<p><b>Controls:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the plugin does not work.
	</li>
	<li><b>On</b> - turns on the analysis of the channel.</li>
	<?php if ($nc > 1) { ?>
	<li><b>Solo</b> - allows to individually inspect the corresponding channel.</li>
	<?php } ?>
	<li><b>Hold</b> - forces the frequency graph of the channel to freeze.</li>
	<li><b>Hue</b> - allows to adjust the color (hue) of the channel.</li>
	<li><b>Shift</b> - allows to adjust the gain of the channel.</li>
	<li><b>Preamp</b> - allows to adjust the overall gain of the analysis.</li>
	<?php if ($nc > 1) { ?>
	<li><b>Channel</b> - allows to select the channel for level metering.</li>
	<?php } ?>
	<li><b>Selector</b> - allows to select the available frequency for level metering.</li>
	<li><b>Reactivity</b> - the reactivity (smoothness) of the spectral analysis.</li>
	<li><b>Level</b> - the level of the selected frequency.</li>
	<li><b>Window</b> - the window applied to the audio data before processing.</li>
	<li><b>Tolerance</b> - the number of points for the FFT (Fast Fourier Transform) analysis.</li>
	<li><b>Envelope</b> - the envelope compensation of the signal.</li>
</ul>

<p><b>Meters:</b></p>
<ul>
	<li><b>Frequency</b> - the current frequency selected by <b>Selector</b> control.</li>
	<li><b>Level</b> - the level of the selected <b>Frequency</b><?php
		if ($nc > 1) print " for the channel specified by <b>Channel</b> control";
	?>.</li>
</ul>
