<?php
	$CTL = $RES_ROOT . "/img/controls/";
?>
<p>The following picture demonstrates typical Graphical User Interface (GUI) of plugin:</p>
<img src="<?= $RES_ROOT ?>/img/plugins/phase_detector.png">

<p>Each plugin consists of the following elements:</p>
<ul>
	<li><b>AudioFile</b> - audio file chooser and editor.</li>
	<li><b>Button</b> - button control.</li>
	<li><b>Combo</b> - combo box control.</li>
	<li><b>Combo Group</b> - combo group control.</li>
	<li><b>Fader</b> - fader control.</li>
	<li><b>File Saver</b> - file saving control.</li>
	<li><b>Fraction</b> - control for setting musical time signature.</li>
	<li><b>Graph</b> - graphical output area.</li>
	<li><b>Group</b> - group control.</li>
	<li><b>Indicator</b> - digital LED indicator.</li>
	<li><b>Inline Display</b> - inline display.</li>
	<li><b>Knob</b> - rotating knob control.</li>
	<li><b>Label</b> - label with text information.</li>
	<li><b>Led</b> - LED.</li>
	<li><b>Meter</b> - metering control.</li>
	<li><b>Parameter</b> - the value of controllable parameter.</li>
	<li><b>Progress Bar</b> - progress control.</li>
	<li><b>Rack</b> - Rack ears with menu and mounting studs.</li>
	<li><b>Sample Editor</b> - sample editor.</li>
	<li><b>Switch</b> - Switch control.</li>
</ul>

<p>Colors of widgets may vary but the color scheme of widgets often follows these rules:</p>
<ul>
	<li>Widgets that adjust parameters of similar objects are consolidated into one common group.</li>
	<li>Widgets that control parameters of one object often have similar colors.</li>
	<li>Widgets associated with audio channels often use the following color scheme:</li>
	<ul>
		<li><b style="color:#0000ff">Blue</b> and <b style="color:#ff00ff">Magenta</b> - for thresholds.</li>
		<li><b style="color:#00c0ff">Sapphirine</b> - for mono channel, stereo channel (when both left and right channels are controlled at same time), middle channel.</li>
		<li><b style="color:#ff0000">Red</b> - for left channel in stero pair.</li>
		<li><b style="color:#00ffff">Cyan</b> - for right channel in stereo pair, direct output channel.</li>
		<li><b style="color:#00cc00">Green</b> - for side channel or sidechain.</li>
	</ul>
	<li>Each widget that allows to set continuous parameters has corresponding label displaying it's current value and, optionally, units</li>
</ul>

<p>Below the detailed description of controls is present.</p>

<h2>AudioFile</h2>

<p>AudioFile widget is used for loading and editing audio files. It tells to the plugin the actual location of file on
file system.</p>

<p>By default there is no file associated with plugin, so the file widget displays
<b style="color: #00c000">'Click to load'</b> text.</p>
<div class="images">
	<img src="<?= $CTL ?>file_unloaded.png">
</div>

<p>By clicking left mouse button on the widget you may open file choosing dialog and select the file
to use by plugin.</p>

<p>After the path to the file will be passed to plugin, the plugin starts to load the file, and the status
displayed by the file widget changes. If file was successfully loaded, file widget will display
the corresponding content of the file and it's name (without path). On error, error message is displayed
with red color.</p>
<div class="images">
	<img src="<?= $CTL ?>file_error.png">
</div>

<p>By clicking right mouse button, popup dialog appears that allows to cut, copy, paste and clear contents of widget</p>
<p>There is also the way to force plugin to unload file. For this purpose simply double-click by right
mouse button on widget's area if popup menu does not appear.</p>

<p>Example of file widget:</p>
<div class="images">
	<img src="<?= $CTL ?>file_loaded.png">
</div>

<h2>Button</h2>

<p>Buttons are mostly used to turn on/turn off some binary parameter. Rarely they are also used for switching between different processing modes
(See <?php plugin_ref('comp_delay_mono'); ?> plugin for example). There are two different types of buttons:</p>
<ul>
	<li><b>push/pop</b> - these buttons allow to change the state of parameter and keep it until next button push occurs.</li>
	<li><b>trigger</b> - these buttons allow to trigger some event while the button is pressed and return to initial state when the button is released.</li>
