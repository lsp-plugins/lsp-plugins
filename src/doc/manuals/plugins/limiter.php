<?php
	plugin_header();
	
	$sc     =   (strpos($PAGE, 'sc_') === 0);
	$m      =   (strpos($PAGE, '_mono') > 0) ? 'm' : 's';
	$cc     =   ($m == 'm') ? 'mono' : 'stereo';
?>

<p>
	This plugin implements a limiter with flexible configuration. In most cases it acts as a brick-wall limiter but there are several settings
	for which is acts as an compressor with extreme settings, so the output signal may exceed the limiter's threshold. 
	It prevents input <?= ($m) ? 'mono' : 'stereo' ?>signal
	from raising over the specified <b>Threshold</b>. <?php if ($sc) {?> Additional sidechain inputs are provided for better use.<?php } ?>
</p>
<p><u>Attention:</u> this plugin implements set of limiting modes, most of them are iterative. That means that CPU load may be not stable, in other
words: the more work should be done, the more CPU resources will be used. Beware from extreme settings.</p>
<p>The gain reduction algorithm is split into two stages:</p>
<ul>
    <li>
    	<b>Automated Level Regulation</b> (<b>ALR</b>) which acts like a compressor with infinite ratio 
    	for purpose of estimating the smoothed gain reduction level. The gain reduction level is controlled 
    	by the envelope of the signal applied to the compressor's gain reduction curve. The smoothness of the 
    	envelope is controlled by the <b>Attack</b> and <b>Release</b> knobs in the <b>ALR</b> section. Since 
    	different signals have different envelopes with short release time, the <b>Knee</b> knob allows to adjust 
    	the threshold of the compressor. Adjusting <b>Knee</b> may give possibility to gain some additional decibels
    	of loudness in the final result.
    </li>
    <li>
        Peak cutting algorithm which searches peaks above the threshold values and applies short gain reduction patches
        to the signal. The form of patch is controlled by the <b>Mode</b> selector and it's length is controlled by 
        the corresponding <b>Attack</b> and <b>Release</b> knobs. Note that <b>Lookahead</b> also affects these values:
        <b>Attack</b> can not be larger than <b>Lookahead</b> and <b>Release</b> can not be twice larger as <b>Lookahead</b>.
        Setting these values larger than allowed automatically makes them considered to be set to maximum possible values.
    </li>
</ul>

<p>Simplified peak processing example is shown on the following picture:</p>
<?php out_image('graph/limiter-reduction', 'Simplified peak processing example') ?>
<p>
	Of course, the output signal does not repeat the envelope form of input signal because it's amplitude is changed 
	smoothly, so in fact the form of output signal is more complicated.
</p> 
<p>
	Currently there are three forms of patches applied to the gain curve -
	<b>hermite</b> (using cubic polynom for interpolation transients), <b>exponential</b> and <b>linear</b>.
	These forms can be explained with following picture:
</p>
<?php out_image('graph/limiter-patches', 'Forms of patches applied to signal') ?>
<p>
	Gain reduction patch affects not only the peak sample, but also surrounding samples.
	The position and form of this interpolation is related to the peak, so there are four 
	different variants of patch envelope - <b>thin</b>, <b>tail</b>, <b>duck</b> and <b>wide</b>.
	All these forms related to the peak are shown on the following picture:
</p>
<?php out_image('graph/limiter-envelope', 'Envelope forms of the patch') ?>
<p>
	On this image, sloping lines mean the transision part of the patch.
	The flat cap in the middle before the peak is a half of attack time, the flat cap in the middle after the peak is a half of release time.
	Also it's obvious that different envelope forms differently affect dynamics of the signal.
</p>

