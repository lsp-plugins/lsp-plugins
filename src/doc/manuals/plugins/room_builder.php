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

<?php out_image('graph/roombuilder-model', 'The physical model of Room Builder simulations') ?>

<p>The sound source emits unit audio impulse which consists of infinite number of audio rays.</p>
<p>Each ray has it's own characteristics: the velocity vector v<sub>falling</sub> and the amplitude A<sub>falling</sub>.</p>
<p>
	This ray  reaches the border of two different materials at the the &alpha;<sub>falling</sub> angle to the border's
	normal vector n<sub>face</sub>.
</p>
<p>Then, the ray is split into two rays: reflected and refracted.</p>
<p>
	The reflected ray has the same velocity value but the negated projection to the face normal. This, still, keeps
	the rule that the reflected angle is similar to the falling angle.
</p>
<p>
	The refracted ray has the altered velocity value multiplied by the relative permeability of the material n.
	Also, if n is not equal to 1, the refracted angle &alpha;<sub>refracted</sub> is not equal to &alpha;<sub>falling</sub>.
</p>
<p>
	Additionally, the physical model considers that crossing of the border yields to some energy losses K<sub>absorption</sub> (in percents)
	and that fact that each material has it's own transparency factor K<sub>transparency</sub> (in percents).
</p>
<p>All these rules can be written using the following equations:</p>

<?php out_image('graph/roombuilder-equations', 'The physical equations of Room Builder simulations') ?>     

<p>
	To simulate sharpness of the surface, additional coefficients K<sub>diffusion</sub> and
	K<sub>dispersion</sub> are available, as shown on the image above.
</p>
<p>
	To simulate anisotropy features of the material, different coefficients K<sub>absorption</sub>,
	K<sub>transparency</sub>, K<sub>diffusion</sub>, K<sub>dispersion</sub> are available for both cases:
	when audio ray enters the material and when audio ray leaves the material.
</p>

<p><b>Controls</b>:</p>
<ul>
    <li><b>FFT frame</b> - the maximum size of the FFT frame used for convolvers.</li>
    <li><b>IR Eequalizer</b> - enables additional equalizer for processed (wet) signal.</li>
</ul>

<p><b>'Room browser' section:</b></p>
<ul>
	<li><b>Room orientation</b> - set the orientation of the 3D model. This is useful for
	cases when the model is exported in different coordinate system.</li>
	<li><b>Sz X</b> - additional 3D scene scaling coefficient by the X axis.</li>
	<li><b>Sz Y</b> - additional 3D scene scaling coefficient by the Y axis.</li>
	<li><b>Sz z</b> - additional 3D scene scaling coefficient by the Z axis.</li>
	<li><b>3D model</b> - button for loading 3D scene.</li>
	<li><b>Camera</b> - set of parameters for manipulating camera position:</li>
	<ul>
    	<li><b>X Axis</b> - position of the camera relative to the origin by the X axis.</li>
    	<li><b>Y Axis</b> - position of the camera relative to the origin by the Y axis.</li>
    	<li><b>Z Axis</b> - position of the camera relative to the origin by the Z axis.</li>
    	<li><b>Yaw</b> - camera's yaw rotation angle.</li>
    	<li><b>Pitch</b> - camera's pitch rotation angle.</li>
    </ul>
</ul>

<p><b>'Sample' section:</b></p>
<ul>
	<li><b>Export</b> - button for saving the rendered audio sample to the file.</li>
	<li><b>Reverse</b> - allows to reverse sample in time domain.</li>
	<li><b>Head cut</b> - cut amount of milliseconds from the beginning of the sample, can be used to remove early reflections of reverb.</li>
	<li><b>Tail cut</b> - cut amount of milliseconds from the end of the sample, can be used to remove large reverberation tail.</li>
	<li><b>Fade in</b> - adds additional fading at the beginning of the sample.</li>
	<li><b>Fade out</b> - adds additional fading at the end of the sample.</li>
	<li><b>Listen</b> - this button allows to listen contents of the sample.</li>
	<li><b>Makeup</b> - additional fader to control the gain of the rendered impulse response.</li>
</ul>

