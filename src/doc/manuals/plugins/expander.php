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
	This plugin performs increasing of dynamic range of <?= $cc ?> input signal<?php 
	if ($m == 'ms') echo " in Mid-Side mode";
	elseif ($m == 'lr') echo " by applying individual processing to left and right channels separately";
	?>. Flexible sidechain-control configuration <?php
		if ($sc)
			echo " and additional sidechain input" . (($m == 'm') ? '' : 's') . " are ";
		else
			echo " is";
	?>provided. Both <b>downward</b> and <b>upward</b> modes are available. Also additional
	dry/wet control allows to mix processed and unprocessed signal together. 
</p>
<p><b>Controls:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the plugin bypasses signal.
	</li>
	<li><b>Pause</b> - pauses any updates of the expander graph.</li>
	<li><b>Clear</b> - clears all graphs.</li>
	<?php if ($m == 'ms') { ?>
		<li><b>MS Listen</b> - passes mid-side signal to the output of expander instead of stereo signal.</li>
	<?php } ?>
	<li><b>Gain<?= $sm ?></b> - enables drawing of gain amplification line and corresponding amplification meter.</li>
	<li><b>SC<?= $sm ?></b> - enables drawing of sidechain input graph and corresponding level meter.</li>
	<li><b>Env<?= $sm ?></b> - enables drawing of expander's envelope graph and corresponding level meter.</li>
	<li><b>In<?= $sm ?></b> - enables drawing of expander's input signal graph and corresponding level meter.</li>
	<li><b>Out<?= $sm ?></b> - enables drawing of expander's output signal graph and corresponding level meter.</li>
</ul>
<p><b>'Sidechain' section:</b></p>
<ul>
	<li><b>Preamp</b> - pre-amplification of the sidechain signal.</li>
	<li><b>Reactivity</b> - reactivity of the sidechain signal.</li>
	<li><b>Setup</b> - Sidechain configuration, available values:</li>
	<ul>
		<?php if ($sc) { ?>
			<li><b>Internal</b> - sidechain input is connected to expander's input.</li>
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
</ul>
<p><b>'Expander' section:</b></p>
<ul>
	<li><b>Mode</b> - expander mode: <b>Up</b> (upward) or <b>Down</b> (downward).</li>
	<li><b>Ratio</b> - expander ratio.</li>
	<li><b>Knee</b> - size of the knee.</li>
	<li><b>Makeup</b> - additional amplification gain after processing stage.</li>
	<li><b>Attack Level</b> - threshold of the expander, placed in the middle of the knee.</li>
	<li><b>Attack Time</b> - attack time of the expander.</li>
	<li><b>Release Level</b> - relative to the <b>Attack Level</b> threshold that sets up the threshold of <b>Release Time</b>.</li>
	<li><b>Release Time</b> - release time of the expander.</li>
	<li><b>Mix Dry</b> - the amount of dry (unprocessed) signal.</li>
	<li><b>Mix Wet</b> - the amount of wet (processed) signal.</li>
</ul>

<p><b>'Signal' section:</b></p>
<ul>
	<li><b>Input</b> - overall input gain.</li>
	<li><b>Output</b> - overall output gain.</li>
</ul>