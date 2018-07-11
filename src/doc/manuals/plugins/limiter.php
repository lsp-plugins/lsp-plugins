<?php
	plugin_header();
	
	$sc     =   (strpos($PAGE, 'sc_') === 0);
	$m      =   (strpos($PAGE, '_mono') > 0) ? 'm' : 's';
	$cc     =   ($m == 'm') ? 'mono' : 'stereo';
?>

<p>
	This plugin implements a brick-wall limiter with flexible configuration. It prevents input <?= ($m) ? 'mono' : 'stereo' ?>signal
	from raising over the specified <b>Threshold</b>. <?php if ($sc) {?> Additional sidechain inputs are provided for better use.<?php } ?>
</p>
<p><u>Attention:</u> this plugin implements set of limiting modes, most of them are iterative. That means that CPU load may be not stable, in other
words: the more work should be done, the more CPU resources will be used. Beware from extreme settings.</p>
<p><b>Controls:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the plugin bypasses signal.
	</li>
	<li><b>Pause</b> - pauses any updates of the limiter graph.</li>
	<li><b>Clear</b> - clears all graphs.</li>
	<li><b>Mode</b> - the selected mode to use by limiter:</li>
	<ul>
		<li><b>Classic</b> - limiter works like compressor with infinite ratio. May cause sound artifacts and clicking on extreme setup.</li>
		<li><b>Herm Thin</b>, <b>Herm Wide</b>, <b>Herm Tail</b>, <b>Herm Duck</b> - hermite-interpolated cubic functions are used to apply gain reduction.</b>
		<li><b>Exp Thin</b>, <b>Exp Wide</b>, <b>Exp Tail</b>, <b>Exp Duck</b> - exponent-interpolated functions are used to apply gain reduction.</b>
		<li><b>Line Thin</b>, <b>Line Wide</b>, <b>Line Tail</b>, <b>Line Duck</b> - linear-interpolated functions are used to apply gain reduction.</b>
		<li><b>Mixed Herm</b>, <b>Mixed Exp</b>, <b>Mixed Line</b> - classic mode with accurate overloaded peak removal by linear-interpolated, exponent-interpolated and linear-interpolated functions.</b>
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
	<li><b>Knee</b> - additional knee applied to the gain reduction algorithm.</li>
	<?php if (!$m) {?> 
	<li><b>Knee</b> - stereo link, the degree of mutual influence between gain reduction of stereo channels</li>
	<?php } ?>
</ul>
<p><b>'Signal' section:</b></p>
<ul>
	<li><b>Input</b> - overall input gain.</li>
	<li><b>Output</b> - overall output gain.</li>
</ul>
