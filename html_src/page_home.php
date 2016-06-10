<?php $page = 'home';
include "_start.php"; ?>

<h1>Ovládání motoru</h1>

<div class="Box center">
	<a class="button btn-blue" id="btn-rev">&lt;&lt;</a>&nbsp;&nbsp;&nbsp;
	<a class="button btn-red" id="btn-stop">Stop</a>&nbsp;&nbsp;&nbsp;
	<a class="button btn-blue" id="btn-fwd">&gt;&gt;</a>
</div>

<div class="Box center">
	<input type="range" id="control-range" value="0" min="-100" max="100" step="0.1">
</div>

<script>
	$().ready(page_home.init);
</script>

<?php include "_end.php"; ?>
