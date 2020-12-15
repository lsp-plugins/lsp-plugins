<?php
	plugin_header();
	
	$m      =   (strpos($PAGE, '_mono') > 0) ? 'm' : (
				(strpos($PAGE, '_stereo') > 0) ? 's' : (
				(strpos($PAGE, '_lr') > 0) ? 'lr' : (
				(strpos($PAGE, '_ms') > 0) ? 'ms' : '?'
				)));
?>

<p>
    <?php if ($m == 'm') { ?>
    	This plugin performs split of mono input signal into separate frequency bands passed to individual mono output channels.
    	Additinally it provides output monitoring channel to control individual channels or result of summing signals passed
    	to individual mono output channels.
    <?php } elseif ($m == 's') { ?>
		This plugin performs split of stereo input signal into separate frequency bands passed to individual stereo output channels.
		Additinally it provides output monitoring channel to control individual channels or result of summing signals
		passed to individual stereo output channels.
	<?php } elseif ($m == 'lr') { ?>
		This plugin performs split of individual channels of the stereo input stereo signal into separate frequency bands passed to 
		individual stereo output channes. Additinally it provides output monitoring channel to control individual channels or
		result of summing signals passed to individual stereo output channels.
	<?php } else { ?>
		This plugin converts the stereo input singnal into Mid/Side signal and performs split of the Mid/Side signal into separate frequency
		bands passed to individual Mid/Side output channels. Additionally it provides output monitoring channel to control the result of summing
		signals passed to individual Mid/Side output channels. The output Mid/Side signal also can be inspected by pressing <b>MS Listen</b> button.
	<?php } ?>
	The additional benefit of using monitoring channel is that the plugin may be placed in the middle of track/bus without creating an additional
	bus for processing.
</p>
<p>
    By default plugin enables 4 frequency bands but overall number of used frequency bands may vary from 1 to 8.
    Each range of the frequency band can be adjusted by specifying the slope of the corresponding Low-pass and High-pass filters.
    The frequency graph allows to view characteristics of filters for each frequency band and the result of summing the frequency response
    for all filters. For better audio engineering the plugin addionally provides FFT analysis of input and output signal.
</p>
<p>
	Note that <b>Phase</b> and <b>Delay</b> band parameters do not affect the estimated frequency graph which is computed
	for non-inverted band phase and zero delay value. For another settings, the actual frequency response can be obtained from
	the FFT analysis of the output signal.
</p>

<p><b>Controls:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, passes the signal from the input of plugin to the monitoring output without any changes.
	</li>
	<li><b>FFT In</b> - enables FFT analysis of the input signal for the corresponding input channel.
	<li><b>FFT Out</b> - enables FFT analysis of the input signal for the corresponding input channel.
	<li><b>Filters</b> - shows filter characteristics of each band for the corresponding channel.
	<li><b>Curve</b> - shows the output signal curve of each band for the corresponding channel.
	<?php if ($m == 'ms') { ?>
	<li><b>MS Listen</b> - passes Mid/Side signal to the monitoring channel instead of stereo signal.
	<?php } ?>
	<li><b>Zoom</b> - zoom fader, allows to adjust zoom on the frequency chart.</li>
</ul>
<p><b>'Processor' section:</b></p>
<ul>
	<li><b>Slope</b> - allows to select the slope of filters attached to corresponding frequency band or disable the frequency band.</li>
	<li><b>Range</b> - allows to adjust the frequency range for each band.</li>
	<li><b>Gain</b> - adjusts gain to the corresponding frequency band.</li>
	<li><b>Solo</b> - enables soloing of the corresponding frequency band for the monitoring channel.</li>
	<li><b>Mute</b> - mutes the corresponding frequency band for the monitoring channel.</li>
	<li><b>Phase</b> - inverts phase for the the corresponding frequency band.</li>
	<li><b>Delay</b> - adds specified delay for the corresponding frequency band.</li>
	<li><b>Hue</b> - adjusts hue of the frequency band in the UI.</li>
</ul>
<p><b>'Signal' section:</b></p>
<ul>
	<li><b>Input</b> - the amount of gain applied to the input signal before processing.</li>
	<li><b>Output</b> - the amount of gain applied to the output signal before processing.</li>
</ul>
<p><b>'Analysis' section:</b></p>
<ul>
	<li><b>Reactivity</b> - the reactivity (smoothness) of the spectral analysis.</li>
	<li><b>Shift</b> - allows to adjust the overall gain of the analysis.</li>
</ul>

