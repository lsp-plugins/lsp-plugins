<?php
	plugin_header();
	
	$m      =   ($PAGE == 'art_delay_mono') ? 'm' : (
	            ($PAGE == 'art_delay_stereo') ? 's' : 
	            null );
?>

<p>This plugin allows to construct almost any desired delay using up to 16 delay lines and 8 tempo settings.</p>
<p>Each delay line provides possibility to control:</p>
<ul>
	<li>The delay time using a pretty flexible formula.</li>
	<li>The feedback delay time and gain with a bit less flexible formula.</li>
	<li>Filtering and equalization of each individual delay line.</li>
	<li>Panning of each individual line.</li>
</ul>
<p>Almost all parameters can be smootly automated.</p>
<p>The delay alows to set up to 256 seconds delay for each delay line. This may yield to huge memory
consumption. To get some memory economy, the memory is consumed in <b>on-demand</b> mode: the delay line
allocates memory only if it is turned on in the delay effect.</p>
<p>The following restrictions are defined for parameters:</p>
<ul>
	<li>Delay time can not exceed the maximum delay time specified in the <b>Max Delay</b> setting.
	Otherwise it will be considered to be set to maximum possible value.</li>
	<li>The feedback delay time can not exceed the current delay value of the delay time. Otherwise
	it will be considered to be set to maximum possible value.</li>
	<li>Delay lines can not organize a loop while referencing one to another. Otherwise the delay time
	for the specific delay can not be estimated.</li>
</ul>

<b>Master Channel</b> section:
<ul>
	<li><b>Max Delay</b> - the maximum allowed delay per each delay line in seconds.</li>
	<li><b>Feedback</b> - the button and knob that allow to enable or disable feedback and control feedback gain for all delay lines and simultaneously.</li>
	<li><b>Output</b> - the overall output volume of the plugin.</li>
	<?php if ($m == 'm') { ?>
		<li><b>Dry pan</b> - the panorama of input (dry) signal.</li>
	<?php } else { ?>
		<li><b>Dry pan Left</b> - the panorama of the left channel of the input (dry) signal.</li>
		<li><b>Dry pan Right</b> - the panorama of the right channel of the input (dry) signal.</li>
	<?php }?>
	<li><b>Dry</b> - the level of the dry (unprocessed) signal.</li>
	<li><b>Wet</b> - the level of the wet (processed) signal.</li>
</ul>

<b>Indicators</b>:
<ul>
	<li><b>Memory</b> - the overall memory consumption by delay lines of the plugin in megabytes.</li>
</ul>

<b>Global Delay Line Controls</b> section:
<ul>
	<li><b>On</b> - enables the corresponding delay. When turned on, the desired memory area is allocated for the delay so it could
		operate in real-time mode. When turned off, the allocated memory gets freed.</li>
	<li><b>Solo</b> - turns the corresponding delay line into soloing mode. All other non-soloing delay lines become muted.</li>
	<li><b>Mute</b> - mutes the corresponding delay line.</li>
</ul>

<b>Tempo Settings</b> section:
<ul>
	<li><b>Tap</b> - the button that allows to set custom tempo by just tapping it.</li>
	<li><b>Knob</b> - allows to set the custom tempo manually.</li>
	<li><b>Combo box</b> - allows to set the tempo ratio.</li>
	<li><b>Sync</b> - enables tempo synchronization with host.</li>
	<li><b>Led indicator</b> - displays the actual computed tempo.</li>
</ul>

<b>Delay Line</b> section - allows to control settings for each of 16 delay lines:
<ul>
	<li><b>Delay Time Formula</b> - the formula that allows to compute the delay time of the delay line. It provides the following terms:</li>
	<ul>
		<li>
			<b>Delay Reference</b> - the reference to another delay and it's multiplier.
			The delay value will be taken from another delay and multiplied by the specified multiplier's value.
		</li>
		<li>
			<b>Tempo-Based Delay</b> - the delay computed from specified tempo and two fractions. The first fraction specifies the size of bar
			and provides the multiplier which defines the number of whole bars for the delay. The second fraction specifies the time offset
			from the end of the last bar.
		</li>
		<li>
			<b>Delay</b> - the additional delay in seconds that can be used as an addendum.
		</li>
	</ul>
	<li><b>Feedback Time Formula</b> - the formula that allows to compute the feedback delay time of the delay line. It provides the following terms:</li>
	<ul>
		<li>
			<b>Tempo-Based Feedback Delay</b> - the feedback delay computed from specified tempo and two fractions.
			The first fraction specifies the size of bar and provides the multiplier which defines the number of whole
			bars for the delay. The second fraction specifies the time offset from the end of the last bar.
		</li>
		<li>
			<b>Delay</b> - the additional delay in seconds that can be used as an addendum.
		</li>
	</ul>
	<li><b>Delay Line Processing</b> - the main delay controls.</li>
	<ul>
		<li><b>On</b> - enables the delay. When turned on, the desired memory area is allocated for the delay so it could
		operate in real-time mode. When turned off, the allocated memory gets freed.</li>
		<li><b>Solo</b> - turns the delay line into soloing mode. All other non-soloing delay lines become muted.</li>
		<li><b>Mute</b> - mutes the delay line.</li>
		<li><b>Hue</b> - allows to set up hue of the delay line on the graph and some other related controls.</li>
		<li><b>Equalizer</b> - provides additional controls for equalization of the delayed signal:</li>
		<ul>
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
	</ul>
	<li>
		<b>Feedback</b> - the button and knob that allow to enable or disable feedback and control feedback gain
		of the corresponding delay line.
	</li>
	<?php if ($m == 'm') { ?>
	    <li><b>Panorama</b> - the panorama of the signal for the corresponding delay line.</li>
    <?php } else { ?>
    	<li><b>Panorama Left</b> - the panorama of the signal for the left channel of the corresponding delay line.</li>
    	<li><b>Panorama Right</b> - the panorama of the signal for the right channel of the corresponding delay line.</li>
    <?php }?>
    <li>
		<b>Gain</b> - the output gain of the delay line.
	</li>
</ul>










