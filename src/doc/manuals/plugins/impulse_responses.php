<?php
	plugin_header();
	
	$s      =   (strpos($PAGE, '_stereo') > 0);
?>
<p>
	This plugin performs highly optimized real time zero-latency convolution
	to the input signal. It can be used as a cabinet emulator, some sort of equalizer
	or as a reverb simulation plugin. All what is needed is audio file with impulse
	response taken from the linear system (cabinet, equalizer or hall/room).
</p>
<p><b>Controls:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the plugin bypasses signal (but still performs processing).
	</li>
	<li><b>FFT frame</b> - the maximum size of the FFT (Fast Fourier Transform) frame that can be used for time-continuous convolution</li>
	<?php if ($s) { ?>
	<li><b>File</b> - file selector, allows to load additional file that can be taken as impulse response for one of audio channels.</li>
	<?php } ?>
	<li><b>IR equalizer</b> - enables wet (processed) signal equalization in additional <b>Wet Signal Equalization</b> section</li>
	<li><b>Show</b> - Displays the additional <b>Wet Signal Equalization</b> section in the UI</li>
	<li><b>Head cut</b> - cut amount of milliseconds from the beginning of the impulse files, can be used to remove early reflections of reverb.</li>
	<li><b>Tail cut</b> - cut amount of milliseconds from the end of the impulse files, can be used to remove large reverberation tail.</li>
	<li><b>Fade in</b> - adds additional fading at the beginning of the impulse file.</li>
	<li><b>Fade out</b> - adds additional fading at the end of the impulse file.</li>
	<li><b>Listen</b> - this button allows to listen contents of the audio file.</li>
	<li><b>Source</b> - this combo allows to select file channel to use for the convolution.</li>
	<li><b>Active</b> - led that indicates that convolution is applied to the channel.</li>
	<li><b>Pre-delay</b> - amount of pre-delay added to the processed signal.
		<?php if ($s) {?>
			Can be used to individually control pre-delay of processed signal for each channel in the stereo pair that can
			provide additional stereo effect for reverbs.
		<?php } ?>
	</li>
	<li><b>Makeup</b> - amount of gain added to the processed signal.
		<?php if ($s) {?>
			Can be used to individually control amount of processed signal for each channel in the stereo pair.
		<?php } ?>
	</li>
	<li><b>Dry</b> - amount of gain applied to the dry (unprocessed) signal.</li>
	<li><b>Wet</b> - amount of gain additionally applied to the wet (processed) signal.</li>
	<li><b>Output</b> - amount of gain additionally applied to the output signal.</li>
</ul>
<p><b>'Wet Signal Equalization' section</b> - visible only when <b>IR equalizer</b> parameter is turned on:</p>
<ul>
	<li><b>Low-cut</b> - sets the slope of the high-pass butterworth filter, possible slopes are 6, 12 and 18 dB/octave.</li>
	<li><b>Low-cut freq</b> - the cutoff frequency of the high-pass butterworth filter.</li>
	<li><b>Faders</b> - faders that allow to change the loudness of eight corresponding frequency bands in range of -12..+12 dB</li>
	<li><b>High-cut</b> - sets the slope of the low-pass butterworth filter, possible slopes are 6, 12 and 18 dB/octave.</li>
	<li><b>High-cut freq</b> - the cutoff frequency of the low-pass butterworth filter.</li>
</ul>