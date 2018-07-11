<?php
function equalizer_doc($filters, $mode)
{
?>
<p>
	This plugin performs equalization in <?php
	if ($mode == 'ms') echo "Mid-Side";
	elseif ($mode == 'lr') echo "Left-Right";
	elseif ($mode == 'mono') echo "Mono";
	elseif ($mode == 'stereo') echo "Stereo";
	?> mode. <?php echo $filters; ?> filters available for signal processing.
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
<?php
		if ($mode == 'ms')
		{
?>
	<li><b>Mid</b> - enables the frequency chart and FFT analysis for the middle channel.</li>
	<li><b>Side</b> - enables the frequency chart and FFT analysis for the side channel.</li>
	<li><b>Listen</b> - allows to listen middle channel and side channel. Passes middle channel to the left output channel, side channel to the right output channel.</li>
<?php
		}
		elseif ($mode != 'mono')
		{
?>
	<li><b>Left</b> - enables the <?php if ($mode != 'stereo') echo "frequency chart and "; ?>FFT analysis for the left channel.</li>
	<li><b>Right</b> - enables the <?php if ($mode != 'stereo') echo "frequency chart and "; ?>FFT analysis for the right channel.</li>
<?php
		}
?>
</ul>
<p><b>'Signal' section:</b></p>
<ul>
	<li><b>Input</b> - input signal amplification.</li>
	<li><b>Output</b> - output signal amplification.</li>
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

<?php
}
?>