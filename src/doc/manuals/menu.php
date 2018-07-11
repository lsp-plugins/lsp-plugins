<ul>
	<?php if (isset($MENUITEM)) {?>
		<li class="sel"><?php echo htmlspecialchars($HEADER); ?></li>
		<li><a href="<?= $DOCROOT ?>index.html">Home</a></li>
	<?php } else {?>
		<li class="sel">Home</li>
	<?php }?>
</ul>
