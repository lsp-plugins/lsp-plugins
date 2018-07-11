<?php
	plugin_header();
	
	$m      =   (strpos($PAGE, '_mono') > 0);
?>

<p>
	This plugin allows to add set of short delays of the original signal to the output mix.
	Each delay can be used to simulate the early reflections of the signal from walls.
	This allows to make the stereo image of the original signal wider, and the position
	of the source more definitive in the mix. Equalizers provided for each delay line allow to
	simulate the fading of the original signal. Every delay can be set in time, distance and
	musical time units. Also common pre-delay and time-stretching mechanisms are provided to allow
	the stereo image to change dynamically. Additionally, ramping mode is provided to smoothen
	change of delay on automation.
</p>
<p>
	Remember that for more realistic acoustic picture the more delay is the more low and high
	frequencies	are cut from the delayed signal.
</p>

<p><b>'Master Channel' section:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the output signal is similar to input signal. That does not mean
		that the delay line is not working. The delay line <u>always</u> collects input signal to prevent clicks or other noise when turning on.
	</li>
	<li><b>Temperature</b> - temperature of the air used for <b>Distance</b> delay calculation.</li>
	<li><b>Pre-delay</b> - pre-delay added to all delay lines relatively to the original signal.</li>
	<li><b>Tempo</b> - adjust tempo of the track.</li>
	<li><b>Tempo Sync</b> - synchronize tempo with host.</li>
	<li><b>Stretch</b> - time stretching, multiplicatively increases delay time for all delay lines.</li>
	<li><b>Settings</b> - different useful settings:</li>
	<ul>
		<li><b>Mono</b> - Output mono signal instead of stereo, useful for testing mono compatibility.</li>
		<li><b>Ramp</b> - Smoothen delay change for each processor (ramping), useful for smoothing automation effect.</li>
		<li><b>Tap</b> - Button that allows to adjust tempo by clicking it, useful when the tempo is not well known and needs to be detected.</li>
	</ul>
	<?php if ($m) {?>
	<li><b>Pan</b> - the panning applied to the original (dry) mono signal.</li>
	<?php } else {?>
	<li><b>Pan Left</b> - the panning applied to the left channel of the original (dry) signal.</li>
	<li><b>Pan Right</b> - the panning applied to the right channel of the original (dry) signal.</li>
	<?php }?>
	<li><b>Dry</b> - The amount of dry (unprocessed) signal.</li>
	<li><b>Dry Mute</b> - Mute the dry (unprocessed) signal.</li>
	<li><b>Wet</b> - The amount of wet (processed) signal.</li>
	<li><b>Wet Mute</b> - Mute the wet (processed) signal.</li>
	<li><b>Output</b> - The overall output amplification of the signal.</li>
</ul>

<p><b>'Delay Lines' section:</b></p>
<ul>
	<li><b>Delay lines</b> - the delay lines selector.</li>
	<li><b>Mode</b> - the delay calculation mode, available modes: <b>Off</b>, <b>Time</b>, <b>Distance</b>.</li>
	<li><b>S</b> - Soloing mode, mutes all non-soloing delay lines.</li>
	<li><b>M</b> - Mute mode, mutes selected delay line.</li>
	<li><b>P</b> - Inverts the phase of the delayed signal.</li>
	<li><b>Delay</b> - the delay assigned to the delay line.</li>
	<?php if ($m) {?>
	<li><b>Panorama</b> - the panning applied to the input signal for this delay line.</li>
	<?php } else {?>
	<li><b>Panorama Left</b> - the panning applied to the left channel of the input for this delay line.</li>
	<li><b>Panorama Right</b> - the panning applied to the right channel of the input for this delay line.</li>
	<?php }?>
	<li><b>Gain</b> - the overall gain of this delay line.</li>
	<li><b>Filters</b> - allows to enable different filters:</li>
	<ul>
		<li><b>LC</b> - enables low-cut filter.
		<li><b>EQ</b> - enables 5-band equalizer.
		<li><b>HC</b> - enables high-cut filter.
	</ul>
	<li><b>Low cut</b> - the cut-off frequency of the low-cut filter with -24db/oct slope.</li>
	<li><b>Equalizer</b> - 5-band equalizer:</li>
	<ul>
		<li><b>Subs</b> - frequency band below 60 Hz.
		<li><b>Bass</b> - frequency band between 60 Hz and 300 Hz.
		<li><b>Middle</b> - frequency band between 300 Hz and 1 KHz.
		<li><b>Presence</b> - frequency band between 1 KHz and 6 KHz.
		<li><b>Treble</b> - frequency band above 6 KHz.
	</ul>	
	<li><b>High cut</b> - the cut-off frequency of the high-cut filter with -24db/oct slope.</li>
</ul>