<p><b>Controls:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the plugin bypasses signal.
	</li>
	<li><b>Pause</b> - pauses any updates of the limiter graph.</li>
	<li><b>Clear</b> - clears all graphs.</li>
	<li><b>Mode</b> - the selected mode to use by limiter:</li>
	<ul>
		<li><b>Herm Thin</b>, <b>Herm Wide</b>, <b>Herm Tail</b>, <b>Herm Duck</b> - hermite-interpolated cubic functions are used to apply gain reduction.</b>
		<li><b>Exp Thin</b>, <b>Exp Wide</b>, <b>Exp Tail</b>, <b>Exp Duck</b> - exponent-interpolated functions are used to apply gain reduction.</b>
		<li><b>Line Thin</b>, <b>Line Wide</b>, <b>Line Tail</b>, <b>Line Duck</b> - linear-interpolated functions are used to apply gain reduction.</b>
	</ul>
	<li><b>O/S</b> - oversampling mode:</li>
	<ul>
		<li><b>None</b> - oversampling is not used.</li>
		<li><b>Half 2x(2L)</b>, <b>Half 2x(3L)</b> - 2x Lanczos oversampling of Sidechain signal with 2 or 3 lobes (L) in the kernel.</li>
		<li><b>Half 3x(2L)</b>, <b>Half 3x(3L)</b> - 3x Lanczos oversampling of Sidechain signal with 2 or 3 lobes (L) in the kernel.</li>
		<li><b>Half 4x(2L)</b>, <b>Half 4x(3L)</b> - 4x Lanczos oversampling of Sidechain signal with 2 or 3 lobes (L) in the kernel.</li>
		<li><b>Half 6x(2L)</b>, <b>Half 6x(3L)</b> - 6x Lanczos oversampling of Sidechain signal with 2 or 3 lobes (L) in the kernel.</li>
		<li><b>Half 8x(2L)</b>, <b>Half 8x(3L)</b> - 8x Lanczos oversampling of Sidechain signal with 2 or 3 lobes (L) in the kernel.</li>
		<li><b>Full 2x(2L)</b>, <b>Full 2x(3L)</b> - 2x Lanczos oversampling of Sidechain and Input signal with 2 or 3 lobes (L) in the kernel.</li>
		<li><b>Full 3x(2L)</b>, <b>Full 3x(3L)</b> - 3x Lanczos oversampling of Sidechain and Input signal with 2 or 3 lobes (L) in the kernel.</li>
		<li><b>Full 4x(2L)</b>, <b>Full 4x(3L)</b> - 4x Lanczos oversampling of Sidechain and Input signal with 2 or 3 lobes (L) in the kernel.</li>
		<li><b>Full 6x(2L)</b>, <b>Full 6x(3L)</b> - 6x Lanczos oversampling of Sidechain and Input signal with 2 or 3 lobes (L) in the kernel.</li>
		<li><b>Full 8x(2L)</b>, <b>Full 8x(3L)</b> - 8x Lanczos oversampling of Sidechain and Input signal with 2 or 3 lobes (L) in the kernel.</li>
	</ul>
	<li><b>Dither</b> - allows to enable dithering for the specified sample bitness.</li>
	<li><b>SC</b> - enables drawing of sidechain input graph and corresponding level meter.</li>
	<li><b>Gain</b> - enables drawing of gain amplification line and corresponding amplification meter.</li>
	<li><b>In</b> - enables drawing of limiter's input signal graph and corresponding level meter.</li>
	<li><b>Out</b> - enables drawing of limiter's output signal graph and corresponding level meter.</li>
</ul>
<p><b>'ALR' section:</b></p>
<ul>
	<li>
		<b>Attack</b> - allows to control how the raise of the input signal affects the ALR envelope curve. 
		The more value, the more quickly the envelope goes to it's maximum.
	</li>
	<li>
		<b>Release</b> - allows to control how the fall of the input signal affect the ALR envelope curve. 
		The more value, the more quickly the envelope goes to it's minimum.
	</li>
	<li>
		<b>Knee</b> - allows to control the threshold of the ALR gain curve and, in fact, adjust the balance between
		two gain reduction stages. Raising the value delegates more work to the peak-cutting algorithm. Lowering the
		value delegates more work to the ALR gain reduction algorithm.
	</li>
</ul>
<p><b>'Limiter' section:</b></p>
<ul>
	<li><b>SC Preamp</b> - sidechain pre-amplification gain.</li>
	<?php if ($sc) {?> 
		<li><b>Ext</b> - this button enables external sidechain.</li>
	<?php }?>
	<li><b>Lookahead</b> - the size of lookahead buffer in milliseconds. Forces the limiter to add the corresponding latency to output signal.</li>
	<li><b>Threshold</b> - the maximum input level of the signal allowed by limiter.</li>
	<li><b>Boost</b> - applies corresponding to the <b>Threshold</b> gain to the output signal.</li>
	<li><b>Attack</b> - the attack time of the limiter. Can not be greater than Lookahead time (greater values are truncated) for some modes.</li>
	<li><b>Release</b> - the attack time of the limiter. Can not be twice greater than Lookahead time (greater values are truncated) for some modes.</li>
	<?php if (!$m) {?> 
	<li><b>Knee</b> - stereo link, the degree of mutual influence between gain reduction of stereo channels</li>
	<?php } ?>
</ul>
<p><b>'Signal' section:</b></p>
<ul>
	<li><b>Input</b> - overall input signal gain adjustment.</li>
	<li><b>Output</b> - overall output signal gain adjustment.</li>
</ul>
