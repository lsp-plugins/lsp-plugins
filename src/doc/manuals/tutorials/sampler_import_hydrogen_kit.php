<?php
$CTL = $RES_ROOT . "/img/tutorials/";
?>
<p>
	Our goal is to import an existing 808 drumkit into the Multi-Sampler,
	configured so that:
</p>
<ul>
	<li>each drum is triggered by a single key on a midi keyboard</li>
	<li>each drum sends stereo out to its own track in the drum subgroup</li>
</ul>
<p>
	This allows us to program a full drum kit using a single sampler and to mix,
	modify, or add effects to each drum individually.
</p>
<p>
	<b>Download a Hydrogen drumkit:</b>
</p>
<p>
	There are a lot of free Hydrogen drumkits available on the internet.
	For now, lets start with the 808 kit that comes with
	<a href="https://github.com/hydrogen-music/hydrogen/tree/master/data/drumkits">Hydrogen</a>.
</p>

<p>
	<b>Import the Hydrogen drumkit:</b>
</p>
<ul>
	<li>create a new midi track with lsp-sampler <?php plugin_ref('multisampler'); ?> x24.</li>
	<li>open the lsp-sampler plugin and click on the blue <em>LSP</em> button</li>
	<li>select <em>import</em> >> <em>hydrogen drumkit file</em> and
		navigate to the <em>drumkit.xml</em> you just downloaded
	</li>
</ul>

<p>
	<b>Test the kit:</b>
</p>

<p>
	<img src="<?= $RES_ROOT ?>/img/tutorials/lsp-sampler-hydrogen-kit.png">
</p>

<ul>
	<li>select the <em>Working area</em> dropdown in the top left of the
		screen and choose <em>Mixer 0-11</em> or <em>Mixer 12-23</em></li>
	<li>click the white <em>listen</em> button at the bottom of the screen
		to hear the drum sample
	</li>
	<li><b>NOTE</b> This kit only has 18 different samples so instruments
		19-23 will be blank.</li>
</ul>

<h3>Create a template</h3>
<p>
	<b>Adjust panning</b>
</p>

<p>The good recommendation to start with every instrument panned to the center and then
	adjust panning per session as desired.</p>
<ul>
	<li>Open the LSP-Sampler plugin</li>
	<li>select the <em>Working area</em> dropdown in the top left of the
		screen and choose <em>Mixer 0-11</em></li>
	<li><b>To pan an instrument to the center</b></li>
	<ul>
		<li>double left click on the <em>(%)</em> for instrument 0 pan left
		</li>
		<li>enter <em>0.0</em></li>
		<li>repeat for pan right</li>
	</ul>

	<li><b>To pan an instrument to the left</b></li>
	<ul>
		<li>double left click on the <em>(%)</em> for instrument 0 pan left
		</li>
		<li>enter <em>-100</em></li>
		<li>double left click on the <em>(%)</em> for instrument 0 pan right
		</li>
		<li>enter <em>0</em></li>
	</ul>

	<li><b>To pan an instrument to the right</b></li>
	<ul>
		<li>double left click on the <em>(%)</em> for instrument 0 pan left
		</li>
		<li>enter <em>0</em></li>
		<li>double left click on the <em>(%)</em> for instrument 0 pan right
		</li>
		<li>enter <em>100</em></li>
	</ul>
	<li><b>Repeat for all instruments</b></li>
</ul>

<p>
	<b>Set one channel and note per instrument</b>
</p>

<p>Right now if we use a midi keyboard every sample is triggered at the
	same time.</p>

<p>Let's configure it so that each sample is triggered by a single key
	and sent out on a single channel.</p>

<ul>
	<li>Open the LSP-Sampler plugin</li>
	<li>select the <em>Working area</em> dropdown in the top left of the
		screen and choose <em>Instruments</em>
		<ul>
			<li><b>Instrument 0</b> Note: <em>C 3</em></li>
			<li><b>Instrument 1</b> Note: <em>C# 3</em></li>
			<li><b>Instrument 2</b> Note: <em>D 3</em></li>
			<li><b>Instrument 3</b> Note: <em>D# 3</em></li>
			<li><b>Instrument 4</b> Note: <em>E 3</em></li>
			<li><b>Instrument 5</b> Note: <em>F 3</em></li>
			<li><b>Instrument 6</b> Note: <em>F# 3</em></li>
			<li><b>Instrument 7</b> Note: <em>G 3</em></li>
			<li><b>Instrument 8</b> Note: <em>G# 3</em></li>
			<li><b>Instrument 9</b> Note: <em>A 3</em></li>
			<li><b>Instrument 10</b> Note: <em>A# 3</em></li>
			<li><b>Instrument 11</b> Note: <em>B 3</em></li>
			<li><b>Instrument 12</b> Note: <em>C 4</em></li>
			<li><b>Instrument 13</b> Note: <em>C# 4</em></li>
			<li><b>Instrument 14</b> Note: <em>D 4</em></li>
			<li><b>Instrument 15</b> Note: <em>D# 4</em></li>
			<li><b>Instrument 16</b> Note: <em>E 4</em></li>
			<li><b>Instrument 17</b> Note: <em>F 4</em></li>
			<li><b>Instrument 18</b> Note: <em>F# 4</em></li>
		</ul>
	
	<li>Now play the corresponding note on your midi keyboard or the Ardour
		piano roll and you should hear the appropriate drum sample.</li>
</ul>
<p>
	<b>Save your kit</b>
</p>
<ul>
	<li>click on the blue <em>LSP</em> button
	</li>
	<ul>
		<li>select <em>export settings to file</em> and save your file
		</li>
	</ul>
	<li>you can now load this ready to use drumkit into Multi-Sampler into any session!</li>
</ul>
