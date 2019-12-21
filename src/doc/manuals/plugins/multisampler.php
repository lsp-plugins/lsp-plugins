<?php
	plugin_header();

	$nc     =   (strpos($PAGE, '_x12_') > 0) ? 12 : (
				(strpos($PAGE, '_x24_') > 0) ? 24 : (
				(strpos($PAGE, '_x48_') > 0) ? 48 : 1
				) );
	$do     =   (strpos($PAGE, '_do') > 0);
?>

<p>This plugin implements <?= $nc ?>-instrument MIDI sample player with stereo input and stereo output. For each instrument there are up to eight samples available to play for different note velocities.
<?php if ($do) { ?>
Also each instrument has it's own stereo output that makes possible to record instrument outputs into individual tracks.
<?php } ?>
</p>

<p><b>Controls:</b></p>
<ul>
	<li><b>Bypass</b> - hot bypass switch, when turned on (led indicator is shining), the plugin does not affect the input signal.</li>
	<li><b>Working area</b> - combo box that allows to switch between instrument setup and instrument mixer.</li>
</ul>

<p><b>'Instrument mixer' section:</b></p>
<ul>
	<li><b>Enabled</b> - enables the corresponding instrument.</li>
	<li><b>Mix gain</b> - the volume of the instrument in the output mix.</li>
	<?php if ($do) { ?>
	<li><b>Direct Out</b> - enables the output of the instrument to the separate track.</li>
	<?php } ?>
	<li><b>Pan Left</b> - the panorama of the left channel of the corresponding instrument.</li>
	<li><b>Pan Right</b> - the panorama of the right channel of the corresponding instrument.</li>
	<li><b>MIDI #</b> - the MIDI number of the note associated with the corresponding instrument.</li>
	<li><b>Note on</b> - indicates that the corresponding instrument has triggered the MIDI Note On event.</li>
	<li><b>Listen</b> - forces the corresponding instrument to trigger the Note On event.</li>
</ul>

<p><b>'Instrument' section:</b></p>
<ul>
	<li><b>Channel</b> - the MIDI channel to trigger notes by the selected instrument.</li>
	<li><b>Note</b> - the note and the octave of the note to trigger for the selected instrument.</li>
	<li><b>MIDI #</b> - the MIDI number of the note for the selected instrument.</li>
	<li><b>Group</b> - The group assigned to the instrument. The sample playback will be stopped for all
	instruments in the same group except the one's that has triggered the Note On event.</li>
	<li><b>Muting</b> - when enabled, turns off sample playback for selected instrument 
	when the Channel Control MIDI message is received.</li>
	<li><b>Note off</b> - when enabled, turns off sample playback for selected channel when the Note Off
	MIDI message is received. The sample fade-out time can be controlled by the corresponding knob.</li>
	<li><b>Dynamics</b> - allows to randomize the output gain of the selected instrument.</li>
	<li><b>Time drifting</b> - allows to randomize the time delay between the MIDI Note On event and the start of the sample's playback for the selected instrument.</li>
</ul>
<p><b>'Samples' section:</b></p>
<ul>
	<li><b>Sample #</b> - the selector of the current displayable/editable sample for the selected instrument.</li>
	<li><b>Head cut</b> - the time to be cut from the beginning of the current sample for the selected instrument.</li>
	<li><b>Tail cut</b> - the time to be cut from the end of the current sample for the selected instrument.</li>
	<li><b>Fade in</b> - the time to be faded from the beginning of the current sample for the selected instrument.</li>
	<li><b>Fade out</b> - the time to be faded from the end of the current sample for the selected instrument.</li>
	<li><b>Makeup</b> - the makeup gain of the sample volume for the selected instrument.</li>
	<li><b>Pre-delay</b> - the time delay between the MIDI note has triggered and the start of the sample's playback for the selected instrument</li>
	<li><b>Listen</b> - the button that forces the playback of the selected sample for the selected instrument</li>
</ul>
<p><b>'Sample matrix' section:</b></p>
<ul>
	<li><b>Enabled</b> - enables/disables the playback of the corresponding sample for the selected instrument.</li>
	<li><b>Active</b> - indicates that the sample is loaded, enabled and ready for playback.</li>
	<li><b>Velocity</b> - the maximum velocity of the note the sample can trigger. Allows to set up velocity layers between different samples.</li>
	<li><b>Pan Left</b> - the panorama of the left audio channel of the corresponding sample.</li>
	<li><b>Pan Right</b> - the panorama of the right audio channel of the corresponding sample.</li>
	<li><b>Listen</b> - the button that forces the playback of the corresponding sample.</li>
	<li><b>Note on</b> - indicates that the playback event of the correponding sample has triggered.</li>
</ul>
<p><b>'Audio channel' section:</b></p>
<ul>
	<li><b>Muting</b> - when enabled, turns off any sample playback when the Channel Control MIDI message is received.</li>
	<li><b>Note off</b> - when enabled, turns off any sample playback when the Note Off MIDI message is received. 
	The sample fade-out time can be controlled by the corresponding knob.</li>
	<li><b>Dry amount</b> - the gain of the input signal passed to the audio inputs of the plugin.</li>
	<li><b>Wet amount</b> - the gain of the processed signal.</li>
	<li><b>Output gain</b> - the overall output gain of the plugin.</li>
	<li><b>Mute</b> - the button that forces any sample playback to turn off.</li>
</ul>
