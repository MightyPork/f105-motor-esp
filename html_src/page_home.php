<?php $page = 'home';
include "_start.php"; ?>

<h1>Motor Control</h1>

<div class="Box center">
	<a class="button btn-blue" id="btn-rev">REV</a><span style="display:inline-block;width:2em"></span>
	<a class="button btn-blue" id="btn-fwd">FWD</a>
</div>

<script>
	$().ready(page_home.init);
</script>

<?php include "_end.php"; ?>
