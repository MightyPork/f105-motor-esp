<?php $page = 'spectrogram'; include "_start.php"; ?>

<h1>Spectrogram</h1>

<div class="Box center" id="samp-ctrl">
	<div>
		<label for="count">Bins</label>
		<label for="count" class="select-wrap">
			<select name="count" id="count">
				<option value="16">8
				<option value="32">16
				<option value="64">32
				<option value="128">64
				<option value="256">128
				<option value="512">256
				<option value="1024" selected>512
<!--				<option value="2048" selected>1024-->
			</select>
		</label>
	</div>
	<div>
		<label for="freq">f<sub>bw</sub> <span class="mq-normal-min nb">=</span><span class="mq-tablet-max nb">(Hz)</span></label>
		<input id="freq" type="number" value="2048">
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
