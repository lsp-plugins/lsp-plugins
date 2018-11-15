<?php
	plugin_header();
	
	$m      =   ($PAGE == 'comp_delay_mono') ? 'm' : (
	            ($PAGE == 'comp_delay_stereo') ? 's' : (
	            ($PAGE == 'comp_delay_x2_stereo') ? 'x2s' :
				null ));
	$c      =   ($m == 'm') ? ' the mono channel' : (
	            ($m == 's') ? ' the stereo channel' : (
	            ($m == 'x2s') ? ' each individual channel of the stereo channel' :
				null ));
	$n      =   ($m == 'x2s') ? 2 : 1;
	$cc     =   ($m == 'x2s') ? ' for the corresponding channel' : '';
	$oc     =   ($m == 'x2s') ? ' of the corresponding channel' : '';
	

?>

<p>
	This plugin allows to add short delay to<?= $c ?> to compensate the phase offset relatively to other channels.
	The main idea is to add some delay of the signal recorded by set of microphones placed at different positions and distances
	from the sound source. Because the speed of the sound is low (about 340 m/s), the signal from the sources placed on different
	position comes with different delays. When the receivers are placed on the different distances, the signal comes with different
	delays to them. So the final mix of the recorded tracks can sound muddy or, on the contrary, very thick.
	Note that even 1 centimeter of the difference can take the negative effect. To avoid this, you can add more delay to the receivers
	placed near the source and less delay to the receivers placed far from the source. This plugin helps to do this in three ways.
	In conjunction with <?php plugin_ref('phase_detector'); ?> you can very fast phase your sources.
</p>

<p><b>Controls:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the output signal is similar to input signal. That does not mean
		that the delay line is not working. The delay line <u>always</u> collects input signal to prevent clicks or other noise when turning on.
	</li>
	<li><b>Output</b> - the loudness of the processed output signal<?= $cc ?>.</li>
	<li><b>Dry amount</b> - the amount of the unprocessed (dry) signal in the output signal<?= $cc ?>.</li>
	<li><b>Wet amount</b> - the amount of the processed (wet) signal in the output signal<?= $cc ?>.</li>
	<li>
		<b>Mode</b> - the switch button that selects active mode of the delay<?= $cc ?>: <b>Samples</b>, <b>Distance</b> or <b>Time</b>. When the mode is active,
		then the <b>Active</b> led of the appropriate control group is shining.
	</li>
	<li>
		<b>Ramp</b> - the button that enables ramping mode of the delay<?= $cc ?>. This feature allows to smooth delay jumps when applying automation by
		linearizing the delay change when processing audio.
	</li>
	<li><b>Samples</b> - sets the delay in samples for the <b>Samples</b> mode<?= $oc ?>.</li>
	<li><b>Meters</b> - inaccurate distance control, sets the distance in meters for the <b>Distance</b> mode<?= $oc ?>.</li>
	<li><b>Centimeters</b> - accurate distance control, sets the additional distance in centimeters for the <b>Distance</b> mode<?= $oc ?>.</li>
	<li><b>Temperature</b> - temperature of the air for the <b>Distance</b> mode<?= $oc ?>. <u>Note</u> that the speed of the sound in the air highly depends on the temperature of the air.</li>
	<li><b>Time</b> - the time to delay the input signal for the <b>Time</b> mode<?= $oc ?>.</li>
</ul>

<p><b>Indicators:</b></p>
<ul>
	<li><b>Time</b> - the real delay (in time units) applied to input signal<?= $cc ?>.</li>
	<li><b>Samples</b> - the real number of samples the input signal was delayed<?= $cc ?>.</li>
	<li><b>Distance</b> - the real distance added to the receiver or the sound source<?= $cc ?>.</li>
</ul>

<p>The delay line always converts the parameters set in the active section into samples that are displayed on the <b>Samples</b> meter.
All other metering values are converted from samples, so the real metering values may differ from the values set in the appropriate control group.
</p>
