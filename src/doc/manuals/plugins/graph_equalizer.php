<?php
	plugin_header();
	
	$nf     =   (strpos($PAGE, '_x32') > 0) ? 32 : 16;
	$m      =   (strpos($PAGE, '_mono') > 0) ? 'm' : (
				(strpos($PAGE, '_stereo') > 0) ? 's' : (
				(strpos($PAGE, '_lr') > 0) ? 'lr' : (
				(strpos($PAGE, '_ms') > 0) ? 'ms' : '?'
				)));
	$cc     =   ($m == 'm') ? 'mono' : 'stereo';
?>
<?php require_once("${DOC_BASE}/manuals/common/filters.php"); ?>
<p>
	This plugin performs graphic equalization of <?= $cc ?> channel<?php 
	if ($m == 'ms') echo " in Mid-Side mode";
	elseif ($m == 'lr') echo " by applying individual equalization to left and right channels separately";
	?>. Up to <?= $nf ?> frequency bands are available for correction in range of 72 dB (-36..+36 dB).
</p>
<p><b>Meters:</b></p>
<ul>
	<?php if ($m == 'ms') { ?>
		<li><b>Input</b> - the level meter for left and right channels of the input signal.
		If <b>Listen</b> button is pressed, it shows the level of middle and side channels of the input signal respectively.</li>
		<li><b>Output</b> - the level meter for left and right channels of the output signal.
		If <b>Listen</b> button is pressed, it shows the level of middle and side channels of the output signal respectively.</li>
	<?php } elseif ($m != 'm') { ?>
		<li><b>Input</b> - the level meter for left and right channels of the input signal.</li>
		<li><b>Output</b> - the level meter for left and right channels of the output signal.</li>
	<?php } else { ?>
		<li><b>Input</b> - the level meter of the input mono signal.</li>
		<li><b>Output</b> - the level meter of the output mono signal.</li>
	<?php } ?>
</ul>
<p><b>Controls:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the plugin bypasses signal.
	</li>
	<li><b>Mode</b> - equalizer slope and working mode, enables the following settings:</li>
	<ul>
		<li><b>BT48</b> - Filters with 48dB/oct slope, bilinear Z-transform.</li>
		<li><b>MT48</b> - Filters with 48dB/oct slope, matched Z-transform.</li>
		<li><b>BT72</b> - Filters with 72dB/oct slope, bilinear Z-transform.</li>
		<li><b>MT72</b> - Filters with 72dB/oct slope, matched Z-transform.</li>
		<li><b>BT96</b> - Filters with 96dB/oct slope, bilinear Z-transform.</li>
		<li><b>MT96</b> - Filters with 96dB/oct slope, matched Z-transform.</li>
		<li><b>IIR</b> - Infinite Impulse Response filters, nonlinear minimal phase. In most cases does not add noticeable latency to output signal.</li>
		<li><b>FIR</b> - Finite Impulse Response filters with linear phase, finite approximation of equalizer's impulse response. Adds noticeable latency to output signal.</li>
		<li><b>FFT</b> - Fast Fourier Transform approximation of the frequency chart, linear phase. Adds noticeable latency to output signal.</li>
		<li><b>SPM</b> - Spectral Processor Mode of equalizer, equalizer transforms the magnitude of signal spectrum instead of applying impulse response to the signal.</li>
	</ul>
	<?php if ($m == 'ms') { ?>
	<li><b>Mid</b> - button enables the frequency chart and FFT analysis for the middle channel, knob allows to adjust the level of the middle channel.</li>
	<li><b>Side</b> - button enables the frequency chart and FFT analysis for the side channel, knob allows to adjust the level of the side channel.</li>
	<li><b>Listen</b> - allows to listen middle channel and side channel. Passes middle channel to the left output channel, side channel to the right output channel.</li>
	<?php } elseif ($m != 'm') { ?>
	<li><b>Left</b> - enables the <?php if ($m != 's') echo "frequency chart and "; ?>FFT analysis for the left channel.</li>
	<li><b>Right</b> - enables the <?php if ($m != 's') echo "frequency chart and "; ?>FFT analysis for the right channel.</li>
	<?php } ?>
	<li><b>Zoom</b> - zoom fader, allows to adjust zoom on the frequency chart.</li>
</ul>
<p><b>'Signal' section:</b></p>
<ul>
	<li><b>Input</b> - input signal amplification.</li>
	<li><b>Output</b> - output signal amplification.</li>
	<?php if ($m != 'm') { ?>
	<li><b>Balance</b> - balance between left and right output signal.</li>
	<?php }?>
</ul>
<p><b>'Analysis' section:</b></p>
<ul>
	<li><b>FFT</b> - enables FFT analysis before or after filter processing stage.</li>
	<li><b>Reactivity</b> - the reactivity (smoothness) of the spectral analysis.</li>
	<li><b>Shift</b> - allows to adjust the overall gain of the analysis.</li>
</ul>
<p><b>'Bands' section:</b></p>
<ul>
	<li><b>On</b> - enables the filter for the corresponding band.</li>
	<li><b>S</b> - the soloing button, allows to inspect corresponding band.</li>
	<li><b>M</b> - the mute button, allows to mute corresponding filters.</li>
	<li><b>Gain</b> - the amplifiction gain of the band.</li>
</ul>
