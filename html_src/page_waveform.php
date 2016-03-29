<?php $page = 'waveform';
include "_start.php"; ?>

<h1>Waveform</h1>

<div class="Box center" id="samp-ctrl">
	<div>
		<label for="count">Samples</label>
		<input id="count" type="number" value="500">
	</div>
	<div>
		<label for="freq">Freq. <span class="mq-tablet-max" style="font-weight:normal;">(Hz)</span></label>
		<input id="freq" type="number" value="4000">
		<span class="mq-normal-min">Hz</span>
	</div>
	<div>
		<a id="load" class="button btn-green">Load</a>
	</div>
</div>

<div class="Box medium">
	<div id="chart" class="ct-chart ct-wide"></div>
</div>

<script>
	$().ready(page_waveform.init());
</script>

<?php include "_end.php"; ?>
