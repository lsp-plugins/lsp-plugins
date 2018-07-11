<?php
	plugin_header();
	
	$nf     =   (strpos($PAGE, '_x32') > 0) ? 32 : 16;
	$m      =   (strpos($PAGE, '_mono') > 0) ? 'm' : (
				(strpos($PAGE, '_stereo') > 0) ? 's' : (
				(strpos($PAGE, '_lr') > 0) ? 'lr' : (
				(strpos($PAGE, '_ms') > 0) ? 'ms' : '?'
				)));
	$cc     =   ($m = 's') ? 'mono' : 'stereo';
?>
<p>
	This plugin performs parameteric equalization of <?= $cc ?> channel<?php 
	if ($m == 'ms') echo " in Mid-Side mode";
	elseif ($m == 'lr') echo " by applying individual equalization to left and right channels separately";
	?>. Up to <?= $nf ?> filters are available for signal processing simultaneously.
</p>
<p><b>Controls:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the plugin bypasses signal.
	</li>
	<li><b>Filters</b> - filter panel selection</li>
	<li><b>Mode</b> - equalizer working mode, enables the following mode for all filters:</li>
	<ul>
		<li><b>IIR</b> - Infinite Impulse Response filters, nonlinear minimal phase.</li>
		<li><b>FIR</b> - Finite Impulse Response filters with linear phase, finite approximation of equalizer's impulse response.</li>
		<li><b>FFT</b> - Fast Fourier Transform approximation of the frequency chart, linear phase.</li>
	</ul>
	<?php if ($m == 'ms') { ?>
	<li><b>Mid</b> - enables the frequency chart and FFT analysis for the middle channel.</li>
	<li><b>Side</b> - enables the frequency chart and FFT analysis for the side channel.</li>
	<li><b>Listen</b> - allows to listen middle channel and side channel. Passes middle channel to the left output channel, side channel to the right output channel.</li>
	<?php } elseif ($m != 'm') { ?>
	<li><b>Left</b> - enables the <?php if ($m != 's') echo "frequency chart and "; ?>FFT analysis for the left channel.</li>
	<li><b>Right</b> - enables the <?php if ($m != 's') echo "frequency chart and "; ?>FFT analysis for the right channel.</li>
	<?php } ?>
</ul>
<p><b>'Signal' section:</b></p>
<ul>
	<li><b>Input</b> - input signal amplification.</li>
	<li><b>Output</b> - output signal amplification.</li>
	<?php if ($m != 'm') { ?>
	<li><b>Balance</b> - balance between left and right output signal.</li>
		<?php if ($m == 'ms') { ?>
		<li><b>L</b> - the measured level of the output signal for the left channel, visible only when <b>Listen</b> button is off.</li>
		<li><b>R</b> - the measured level of the output signal for the right channel, visible only when <b>Listen</b> button is off.</li>
		<li><b>M</b> - the measured level of the output signal for the middle channel, visible only when <b>Listen</b> button is on.</li>
		<li><b>S</b> - the measured level of the output signal for the side channel, visible only when <b>Listen</b> button is on</li>
		<?php } else { ?>
		<li><b>L</b> - the measured level of the output signal for the left channel.</li>
		<li><b>R</b> - the measured level of the output signal for the right channel.</li>
		<?php }?>
	<?php } else { ?>
	<li><b>Signal</b> - the measured level of the output signal.</li>
	<?php } ?>
</ul>
<p><b>'Analysis' section:</b></p>
<ul>
	<li><b>FFT</b> - enables FFT analysis before or after processing.</li>
	<li><b>Reactivity</b> - the reactivity (smoothness) of the spectral analysis.</li>
	<li><b>Shift</b> - allows to adjust the overall gain of the analysis.</li>
</ul>
<p><b>'Filters' section:</b></p>
<ul>
	<li><b>Filter</b> - sets up the mode of the selected filter. Three filter classes available in <b>BT</b> (Bilinear transform) and <b>MT</b> (Matched Z-Transform) mappings:</li>
	<ul>
		<li><b>RLC</b> - Very smooth filters based on similar cascades of RLC contours.</li>
		<li><b>BWC</b> - Butterworth-Chebyshev-type-1 based filters.</li>
		<li><b>LRX</b> - Linkwitz-Riley based filters.</li>
	</ul>
	<li><b>Slope</b> - the slope of the filter characteristics.</li>
	<li><b>S</b> - the soloing button, allows to inspect selected filters.</li>
	<li><b>M</b> - the mute button, allows to mute selected filters.</li>
	<li><b>Freq</b> - the cutoff/resonance frequency of the filter.</li>
	<li><b>Gain</b> - the gain of the filter, disabled for lo-pass/hi-pass/notch filters.</li>
	<li><b>Q</b> - the quality factor of the filter.</li>
	<li><b>Hue</b> - the color of the frequency chart of the filter on the graph.</li>
</ul>
