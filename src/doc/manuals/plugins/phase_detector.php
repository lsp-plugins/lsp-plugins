<?php plugin_header(); ?>

<p>This plugin allows to detect phase between two sources. For example, for two or more microphones
set at the different positions and distances from the sound source.</p>
<p>The internal algorithm is based on correlation function calculation between two sources.</p>
<p><u>Be aware</u>: because there are many correlation functions for different phases calculated at one time,
the entire analyzing process can take a lot of CPU resources. You can also reduce CPU utilization
by lowering the maximum analysis time.<p>
<p>The plugin bypasses input signal without any modifications, so it can be placed everywhere it's needed.</p>
<p><b>Controls:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the analyser doesn't do anything.
		After the bypass is turned off, the state of analyser will be automatically reset.
	</li>
	<li>
		<b>Max time</b> - maximum analysis time. Sets maximum time interval of phase analysis.
		<u>Note</u> that analysis shows both negative and positive time offsets, so the actual analysis time is twice greater than the value set by this control.
		The state of analyser will be automatically reset on change of this control.
	</li>
	<li>
		<b>Reactivity</b> - the parameter that allows to configure how the correlation function value reacts on the correlation function change. Is similar to the
		case when each individual curve of the correlation function is passed thru the lowpass filter.
		The lesser reactivity value causes less stable correlation graph but gives more tolerance and vice verse.
	</li>
	<li>
		<b>Sel time</b> - the custom time selected for metering. 
		Actually is the amount in percent (%) of the maximum analysis time.
		The metering values for this parameter are colored with yellow in monitoring section.
	</li>
	<li><b>Reset</b> - this control allows to immediately reset the state of analyser.</li>
</ul>

<p><b>Meters:</b></p>
<ul>
	<li><b>Best</b> - row of the monitoring section, displays values for the best detected phase that gives the best value from the correlation function set.</li>
	<li><b>Selected</b> - row of the monitoring section, displays values for the <b>Sel time</b> parameter that give the value of correlation function at the selected time.</li>
	<li><b>Worst</b> - row of the monitoring section, displays values for the worst detected phase that gives the worst value from the correlation function set.</li>
	<li><b>Delay</b> - column of the monitoring section, displays the time difference between two input channels for the correlation function value.</li>
	<li><b>Offset</b> - column of the monitoring section, displays the sample difference between two input channels for the correlation function value.</li>
	<li><b>Distance</b> - column of the monitoring section, displays the relative to the sound speed distance difference between two input channels for the correlation function value.</li>
	<li><b>Value</b> - column of the monitoring section, displays the normalized value of the correlation function.</li>
</ul>

<p>The <b>Correlation Graph</b> is two-dimensional graph that allows to monitor immediate values of the set of correlation functions.
The horizontal axis is the relative time offset, the vertical axis is the normalized correlation function value.
The thick blue line shows the normalized values of the set of correlation functions depending on the time offset.
The values of the <b>Best</b>, <b>Selected</b> and <b>Worst</b> meters are drawn as crossing lines with the matching colors.
</p>
<p>When the signal from the <b>left</b> (<b>A</b>) channel comes with delay relatively to the <b>right</b> (<b>B</b>) channel, the offset values will be positive.
When the signal from the <b>B</b> channel comes with delay relatively to the <b>A</b>, the offset values become negative. In most cases, when sources are in
phase, the <b>Value</b> indicator for the <b>Best</b> row takes the value near to 1, otherwise the <b>Value</b> indicator of the <b>Worst</b> row takes value near to -1,
so for correctly phasing two sources the phase for one of the sources has to be inverted.
Two sources are in-phase when the <b>Value</b> of the <b>Best</b> row shows 1 and other columns
(especially <b>Offset</b>) show the values near to the 0 the most time.
</p>