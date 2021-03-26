<?php
	plugin_header();
	
	$x1     =   strpos($PAGE, '_x1') > 0;
	$x2     =   strpos($PAGE, '_x2') > 0;
	$x4     =   strpos($PAGE, '_x4') > 0;
?>

<p>
This plugin implements a simple, but flexible, oscilloscope. 
</p>

<p>
<? if ($x1) { ?>
	The oscilloscope channel has the following features:
<? } else { ?>
	Each oscilloscope channel has the following features:
<? } ?>
</p>
<ul>
    <li><b>X</b> input channel.</li>
    <li><b>X</b> output channel.</li>
    <li><b>Y</b> input channel.</li>
    <li><b>Y</b> output channel.</li>
    <li><b>EXT</b> input channel.</li>
    <li><b>Triggered</b> operation.</li>
    <li><b>XY</b> operation.</li>
    <li><b>Goniometer</b> operation.</li>
</ul>
<p>
	The <b>X</b> channel is used to drive the horizontal coordinate of the trace plotted on the oscilloscope graph in
	<b>XY</b> and <b>Goniometer</b> operation. In <b>Triggered</b> mode the horizontal coordinate is driven by an
	internal sweep generator.
</p>
<p>
	The <b>Y</b> channel is used to drive the vertical coordinate of the trace plotted on the oscilloscope graph.
</p>
<p>
	The <b>EXT</b> (external) channel is an alternative channel that can be used to drive the trigger.
</p>
<p>
	All channels can be freely oversampled and passed through a DC blocking filter.
</p>
<p>
	In a more general fashion with respect most scopes, each channel has a full set of channels and can have a different
	timebase. Multichannel scopes also have general controls that can be used to override multiple channels.  
</p>

<p><b>Controls:</b></p>

<p><b>'OSCILLOSCOPE GRAPH' section:</b></p>
<p>
	The Oscilloscope Graph and the controls above and below it are documented below.
</p>
<ul>
	<li>
		<b>OSCILLOSCOPE GRAPH</b> - This graph will show the various traces the oscilloscope is able to produce.
		The graticule shows bright lines and faint lines. The <b>Divisions</b>, both vertical and horizontal, are shown as bright lines.
	</li>
	<li>
	    <b>Freeze Controls</b> - These controls are available on the top left of the oscilloscope graph. Use these
	    controls to freeze the entire scope graph (<b>Freeze</b> button) or single channels with the dedicated buttons
	    (for multichannel versions).
	</li>
	<li>
	    <b>Mute/Solo Controls</b> - These controls are available for multichannel scopes on the top right of the oscilloscope
	    graph. They allow to set each individual channel to solo or mute.
	</li>
	<li>
	    <b>History</b> - This control, located at the bottom of the oscilloscope graph, allows the user to set the amount of
	    previous traces that are allowed to persist on the oscilloscope graph together with the current trace. Older traces
	    are plotted with progressive transparency. For stable signals, the resulting effect is akin to that of the persistency
	    of analog scopes.
	</li>
	<li>
	    <b>XY Sweep</b> - This control, located at the bottom of the oscilloscope graph, is enabled only for <b>XY</b> and 
	    <b>Goniometer</b> modes. In these modes, it controls the amount of memory the oscilloscope uses to draw the traces.
	</li>
	<li>
	    <b>Max Dots</b> - This control, located at the bottom of the oscilloscope graph, allows the user to tune the amount of
	    dots that are used to draw the traces on the oscilloscope graph. The higher number of dots can produce smoother traces, but
	    at the expenses of UI load.
	</li>
	<li>
	    <b>H|V|T Labels</b> - These labels mirror the values of division chosen by the user on the main scopes settings -
	    <b>horizontal</b>, <b>vertical</b> and <b>time</b>. They are located at the bottom of the oscilloscope graph for quick
	    reference.
	</li>
</ul>

<p><b>'Controls' section:</b></p>
<p>
	This section, located at the rigtmost side of the UI, contains the main oscilloscope controls.
