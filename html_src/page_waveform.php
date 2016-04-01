<?php $page = 'waveform'; include "_start.php"; ?>

<h1>Waveform</h1>

<div class="Box center" id="samp-ctrl">
	<div>
		<label for="count">Samples</label>
		<input id="count" type="number" value="500">
	</div>
	<div>
		<label for="freq">Rate <span class="mq-tablet-max" style="font-weight:normal;">(Hz)</span></label>
		<input id="freq" type="number" value="4000">
		<span class="mq-normal-min">Hz</span>
	</div>
	<div>
		<a id="load" class="button btn-green">Load</a>
	</div>
</div>

<div class="Box medium chartbox">
	<div id="chart" class="ct-chart ct-wide"></div>
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
	$().ready(page_waveform.init('raw'));
</script>

<?php include "_end.php"; ?>