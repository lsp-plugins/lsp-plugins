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
	This plugin applies equal loudness contour corrections to the input signal.
</p>
<p>	
	An equal-loudness contour is a measure of sound pressure level (SPL), over the frequency spectrum, for which 
	a listener perceives a constant loudness when presented with pure steady tones. The unit of measurement 
	for loudness levels is the phon and is arrived at by reference to equal-loudness contours.
	Additionally, the phon unit shows the actual SPL (in decibels) of the pure sine wave at the 1000 Hz frequency. 
	By definition, two sine waves of differing frequencies are said to have equal-loudness level measured
	in phons if they are perceived as equally loud by the average young person without significant hearing
	impairment.
</p>
<p>
	The figure below illustrates the measured equal loudness contours for previous standards -
	Fletcher and Munson (published in 1933) and Robinson and Dadson (published 1956) and the actual
	ISO 226:2003 standard published in 2003:
</p>
<?php out_image('graph/equal-loudness-contours', 'Equal loudness contours defined by ISO-226-2003 standard') ?>
<p>
    The equal loudness contour curves show two aspects of the way sounds are perceived. 
    For one, sounds at different frequencies need to have substantially different SPL levels 
    to sound equally loud so that, for instance, a 30Hz sinewave and hence the 30Hz content of 
    any sound in general needs to be more than 10 db SPL louder than the 1000Hz content to sound 
    equally loud, when the latter is at 100 dB SPL, i.e. at 100 phon.
</p>
<p>
    Apart from that though the curves do not retain their overall shape as the volume changes, so 
    that if you attenuated the 1000 Hz content by 20 dB SPL (ending up at 80 phon), you would need 
    to attenuate the 30 Hz content by only 10 dB SPL to maintain equal loudness. Seen the other way 
    round, if you simply attenuated all content by 20 dB SPL, the 30 Hz content would sound significantly
    less loud in comparison to the 1000 Hz content than before, i.e. turning a volume knob changes
    equalization along with volume.
</p>
<p>
    The upshot of this in practice is that if a track was mixed to sound a certain way at, say 83 dB SPL,
    i.e. with the equipment set up so that it will reproduce a 1000 Hz tone at 83 dB SPL, then it will no
    longer sound the same when listened to at lower levels, as the bass and treble content will sound less
    loud than the midrange content. Conversely, if it was mixed at lower levels, but reproduced at higher
    levels, bass and treble content will be boosted.
</p>
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
	responses for other	SPL levels are computed by subtracting the corresponding equal loudness curve from the
	83 phon curve.
</p>
<p>
	This plugin performs such frequency response computations and applies the computed frequency
	response to the input signal depending on the output volume settings. Additionally it can provide ear
	protection by applying hard-clipping to the output signal if it exceeds the allowed configurable level.
</p>

<p><b>Preparing the sound system for work.</b></p>
<p>
	The plugin assumes the 83 phon equal loudness curve being a flat frequency response. For that case the
	audio system needs to be callibrated for the 83 dB output loudness. The plugin can generate a 0 dBFS sine wave 
	digital reference signal at 1000 Hz which can be reproduced by the audio system and measured by the SPL meter.
</p>
<p>
	Since measuried signal at 1 kHz matches the same loudness level in phons, the amplification knob of the audio
	system should be adjusted to reach the 83 dB SPL level on the SPL meter. After that, the <b>Volume</b> knob
	can be used to attenuate the overall loudness of the system while preserving equal loudness perception by the ear.   
</p>

<p><b>Controls:</b></p>
<ul>
	<li><b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the plugin bypasses signal.</li>
	<li><b>Input</b> - the input gain settings, allows to adjust the level of the input signal to the desired level.</li>
	<li><b>Reference</b> - turns on the reference sine wave with 0 dBFS amplitude and 1000 Hz frequency for callibration purposes.</li>
	<li><b>Contour</b> - allows to select different equal loudness contour:</li>
	<ul>
		<li><b>Flat</b> - applies flat frequency response to the whole spectum. Is similar to just a gain knob but useful to perform a comparison to other mode</li>
		<li><b>ISO 226:2003</b> - applies recent ISO 226-2003 contours to the signal</li>
		<li><b>Fletcher and Munson</b> - applies Fletcher and Munson (1933) contours to the signal</li>
		<li><b>Robinson and Dadson</b> - applies Robinson and Dadson (1956) contours to the signal</li>
	</ul>
	<li><b>FFT</b> - allows to select size of the FFT frame used for the processing. The larger FFT frame is, the more precize the curve is approximated and the more latency the plugin introduces</li>
	<li><b>Volume</b> - the output volume of the signal with applied equal loudness contour, controls the loudness of the 1 kHz pure sine wave</li>
	<li><b>Clipping</b> - allows to enable and set the gap level for the hard clipping of the output signal</li>
	<li><b>Reset</b> - reset leds that indicate that hard clip occured in the clipping mode</li>
	<li><b>Relative</b> - draws the frequency response in relative to the loudness mode when enabled</li>
</ul>

