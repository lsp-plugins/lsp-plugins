<?php
	plugin_header();
	
	$m      =   ($PAGE == 'surge_filter_mono') ? 'm' : (
	            ($PAGE == 'surge_filter_stereo') ? 's' : null
	            );
	$c      =   ($m == 'm') ? 'mono' : (
	            ($m == 's') ? 'stereo' : null
	            );
?>

<p>
	This plugin is designed mostly as a workaround for systems which don't support
	smooth fade-ins and fade-outs of audio stream on playback start and stop events.
	Such events may produce noticeable pops, especially when the audio stream is additionally
	amplified.
</p>
<p>
	For example, the <?php plugin_ref('compressor_stereo'); ?> plugin while working in
	upward mode significantly amplifies the input signal below the threshold. When the audio
	playbach suddenly appears, the compressor doesn't stop to amplify the signal instantly.
	Instead of this, an envelope of the signal is computed by the compressor which contains
	the reactive part. As the result, the amplification coefficient lowers with some short delay
	but it is still enough to produce very loud and annoying pop.
</p>
<p>
	This plugin detects start of playback and end of playback by monitoring the RMS envelope
	of the signal.
</p>
<p>
	If the envelope is below the <b>Fade-In threshold</b>, the plugin doesn't
	pass any signal	to the output. The plugin triggers smooth fade-in process when the RMS envelope
	exceeds the <b>Fade-In threshold</b>. To protect the plugin from the accidental fade-out immediately
	after fade-in (which may be caused by non-significant oscillations of the RMS envelope), additional
	<b>fade-in timeout</b> is added which allows the envelope to grow enough and to not to trigger fade-in
	event.
</p>
<p>
	If the envelope goes below the <b>Fade-Out threshold</b>, the plugin performs the smooth fade-out process.
	The only problem is, that when the RMS fall-off is detected, there's almost nothing to fade-out.
	To perform proper fade-out and complete the fade-out at the point where the RMS fall-off was detected,
	plugin performs lookahead which may yield to long playback delays for big <b>RMS</b> and <b>Fade-Out time</b>
	values. This introduces the latency from the plugin side which may be computed as a sum of <b>RMS</b> and
	<b>Fade-Out time</b>. If the plugin issues fade-out, it doesn't allow to trigger fade-in for the 
	<b>Fade-Out delay</b> time after the fade-out completes. This also saves plugin from issuing accidental
	fade-in events which can be caused by oscillations of the falling down RMS envelope. 
</p>
<b>Controls below the graph:</b>
<ul>
	<li><b>Input</b> - enables drawing of the input signal on the graph.</li>
	<li><b>Output</b> - enables drawing of the output signal on the graph.</li>
	<li><b>Envelope</b> - enables drawing of the RMS envelope signal on the graph.</li>
	<li><b>Gain</b> - enables drawing of the gain applied to the input signal on the graph.</li>
</ul>
<b>Main controls:</b>
<ul>
	<li><b>Input</b> - the additional gain applied to the input signal before it gets processed.</li>
	<li><b>RMS</b> - the time period for which the immediate value of RMS is computed, <b>impacts the latency of plugin</b>.</li>
	<li><b>Fade In</b> - parameters of the Fade-in transition:</li>
	<ul>
		<li><b>Fade In</b> - The form of the fade-in function:</li>
		<ul>
			<li><b>Linear</b> - Linear increasing gain</li>
    		<li><b>Cubic</b> - Cubic spline is used to raise the gain value</li>
    		<li><b>Sine</b> - Sine form is used to raise the gain value</li>
    		<li><b>Gaussian</b> - Gaussian function (form of the bell) is used to raise the gain value</li>
    		<li><b>Parabolic</b> - The parabolic funcion is used to raise the gain value</li>
		</ul>
    	<li><b>Thresh</b> - The threshold of the RMS envelope for detecting fade-in event</li>
    	<li><b>Time</b> - The overall transition time</li>
    	<li><b>Delay</b> - The protection time to not to trigger accidental fade-out event</li>
	</ul>
	<ul>
		<li><b>Fade Out</b> - The form of the fade-out function:</li>
		<ul>
			<li><b>Linear</b> - Linear lowering gain</li>
    		<li><b>Cubic</b> - Cubic spline is used to lower the gain value</li>
    		<li><b>Sine</b> - Sine form is used to lower the gain value</li>
    		<li><b>Gaussian</b> - Gaussian function (form of the bell) is used to lower the gain value</li>
    		<li><b>Parabolic</b> - The parabolic funcion is used to lower the gain value</li>
		</ul>
    	<li><b>Thresh</b> - The threshold of the RMS envelope for detecting fade-out event</li>
    	<li><b>Time</b> - The overall transition time, <b>impacts the latency of plugin</b></li>
    	<li><b>Delay</b> - The protection time to not to trigger accidental fade-out event</li>
	</ul>
	<li><b>Output</b> - the additional gain applied to the output signal of the plugin.</li>
</ul>