<p><b>'Source editor' section:</b></p>
<ul>
	<li><b>Source</b> - the selector of the current audio source.</li>
	<li><b>Enabled</b> - the button that enables the audio source.</li>
	<li><b>Shape</b> - allows to select the shape of the wave front generated by the audio source.</li>
	<li><b>Phase</b> - allows to invert the phase of the audio source.</li>
	<li><b>X Axis</b> - position of the audio source relative to the origin by the X axis.</li>
	<li><b>Y Asis</b> - position of the audio source relative to the origin by the Y axis.</li>
	<li><b>Z Axis</b> - position of the audio source relative to the origin by the Z axis.</li>
	<li><b>Yaw</b> - yaw rotation angle of the audio source.</li>
	<li><b>Pitch</b> - pitch rotation angle of the audio source.</li>
	<li><b>Roll</b> - roll rotation angle of the audio source.</li>
	<li><b>Hue</b> - the hue of the audio source color in a Room browser view.</li>
	<li><b>Angle</b> - The additional diffuse angle of the audio source.</li>
	<li><b>Size</b> - The size of the audio source.</li>
	<li><b>Height</b> - Additional height of the audio source.</li>
	<li><b>Curvature</b> - Additional curvature of the audio source.</li>
</ul>

<p><b>'Capture editor' section:</b></p>
<ul>
	<li><b>Capture</b> - the selector of the current audio capture.</li>
	<li><b>Enabled</b> - the button that enables the audio capture.</li>
	<li><b>Mode</b> - the microphone setting mode for audio capture, available configurations are:</li>
	<ul>
		<li><b>Mono</b> - mono capsule.</li>
		<li><b>XY</b> - two capsules pointing at 90 degrees one to another by default.</li>
		<li><b>AB</b> - two parallel capsules set far one from another.</li>
		<li><b>ORTF</b> - two capsules simulating the location of human ears.</li>
		<li><b>MS</b> - mid/side configuration of microphones.</li>
	</ul>
	<li><b>Reflections</b> - sets the range of reflection orders that will be captured by this audio capture. 
		This allows to eliminate early reflections or late reflections in the rendered result.
	</li>
	<li><b>Mic. direction</b> - specifies the microphone direction:</li>
	<ul>
		<li><b>Cardioid</b></li>
		<li><b>Supercardioid</b></li>
		<li><b>Hypercardioid</b></li>
		<li><b>Bidirectional</b></li>
		<li><b>8-directional</b></li>
		<li><b>Omnidirectional</b></li>
	</ul>
	<li><b>Side direction</b> - specifies the side microphone direction for the <b>XY</b> mode:</li>
	<ul>
		<li><b>Cardioid</b></li>
		<li><b>Supercardioid</b></li>
		<li><b>Hypercardioid</b></li>
		<li><b>Bidirectional</b></li>
		<li><b>8-directional</b></li>
		<li><b>Omnidirectional</b></li>
	</ul>
	<li><b>X Axis</b> - position of the audio capture relative to the origin by the X axis.</li>
	<li><b>Y Asis</b> - position of the audio capture relative to the origin by the Y axis.</li>
	<li><b>Z Axis</b> - position of the audio capture relative to the origin by the Z axis.</li>
	<li><b>Yaw</b> - yaw rotation angle of the audio capture.</li>
	<li><b>Pitch</b> - pitch rotation angle of the audio capture.</li>
	<li><b>Roll</b> - roll rotation angle of the audio capture.</li>
	<li><b>Hue</b> - the hue of the audio capture color in a Room browser view.</li>
	<li><b>Capsule</b> - the size of the microphone capsule.</li>
	<li><b>Distance</b> - the distance between microphones.</li>
	<li><b>Angle</b> - the angle between microphones.</li>
</ul>

<p><b>'Object editor' section:</b></p>
<ul>
	<li><b>Object</b> - current object selector</li>
	<li><b>Enabled</b> - enables the object in the scene</li>
	<li><b>X Move</b> - position of the object relative to it's initial location by the X axis.</li>
	<li><b>Y Move</b> - position of the object relative to it's initial location by the Y axis.</li>
	<li><b>Z Move</b> - position of the object relative to it's initial location by the Z axis.</li>
	<li><b>Yaw</b> - yaw rotation angle of the object.</li>
	<li><b>Pitch</b> - pitch rotation angle of the object.</li>
	<li><b>Roll</b> - roll rotation angle of the object.</li>
	<li><b>XScale</b> - assitional scale factor of the object by the X axis.</li>
	<li><b>YScale</b> - assitional scale factor of the object by the Y axis.</li>
	<li><b>ZScale</b> - assitional scale factor of the object by the Z axis.</li>
	<li><b>Hue</b> - the hue of the object color in a Room browser view.</li>
</ul>