</p>
<ul>
    <li>
        <b>Channel Selector</b> - For multichannel scopes, this control is located at the top right of the UI and is used
        to select the channel on which the controls below will operate.
    </li>
    <li>
        <b>Freeze/Solo/Mute</b> - These buttons are used to set the selected channel to <b>Freeze</b>, <b>Solo</b> end
        <b>Mute</b> respectively.
    </li>
    <li>
        <b>Oversampler</b> - This control is used to select the oversampler mode. High oversampling produces smoother traces.
        Available modes are below.
    </li>
    <ul>
	    <li><b>None</b> - oversampling is not used.</li>
	    <li><b>x2</b> - 2x downsampling.</li>
	    <li><b>x3</b> - 3x downsampling.</li>
	    <li><b>x4</b> - 4x downsampling.</li>
	    <li><b>x6</b> - 6x downsampling.</li>
	    <li><b>x8</b> - 8x downsampling.</li>
    </ul>
    <li>
        <b>Mode</b> - This control sets the main oscilloscope mode. The following modes are available:
    </li>
    </li>
    <ul>
	    <li>
	    	<b>XY</b> - In this mode the the trace horizontal coordinate is driven by the channel <b>X</b> input, while the vertical
	    	trace coordinate is driven by the channel <b>Y</b> input.
	    </li>
	    <li>
	    	<b>Triggered</b> - In this mode the trace horizontal coordinate is driven by the internal sweep generator, which is in
	    	turn controlled by the channel trigger, while the trace vertical coordinate is driven by the channel <b>Y</b> input.
    	</li>
	    <li>
	    	<b>Goniometer</b> - In this mode the trace horizontal coordinate is driven by half of the sum of the <b>X</b> and <b>Y</b>
	    	inputs, the vertical trace coordinate is driven by half their difference.
    	</li>
    </ul>
    <li>
        <b>Sweep Type</b> - This control is active in <b>Triggered</b> mode and sets the type of sweep signals which drives the trace
        X value. The sweeps are initiated when the trigger fires and, after completion, remain dormant until the next trigger fire is
        allowed. The following types are available.
    </li>
    <ul>
        <li><b>Sawtooth</b> - The sweep is a sawtooth wave.</li>
        <li><b>Triangular</b> - The sweep is a triangular wave.</li>
        <li><b>Sine</b> - The sweep is a sine wave.</li>
    </ul>
    <li>
        <b>Horizontal</b> - These controls allow tuning of the horizontal trace properties. The available controls are listed below.
    </li>
    <ul>
        <li>
        	<b>Division</b> - In <b>Triggered</b> mode, this control sets the size, in milliseconds, of the horizontal division value
        	in the oscilloscope graph. In <b>XY</b> and <b>Goniometer</b> instead it controls, in units of amplitude, the horizontal
        	division value in the oscilloscope graph.
        </li>
        <li>
        	<b>Position</b> - This trigger allows to introduce an horizontal shift in the trace. The shift is specified as a percentage
        	of the half horizontal span of the oscilloscope graph.
        </li>
    </ul>
    <li>
        <b>Vertical</b> - These controls allow tuning of the vertical trace properties. The available controls are listed below.
    </li>
    <ul>
        <li><b>Division</b> - This control sets, in units of amplitude, the vertical division value in the oscilloscope graph.</li>
        <li>
        	<b>Position</b> - This trigger allows to introduce a vertical shift in the trace. The shift is specified as a percentage of
        	the half vertical span of the oscilloscope graph.
        </li>
    </ul>
    <li>
        <b>Trigger</b> - These controls tune the channel trigger. They are active only in <b>Triggered</b> mode.
    </li>
    <ul>
        <li>
        	<b>Level</b> - This control sets the level of the trigger. The level is specified as a percentage of the half vertical
        	span of oscilloscope graph. When the <b>Edge Type</b> is Simple, this level is shown in the oscillator graph as an
        	horizontal line.
        </li>
        <li>
        	<b>Hysteresis</b> - This control is active only for the <b>Advanced</b> Edge types. This control allows to split the
        	trigger level in two levels, separated by the hysteresis value, that need to be crossed in the right order to cause the
        	trigger to fire. The hysteresis value is specified as a percentage of the full vertical span of the oscilloscope graph.
        	When the <b>Edge Type</b> is Advanced the values of level +/- hysteresis are shown on the oscilloscope graph as horizontal
        	lines.
        </li>
        <li>
        	<b>Hold Time</b> - This control sets, in seconds, the amount of time that needs to elapse between consecutive trigger firings.
        	This time has a lower bound calculated internally from the sweep properties.
        </li>
        <li>
        	<b>Input</b> - This control is used to select the input channel for the trigger. Available channels are <b>Y</b> and <b>EXT</b>.
        </li>
        <li>
        	<b>Run/Stop</b> - This button is only active for <b>Single</b> and <b>Manual</b> modes and allows to start a trigger and
			sweep cycle.
		</li>
        <li><b>Mode</b> - This control sets the trigger mode. The modes below are available.</li>
        <ul>
            <li>
            	<b>Single</b> - In this mode, the scope is allowed to complete a single trigger and sweep cycle after which it freezes.
            	To restart the cycle, use the <b>Run/Stop</b> button.
            </li>
            <li>
            	<b>Manual</b> - In this mode the scope is allowed, after pressing the <b>Run/Stop</b> button, to complete a single trigger
            	and sweep cycle after which it freezes.
            </li>
        </ul>
        <li><b>Edge Type</b> - This controls set the trigger edge type. The following edge types are available</li>
        <ul>
            <li><b>None</b> - In this edge type the trigger is firing with sweep period.</li>
            <li>
            	<b>Simple Rising</b> - This edge type causes the trigger to fire when the signal passes through the threshold in the
            	ascending direction. The threshold is set by the trigger <b>Level</b> control.
            </li>
            <li>
            	<b>Simple Falling</b> - This edge type causes the trigger to fire when the signal passes through the threshold in the
            	falling direction. The threshold is set by the trigger <b>Level</b> control.
            </li>
            <li>
            	<b>Advanced Rising</b> - This edge type causes the trigger to fire when the signal passes through two thresholds in
            	the ascending direction. The thresholds are set by the trigger <b>Level</b> and <b>Hysteresis</b> controls.
            </li>
            <li>
            	<b>Advanced Falling</b> - This edge type causes the trigger to fire when the signal passes through two thresholds in
            	the falling direction. The thresholds are set by the trigger <b>Level</b> and <b>Hysteresis</b> controls.
            </li>
        </ul>
        <li>
        	<b>Coupling X|Y|EXT</b> - For each of the <b>X</b>, <b>Y</b> and <b>EXT</b> channels, these controls allow to set
        	the coupling. The following couplings are available.
        </li>
        <ul>
            <li><b>AC</b> - In this coupling type, the signal is passed through a DC block filter.</li>
            <li><b>DC</b> - In this coupling type, the signal is unfiltered.</li>
        </ul>
    </ul>
</ul>

<p><b>'Global Settings' section:</b></p>
<p>
	These controls are available only for multichannel scopes, and are located on the right of the oscilloscope graph.
	They are exactly the same of the <b>'Controls' section:</b> and are used to override the single channels settings.
</p>
<p>
	In order to override one channel control with the global, select the channel with the <b>Channel Selector</b> and activate the
	<b>On</b> button in the top of the <b>Global Settings<b> section.
</p>

