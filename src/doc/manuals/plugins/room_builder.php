<?php
	plugin_header();
	
	$s      =   (strpos($PAGE, '_stereo') > 0);
?>

<p>
	This plugin allows to simulate impulse response of any modelled room or space.
	The basic principle is based on generating of unit audio impulse and then capturing all
	it's reflections from environment by special capturing objects that simulate microphone capsules.
</p>
<p>
	So, in fact, this algorithm is a kind of raytracing algorithm but instead of single rays it
	performs tracing of ray groups. This allows to simulate large spaces withous significantly loose
	in quality and precision.
</p>
<p>The typical simulation workflow is peformed in the following way:</p>
<ul>
	<li>Prepare a room model in <a href="https://en.wikipedia.org/wiki/Wavefront_.obj_file">Wavefont OBJ file format</a>.</li>
	<li>Load room model in the plugin's main window.</li>
	<li>Edit environment settings like: audio sources, audio captures, objects' spacial parameters and material properties.</li>
	<li>Launch the rendering process which will simulate physical processes and generate the impulse response of the room.</li>
	<li>Obtain final samples after the rendeing is complete.</li>
</ul>
<p>Additionally, generated samples can be immediately applied to the input signal as a convolution. 
This allows to understand the final result with minimum number of actions.</p>

<p>The simulation algorithm relays on the following physical model.</p>
<!-- TODO -->