<p><b>'Material editor' section:</b></p>
<ul>
	<li><b>Object</b> - current object selector.</li>
	<li><b>Enabled</b> - enables the object in the scene.</li>
	<li><b>Material preset</b> - allows to quickly adjust sound speed and absorption of the material.</li>
	<li><b>Sound speed</b> - the knob for controlling speed of the sound in the material.</li>
	<li><b>Absorption</b>:</li>
	<ul>
		<li><b>Out-&gt;In</b> - the absorption of material when the sound ray enters the object.</li>
		<li><b>Link</b> - the linkage between <b>Out-&gt;In</b> and <b>In-&gt;Out</b> parameters.</li>
		<li><b>In-&gt;Out</b> - the absorption of material when the sound ray leaves the object.</li>
	</ul>
	<li><b>Transparency</b>:</li>
	<ul>
		<li><b>Out-&gt;In</b> - the transparency of material when the sound ray enters the object.</li>
		<li><b>Link</b> - the linkage between <b>Out-&gt;In</b> and <b>In-&gt;Out</b> parameters.</li>
		<li><b>In-&gt;Out</b> - the transparency of material when the sound ray leaves the object.</li>
	</ul>
	<li><b>Dispersion</b>:</li>
	<ul>
		<li><b>Out-&gt;In</b> - the dispersion of material when the sound ray enters the object.</li>
		<li><b>Link</b> - the linkage between <b>Out-&gt;In</b> and <b>In-&gt;Out</b> parameters.</li>
		<li><b>In-&gt;Out</b> - the dispersion of material when the sound ray leaves the object.</li>
	</ul>
	<li><b>Diffusion</b>:</li>
	<ul>
		<li><b>Out-&gt;In</b> - the diffusion of material when the sound ray enters the object.</li>
		<li><b>Link</b> - the linkage between <b>Out-&gt;In</b> and <b>In-&gt;Out</b> parameters.</li>
		<li><b>In-&gt;Out</b> - the diffusion of material when the sound ray leaves the object.</li>
	</ul>
</ul>

<p><b>'Rendering' section:</b></p>
<ul>
	<li><b>Status</b> - current rendering status</li>
	<li><b>Threads</b> - allows to select number of threads for parallel rendering</li>
	<li><b>Quality</b> - specifies the quality of rendering. The higher quality takes more time.</li>
	<li><b>Progress</b> - the indicator of the completion of the rendering process.</li>
	<li><b>Launch</b> - the button that starts the offline rendering process.</li>
	<li><b>Stop</b> - the button that terminates the offline rendering process.</li>
</ul>
    
<p><b>'Output' section:</b></p>
<ul>
	<?php if ($s) { ?>	
		<li><b>Dry Pan L/R</b> - the panning of the left and right channels of the dry (unprocessed) signal.</li>
	<?php } else { ?>
		<li><b>Dry Pan L/R</b> - the panning of the dry (unprocessed) signal.</li>
	<?php } ?>
	<li><b>Pre-delay</b> - amount of pre-delay added to the wet (processed) signal.</li>
	<li><b>Dry</b> - amount of gain applied to the dry (unprocessed) signal.</li>
	<li><b>Wet</b> - amount of gain additionally applied to the wet (processed) signal.</li>
	<li><b>Output</b> - amount of gain additionally applied to the output signal.</li>
</ul>

<p><b>'Convolvers' section:</b></p>
<ul>
	<?php if ($s) { ?>
	<li><b>In L/R</b> - the balance between left and right channels of input signal that will be passed to the processor</li>
	<?php } ?>
	<li><b>Pre-delay</b> - amount of pre-delay added to the processed signal. Can be used to individually control
	    pre-delay of processed signal for each processor that can provide additional stereo effect for reverbs.
	</li>
	<li><b>Source</b> - combos allow to select file and track to use as the convolution for the processor.</li>
	<li><b>Mute</b> - button allows to disable the processor.</li>
	<li><b>Active</b> - led that indicates that the processor is active.</li>
	<li><b>Makeup</b> - amount of gain added to the processed signal for the processor.</li>
	<li><b>Out L/R</b> - the output balance of the processed signal between two channels in the stereo pair.</li>
</ul>

<p><b>'IR Equalizer' section:</b></p>
<ul>
	<li><b>Low-cut</b> - sets the slope of the high-pass butterworth filter, possible slopes are 6, 12 and 18 dB/octave.</li>
	<li><b>Low-cut freq</b> - the cutoff frequency of the high-pass butterworth filter.</li>
	<li><b>Faders</b> - faders that allow to change the loudness of eight corresponding frequency bands in range of -12..+12 dB</li>
	<li><b>High-cut</b> - sets the slope of the low-pass butterworth filter, possible slopes are 6, 12 and 18 dB/octave.</li>
	<li><b>High-cut freq</b> - the cutoff frequency of the low-pass butterworth filter.</li>
</ul>
