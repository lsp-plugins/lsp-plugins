<?php
	plugin_header();
?>

<p>
	A simple plugin that allows to measure the latency of a transmission line by using chirp - inverse filter convolution.
	Latency is determined as the lag of the convolution peak.
</p>
<p>
	To measure the latency, first connect output of the plugin to the input of the transmission line. Then connect the output
	of the transmission line to the input of the plugin. Then you're ready to press the <b>Measure</b> button. The plugin will
	generate short chirp signal on the output and await the response on the input. If the signal will be captured and
	detected on the input of the plugin, it will display the overall latency value.
</p>
<p><b>Controls:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the plugin bypasses signal.
	</li>
</ul>
<p><b>'Gain' section:</b></p>
<ul>
	<li><b>Input</b> - Set input gain for the plugin.</li>
	<li><b>Feedback</b> - If off, the feedback loop between input and output is disabled to avoid gain buildup.</li>
    <li><b>Output</b> - Set output gain for the plugin.</li>
</ul>
<p><b>'Parameters' section:</b></p>
<ul>
	<li><b>Max latency</b> - Maximum value of the expected transmission line latency, in milliseconds (needs to include buffering latency).</li>
	<li><b>Peak</b> - Peak threshold for early detection: if the gap between consecutive local 
	convolution peaks is higher than this value the plugin returns the latency associated with the highest peak.</li>
    <li><b>Absolute</b> - Absolute threshold for the detection algorithm: values of convolution
    smaller than this are ignored by the algorithm.</li>
</ul>
<p><b>'Latency' section:</b></p>
<ul>
	<li><b>Latency</b> - Indicator that reports the measured latency in ms.</li>
	<li><b>Measure</b> - Button that forces the plugin to perform a single latency measurement.</li>
    <li><b>Level meter</b> - Measures the level of plugin's input signal, useful to ensure appropriate level of the test chirp.</li>
</ul>