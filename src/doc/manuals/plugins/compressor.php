<?php
	plugin_header();
	
	$sc     =   (strpos($PAGE, 'sc_') === 0);
	$m      =   (strpos($PAGE, '_mono') > 0) ? 'm' : (
			    (strpos($PAGE, '_stereo') > 0) ? 's' : (
				(strpos($PAGE, '_lr') > 0) ? 'lr' : (
				(strpos($PAGE, '_ms') > 0) ? 'ms' : '?'
				)));
	$sm     =   ($m == 'ms') ? ' M, S' : (($m != 'm') ? ' L, R' : '');
	$cc     =   ($m == 'm') ? 'mono' : 'stereo';
	$tt     =   ($m == 'ms') || ($m == 'lr');
?>

<p>
	This plugin performs compression of <?= $cc ?> input signal<?php 
	if ($m == 'ms') echo " in Mid-Side mode";
	elseif ($m == 'lr') echo " by applying individual processing to left and right channels separately";
	?>. Flexible sidechain-control configuration <?php
		if ($sc)
			echo " and additional sidechain input" . (($m == 'm') ? '' : 's') . " are ";
		else
			echo " is";
	?>provided. Different types of compression are
	possible: <b>downward</b>, <b>upward</b> and <b>parallel</b>. Also compressor may work as limiter
	in <b>Peak</b> mode with high <b>Ratio</b> and low <b>Attack</b> time. 
</p>
<p><b>Controls:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the plugin bypasses signal.
	</li>
	<li><b>Pause</b> - pauses any updates of the compressor graph.</li>
	<li><b>Clear</b> - clears all graphs.</li>
	<?php if ($m == 'ms') { ?>
		<li><b>MS Listen</b> - passes mid-side signal to the output of compressor instead of stereo signal.</li>
	<?php } ?>
	<li><b>Gain<?= $sm ?></b> - enables drawing of gain amplification line and corresponding amplification meter.</li>
	<li><b>SC<?= $sm ?></b> - enables drawing of sidechain input graph and corresponding level meter.</li>
	<li><b>Env<?= $sm ?></b> - enables drawing of compressor's envelope graph and corresponding level meter.</li>
	<li><b>In<?= $sm ?></b> - enables drawing of compressor's input signal graph and corresponding level meter.</li>
	<li><b>Out<?= $sm ?></b> - enables drawing of compressor's output signal graph and corresponding level meter.</li>
</ul>
<p><b>'Sidechain' section:</b></p>
<ul>
	<?php if (!$tt) { ?>
	<li><b>Position</b> - the position of the sidechain input. Available variants:</li>
	<ul>
		<li><b>Feed-forward</b> - sidechain input is connected to compressor's input. More aggressive compression.</li>
		<li><b>Feed-back</b> - sidechain input is connected to compressor's output. Vintage-style more accurate compression.</li>
		<?php if ($sc) { ?>
		<li><b>External</b> - sidechain signal is taken from additional (external) sidechain inputs of plugin.</li>
		<?php }?>
	</ul>
	<?php } ?>
	<li><b>Listen</b> - allows to listen the <b>processed</b> sidechain signal.</li>
	<li><b>Preamp</b> - pre-amplification of the sidechain signal.</li>
	<li><b>Reactivity</b> - reactivity of the sidechain signal.</li>
	<li><b>Lookahead</b> - look-ahead time of the sidechain relative to the input signal.</li>
	<?php if (!$tt) { ?>
	<li><b>Type</b> - combo box that allows to switch different types for sidechain processing. Available types are:</li>
	<?php } else {?>
	<li><b>Source</b> - set of combo boxes that allow to control type, position and source of sidechain. Available types are:</li>
	<?php } ?>
	<ul>
		<?php if ($tt) { ?>
			<li><b>Feed-forward</b> - sidechain input is connected to compressor's input. More aggressive compression.</li>
			<li><b>Feed-back</b> - sidechain input is connected to compressor's output. Vintage-style more accurate compression.</li>
			<?php if ($sc) { ?>
				<li><b>External</b> - sidechain signal is taken from additional (external) sidechain inputs of plugin.</li>
			<?php }?>
		<?php }?>
		<li><b>Peak</b> - peak mode.</li>
		<li><b>RMS</b> - root mean square of the input signal.</li>
		<li><b>Low-pass</b> - input signal processed by low-pass filter.</li>
		<li><b>Uniform</b> - input signal processed by uniform filter.</li>
		<?php if ($m != 'm') { ?>
			<li><b>Middle</b> - middle part of signal is used for sidechain processing.</li>
			<li><b>Side</b> - side part of signal is used for sidechain processing.</li>
			<li><b>Left</b> - only left channel is used for sidechain processing.</li>
			<li><b>Right</b> - only right channel is used for sidechain processing.</li>
		<?php } ?>
	</ul>
	<li><b>LPF</b> - allows to set up slope and cut-off frequency for the low-pass filter applied to sidechain signal.</li>
	<li><b>HPF</b> - allows to set up slope and cut-off frequency for the high-pass filter applied to sidechain signal.</li>
</ul>
<p><b>'Compressor' section:</b></p>
<ul>
	<li><b>Mode</b> - compression mode</li>
	<ul>
		<li><b>Downward (Down)</b> - the downward compressor.</li>
		<li><b>Upward (Up)</b> - the upward compressor.</li>
		<li><b>Boosting (Boost)</b> - the boosting compressor (upward with precise boost control).</li>
	</ul>
	<li><b>Ratio</b> - compression ratio.</li>
	<li><b>Knee</b> - size of compression knee.</li>
	<li><b>Makeup</b> - additional amplification gain after compression stage.</li>
	<li><b>Boost</b> - this knob is related to <b>upward</b> and <b>boosting</b> compression modes only.
		For <b>upward</b> mode it defines the threshold below which the constant amplification will be applied to the input signal.
		This prevents compressor from applying infinite amplification to very quiet signals.
		For <b>boosting</b> mode it defines the overall boost of the signal. The boost threshold is computed automatically. 
	</li>
	<li><b>Attack Thresh</b> - threshold of the compressor, placed in the middle of the knee.</li>
	<li><b>Attack Time</b> - attack time of the compressor.</li>
	<li><b>Release Thresh</b> - relative to the <b>Attack Thresh</b> threshold that sets up the threshold of <b>Release Time</b>.</li>
	<li><b>Release Time</b> - release time of the compressor.</li>
	<li><b>Mix Dry</b> - the amount of dry (unprocessed) signal, useful for parallel compression.</li>
	<li><b>Mix Wet</b> - the amount of wet (processed) signal, useful for parallel compression.</li>
</ul>

<p><b>'Signal' section:</b></p>
<ul>
	<li><b>Input</b> - overall input gain.</li>
	<li><b>Output</b> - overall output gain.</li>
</ul>