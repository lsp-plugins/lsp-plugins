<?php
	plugin_header();
	
	$nc     =   (strpos($PAGE, '_x16') > 0) ? 16 : (
				(strpos($PAGE, '_x12') > 0) ? 12 : (
				(strpos($PAGE, '_x8') > 0) ? 8 : (
				(strpos($PAGE, '_x4') > 0) ? 4 : (
				(strpos($PAGE, '_x2') > 0) ? 2 : (
				(strpos($PAGE, '_x1') > 0) ? 1 : '?'
				)))));
	$cd     =   ($nc == 1) ? 'a single audio channel' : (
				($nc == 2) ? 'a pair of audio channels' :
					"${nc} independent channels simultaneously"
				);
?>

<p>
	This plugin performs spectral analysis of <?= $cd ?>.
	<?php if ($nc > 1) { ?>
		Stereo channels also are possible to analyze by utilizing special modes and selecting corresponsing channels.
	<?php } ?>
	It does not affect any changes to the input signal, so can be placed anywhere where the metering is needed.
</p>

<p><b>Controls:</b></p>
<ul>
	<li><b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the plugin does not work.</li>
	<li><b>Mode</b> - depending on operating modes, different analysis can be performed:</li>
	<ul>
    	<li><b>Analyzer</b> - performs analysis of <?= $cd ?>.</li>
    	<?php if ($nc > 2) {?>
    	<li><b>Analyzer Stereo</b> - performs analysis of two selected audio channels.</li>
    	<?php }?>
    	<li><b>Mastering</b> - performs less accurate but more demonstrative analysis of <?= $cd ?>.</li>
    	<?php if ($nc > 2) {?>
    	<li><b>Mastering Stereo</b> - performs less accurate but more demonstrative analysis of two selected channels.</li>
    	<?php }?>
    	<li><b>Spectralizer</b> - draws spectral analysis of single channel.</li>
    	<?php if ($nc >= 2) {?>
    	<li><b>Spectralizer Stereo</b> - draws spectral analysis of <?php if ($nc > 2) {?>two selected channels<?php } else {?>stereo channel <?php }?>.</li>
    	<?php }?>
    </ul>
    <li><b>Freeze</b> - immediately freezes all frequency graphs.</li>
	<li><b>Spectrogram</b> - the rendering mechanism of the spectrogram in the <b>Spectralizer</b> mode:</li>
	<ul>
		<li><b>Rainbow</b> - each dot on a spectrogram changes it's color's hue component depending on the amplitude of the signal starting with <b>Hue</b> of the selected channel.</li>
		<li><b>Fog</b> - each dot on a spectrogram changes it's transparency depending on the amplitude of the signal.</li>
		<li><b>Color</b> - each dot on a spectrogram changes it's color's saturation component depending on the amplitude of the signal.</li>
		<li><b>Lightning</b> - each dot changes it's solor's lightness component depending on the amplitude of the signal, the <b>Hue</b> is interpreted as a middle value of the possible value range.</li>
		<li><b>Lightness</b> - each dot changes it's solor's lightness component depending on the amplitude of the signal, the <b>Hue</b> is interpreted as a maximum value of the possible value range.</li>
	</ul>
    <?php if ($nc > 1) {?>
		<li><b>Channel(s)</b> - numbers of channels currently displayed on the graph.</li>
	<?php } ?>
	<li><b>Log scale</b> - the way how dots on the spectrogram are rendered. When button is down, logarithmic scale is used to translate amplitude into dot's color.</li>
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
	<li><b>Zoom</b> - zoom fader, allows to adjust zoom on the frequency chart.</li>
</ul>

<p><b>Meters:</b></p>
<ul>
	<li><b>Frequency</b> - the current frequency selected by <b>Selector</b> control.</li>
	<li><b>Level</b> - the level of the selected <b>Frequency</b><?php
		if ($nc > 1) print " for the channel specified by <b>Channel</b> control";
	?>.</li>
</ul>
