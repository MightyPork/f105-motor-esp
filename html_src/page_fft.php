<?php $page = 'fft'; include "_start.php"; ?>

<h1>FFT</h1>

<div class="Box center" id="samp-ctrl">
	<div>
		<label for="count">Bins</label>
		<label for="count" class="select-wrap">
			<select name="count" id="count">
<!--				<option value="16">8-->
<!--				<option value="32">16-->
<!--				<option value="64">32-->
<!--				<option value="128">64-->
<!--				<option value="256">128-->
				<option value="512">256
				<option value="1024">512
				<option value="2048" selected>1024
			</select>
		</label>
	</div>
	<div>
		<label for="freq">Rate <span class="mq-tablet-max" style="font-weight:normal;">(Hz)</span></label>
		<input id="freq" type="number" value="4096">
		<span class="mq-normal-min">Hz</span>
	</div>
	<div>
		<a id="load" class="button btn-green">Load</a>
	</div>
</div>

<div class="Box medium chartbox">
	<div id="chart" class="ct-chart ct-wide ct-with-area"></div>
	<div class="stats invis">
		<table>
			<tr>
				<th>Samples</th>
				<td id="stat-count"></td>
			</tr>
			<tr>
				<th>f<sub>s</sub></th>
				<td id="stat-f-s"></td>
			</tr>
			<tr>
				<th>I<sub>peak</sub></th>
				<td id="stat-i-peak"></td>
			</tr>
			<tr>
				<th>I<sub>RMS</sub></th>
				<td id="stat-i-rms"></td>
			</tr>
		</table>
		<div class="ar"><!-- auto reload -->
			<input type="number" id="ar-time" step="0.5" value="1" min="0">&nbsp;s
			<input type="button" id="ar-btn" class="btn-blue narrow" value="Auto">
		</div>
	</div>
</div>

<script>
	$().ready(page_waveform.init('fft'));
</script>

<?php include "_end.php"; ?>
