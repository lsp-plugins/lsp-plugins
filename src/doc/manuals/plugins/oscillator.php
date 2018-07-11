<?php
	plugin_header();
?>

<p>
	This plugin implements a simple yet flexible utility signal generator.
	It can be used to provide reference waves in replacement, additive and
	multiplicative (modulation) <b>Mode</b>.
</p>
<p><b>Controls:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the plugin bypasses signal.
	</li>
</ul>
<p><b>'Oscillator Function' section:</b></p>
<ul>
	<li><b>Wave Selection</b> - oscillator waveform.</li>
	<ul>
		<li><b>Sine</b> - Pure sine wave.</li>
		<li><b>Cosine</b> - Pure cosine wave.</li>
		<li><b>Square Sine</b> - Pure squared sine wave.</li>
			<ul>
				<li><b>Invert</b> - Switch the wave sign.</li>
			</ul>
		<li><b>Squared Cosine</b> - Pure squared cosine wave.</li>
			<ul>
				<li><b>Invert</b> - Switch the wave sign.</li>
			</ul>
		<li><b>Rectangular</b> - Mathematically exact square wave (aliased).</li>
		<ul>
			<li><b>Duty Ratio</b> - Fraction of the period in which the wave is positive.</li>
		</ul>
		<li><b>Sawtooth</b> - Mathematically exact sawtooth wave (aliased).</li>
		<ul>
			<li><b>Width</b> - Fraction of the period at which the wave peaks.</li>
		</ul>
		<li><b>Trapezoid</b> - Mathematically exact trapezoidal wave (aliased).</li>
		<ul>
			<li><b>Raise</b> - Fraction of half period in which the wave ramps up.</li>
			<li><b>Fall</b> - Fraction of half period in which the wave ramps down.</li>
		</ul>
		<li><b>Pulsetrain</b> - Mathematically exact pulsetrain wave (aliased).</li>
		<ul>
			<li><b>Positive Width</b> - Fraction of half period occupied by the positive pulse.</li>
			<li><b>Negative Width</b> - Fraction of half period occupied by the negative pulse.</li>
		</ul>
		<li><b>Parabolic</b> - Mathematically exact parabolic wave (aliased).</li>
		<ul>
			<li><b>Width</b> - Fraction of the period in which the parabula is contained.</li>
			<li><b>Invert</b> - Switch the wave sign.</li>
		</ul>
		<li><b>Band Limited Rectangular</b> - Band limited rectangular wave (antialiasing).</li>
		<ul>
			<li><b>Duty Ratio</b> - Fraction of the period in which the wave is positive.</li>
		</ul>
		<li><b>Band Limited Sawtooth</b> - Band limited sawtooth wave (antialiasing).</li>
		<ul>
			<li><b>Width</b> - Fraction of the period at which the wave peaks.</li>
		</ul>
		<li><b>Band Limited Trapezoid</b> - Band limited trapezoid wave (antialiasing).</li>
		<ul>
			<li><b>Raise</b> - Fraction of half period in which the wave ramps up.</li>
			<li><b>Fall</b> - Fraction of half period in which the wave ramps down.</li>
		</ul>
		<li><b>Band Limited Pulsetrain</b> - Band limited pulsetrain wave (antialiasing).</li>
		<ul>
			<li><b>Positive Width</b> - Fraction of half period occupied by the positive pulse.</li>
			<li><b>Negative Width</b> - Fraction of half period occupied by the negative pulse.</li>
		</ul>
		<li><b>Band Limited Parabolic</b> - Band limited parabolic wave (antialiasing).</li>
		<ul>
			<li><b>Width</b> - Fraction of the period in which the parabula is contained.</li>
			<li><b>Invert</b> - Switch the wave sign.</li>
		</ul>
	</ul>
</ul>
<p><b>'General' section:</b></p>
<ul>
	<li><b>Mode</b> - oscillator mode.</li>
	<ul>
		<li><b>Add</b> - add the oscillator wave to the plugin input.</li>
		<li><b>Multiply</b> - multiply the plugin input by the oscillator wave (amplitude modulation).</li>
		<li><b>Replace</b> - replace the plugin input with the oscillator wave.</li>
	</ul>
	<li><b>Oversampling</b> - oversampling mode, available only for <b>band limited synthesis</b>.</li>
	<ul>
		<li><b>None</b> - oversampling is not used.</li>
		<li><b>x2</b> - 2x downsampling of band-limited signal.</li>
		<li><b>x3</b> - 3x downsampling of band-limited signal.</li>
		<li><b>x4</b> - 4x downsampling of band-limited signal.</li>
		<li><b>x6</b> - 6x downsampling of band-limited signal.</li>
		<li><b>x8</b> - 8x downsampling of band-limited signal.</li>
	</ul>
	<li><b>Frequency</b> - fundamental frequency of oscillator wave.</li>
	<li><b>Amplitude</b> - amplitude of oscillator wave.</li>
	<li><b>Phase</b> - phase offset of oscillator wave.</li>
	<li><b>DC Offset</b> - DC offset of oscillator wave.</li>
	<li><b>DC Reference</b> - reference value for DC offset.</li>
	<ul>
		<li><b>Wave DC</b> - Reference with respect natural wave DC: when <b>DC Offset</b>
			is zero the wave DC equals the natural DC value of the oscillator wave.</li>
		<li><b>Zero DC</b> - Reference with respect zero DC: when <b>DC Offset</b> is zero the wave DC equals zero.</li>
	</ul>
</ul>
