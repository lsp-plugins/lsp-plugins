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
	This plugin performs dynamic processing of <?= $cc ?> input signal<?php 
	if ($m == 'ms') echo " in Mid-Side mode";
	elseif ($m == 'lr') echo " by applying individual processing to left and right channels separately";
	?>. Flexible sidechain-control configuration <?php
		if ($sc)
			echo " and additional sidechain input" . (($m == 'm') ? '' : 's') . " are ";
		else
			echo " is";
	?>provided. There are possible different variants of dynamic processor to build:
	<b>compressor</b>, <b>limiter</b>, <b>gate</b>, <b>expander</b>, some kind of <b>transient designer</b>
	and many others due to possibility of flexible dynamic curve configuration. Also dynamic range can
	be split into sub-ranges that may have their individual <b>attack</b> and <b>release</b> timings.
</p>
<p><b>Controls:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the plugin bypasses signal.
	</li>
	<li><b>Pause</b> - pauses any updates of the time graph.</li>
	<li><b>Clear</b> - clears all graphs.</li>
	<?php if ($m == 'ms') { ?>
		<li><b>MS Listen</b> - passes mid-side signal to the output of the processor instead of stereo signal.</li>
	<?php } ?>
	<li><b>Gain<?= $sm ?></b> - enables drawing of gain amplification line and corresponding amplification meter.</li>
	<li><b>SC<?= $sm ?></b> - enables drawing of sidechain input graph and corresponding level meter.</li>
	<li><b>Env<?= $sm ?></b> - enables drawing of envelope graph and corresponding level meter.</li>
	<li><b>In<?= $sm ?></b> - enables drawing of input signal graph and corresponding level meter.</li>
	<li><b>Out<?= $sm ?></b> - enables drawing of output signal graph and corresponding level meter.</li>
	<?php if (($m == 'ms') || ($m == 'lr')) { ?>
	<li><b>Processor</b> - selects the corresponding channel for configuration.</li>
	<?php } ?>
</ul>
<p><b>'Processor' section:</b></p>
<ul>
	<li><b>Listen</b> - allows to listen the signal processed by sidechain<?php if (($m == 'ms') || ($m == 'lr')) { ?> for the selected processor<?php } ?>.</li>
	<li><b>Type</b> - set of combo boxes that allow to control type, position and source of sidechain. Available types are:</li>
	<ul>
		<?php if ($tt) { ?>
			<li><b>Feed-forward</b> - sidechain input is connected to processor's input. More aggressive compression.</li>
			<li><b>Feed-back</b> - sidechain input is connected to processor's output. Vintage-style more accurate compression.</li>
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
	<li><b>Preamp</b> - pre-amplification of the sidechain signal.</li>
	<li><b>Reactivity</b> - reactivity of the sidechain signal.</li>
	<li><b>Mode</b> - compression mode: <b>Upward (Up)</b> or <b>Downward (Down)</b>.</li>
	<li><b>Ratio low</b> - compression/expansion ratio below the lowest-threshold knee.</li>
	<li><b>Ratio high</b> - compression/expansion ratio after the highest-threshold knee.</li>
	<li><b>Attack</b> - default attack time used on the whole dynamics range.</li>
	<li><b>Release</b> - default release time used on the whole dynamics range.</li>
	<li><b>Makeup</b> - additional amplification gain after dynamic processing stage.</li>
	<li><b>Dry</b> - the amount of dry (unprocessed) signal.</li>
	<li><b>Wet</b> - the amount of wet (processed) signal.</li>
	<li><b>Ranges</b> - allows to configure up to four additional knees, attack and release ranges:</li>
	<ul>
		<li><b>Thr</b> - Knob that enables additional knee.</li>
		<li><b>Att</b> - Knob that enables additional attack range.</li>
		<li><b>Rel</b> - Knob that enables additional release range.</li>
		<li><b>Thresh</b> - Threshold of the additional knee, works only if corresponding <b>Thr</b> button is turned on.</li>
		<li><b>Gain</b> - Gain of the additional knee, works only if corresponding <b>Thr</b> button is turned on.</li>
		<li><b>Knee</b> - Softness of the knee, works only if corresponding <b>Thr</b> button is turned on.</li>
		<li><b>Attack</b> - Pair of knobs that allows to adjust the attack threshold and attack time of the additional
			attack range. The new attack time is applied if the envelope is <b>over</b> the specified threshold. Otherwise
			the attack time of previous range or default attack time (if there is no previous range) will be applied.
		</li>
		<li><b>Release</b> - Pair of knobs that allows to adjust the release threshold and release time of the additional
			release range. The new release time is applied if the envelope is <b>over</b> the specified threshold. Otherwise
			the release time of previous range or default release time (if there is no previous range) will be applied.
		</li>
	</ul>
</ul>
<p><b>'Signal' section:</b></p>
<ul>
	<li><b>Input</b> - overall input gain.</li>
	<li><b>Output</b> - overall output gain.</li>
</ul>