</ul>

<p>Push/pop buttons may be pressed by left mouse button click and do not affect
any parameter changes until the left mouse button is released over the button widget.
It is possible to cancel button press by moving mouse cursor out of the widget's area
or by additionally pressing right mouse button.</p>

<p>Trigger buttons have radically different behaviour. They trigger event on left mouse
button click and every time they fall into pushed state. So there is possible to trigger
sequence of events by pressing left mouse button over the button and repeatedly moving
mouse cursor outside widget's area and back.</p>

<p>For space economy and clarity improving purposes buttons may be combined with LEDs.</p>

<p>Here is example of different button widgets:</p>
<div class="images">
	<img src="<?= $CTL ?>button.png">
	<img src="<?= $CTL ?>button_pressed.png">
	<img src="<?= $CTL ?>button_led_blue.png">
	<img src="<?= $CTL ?>button_led_green.png">
	<img src="<?= $CTL ?>button_led_yellow.png">
</div>

<h2>Combo</h2>

<p>Combo is a drop-down list that in normal state displays only currently selected item. When clicking
by left mouse button, it shows drop-down list with all possible variants for choosing. Only one list item
may be selected at the same time.</p>

<p>It is very useful for defining controls that use enumerations or for switching between different
control groups.</p>

<p>Here is example of combo box widgets:</p>
<div class="images">
	<img src="<?= $CTL ?>combo.png">
</div>

<h2>Combo Group</h2>

<p>Combo group is a <b>Group</b> control that allows to select the displayed content by calling a drop-down list.
The drop-down list is accessible by clicking with left mouse button the header header of the group.</p>

<p>Here is example of combo group widget:</p>
<div class="images">
	<img src="<?= $CTL ?>combo_group.png">
</div>

<p>When clicking the group header, we get all possible variants for choosing widget groups for displaying:</p>
<div class="images">
	<img src="<?= $CTL ?>combo_group_select.png">
</div>

<h2>Fader</h2>
<p>The Fader widget allows to adjust value for continuous parameters in the pre-defined range.</p>
<p>It is possible to achieve more precision by using the right mouse button instead of left when changing fader's value.</p>
<p>To cancel editing, the opposite mouse button should be pressed (right if used left and vice verse).</p> 
<p>To reset parameter to it's default value, issue double click by the left mouse button.</p>
<div class="images">
	<img src="<?= $CTL ?>fader.png">
</div>

<h2>FileSaver</h2>
<p>This widget allows to choose the file and tell plugin to save some data to it. It also allows to display progress
and status of operation.</p>
<div class="images">
	<img src="<?= $CTL ?>save_v1.png">
	<img src="<?= $CTL ?>save_v2.png">
	<img src="<?= $CTL ?>save_v3.png">
</div>

<h2>Fraction</h2>
<p>This widget allows to set-up time signature of the audio that is present as a fraction where top and bottom parts are
integer values. Both numerator and denominator of fraction is adjustable by mouse wheel. Also, when clicking numerator
or denominator with left mouse button, popup list box appears that allows to quickly set the required value.</p>
<div class="images">
	<img src="<?= $CTL ?>fraction.png">
</div>

<h2>Graph</h2>

<p>Graph is special widget for displaying graphical information about processes that flow
in the DSP core of plugin. It may contain lines, text labels, different curves (meshes),
markers and dots.</p>

<p>Example of graph widget:</p>
<div class="images">
	<img src="<?= $CTL ?>graph.png">
</div>

<p>All elements of graph except dots can not change input parameters of plugin. Dots may control
simultaneously up to three parameters by the following events:</p>
<ul>
	<li>moving dot horizontally;</li>
	<li>moving dot vertically;</li>
	<li>scrolling mouse wheel over the dot.</li>
