<p>The following page contains references to video tutorials available at <a href="https://youtube.com/">YouTube</a>.</p>

<?php 
foreach ($YOUTUBE as $key => $value)
{
?>
	<h2><?php echo htmlspecialchars($value); ?></h2>
	<iframe width="560" height="315" src="https://www.youtube.com/embed/<?php echo $key; ?>" frameborder="0" allowfullscreen></iframe>
<?php
}
?>
