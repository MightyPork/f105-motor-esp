<?php $page = 'waveform';
include "_start.php"; ?>

<h1>Waveform</h1>

<div class="Box center">
	<label for="count">Samples:</label>
	<input id="count" type="number" value="100" style="width:100px">&nbsp;
	<a id="load" class="button btn-green">Load</a>
</div>

<div class="Box medium">
	<div id="chart" class="ct-chart ct-wide"></div>
</div>

<script>
	$().ready(page_waveform.init());
</script>

<?php include "_end.php"; ?>