</ul>
<p>By moving dot over the graph widget two parameters may be changed simultaneously. This can be
done by left-clicking on the dot, holding mouse button and moving mouse cursor over the graph
widget's area. Also, additional tolerance may be reached by right-clicking the dot instead of
left-clicking, this makes the change of parameters more accurate. The movement may be cancelled
by pressing the opposite button of the mouse.</p>
<p>Additional tolerance for mouse scroll may be achieved by pressing shift key on keyboard</p> 

<p>Example showing dot controls:</p>
<div class="images">
	<img src="<?= $CTL ?>dots.png">
</div>

<h2>Group</h2>

<p>Group widget is a special widget that allows to visually distinguish set of widgets that
control parameters of some device or set of similar devices.</p>

<p>Example showing controls consolidated into group:</p>
<div class="images">
	<img src="<?= $CTL ?>group.png">
</div>

<h2>Indicator</h2>

<p>Indicators are widgets that display measured or computed value by the plugin.</p>

<p>Example of indicator widget:</p>
<div class="images">
	<img src="<?= $CTL ?>indicator.png">
</div>

<h2>Inline display</h2>

<p>Inline displays are not widgets or elements of plugin's UI at all. Instead of this, they're part of
the host UI since the <a href="http://ardour.org/">Ardour DAW</a> implemented Inline Display extension for
LV2 format.</p>
<p>So they're available in the Ardour's mixer strip even if UI is not shown. Inline displays also are
available in <a href="http://harrisonconsoles.com/">Mixbus DAW</a> as the relative to Ardour product.</p>
<p>Because inline display is an LV2-specific exension, it is available only for LV2 version of LSP plugins.
But standalone JACK versions of plugins that support inline displays in LV2, draw them on window's icon.</p>

<p>Example of inline displays:</p>
<div class="images">
	<img src="<?= $CTL ?>inline_display1.png">
	<img src="<?= $CTL ?>inline_display2.png">
</div>

<h2>Knob</h2>
<p>Knobs are the mostly used controls by plugin GUIs. They allow to adjust value for continuous parameters
in the pre-defined range. Higlighted part of the knob's scale shows deviation from it's zero position.
There are many ways to adjust the controlled parameter.<p>
<p>The first way to change the parameter is performing left mouse click on the knob's cap, holding mouse
button and moving cursor up and down. To apply more accurate adjustment, right button of mouse may be pressed
while moving cursor.</p>
<p>The second way to change parameter's value may be reached by using mouse scroll. To perform more accurate
adjustment, shift key may be pressed on keyboard. To accelerate the adjustment, control key may be pressed on keyboard.</p>
<p>For all knobs (especially when they are stiff) quick adjustment of parameter may be achived by left-clicking
knob's scale. Also, additionally mouse button may be hold and parameter will be adjusted by moving mouse
clockwise/counter-clockwise.</p>
<p>To reset parameter to it's default value, left mouse button double click may be issued.</p>
 
<p>Example showing knob controls:</p>
<div class="images">
	<img src="<?= $CTL ?>knob_blue.png">
	<img src="<?= $CTL ?>knob_green.png">
</div>

<h2>Label</h2>
<p>Labels are widgets that display static text information.<p>

<p>Example of label widgets:</p>
<div class="images">
	<img src="<?= $CTL ?>labels.png">
</div>

<h2>Led</h2>
<p>LEDs are used to display state of binary output parameter.<p>
<p>Example of LEDs:</p>
<div class="images">
	<img src="<?= $CTL ?>led_dark.png">
	<img src="<?= $CTL ?>led_bright.png">
</div>

<h2>Meter</h2>
<p>Meters are used for metering some values (mostly, levels of the audio signal). They may contain yellow
and red zones. They respectivelly show that signal exceeds -6dB and 0dB levels.<p>
<p>Example of meters:</p>
<div class="images">
	<img src="<?= $CTL ?>vu_meters.png">
	<img src="<?= $CTL ?>vu_meters_red.png">
</div>

<h2>Parameter</h2>
<p>Parameters are widgets that display the actual value and measuring units of some controlled parameter.
It is possible do double-click the parameter and enter it's value manually in the popup window.
<p>
	<img src="<?= $CTL ?>params.png">
	<img src="<?= $CTL ?>param_enter.png">
