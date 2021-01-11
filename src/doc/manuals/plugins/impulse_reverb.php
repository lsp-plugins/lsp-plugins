<?php
	plugin_header();
	
	$s      =   (strpos($PAGE, '_stereo') > 0);
?>
<p>
	This plugin performs highly optimized real time zero-latency convolution
	to the input signal. It can be used as a cabinet emulator, some sort of equalizer
	or as a reverb simulation plugin. All what is needed is audio file(s) with impulse
	response taken from the linear system (cabinet, equalizer or hall/room).
</p>
<p>
	Unlike the Impulsantworten (Impulse Responses) plugin series, up to four input files
	and convolution processors can be used to implement and mix different kinds of reverberations.
</p>
<?php if ($s) { ?>
<p>
	The stereo version of plugin allows to apply the True Stereo Reverberation effect to the input signal.
</p>
<p>
	To apply True Stereo Reverberation, the left input channel should be convolved with
	the left and right channels from the first impulse response file and the right input
	channel is convolved with the left and right channels from the second impulse response file or with
	third and fourth channels from the first impulse response file.
	The two output convolutions' respective left and right components should be then summed into
	a single stereo output. This configuration is necessary to take full advantage of true stereo
	impulse responses.
</p>
<p>
	By default, all controls of convolvers are set up to implement true stereo reverberation
	by setting left and right channels of input file as input source for convolvers 0 and 1,
	and also setting third and fourth channels of input file as input source for convolvers 2 and 3.
	If only stereo files are used to build true stereo reverberation then setting left and right channels of
	second input file as input source for convolvers 2 and 3 will take a true stereo reverberation effect.
</p>
<p>
	To use simple stereo reverberation on default settings, the left channel of input file can be set as
	source of convolver 0 and the right channel of input file can be set as source of convolver 3.
</p>
<?php }?>
<p><b>Controls:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the plugin bypasses signal (but still performs processing).
	</li>
</ul>
<p><b>'Impulse response' section:</b></p>
<ul>
	<li><b>File</b> - file editor selector</li>
	<li><b>FFT frame</b> - the maximum size of the FFT (Fast Fourier Transform) frame that can be used for time-continuous convolution</li>
	<li><b>IR equalizer</b> - enables wet (processed) signal equalization in additional <b>Wet Signal Equalization</b> section</li>
	<li><b>Show</b> - Displays the additional <b>Wet Signal Equalization</b> section in the UI</li>
	<li><b>Reverse</b> - allows to reverse impulse file in time domain.</li>
	<li><b>Head cut</b> - cut amount of milliseconds from the beginning of the impulse files, can be used to remove early reflections of reverb.</li>
	<li><b>Tail cut</b> - cut amount of milliseconds from the end of the impulse files, can be used to remove large reverberation tail.</li>
	<li><b>Fade in</b> - adds additional fading at the beginning of the impulse file.</li>
	<li><b>Fade out</b> - adds additional fading at the end of the impulse file.</li>
	<li><b>Listen</b> - this button allows to listen contents of the audio file.</li>
</ul>
<p><b>'Processors' section:</b></p>
<ul>
	<?php if ($s) { ?>
	<li><b>In L/R</b> - the balance between left and right channels of input signal that will be passed to the processor</li>
	<?php } ?>
	<li><b>Pre-delay</b> - amount of pre-delay added to the processed signal. Can be used to individually control
	    pre-delay of processed signal for each processor that can provide additional stereo effect for reverbs.
	</li>
	<li><b>Source</b> - combos allow to select file and track to use as the convolution for the processor.</li>
	<li><b>Mute</b> - button allows to disable the processor.</li>
	<li><b>Active</b> - led that indicates that the processor is active.</li>
	<li><b>Makeup</b> - amount of gain added to the processed signal for the processor.</li>
	<li><b>Out L/R</b> - the output balance of the processed signal between two channels in the stereo pair.</li>
</ul>

<p><b>'Output' section:</b></p>
<ul>
	<?php if ($s) { ?>	
		<li><b>Dry Pan L/R</b> - the panning of the left and right channels of the dry (unprocessed) signal.</li>
	<?php } else { ?>
		<li><b>Dry Pan L/R</b> - the panning of the dry (unprocessed) signal.</li>
	<?php } ?>
	<li><b>Pre-delay</b> - amount of pre-delay added to the wet (processed) signal.</li>
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
