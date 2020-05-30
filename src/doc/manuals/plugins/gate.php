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
?>

<p>
	This plugin performs gating of <?= $cc ?> input signal<?php 
	if ($m == 'ms') echo " in Mid-Side mode";
	elseif ($m == 'lr') echo " by applying individual processing to left and right channels separately";
	?>. Flexible sidechain-control configuration <?php
		if ($sc)
			echo " and additional sidechain input" . (($m == 'm') ? '' : 's') . " are ";
		else
			echo " is";
	?>provided. Additional <b>Hysteresis</b> curve is available to provide accurate control of the fading of the signal.
	Also additional	dry/wet control allows to mix processed and unprocessed signal together. 
</p>
<p><b>Controls:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the plugin bypasses signal.
	</li>
	<li><b>Pause</b> - pauses any updates of the gate graph.</li>
	<li><b>Clear</b> - clears all graphs.</li>
	<?php if ($m == 'ms') { ?>
		<li><b>MS Listen</b> - passes mid-side signal to the output of gate instead of stereo signal.</li>
	<?php } ?>
	<li><b>Gain<?= $sm ?></b> - enables drawing of gain amplification line and corresponding amplification meter.</li>
	<li><b>SC<?= $sm ?></b> - enables drawing of sidechain input graph and corresponding level meter.</li>
	<li><b>Env<?= $sm ?></b> - enables drawing of gate's envelope graph and corresponding level meter.</li>
	<li><b>In<?= $sm ?></b> - enables drawing of gate's input signal graph and corresponding level meter.</li>
	<li><b>Out<?= $sm ?></b> - enables drawing of gate's output signal graph and corresponding level meter.</li>
</ul>
<p><b>'Sidechain' section:</b></p>
<ul>
	<li><b>Preamp</b> - pre-amplification of the sidechain signal.</li>
	<li><b>Reactivity</b> - reactivity of the sidechain signal.</li>
	<li><b>Lookahead</b> - look-ahead time of the sidechain relative to the input signal.</li>
	<li><b>Setup</b> - Sidechain configuration, available values:</li>
	<ul>
		<?php if ($sc) { ?>
			<li><b>Internal</b> - sidechain input is connected to gate's input.</li>
			<li><b>External</b> - sidechain signal is taken from additional (external) sidechain inputs of plugin.</li>
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
	<li><b>Listen</b> - allows to listen the <b>processed</b> sidechain signal.</li>
	<li><b>LPF</b> - allows to set up slope and cut-off frequency for the low-pass filter applied to sidechain signal.</li>
	<li><b>HPF</b> - allows to set up slope and cut-off frequency for the high-pass filter applied to sidechain signal.</li>
</ul>
<p><b>'Gate' section:</b></p>
<ul>
	<li><b>Reduction</b> - the amount of gain applied to the input signal when the gate is cloed.</li>
	<li><b>Makeup</b> - additional amplification gain after processing stage.</li>
	<li><b>Attack</b> - attack time of the gate.</li>
	<li><b>Release</b> - release time of the gate.</li>
	<li><b>Curve</b> - Basic gate curve characteristics:</li>
	<ul>
		<li><b>Thresh</b> - threshold, the level of input signal at which the gate becomes open.</li>
		<li><b>Zone</b> - the size of the transition zone below the threshold.</li>
	</ul>
	<li><b>Hysteresis <?= $sm ?></b> - enables additional hysteresis curve:</li>
	<ul>
		<li><b>Thresh</b> - threshold (relative to basic curve), the level of input signal at which the gate starts to close.</li>
		<li><b>Zone</b> - the size of the transition zone below the hysteresis threshold.</li>
	</ul>
	<li><b>Mix Dry</b> - the amount of dry (unprocessed) signal.</li>
	<li><b>Mix Wet</b> - the amount of wet (processed) signal.</li>
</ul>

<p><b>'Signal' section:</b></p>
<ul>
	<li><b>Input</b> - overall input gain.</li>
	<li><b>Output</b> - overall output gain.</li>
</ul>