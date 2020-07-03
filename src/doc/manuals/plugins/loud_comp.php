<?php
	plugin_header();
	
	$m      =   ($PAGE == 'loud_comp_mono') ? 'm' : (
	            ($PAGE == 'loud_comp_stereo') ? 's' : null
	            );
	$c      =   ($m == 'm') ? 'mono' : (
	            ($m == 's') ? 'stereo' : null
	            );
?>

<p>
	This plugin applies equal loudness contours to the input signal which are defined in the ISO-226-2003 standard.
</p>
<p>	
	An equal-loudness contour is a measure of sound pressure level, over the frequency spectrum, for which 
	a listener perceives a constant loudness when presented with pure steady tones. The unit of measurement 
	for loudness levels is the phon and is arrived at by reference to equal-loudness contours. By definition, 
	two sine waves of differing frequencies are said to have equal-loudness level measured in phons if they 
	are perceived as equally loud by the average young person without significant hearing impairment.
	Phon unit shows the actual SPL (in decibels) of the pure sine wave at the 1000 Hz frequency. 
</p>
<p>
	The figure below illustrates the measured equal loudness contours for previous standards -
	Fletcher and Munson (published in 1933) and Robinson and Dadson (published 1956) and the actual ISO 
	standard published in 2003:
</p>
<?php out_image('graph/equal-loudness-contours', 'Equal loudness contours defined by ISO-226-2003 standard') ?>
<p>
	Usage of equal-loudness contours solves many mixing problems that every sound engineer meets while
	working on the track. The main problem is that human ear perceives different frequencies for
	different volume settings in a different way. In other words, applying changes to the mix on the low
	volume settings may	cause unexpected (sometimes horrible) sounding of the mix at the maximum loudness.
</p>
<p>
	To avoid this, the calibration of the audio system is peformed, so the digital signal with the maximized
	loudness gives a 83 dB SPL level at the output. The 83 Phon equal loudness curve (can be linerarly
	approximated from other curves on the figure) is considered having flat frequency response, frequency
	responses for other	SPL levels be computed by subtracting the corresponding equal loudness curve from the
	83 phon curve.
</p>
<p>
	This plugin performs such frequency response computations and applies the computed frequency
	response to the input signal depending on the output volume settings. Additionally it can provide ear
	protection by applying hard-clipping to the output signal if it exceeds the allowed configurable level.
</p>
<p><b>Controls:</b></p>
<ul>
	<li><b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the plugin bypasses signal.</li>
	<li><b>Input</b> - the input gain settings, allows to adjust the level of the input signal to the desired level.</li>
	<li><b>Contour</b> - allows to select different equal loudness contour:</li>
	<ul>
		<li><b>Flat</b> - applies flat frequency response to the whole spectum. Is similar to just a gain knob but useful to perform a comparison to other mode</li>
		<li><b>ISO-226-2003</b> - applies recent ISO 226-2003 contours to the signal</li>
		<li><b>Fletcher and Munson</b> - applies Fletcher and Munson (1933) contours to the signal</li>
		<li><b>Robinson and Dadson</b> - applies Robinson and Dadson (1956) contours to the signal</li>
	</ul>
	<li><b>FFT</b> - allows to select size of the FFT frame used for the processing. The larger FFT frame is, the more precize the curve is approximated and the more latency the plugin introduces</li>
	<li><b>Volume</b> - the output volume of the signal with applied equal loudness contour, controls the loudness of the 1 kHz pure sine wave</li>
	<li><b>Clipping</b> - allows to enable and set the gap level for the hard clipping of the output signal</li>
	<li><b>Reset</b> - reset leds that indicate that hard clip occured in the clipping mode</li>
	<li><b>Relative</b> - draws the frequency response in relative to the loudness mode when enabled</li>
</ul>

