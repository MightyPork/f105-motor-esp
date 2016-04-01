<?php $page = 'spectrogram'; include "_start.php"; ?>

<h1>Spectrogram</h1>

<div class="Box center" id="samp-ctrl">
	<div>
		<label for="freq">Rate <span class="mq-tablet-max" style="font-weight:normal;">(Hz)</span></label>
		<input id="freq" type="number" value="4096">
		<span class="mq-normal-min">Hz</span>
	</div>
	<div>
		<label for="interval">Interval <span class="mq-tablet-max" style="font-weight:normal;">(ms)</span></label>
		<input id="interval" type="number" value="100" step=100 min=0>
		<span class="mq-normal-min">ms</span>
	</div>
	<div>
		<a id="go-btn" class="button btn-green">Start</a>
	</div>
</div>

<div class="Box center">
	<canvas id="sg" width=860 height=512></canvas>
</div>

<script>
	$().ready(page_spectrogram.init());
</script>

<?php include "_end.php"; ?>
