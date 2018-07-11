<p>This plugin implements single-note MIDI sampler with mono input and mono output. There are up to eight samples available to play for different note velocities.</p>

<p><b>Controls:</b></p>
<ul>
	<li><b>Bypass</b> - hot bypass switch, when turned on (led indicator is shining), the plugin does not affect the input signal.</li>
</ul>

<p><b>'MIDI Setup' section:</b></p>
<ul>
	<li><b>Channel</b> - the MIDI channel to trigger notes.</li>
	<li><b>Note</b> - the note and the octave of the note to trigger.</li>
	<li><b>Muting</b> - when enabled, turns off any playback when the Channel Control MIDI message is received.</li>
	<li><b>MIDI #</b> - the MIDI number of the note.</li>
	<li><b>Mute</b> - the button that forces the playback to turn off.</li>
</ul>
<p><b>'Samples' section:</b></p>
<ul>
	<li><b>Sample #</b> - the selector of the current displayable/editable sample.</li>
	<li><b>Head cut</b> - the time to be cut from the beginning of the current sample.</li>
	<li><b>Tail cut</b> - the time to be cut from the end of the current sample.</li>
	<li><b>Fade in</b> - the time to be faded from the beginning of the current sample.</li>
	<li><b>Fade out</b> - the time to be faded from the end of the current sample.</li>
	<li><b>Makeup</b> - the makeup gain of the sample volume.</li>
	<li><b>Pre-delay</b> - the time delay between the MIDI note has triggered and the start of the sample's playback</li>
	<li><b>Listen</b> - the button that forces the sample playback of the selected sample</li>
</ul>
<p><b>'Sample matrix' section:</b></p>
<ul>
	<li><b>Enabled</b> - enables/disables the playback of the corresponding sample.</li>
	<li><b>Active</b> - indicates that the sample is loaded, enabled and ready for playback.</li>
	<li><b>Velocity</b> - the maximum velocity of the note the sample can trigger. Allows to set up velocity layers between different samples.</li>
	<li><b>Gain</b> - the additional gain adjust for the corresponding sample.</li>
	<li><b>Listen</b> - the button that forces the playback of the corresponding sample.</li>
	<li><b>Note on</b> - indicates that the playback event of the correponding sample has triggered.</li>
</ul>
<p><b>'Audio channel' section:</b></p>
<ul>
	<li><b>Dynamics</b> - allows to randomize the output gain of the samples.</li>
	<li><b>Time drifting</b> - allows to randomize the time delay between the MIDI Note On event and the start of the sample's playback.</li>
	<li><b>Dry amount</b> - the gain of the input signal passed to the audio inputs of the plugin.</li>
	<li><b>Wet amount</b> - the gain of the processed signal.</li>
	<li><b>Output gain</b> - the overall output gain of the plugin.</li>
</ul>