</p>

<h2>Progress Bar</h2>
<p>
  Progress Bar is a widget for monitoring execution progress of offline tasks.
  It shows the actual completion percentage of the background job.
<p>
<p>Example of progress bar:</p>
<div class="images">
	<img src="<?= $CTL ?>progress_bar.png">
</div>

<h2>Rack</h2>
<p>Rack ears contain the logo 'LSP' in the left part of UI and it's short model code in the right part of UI</p>
<div class="images">
	<img src="<?= $CTL ?>rack_left.png">
	<img src="<?= $CTL ?>rack_right.png">
</div>

<p>Plugins typically use the folowing model code format: <b>XXXX&nbsp;MM&nbsp;NN&nbsp;CC</b>, where:</p>
<ul>
	<li><b>XXXX</b> - arconym from the full name of plugin, for example <b>GE</b> for <b>Grafischer Entzerrer</b>.</li>
	<li><b>MM</b> - modifiers that show additional facilities of plugin like <b>M</b> (MIDI) or <b>SC</b> (Side chain)</li>
	<li><b>NN</b> - number of devices that work simultaneously. For example, <b>16</b> filters for the equalizer.</li>
	<li><b>CC</b> - channels that plugin operates:</li>
	<ul>
		<li><b>M</b> - mono;</li>
		<li><b>S</b> - stereo;</li>
		<li><b>D</b> - stereo with additional direct output channel;</li>
		<li><b>LR</b> - stereo, but separately left and right channels;</li>
		<li><b>MS</b> - middle and side channels.</li>
	</ul>
</ul>

<p>The second function of logo and model bars is showing context menu that allows to save or load state of all
plugin's parameters to/from plain text file:</p>
<div class="images">
	<img src="<?= $CTL ?>context_menu.png">
</div>

<p>The content of text file can be manually edited. Each parameter has detailed description. Example of the
configuration file contents:</p>
<pre>
#-------------------------------------------------------------------------------
#
# This file contains configuration of the audio plugin.
#   Plugin name:         Verzögerungsausgleicher Mono (Delay Compensator Mono)
#   Plugin version:      1.0.0
#   LV2 URI:             http://lsp-plug.in/plugins/lv2/comp_delay_mono
#   VST identifier:      jav8
#   LADSPA identifier:   5002065
#
# (C) Linux Studio Plugins Project 
#   http://lsp-plug.in/ 
#
#-------------------------------------------------------------------------------

# Bypass: true/false
bypass = false

# Mode: 0..2
#   0: Samples
#   1: Distance
#   2: Time
mode = 2

# Samples [samp]: 0..10000
samp = 0

# Meters [m]: 0..200
m = 0

# Centimeters [cm]: 0.000000..100.000000
cm = 0.000000

# Temperature [°C]: -60.000000..60.000000
t = 20.000000

# Time [ms]: 0.000000..1000.000000
time = 3.349236

# Dry amount [G]: 0.000000..10.000000
dry = 0.000000

# Wet amount [G]: 0.000000..10.000000
wet = 1.000000

# Output gain [G]: 0.000000..10.000000
g_out = 1.000000

#-------------------------------------------------------------------------------
</pre>

<p>Additionally, the short form of rack may be turned on by pressing <b>'Toggle rack mount'</b>
from the context menu:</p>
<div class="images">
	<img src="<?= $CTL ?>rack_top.png">
</div>

<h2>Sample Editor</h2>

<p>Sampe Editor widget is used for viewing and editing audio samples. It is similar to AudioFile wiget but
unlike AudioFile widget, it does not allow to load files from file system.</p>

<p>By default if there is no sample data, the file widget displays
<b style="color: #00c000">'No data'</b> text.</p>
<p>Example of sample editor widget:</p>
<div class="images">
	<img src="<?= $CTL ?>sample_editor.png">
</div>

<h2>Switch</h2>
<p>Switch is an alternative to the button widget but has different graphic presentation. It
is mostly used as bypass control for bypassing the processed by plugin signal.</p>

<div class="images">
	<img src="<?= $CTL ?>switch.png">
</div>









